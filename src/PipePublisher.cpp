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
        MSG("msg is to long");
        return false;
    }

    *reinterpret_cast<long*>(m_data) = Text;
    ::strcpy(static_cast<char*>(m_data) + sizeof(long), msg.data());

    if (::msgsnd(m_id, m_data, m_memSize, IPC_NOWAIT) < 0)
    {
        MSG("send message fails");
        return false;
    }

    return true;
}
