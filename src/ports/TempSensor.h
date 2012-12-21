#ifndef __TEMP_SENSOR__
#define __TEMP_SENSOR__

#include <QByteArray>
#include <QTextStream>

//! base class TempSensor for all temperature sensors
class TempSensor
{
public:
    //! constructor
    explicit TempSensor(const QByteArray& name = QByteArray()) : m_name(name), m_temperature(-100.0) { }
    //! virtual destructor
    virtual ~TempSensor(void) { }

    //! must be reimplemented
    /*!
     * grabs sensor. This method should grab the sensor and reads all data and updated the member m_temperature.
     */
    virtual bool grab(void) = 0;

    //! returns the temperature value
    float temperature(void) const { return m_temperature; }
    //! returns the name of the sensor
    const QByteArray& name(void) const { return m_name; }

    //! prints the name and temperature of the sensor in a stream
    friend QTextStream& operator<<(QTextStream& stream, const TempSensor& sensor)
    {
        stream << "TempSensor " << sensor.m_name << ": " << sensor.m_temperature << "°C\n";
        return stream;
    }

protected:

    //! sensor name
    QByteArray m_name;
    //! temperature value
    float m_temperature;
};

//QTextStream& operator<<(QTextStream& stream, const TempSensor& sensor)
//{
//    stream << "TempSensor " << sensor.m_name << ": " << sensor.m_temperature << "°C\n";
//    return stream;
//}

#endif
