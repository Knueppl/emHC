#ifndef __TEMP_SENSOR__
#define __TEMP_SENSOR__

#include <QByteArray>
#include <QTextStream>

//! base class TempSensor for all temperature sensors
class TempSensor : public QObject
{
    Q_OBJECT

public:
    //! constructor
    explicit TempSensor(const QByteArray& name = QByteArray()) : m_name(name), m_temperature(-100.0), m_max(999.9), m_min(-100.0), m_alert(false) { }
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

    //! return a formatted string containing all data of this sensor
    QByteArray dataString(void) const
    {
        QByteArray string;
        QTextStream stream(&string);

        stream << "TempSensor(" << m_name << ")" << m_temperature << ";";
        stream.flush();
        return string;
    }

    //! prints the name and temperature of the sensor in a stream
    friend QTextStream& operator<<(QTextStream& stream, const TempSensor& sensor)
    {
        stream << "TempSensor " << sensor.m_name << ": " << sensor.m_temperature << "°C\n";
        return stream;
    }

signals:
    void alert(void);

protected:

    //! sensor name
    QByteArray m_name;
    //! temperature value
    float m_temperature;
    //! max temperature
    float m_max;
    //! min temperature
    float m_min;
    //! alert state
    bool m_alert;
};

//QTextStream& operator<<(QTextStream& stream, const TempSensor& sensor)
//{
//    stream << "TempSensor " << sensor.m_name << ": " << sensor.m_temperature << "°C\n";
//    return stream;
//}

#endif
