#include "PipeSubscriber.h"
#include "IOHandler.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG(x) (io() << "PipeSubscriber (" << this << "): " << x << ".")

PipeSubscriber::PipeSubscriber(const QByteArray& keyFile, const unsigned int memSize, QObject* parent)
    : Pipe(keyFile, memSize, parent),
      m_type(None)
{
    this->connect(&s_timer, SIGNAL(timeout()), this, SLOT(checkQueue()));
    s_timer.start(100);
}

void PipeSubscriber::checkQueue(void)
{
    if (::msgrcv(m_id, m_data, m_memSize, 0, IPC_NOWAIT) < 0)
        return;

    const long type(*reinterpret_cast<long*>(m_data));

    switch (type)
    {
    case Text:
        m_type = Text;
        break;

    default:
        m_type = None;
        return;
    }

    emit this->messageReceived(this);
}

QByteArray PipeSubscriber::text(void) const
{
    if (!m_data || m_type != Text)
        return QByteArray();

    return QByteArray(static_cast<char*>(m_data) + sizeof(long));
}
