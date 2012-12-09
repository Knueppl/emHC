#include "IOHandler.h"

namespace {
#include "Messages.h"

const int MAX_LOG_SIZE = 4096;
}

#include <QDebug>
#include <QString>

IOHandler io("/var/dryingC/default.log");


IOHandler::IOHandler(const QByteArray& logFile, QObject* parent)
    : QObject(parent),
      m_log(logFile)
{
    if (!m_log.open(QIODevice::WriteOnly))
    {
        if (!m_log.open(stdout, QIODevice::WriteOnly))
            throw "IOHandler: can't open log file and stdout. I don't know what i have to do.";
    }

    m_out.setDevice(&m_log);
}

IOHandler::~IOHandler(void)
{
    m_log.close();
}
