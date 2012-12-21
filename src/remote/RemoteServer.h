#ifndef __REMOTE_SERVER__
#define __REMOTE_SERVER__

#include <QTcpServer>
#include <QString>
#include <QSslKey>
#include <QSslCertificate>
#include <QSslError>

class RemoteClient;

class RemoteServer : public QTcpServer
{
    Q_OBJECT

public:
    RemoteServer(const QString& configFile = QString("tcp-server.conf"), QObject* parent = 0);
    virtual ~RemoteServer(void);

signals:
    void newConnection(RemoteClient* client);
    void delConnection(RemoteClient* client);

protected:
    virtual void incomingConnection(int socketDescriptor);

private slots:
    void ready(void);
    void disconnected(void);
    void error(const QList<QSslError>& errors);

private:
    QString m_configFile;
    QString m_privateKeyFile;
    QString m_certFile;
    QSslKey m_privateKey;
    QSslCertificate m_cert;
    QList<RemoteClient*> m_toDelete;
};

#endif


