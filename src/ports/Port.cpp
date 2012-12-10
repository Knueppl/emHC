#include "Port.h"
#include "IOHandler.h"

#include <QFile>
#include <QDir>
#include <QDomNode>
#include <QDebug>

QByteArray Port::s_path;
QTimer Port::s_timer;

Port::Port(const unsigned int portNumber, const QByteArray& portName, const QByteArray& pinName, const int flags, QObject* parent)
    : QObject(parent),
      m_portNumber(portNumber),
      m_portName(portName),
      m_pinName(pinName),
      m_directionFile(QByteArray(s_path).append("gpio").append(QByteArray::number(m_portNumber)).append("/direction")),
      m_valueFile(QByteArray(s_path).append("gpio").append(QByteArray::number(m_portNumber)).append("/value")),
      m_value(false),
      m_holdTimeHigh(1),
      m_holdTimeLow(1),
      m_tickCounter(1),
      m_multiplexer(pinName, flags)
{
    this->init();
}

Port::Port(const QDomNode& node, QObject* parent)
    : QObject(parent),
      m_portNumber(this->getPortNumberFromNode(node)),
      m_portName(this->getPortNameFromNode(node)),
      m_pinName(this->getPinNameFromNode(node)),
      m_directionFile(QByteArray(s_path).append("gpio").append(QByteArray::number(m_portNumber)).append("/direction")),
      m_valueFile(QByteArray(s_path).append("gpio").append(QByteArray::number(m_portNumber)).append("/value")),
      m_value(false),
      m_holdTimeHigh(this->getHoldTimeHighFromNode(node)),
      m_holdTimeLow(this->getHoldTimeLowFromNode(node)),
      m_tickCounter(m_holdTimeHigh),
      m_multiplexer(m_pinName, this->getFlagsFromNode(node))
{
    this->init();
}

bool Port::init(void)
{
    if (!m_multiplexer.works())
    {
        io() << "Port" << m_portName << ": Multiplexer " << m_multiplexer.name() << " arbeitet nicht richtig.";
        return false;
    }

    QFile file(QByteArray(s_path).append("export"));

    if (!file.open(QIODevice::WriteOnly))
    {
        io() << "Port " << m_portName << ": Kann Datei \"" << file.fileName() << "\" nicht öffnen.";
        return false;
    }

    if (file.write(QByteArray::number(m_portNumber)) == -1)
    {
        io() << "Port " << m_portName << ": Kann nicht in die Datei \"export\" schreiben.";
        file.close();
        return false;
    }

    file.close();
    QDir dir(s_path);
    if (!dir.exists(QByteArray("gpio").append(QByteArray::number(m_portNumber))))
    {
        io() << "Port " << m_portName << ": Fehler beim erstellt des Portes.";
        return false;
    }

    if (!m_directionFile.open(QIODevice::ReadWrite))
    {
        io() << "Port " << m_portName << ": Kann Datei" << m_directionFile.fileName() << "nicht öffnen.";
        return false;
    }

    if (!m_valueFile.open(QIODevice::ReadOnly))
    {
        io() << "Port" << m_portName << ": Kann Datei" << m_valueFile.fileName() << "nicht öffnen.";
        return false;
    }

    io() << "Port " << m_portName << ": Erfolgreich eingerichtet.";
    this->connect(&s_timer, SIGNAL(timeout()), this, SLOT(tick()));
    return true;
}

Port::~Port(void)
{
    this->setType(Input);
    this->disablePull();
    m_directionFile.close();
    m_valueFile.close();

    QFile file(QByteArray(s_path).append("unexport"));

    if (!file.open(QIODevice::WriteOnly))
    {
        io() << "Port " << m_portName << ": Kann Datei \"unexport\" nicht öffnen.";
        return;
    }

    if (file.write(QByteArray::number(m_portNumber)) == -1)
    {
        io() << "Port " << m_portName << ": Fehler beim schliessen des Ports " << m_portName << ".";
        file.close();
        return;
    }

    file.close();
    io() << "Port " << m_portName << ": geschlossen.";
}

int Port::getFlagsFromNode(const QDomNode& node)
{
    int flags = 0;
    QDomElement tag = node.toElement();

    if (!tag.isNull() && tag.attribute("type") == "in")
        flags |= PortMultiplexer::Input;


    QDomNodeList nodes = node.childNodes();

    for (int i = 0; i < nodes.size(); i++)
    {
        QDomElement elm = nodes.at(i).toElement();

        if (elm.isNull())
            continue;

        if (elm.tagName() == "disable_pull")
        {
            if (elm.text() == "true")
                flags |= PortMultiplexer::DisablePull;
        }
        else if (elm.tagName() == "pull")
        {
            if (elm.text() == "up")
                flags |= PortMultiplexer::Pullup;
        }
    }

    return flags;
}

QByteArray Port::getPortNameFromNode(const QDomNode& node)
{
    QDomElement elm = node.toElement();

    if (elm.isNull())
        return QByteArray();

    return elm.attribute("name").toUtf8();
}

unsigned int Port::getPortNumberFromNode(const QDomNode& node)
{
    QDomNodeList nodes = node.childNodes();

    for (int i = 0; i < nodes.size(); i++)
    {
        QDomElement elm = nodes.at(i).toElement();

        if (elm.isNull())
            continue;

        if (elm.tagName() == "num")
        {
            return static_cast<unsigned int>(elm.text().toInt());
        }
    }

    return 0;
}

