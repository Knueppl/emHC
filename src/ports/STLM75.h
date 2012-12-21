#ifndef __STLM75__
#define __STLM75__

#include "TempSensor.h"

class QDomNode;

class STLM75 : public TempSensor
{
public:
    STLM75(const unsigned char busID, const unsigned char busAddress, const QByteArray& name = QByteArray());
    STLM75(const QDomNode& node);
    virtual ~STLM75(void);

    virtual bool grab(void);

private:
    bool open(const unsigned char busID, const unsigned char busAddress);

    int m_device;
    unsigned char m_busID;
    unsigned char m_busAddress;
};

#endif
