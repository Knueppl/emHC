#ifndef __TEMP_SENSOR__
#define __TEMP_SENSOR__

#include <QByteArray>
#include <QTextStream>

class TempSensor
{
public:
    TempSensor(const QByteArray& name = QByteArray()) : m_name(name), m_temperature(-100.0) { }
    virtual ~TempSensor(void) { }

    virtual bool grab(void) = 0;

    float temperature(void) const { return m_temperature; }
    const QByteArray& name(void) const { return m_name; }

    friend QTextStream& operator<<(QTextStream& stream, const TempSensor& sensor);

protected:
    QByteArray m_name;
    float m_temperature;
};

QTextStream& operator<<(QTextStream& stream, const TempSensor& sensor)
{
    stream << "TempSensor " << sensor.m_name << ": " << sensor.m_temperature << "°C\n";
    return stream;
}

#endif
