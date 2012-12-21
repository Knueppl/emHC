#include "STLM75.h"
#include "IOHandler.h"

#include <QDomElement>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#define MSG(x) (io() << "STLM75 (" << m_name << ") : " << x << ".\n")

STLM75::STLM75(const unsigned char busID, const unsigned char busAddress, const QByteArray& name)
: TempSensor(name),
    m_device(0),
    m_busID(busID),
    m_busAddress(busAddress)
{
    this->open(busID, busAddress);
}

STLM75::STLM75(const QDomNode& node)
: TempSensor(),
    m_device(0),
    m_busID(0),
    m_busAddress(0)
{
    QDomElement tag = node.toElement();

    if (tag.isNull())
    {
        MSG("DomNode not valid");
        return;
    }

    if (tag.tagName() != "stlm75")
    {
        MSG("XML Tag has wrong name");
        return;
    }

    for (QDomNode node = tag.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        tag = node.toElement();

        if (tag.isNull())
            continue;

        if (tag.tagName() == "name")
        {
            m_name = tag.text().toUtf8();
        }
        else if (tag.tagName() == "bus_id")
        {
            m_busID = static_cast<unsigned char>(tag.text().toUInt(0, 16));
        }
        else if (tag.tagName() == "bus_address")
        {
            m_busAddress = static_cast<unsigned char>(tag.text().toUInt(0, 16));
        }
    }

    MSG("on bus " << static_cast<unsigned int>(m_busID) << " address " << static_cast<unsigned int>(m_busAddress));
    this->open(m_busID, m_busAddress);
}

STLM75::~STLM75(void)
{
    if (m_device < 0)
        return;

    ::close(m_device);
}

bool STLM75::open(const unsigned char busID, const unsigned char busAddress)
{
    QByteArray busName("/dev/i2c-");

    busName.append(QByteArray::number(static_cast<unsigned int>(busID)));

    if ((m_device = ::open(busName.data(), O_RDWR)) < 0)
    {
        MSG("can't open I2C bus " << busName);
        return false;
    }

    if (ioctl(m_device, I2C_SLAVE, busAddress) < 0)
    {
        MSG("can't setup device");
        ::close(m_device);
        m_device = -1;
        return false;
    }

    return true;
}

float STLM75::readSensor(void) const
{
    const int SIZE = 1;
    char buffer[SIZE] = { 0x00 };

    if (::write(m_device, buffer, SIZE) != SIZE)
    {
        MSG("can't write to device");
        return -100.0;
    }

    if (::read(m_device, buffer, SIZE) != SIZE)
    {
        MSG("can't read from device");
        return -100.0;
    }

    return static_cast<float>(*buffer);
}

QByteArray STLM75::data(void) const
{
    QByteArray string("STLM75(");

    string.append(m_name).append("|");
    string.append(QByteArray::number(static_cast<unsigned int>(m_busID), 16)).append("|");
    string.append(QByteArray::number(static_cast<unsigned int>(m_busAddress), 16)).append("|");
    string.append(QByteArray::number(this->readSensor())).append(")");

    return string;
}
