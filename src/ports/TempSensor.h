#ifndef __TEMP_SENSOR__
#define __TEMP_SENSOR__

#include <QByteArray>

class TempSensor
{
public:
    TempSensor(const QByteArray& name = QByteArray()) : m_name(name) { }
    virtual ~TempSensor(void) { }

    const QByteArray& name(void) const { return m_name; }

    virtual float temperature(void) const = 0;
    virtual QByteArray data(void) const = 0;

protected:
    QByteArray m_name;
};

#endif