unsigned int Port::getHoldTimeHighFromNode(const QDomNode& node)
{
    QDomNodeList nodes = node.childNodes();

    for (int i = 0; i < nodes.size(); i++)
    {
        QDomElement elm = nodes.at(i).toElement();

        if (elm.isNull())
            continue;

        if (elm.tagName() == "holdtime_high")
        {
            return static_cast<unsigned int>(elm.text().toInt());
        }
    }

    return 0;
}

unsigned int Port::getHoldTimeLowFromNode(const QDomNode& node)
{
    QDomNodeList nodes = node.childNodes();

    for (int i = 0; i < nodes.size(); i++)
    {
        QDomElement elm = nodes.at(i).toElement();

        if (elm.isNull())
            continue;

        if (elm.tagName() == "holdtime_low")
        {
            return static_cast<unsigned int>(elm.text().toInt());
        }
    }

    return 0;
}

QByteArray Port::getPinNameFromNode(const QDomNode& node)
{
    QDomNodeList nodes = node.childNodes();

    for (int i = 0; i < nodes.size(); i++)
    {
        QDomElement elm = nodes.at(i).toElement();

        if (elm.isNull())
            continue;

        if (elm.tagName() == "pin")
        {
            return elm.text().toUtf8();
        }
    }

    return QByteArray();
}

void Port::tick(void)
{
    /* if port dosen't work return from function */
    if (!this->works())
    {
        io() << "Port " << m_portName << ": ist nicht funktionsfähig. " << __PRETTY_FUNCTION__ << " wird aus dem Timer ausgetragen.";
        this->disconnect(&s_timer, SIGNAL(timeout()), this, SLOT(tick()));
        return;
    }

    /* if type == Output return from function */
    if (this->type() == Output)
        return;

    /* input processing */
    m_valueFile.seek(0);
    const bool curValue = m_valueFile.readAll().contains("1");

    /* check if value of the port has changed */
    if (m_value != curValue)
    {
        /* if m_tickCounter > 0 decrements counter and return from function */
        if (m_tickCounter)
        {
            m_tickCounter--;
            return;
        }

        /* if m_tickCounter == 0 accept new value */
        m_value = curValue;
        emit this->valueChanged(m_value);
    }
    else
    {
        /* reset m_tickCounter */
        if (m_value)
            m_tickCounter = m_holdTimeLow;
        else
            m_tickCounter = m_holdTimeHigh;
    }
}

void Port::disablePull(const bool disable)
{
    if (!this->works())
        return;

    if (disable)
        m_multiplexer.setFlags(m_multiplexer.flags() | PortMultiplexer::DisablePull);
    else
        m_multiplexer.setFlags(m_multiplexer.flags() & ~PortMultiplexer::DisablePull);
}

void Port::enablePullup(const bool enable)
{
    if (!this->works())
        return;

    if (enable)
        m_multiplexer.setFlags(m_multiplexer.flags() | PortMultiplexer::Pullup);
    else
        m_multiplexer.setFlags(m_multiplexer.flags() & ~PortMultiplexer::Pullup);
}

void Port::setType(const Type type)
{
    if (!this->works())
        return;

    switch (type)
    {
    case Input:
        m_multiplexer.setFlags(m_multiplexer.flags() | PortMultiplexer::Input);
        break;

    case Output:
        m_multiplexer.setFlags(m_multiplexer.flags() & ~PortMultiplexer::Input);
        break;

    default:
        break;
    }
}

void Port::setValue(const bool value)
{
    /* return if any thing didn't init correctly or port is from type input */
    if (!this->works() || m_multiplexer.flags() & PortMultiplexer::Input)
        return;

    if (value)
    {
        if (m_directionFile.write("high") == -1)
        {
            io() << "Port " << m_portName << ": Fehler beim schreiben in die Datei " << m_directionFile.fileName() << ".";
            return;
        }
    }
    else
    {
        if (m_directionFile.write("low") == -1)
        {
            io() << "Port " << m_portName << ": Fehler beim schreiben in die Datei " << m_directionFile.fileName() << ".";
            return;
        }
    }
}

QByteArray& operator<<(QByteArray& cstring, const Port& port)
{
    QTextStream cstr(&cstring);

    cstr << "Port " << port.m_portName              << "\n";
    cstr << "------------------------------\n";
    cstr << "Pin         : " << port.m_pinName      << "\n";
    cstr << "Number      : " << port.m_portNumber   << "\n";
    cstr << "Holdtime hi : " << port.m_holdTimeHigh << "\n";
    cstr << "Holdtime lo : " << port.m_holdTimeLow  << "\n";
    cstr << "Value       : " << port.m_value        << "\n";
    cstr << "------------------------------\n\n";

    return cstring;
}

QTextStream& operator<<(QTextStream& stream, const Port& port)
{
    stream << "Port " << port.m_portName << "\n";
    stream << "------------------------------\n";
    stream << "Value : " << port.m_value << "\n";

    if (port.m_value)
    {
        if (port.m_tickCounter != port.m_holdTimeLow)
            stream << "Pin : value changed since " << port.m_holdTimeLow - port.m_tickCounter << " ticks\n";
    }
    else
    {
        if (port.m_tickCounter != port.m_holdTimeHigh)
            stream << "Pin : value changed since " << port.m_holdTimeHigh - port.m_tickCounter << " ticks\n";
    }

    stream << "------------------------------\n\n";

    return stream;
}
