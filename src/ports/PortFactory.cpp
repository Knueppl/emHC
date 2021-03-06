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

}

Port* PortFactory::build(const QDomNode& node)
{
    /* Check if node is from type Element and the tagname is "port" */
    QDomElement elm = node.toElement();

    if (elm.isNull() || elm.tagName() != "port")
    {
        io() << "PortFactory: Falscher Tag.";
        return 0;
    }

    return new Port(node);
}

Port* PortFactory::build(const unsigned int portNumber, const QByteArray& portName, const QByteArray& pinName, const int flags)
{
    return new Port(portNumber, portName, pinName, flags);
}
