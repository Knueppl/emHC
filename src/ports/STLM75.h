#ifndef __STLM75__
#define __STLM75__

#include "TempSensor.h"

class QDomNode;

//! a temperature sensor on a i2c bus
class STLM75 : public TempSensor
{
public:
    //! constructor
    STLM75(const unsigned char busID, const unsigned char busAddress, const QByteArray& name = QByteArray());

    //! constructor
    /*!
     * constructs a stlm75 sensor from a xml node
     * \param node xml node stlm75
     */
    STLM75(const QDomNode& node);

    //! destructor
    virtual ~STLM75(void);

    //! overloaded method
    virtual bool grab(void);

private:
    //! opens the device
    bool open(const unsigned char busID, const unsigned char busAddress);

    //! device descriptor
    int m_device;
    //! i2c bus id
    unsigned char m_busID;
    //! i2c bus address
    unsigned char m_busAddress;
};

#endif
