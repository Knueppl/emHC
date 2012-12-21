#ifndef __STLM75__
#define __STLM75__

#include "TempSensor.h"

class QDomNode;

class STLM75 : public TempSensor
{
public:
    STLM75(const unsigned char busID, const unsigned char busAddress, const QByteArray& name = QByteArray());
    STLM75(const QDomNode& node);
    ~STLM75(void);

    float temperature(void) const { return this->readSensor(); }
    QByteArray data(void) const;

private:
    bool open(const unsigned char busID, const unsigned char busAddress);
    float readSensor(void) const;

    int m_device;
    unsigned char m_busID;
    unsigned char m_busAddress;
};

#endif
