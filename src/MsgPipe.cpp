#include "MsgPipe.h"
#include "IOHandler.h"

#include <QByteArray>
#include <QFile>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

unsigned int MsgPipe::s_defaultBufSize = 4096;
QTimer MsgPipe::s_timer;

#define MSG(x) (io << "MsgPipe (" << this << "): " << x << ".\n")

MsgPipe::MsgPipe(const QByteArray& keyFile, const Type type, const unsigned int bufSize)
    : m_msgID(-1),
      m_type(None),
      m_data(0),
      m_bufSize(0),
      m_memSize(0)
{
    if (!QFile::exists(keyFile))
    {
        io << "MsgPipe (" << this << "): key file \"" << keyFile << "\" does not exists.\n";
        return;
    }


    if ((m_msgID = ::msgget(ftok(keyFile.data(), type), (IPC_CREAT | IPC_EXCL | 0666))) < 0)
    {
        io << "MsgPipe (" << this << "): generate message queue fails.\n";
        return;
    }

    if (!this->allocBuffer(type, bufSize))
    {
        m_msgID = -1;
        return;
    }
}

MsgPipe::~MsgPipe(void)
{
    this->freeBuffer();
}

bool MsgPipe::allocBuffer(const Type type, const unsigned int size)
{
    if (m_data)
        this->freeBuffer();

    switch (type)
    {
    case Text:
        m_memSize = size * sizeof(char);
        break;

    default:
        MSG("can't alloc buffer, because unknown type");
        return false;
    }

    if (!(m_data = new char[m_memSize + sizeof(long)]))
    {
        MSG("can't alloc buffer");
        return false;
    }

    m_type = type;
    m_bufSize = size;
    return true;
}

void MsgPipe::freeBuffer(void)
{
    this->disconnect(&s_timer, SIGNAL(timeout()), this, SLOT(checkQueue()));

    switch (m_type)
    {
    case Text:
        delete [] static_cast<char*>(m_data);
        break;

    default:
        break;
    }

    m_bufSize = 0;
    m_data    = 0;
    m_memSize = 0;
}

bool MsgPipe::send(const QByteArray& msg)
{
    if (m_msgID < 0)
    {
        MSG("pipe is not configured");
        return false;
    }

    /* bad thing but i must copy the data */
    const unsigned int size = msg.size() + 1 + sizeof(long);
    char* buffer = new char[size];

    if (!buffer)
    {
        MSG("can't alloc send buffer");
        return false;
    }

    *reinterpret_cast<long*>(buffer) = Text;
    const char* src;
    char* des;

    for (des = buffer + sizeof(long), src = msg.data(); des < buffer + size; des++, src++)
        *des = *src;

    if (::msgsnd(m_msgID, buffer, size, IPC_NOWAIT) < 0)
    {
        MSG("send message fails");
        delete buffer;
        return false;
    }

    delete buffer;
    return true;
}

void MsgPipe::checkQueue(void)
{
    if (::msgrcv(m_msgID, m_data, m_memSize, m_type, IPC_NOWAIT) < 0)
    {
        MSG("read message from queue fails");
        return;
    }

    emit this->messageReceived(this);
}

QByteArray MsgPipe::text(void) const
{
    if (!m_data)
        return QByteArray();

    return QByteArray(static_cast<char*>(m_data));
}
