#ifndef __IO_HANDLER__
#define __IO_HANDLER__

#include "MsgPipe.h"

#include <QThread>
#include <QMutex>
#include <QFile>

class QByteArray;
class QString;
class QStringRef;

class IOHandler : public QThread
{
    Q_OBJECT

public:
    IOHandler(const QByteArray& logPath, QObject* parent = 0);
    ~IOHandler(void);

    //! write string to log file
    IOHandler& operator<<(const QString& string);
    IOHandler& operator<<(const QStringRef& string);
    IOHandler& operator<<(const QByteArray& string);
    IOHandler& operator<<(const char* string);
    IOHandler& operator<<(const unsigned int number);
    IOHandler& operator<<(const int number);
    IOHandler& operator<<(const float number);

    void run(void);

signals:
    void messageReceived(const QByteArray& msg);

private:
    QFile m_log;
    QMutex m_mutex;
    MsgPipe m_pipe;
};

extern IOHandler io;

#endif
