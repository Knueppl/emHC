#ifndef __IO_HANDLER__
#define __IO_HANDLER__

#include <QThread>
#include <QMutex>

class IOHandler : public QThread
{
public:
    IOHandler(QObject* parent = 0);

    /* Fühgt string zum Logspeicher hinzu */
    IOHandler& operator<<(const QString& string);
    IOHandler& operator<<(const QStringRef& string);
    IOHandler& operator<<(const QByteArray& string);
    IOHandler& operator<<(const char* string);
    IOHandler& operator<<(const unsigned int number);
    IOHandler& operator<<(const int number);
    IOHandler& operator<<(const float number);

    void run(void);

private:
    QString m_log;
    QMutex m_mutex;
};

extern IOHandler io;

#endif
