#include "AlertHandler.h"
#include "IOHandler.h"

#include <QDomNode>

AlertHandler::AlertHandler(const QDomNode& node, QObject* parent)
    : QObject(parent),
      m_modem(0)
{
    const QDomElement root(node.toElement());

    if (root.isNull() || root.tagName() != "alert_handler")
        return;

    QDomNodeList nodes(root.childNodes());

    for (int i = 0; i < nodes.size(); i++)
    {
        const QDomElement tag(nodes.at(i).toElement());

        if (tag.isNull())
            continue;

        if (tag.tagName() == "phone")
            this->addPhoneNumber(tag);
        else if (tag.tagName() == "modem")
        {
            m_modem = new Modem(tag);
            this->connect(m_modem, SIGNAL(stopped(const Modem::State)), this, SLOT(finishedCall(const Modem::State)));
        }
    }
}

AlertHandler::~AlertHandler(void)
{
    delete m_modem;
}

void AlertHandler::startAlertRoutine(void)
{
    if (!m_modem || !m_phones.size())
        return;

    m_calling = m_phones.begin();
    io() << "AlertHandler: try calling " << *m_calling;
    m_modem->call(*m_calling, 30000);
}

void AlertHandler::finishedCall(const Modem::State state)
{
    if (state == Modem::Close)
    {
        io() << "AlertHandler: Modem closed. Try to reconfigure modem.";
        m_modem->reconfigure();
        this->startAlertRoutine();
    }

    if (state == Modem::CallRejected)
    {
        io() << "AlertHandler: call rejeceted from " << *m_calling;
        return;
    }

    if (++m_calling >= m_phones.end())
        m_calling = m_phones.begin();

    io() << "AlertHandler: try calling " << *m_calling;
    m_modem->call(*m_calling, 30000);
}

void AlertHandler::addPhoneNumber(const QDomNode& node)
{
    const QDomElement root(node.toElement());

    if (root.isNull())
        return;

    QDomNodeList nodes(root.childNodes());

    for (int i = 0; i < nodes.size(); i++)
    {
        const QDomElement tag(nodes.at(i).toElement());

        if (tag.isNull())
            continue;

        if (tag.tagName() == "number")
        {
            m_phones.push_back(tag.text().toUtf8());
            io() << "AlertHandler: add phone number " << tag.text();
        }
    }
}
