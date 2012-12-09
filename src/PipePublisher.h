#ifndef __PIPE_PUBLISHER__
#define __PIPE_PUBLISHER__

#include "Pipe.h"

class PipePublisher : public Pipe
{
    Q_OBJECT

public:
    PipePublisher(const QByteArray& keyFile, const unsigned int memSize = Pipe::defaultMemSize(), QObject* parent = 0)
        : Pipe(keyFile, memSize, parent) { }

public slots:
    bool send(const QByteArray& msg);
    bool send(const int command);
};

#endif
