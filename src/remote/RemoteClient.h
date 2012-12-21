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
            SendTemperature,
            Data
        };

        Command(const Type type = None) : m_type(type) { }

        bool isNull(void) const { return m_type == None; }
        Type type(void) const { return m_type; }
        const QByteArray& data(void) const { return m_data; }

        static QByteArray typeToString(const Type type);
        static Type stringToType(const QStringRef& string);

    private:

        Type m_type;
        QByteArray m_data;
    };

    RemoteClient(QObject* parent = 0);

    void sendData(const QByteArray& data);

signals:
    void command(RemoteClient::Command command);

private slots:
    void receive(void);
};

#endif
