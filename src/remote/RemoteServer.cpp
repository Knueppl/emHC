#include "RemoteServer.h"
#include "RemoteClient.h"
#include "IOHandler.h"

#include <QDomDocument>
#include <QFile>
#include <QSslKey>


namespace {
const unsigned int DEFAULT_PORT = 4321;
const char* PHRASE = "91338";
}

#define MSG(x) (io() << "RemoteServer: " << x << ".")

RemoteServer::RemoteServer(const QString& configFile, QObject* parent)
    : QTcpServer(parent),
      m_configFile(configFile)
{
    /* Default Werte laden */
    QFile cfile(configFile);

    if (!cfile.open(QIODevice::ReadOnly))
    {
        MSG("can't open file " << configFile);
        this->listen(QHostAddress::Any, DEFAULT_PORT);
        return;
    }

    QDomDocument doc;
    if (!doc.setContent(&cfile))
    {
        MSG("corrupt file " << configFile << ". use default settings");
        cfile.close();
        this->listen(QHostAddress::Any, DEFAULT_PORT);
        return;
    }

    cfile.close();
    QDomNode node = doc.documentElement().firstChild();
    unsigned int port = DEFAULT_PORT;

    for (; !node.isNull(); node = node.nextSibling())
    {
        QDomElement tag = node.toElement();

        if (tag.isNull())
            continue;

        if (tag.tagName() == "port")
        {
            port = tag.text().toUInt();
        }
        else if (tag.tagName() == "privatekey")
        {
            QFile file(tag.text());

            if (!file.open(QIODevice::ReadWrite))
                continue;

            QSslKey key(&file, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, QByteArray(PHRASE));

            if (key.isNull())
                continue;

            m_privateKey = key;
            m_privateKeyFile = tag.text();
            file.close();
        }
        else if (tag.tagName() == "CA")
        {
            QFile file(tag.text());

            if (!file.open(QIODevice::ReadWrite))
                continue;

            QSslCertificate cert(&file, QSsl::Pem);

            if (cert.isNull())
                continue;

            m_cert = cert;
            m_certFile = tag.text();
            file.close();
        }
    }

    if (m_privateKey.isNull())
    {
        MSG("no private key found. Try privekey.pem ...");
        QFile file("privkey.pem");

        if (file.open(QIODevice::ReadWrite))
        {
            QSslKey key(&file, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, QByteArray(PHRASE));

            if (!key.isNull())
            {
                m_privateKey = key;
                m_privateKeyFile = "privkey.pem";
            }
        }
        else MSG("can't open privkey.pem");
    }

    if (m_cert.isNull())
    {
        MSG("no CA certificate found. Try cacert.pem ...");
        QFile file("cacert.pem");

        if (file.open(QIODevice::ReadWrite))
        {
            QSslCertificate cert(&file, QSsl::Pem);

            if (!cert.isNull())
            {
                m_cert = cert;
                m_certFile = "cacert.pem";
            }
        }
        else MSG("can't open cacert.pem");
    }

    this->listen(QHostAddress::Any, port);
}

RemoteServer::~RemoteServer(void)
{
    /* Setup als Default speicher */
    QFile file(m_configFile);

    if (file.open(QIODevice::WriteOnly))
    {
        QDomDocument doc;
        QDomElement root = doc.createElement("connection");
        doc.appendChild(root);

        QDomElement tag  = doc.createElement("port");
        QDomText    text = doc.createTextNode(QString::number(this->serverPort()));
        root.appendChild(tag);
        tag.appendChild(text);

        if (m_privateKeyFile.length())
        {
            tag  = doc.createElement("privatekey");
            text = doc.createTextNode(m_privateKeyFile);
            root.appendChild(tag);
            tag.appendChild(text);
        }

        if (m_certFile.length())
        {
            tag  = doc.createElement("CA");
            text = doc.createTextNode(m_certFile);
            root.appendChild(tag);
            tag.appendChild(text);
        }

        QTextStream stream(&file);
        doc.save(stream, 2);
        file.close();
    }
}

void RemoteServer::incomingConnection(int socketDescriptor)
{
    /* Lösche alte getrennte Verbindungen */
    qDeleteAll(m_toDelete);

    /* Erstelle neuen Client */
    RemoteClient* client = new RemoteClient(this);

    if (!m_privateKey.isNull())
        client->setPrivateKey(m_privateKey);

    if (!m_cert.isNull())
        client->setLocalCertificate(m_cert);

    if (!client->setSocketDescriptor(socketDescriptor))
    {
        delete client;
        return;
    }

    this->connect(client, SIGNAL(encrypted()), this, SLOT(ready()));
    this->connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
    this->connect(client, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(error(const QList<QSslError>&)));
    client->startServerEncryption();
}

void RemoteServer::ready(void)
{
    RemoteClient* client = dynamic_cast<RemoteClient*>(this->sender());

    if (!client)
    {
        MSG("error occurred durring open connection. QObject not from type RemoteClient");
        return;
    }

    MSG("new remote client");
    emit newConnection(client);
}

void RemoteServer::disconnected(void)
{
    RemoteClient* client = dynamic_cast<RemoteClient*>(this->sender());

    if (!client)
        return;

    emit this->delConnection(client);

    MSG("disconnected from remote client");
}

void RemoteServer::error(const QList<QSslError>& errors)
{
    /* Fix me */
    qDebug() << errors;
}
