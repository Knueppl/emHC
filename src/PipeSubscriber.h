#ifndef __PIPE_SUBSCRIBER__
#define __PIPE_SUBSCRIBER__

#include "Pipe.h"

class PipeSubscriber : public Pipe
{
    Q_OBJECT

public:
    explicit PipeSubscriber(const QByteArray& keyFile, const unsigned int memSize = s_defaultMemSize, QObject* parent = 0);

    bool isText(void) const { return m_type == Text; }
    QByteArray text(void) const;

    bool isCommand(void) const { return m_type == Command; }
    int command(void) const;

signals:
    void messageReceived(const PipeSubscriber* pipe);

private slots:
    void checkQueue(void);

private:
    Type m_type;
};

#endif
