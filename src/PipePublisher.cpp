#include "PipePublisher.h"
#include "IOHandler.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>

#define MSG(x) (io() << "PipePublisher (" << this << ") : " << x << ".")

bool PipePublisher::send(const QByteArray& msg)
{
    if (m_id < 0)
    {
        MSG("pipe is not configured");
        return false;
    }

    if (msg.size() + 1 > static_cast<int>(m_memSize - sizeof(long)))
    {
        MSG("not enough memory. Msg is to long");
        return false;
    }

    *reinterpret_cast<long*>(m_data) = Text;
    ::strcpy(static_cast<char*>(m_data) + sizeof(long), msg.data());

    if (::msgsnd(m_id, m_data, msg.size() + 1 + sizeof(long), IPC_NOWAIT) < 0)
    {
        MSG("send message fails");
        return false;
    }

    return true;
}

bool PipePublisher::send(const int command)
{
    if (m_id < 0)
    {
        MSG("pipe is not configured");
        return false;
    }

    if (sizeof(int) > m_memSize - sizeof(long))
    {
        MSG("not enough memory");
        return false;
    }

    *reinterpret_cast<long*>(m_data) = Command;
    *reinterpret_cast<int*>(static_cast<char*>(m_data) + sizeof(long)) = command;

    if (::msgsnd(m_id, m_data, sizeof(long) + sizeof(int), IPC_NOWAIT) < 0)
    {
        MSG("send message fails");
        return false;
    }

    return true;
}
