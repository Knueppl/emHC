#include "Modem.h"
#include "IOHandler.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <QByteArray>
#include <QDomNode>
#include <QDebug>

namespace {
const char* MSG_NO_CARRIER = "NOCARRIER";
const char* MSG_OK = "OK";
const char* MSG_REJECTED = "BUSY";
}

Modem::Modem(const QByteArray& device)
    : m_device(0),
      m_state(Close)
{
    if (device.isEmpty())
        return;

    this->configureDevice(device);
}

Modem::Modem(const QDomNode& node)
    : m_device(0),
      m_state(Close)
{
    const QDomElement root(node.toElement());

    if (root.isNull() || root.tagName() != "modem")
        return;

    QDomNodeList nodes(node.childNodes());

    for (int i = 0; i < nodes.size(); i++)
    {
        QDomElement tag(nodes.at(i).toElement());

        if (tag.isNull())
            continue;

        if (tag.tagName() == "device")
            this->configureDevice(tag.text().toUtf8());
    }
}

Modem::~Modem(void)
{
    if (m_state != Close)
        ::close(m_device);
}

void Modem::configureDevice(const QByteArray& device)
{
    if (m_state != Close)
        ::close(m_device);

    if ((m_device = ::open(device.data(), O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
    {
        m_device = 0;
        m_state = Close;
        io << "Modem: can't open device \"" << device << "\"\n";
        return;
    }

    struct termios options;

    fcntl(m_device, F_SETFL, FNDELAY);

    tcgetattr(m_device, &options);

    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    options.c_cflag = 0;
    options.c_cflag |= (CREAD | CLOCAL);
    options.c_cflag |= CS8;
    options.c_cflag |= CRTSCTS;

    options.c_oflag = 0;
    options.c_oflag |= OPOST;
    options.c_oflag |= ONLCR;

    options.c_iflag = 0;
    options.c_iflag |= (IGNPAR | ICRNL);

    options.c_lflag = 0;

    tcsetattr(m_device, TCSANOW, &options);

    m_deviceName = device;
    m_state = Open;

    io << "Modem: device \"" << device << "\" configured.\n";
    this->connect(&m_timer, SIGNAL(timeout()), this, SLOT(readFromDevice()));
}

bool Modem::call(const QByteArray& phoneNumber, const unsigned int timeout)
{
    if (m_state == Close)
        return false;

    this->clearDeviceBuffer();
    QByteArray command(QByteArray("ATDT ").append(phoneNumber).append("\n"));

    if (::write(m_device, command.data(), command.size()) != command.size())
    {
        this->closeDevice();
        return false;
    }

    m_state = Calling;
    m_timeout = timeout;
    m_timestamp.start();
    m_buffer.clear();
    m_timer.start(100);

    return true;
}

void Modem::closeDevice(void)
{
    m_timer.stop();
    this->disconnect(&m_timer, SIGNAL(timeout()), this, SLOT(readFromDevice()));
    io << "Modem: close device \"" << m_deviceName << "\".\n";
    ::close(m_device);
    m_device = 0;
    m_buffer.clear();
    m_state = Close;
}

void Modem::clearDeviceBuffer(void)
{
    const int SIZE = 1024;
    char data[SIZE];

    while (::read(m_device, data, SIZE) == SIZE);
}

void Modem::readFromDevice(void)
{
    if (m_state == Close)
    {
        qDebug() << __PRETTY_FUNCTION__ << "device closed";
        return;
    }

    const unsigned int SIZE = 1024;
    char data[SIZE];
    int ret;

    if ((ret = ::read(m_device, data, SIZE)) <= 0)
    {
        m_buffer.clear();

        if (static_cast<unsigned int>(m_timestamp.elapsed()) >= m_timeout)
            ::write(m_device, "\n", 1);

        return;
    }

    data[ret] = '\0';
    m_buffer.append(QByteArray(data).replace(" ", "").replace("\n", ""));

    if (m_buffer.contains(MSG_OK))
    {
        io << "Modem: MSG OK\n";
        m_timer.stop();
        m_state = Open;
        m_buffer.clear();
        emit this->stopped(m_state);

        return;
    }
    else if (m_buffer.contains(MSG_NO_CARRIER))
    {
        io << "Modem: no carrier\n";
        m_timer.stop();
        m_state = NoCarrier;
        m_buffer.clear();
        emit this->stopped(m_state);

        return;
    }
    else if (m_buffer.contains(MSG_REJECTED))
    {
        io << "Modem: call rejected\n";
        m_timer.stop();
        m_state = CallRejected;
        m_buffer.clear();
        emit this->stopped(m_state);

        return;
    }
    else if (static_cast<unsigned int>(m_timestamp.elapsed()) >= m_timeout + 5000)
    {
        io << "Modem: timeout by calling\n";
        this->closeDevice();
        emit this->stopped(m_state);
    }
}
