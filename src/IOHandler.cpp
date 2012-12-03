#include "IOHandler.h"

namespace {
#include "Messages.h"

const int MAX_LOG_SIZE = 4096;
}

#include <QDebug>
#include <iostream>
#include <string>


IOHandler io;


IOHandler::IOHandler(QObject* parent)
    : QThread(parent)
{
    std::cout  << HELLO_MESSAGES;
}

void IOHandler::run(void)
{
    while (1)
    {
        std::cout  << MENU_MESSAGES;
        std::string in;
        std::cin >> in;

        if (in == SHOW_LOG)
        {
            m_mutex.lock();
            qDebug() << "Logeinträge:";
            qDebug() << "---------------------------------------------------------";
            qDebug() << m_log;
            m_mutex.unlock();
        }
        else if (in == SHOW_TEMP)
        {

        }
        else if (in == CLOSE)
        {
            return;
        }
    }
}

IOHandler& IOHandler::operator<<(const QString& string)
{
    if (m_log.size() >= MAX_LOG_SIZE)
        return *this;

    m_mutex.lock();
    m_log.append(string);
    m_mutex.unlock();

    return *this;
}

IOHandler& IOHandler::operator<<(const QStringRef& string)
{
    if (m_log.size() >= MAX_LOG_SIZE)
        return *this;

    m_mutex.lock();
    m_log.append(string);
    m_mutex.unlock();

    return *this;
}

IOHandler& IOHandler::operator<<(const QByteArray& string)
{
    if (m_log.size() >= MAX_LOG_SIZE)
        return *this;

    m_mutex.lock();
    m_log.append(string);
    m_mutex.unlock();

    return *this;
}

IOHandler& IOHandler::operator<<(const char* string)
{
    if (m_log.size() >= MAX_LOG_SIZE)
        return *this;

    m_mutex.lock();
    m_log.append(string);
    m_mutex.unlock();

    return *this;
}

IOHandler& IOHandler::operator<<(const unsigned int number)
{
    if (m_log.size() >= MAX_LOG_SIZE)
        return *this;

    m_mutex.lock();
    m_log.append(QString::number(number));
    m_mutex.unlock();

    return *this;
}

IOHandler& IOHandler::operator<<(const int number)
{
    if (m_log.size() >= MAX_LOG_SIZE)
        return *this;

    m_mutex.lock();
    m_log.append(QString::number(number));
    m_mutex.unlock();

    return *this;
}

IOHandler& IOHandler::operator<<(const float number)
{
    if (m_log.size() >= MAX_LOG_SIZE)
        return *this;

    m_mutex.lock();
    m_log.append(QString::number(number));
    m_mutex.unlock();

    return *this;
}
