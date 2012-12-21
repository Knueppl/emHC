#ifndef __REMOTE_CLIENT__
#define __REMOTE_CLIENT__

#include <QSslSocket>
#include <QByteArray>

class QStringRef;

class RemoteClient : public QSslSocket
{
    Q_OBJECT

public:

    class Command
    {
    public:

        enum Type {
            None,
            RealTimeOn,
            RealTimeOff,
            SendTemperature
        };

        Command(const Type type = None) : m_type(type) { }

        bool isNull(void) const { return m_type == None; }
        Type type(void) const { return m_type; }

        static QByteArray typeToString(const Type type);
        static Type stringToType(const QStringRef& string);

    private:

        Type m_type;
    };

    RemoteClient(QObject* parent = 0);

signals:
    void command(RemoteClient::Command command);

private slots:
    void receive(void);
};

#endif
