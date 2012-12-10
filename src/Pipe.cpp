#include "Pipe.h"
#include "IOHandler.h"

#include <QByteArray>
#include <QDebug>
#include <QFile>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstdio>

unsigned int Pipe::s_defaultMemSize = 4096;
QTimer Pipe::s_timer;

#define MSG(x) (io() << "Pipe (" << this << "): " << x << ".")

Pipe::Pipe(const QByteArray& keyFile, const unsigned int memSize, QObject* parent)
    : QObject(parent),
      m_id(-1),
      m_data(0),
      m_memSize(0)
{
    if ((m_id = ::msgget(ftok(keyFile.data(), 1), (IPC_CREAT | IPC_EXCL | 0666))) == -1)
    {
        if ((m_id = ::msgget(ftok(keyFile.data(), 1), (IPC_EXCL | 0666))) == -1)
        {
            perror("pipe");
            MSG("generate message queue fails");
            return;
        }
    }

    if (!(m_data = new char[memSize]))
    {
        MSG("can't allocate memory");
        m_id = -1;
        return;
    }

    m_memSize = memSize;
}

Pipe::~Pipe(void)
{
    delete static_cast<char*>(m_data);

//    if (m_id > 0)
//        msgctl(m_id, IPC_RMID, NULL);
}

