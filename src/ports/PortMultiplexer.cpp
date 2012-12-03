#include "PortMultiplexer.h"
#include "IOHandler.h"

#include <QProcess>
#include <QString>
#include <QDomNode>
#include <QDebug>

/*******************************************************/
/* Flags und Mode                                      */
/* ---------------------------------------------       */
/* Bit 5: 1 - Input, 0 - Output                        */
/* Bit 4: 1 - Pull up, 0 - Pull down                   */
/* Bit 3: 1 - Pull disabled, 0 - Pull enabled          */
/* Bit 2 \                                             */
/* Bit 1 |- Mode                                       */
/* Bit 0 /                                             */
/*******************************************************/

/* holds the path to muliplexer directory */
QByteArray PortMultiplexer::s_path;


PortMultiplexer::PortMultiplexer(const QByteArray& pinName, const int flags)
    : m_name(PortMultiplexer::getMultiplexerName(pinName)),
      m_file(QByteArray(s_path).append(m_name)),
      m_flags(0),
      m_mode(this->getModeOfPort(pinName))
{
    if (m_mode == None)
    {
        io << "Multiplexer " << m_name << ": Keinen gültigen Mode gefunden.\n";
        return;
    }

    if (!m_file.open(QIODevice::WriteOnly))
    {
        io << "Multiplexer " << m_name << ": kann Datei" << m_file.fileName() << "nicht öffnen.\n";
        return;
    }

    /* if multiplexer file was found and can open set flags */
    this->setFlags(flags);
}

PortMultiplexer::~PortMultiplexer(void)
{
    /* close multiplexer file */
    m_file.close();
}

void PortMultiplexer::setFlags(const int flags)
{
    /* if flags < 0 return, because flags isn't valid */
    if (flags < 0)
        return;

    /* mask flags to get only possible flags */
    m_flags = (flags & Mask);

    /* write flags + mode to multiplexer file */
    if (!m_file.isOpen() || m_file.write(QByteArray::number(m_flags | m_mode, 16)) < 0)
    {
        io << "Multiplexer " << m_name << ": Kann \"flags\" nicht setzen. Datei" << m_file.fileName() << "ist nicht geöffnet.\n";
        return;
    }

    m_file.flush();
}

PortMultiplexer::Mode PortMultiplexer::getModeOfPort(const QByteArray& pinName)
{
    /* search multiplexer for pin with a shell and grep */
    QProcess process;
    process.start("/bin/sh", QStringList() << "-c" << QString("grep ").append(pinName).append(" ").append(s_path).append('*'));

    if(!process.waitForFinished(10000) || process.exitCode())
    {
        io << __PRETTY_FUNCTION__ << " Fehler beim ausführen von grep.\n";
        return None;
    }

    QByteArray buffer(process.readAllStandardOutput());
    const int position = this->getSwitchPosition(buffer, pinName);
    const Mode modes[] = { Mode0, Mode1, Mode2, Mode3, Mode4, Mode5, Mode6, Mode7 };

    if (position < 0 || position > 7)
    {
        io << __PRETTY_FUNCTION__ << " Keine Multiplexerstelle gefunden.\n";
        return None;
    }

    return modes[position];
}

int PortMultiplexer::getSwitchPosition(const QString& line, const QString& pinName) const
{
    /* get only the line including "signals:" */
    QStringList list(line.right(line.size() - line.indexOf("signals:")).split('|'));

    /* it must gives 8 switcher positions */
    if (list.size() != 8)
        return -1;

    /* search the right position and return */
    for (int i = 1; i < list.size(); i++)
        if (list.at(i).contains(pinName))
            return i;

    return -2;
}

QByteArray PortMultiplexer::getMultiplexerName(const QByteArray& pinName)
{
    /* search multiplexer name in static variable s_path for pinName */
    QProcess process;
    process.start("/bin/sh", QStringList() << "-c" << QString("grep ").append(pinName).append(" ").append(s_path).append('*'));

    if(!process.waitForFinished(10000) || process.exitCode())
    {
        io << __PRETTY_FUNCTION__ << " Fehler beim ausführen von grep.\n";
        return QByteArray();
    }

    QStringList lines = QString(process.readAllStandardOutput()).split("\n");

    for (int line = 0; line < lines.size(); line++)
    {
        /* check if exists ':' in line */
        if (!lines[line].contains(":"))
            continue;

        /* get complete path of multiplexer file and remove s_path from it */
        QStringList list = lines[line].split(":");
        return QByteArray(list[0].remove(' ').toUtf8().replace(s_path, ""));
    }

    return QByteArray();
}
