#include "RemoteClient.h"
#include "IOHandler.h"
#include "TempSensor.h"

#include <QStringRef>

#define MSG(x) (io() << "RemoteClient: " << x << ".")

RemoteClient::RemoteClient(QObject* parent)
    : QSslSocket(parent)
{
    this->connect(this, SIGNAL(readyRead()), this, SLOT(receive()));
}

void RemoteClient::receive(void)
{
    static QString buffer;

    buffer.append(this->readLine());

    int tOpenB = buffer.indexOf('<');
    int tOpenE = buffer.indexOf('>');
    int tCloseB = buffer.lastIndexOf('<');
    int tCloseE = buffer.lastIndexOf('>');

    QStringRef tag = buffer.midRef(tOpenB + 1, tOpenE - tOpenB - 1);
    QStringRef tagClose = buffer.midRef(tCloseB + 1, tCloseE - tCloseB - 1);

    if (tag == "command" && tagClose == "/command")
    {
        Command command(Command::stringToType(buffer.midRef(tOpenE + 1, tCloseB - tOpenE - 1)));

        if (!command.isNull())
        {
            MSG("command " << Command::typeToString(command.type()) << " received");
            emit this->command(command);
        }

        buffer.clear();
    }
}

QByteArray RemoteClient::Command::typeToString(const Type type)
{
    switch (type)
    {
    case RealTimeOn:
        return QByteArray("RealTimeOn");

    case RealTimeOff:
        return QByteArray("RealTimeOff");

    case SendTemperature:
        return QByteArray("SendTemperature");

    default:
        return QByteArray("None");
    }
}

RemoteClient::Command::Type RemoteClient::Command::stringToType(const QStringRef& string)
{
    if (string == "RealTimeOn")
        return RealTimeOn;
    else if (string == "RealTimeOff")
        return RealTimeOff;
    else if (string == "SendTemperature")
        return SendTemperature;
    else
        return None;
}
