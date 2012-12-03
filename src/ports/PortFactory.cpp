#include "PortFactory.h"
#include "Port.h"
#include "IOHandler.h"

#include <QDomNode>
#include <QDebug>
#include <QByteArray>

PortFactory::PortFactory(const QByteArray& multiplexerPath, const QByteArray& portPath)
{
    PortMultiplexer::setPath(multiplexerPath);
    Port::setPath(portPath);

    /* start timer for ports with 100 ms intervals */
    Port::startTimer(100);
}

PortFactory::~PortFactory(void)
{
//    Port::stopTimer();
}

Port* PortFactory::build(const QDomNode& node)
{
    qDebug() << __PRETTY_FUNCTION__;
    /* Check if node is from type Element and the tagname is "port" */
    QDomElement elm = node.toElement();

    if (elm.isNull() || elm.tagName() != "port")
    {
        io << __PRETTY_FUNCTION__ << " Falscher Tag.\n";
        return 0;
    }

    return new Port(node);
}

Port* PortFactory::build(const unsigned int portNumber, const QByteArray& portName, const QByteArray& pinName, const int flags)
{
    return new Port(portNumber, portName, pinName, flags);
}
