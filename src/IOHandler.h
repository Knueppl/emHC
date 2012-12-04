#ifndef __IO_HANDLER__
#define __IO_HANDLER__

#include "MsgPipe.h"

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QByteArray>
#include <QStringRef>
#include <QTime>

class QString;

class IOHandler : public QObject
{
    Q_OBJECT

public:
    IOHandler(const QByteArray& logFile = QByteArray(), QObject* parent = 0);
    ~IOHandler(void);

    IOHandler& operator()(void)
    {
        m_out << "\n[" << QTime::currentTime().toString() << "]";
        return *this;
    }

    //! write string to log file
    IOHandler& operator<<(const QString& string) { m_out << string; return *this; }
    IOHandler& operator<<(const QStringRef& string) { m_out << string.string(); return *this; }
    IOHandler& operator<<(const QByteArray& string) { m_out << string; return *this; }
    IOHandler& operator<<(const char* string) { m_out << string; return *this; }
    IOHandler& operator<<(const unsigned int number) { m_out << number; return *this; }
    IOHandler& operator<<(const int number) { m_out << number; return *this; }
    IOHandler& operator<<(const float number) { m_out << number; return *this; }
    IOHandler& operator<<(const void* ptr) { m_out << ptr; return *this; }

signals:
    void messageReceived(const QByteArray& msg);

private:
    QFile m_log;
    QTextStream m_out;
//    MsgPipe m_pipe;
};

extern IOHandler io;

#endif
