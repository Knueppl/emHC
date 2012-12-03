/*
 *
 * Copyright (C) 2012 knueppl
 *
 * Author: Christian Merkl
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef __MODEM__
#define __MODEM__

#include <QObject>
#include <QByteArray>
#include <QTime>
#include <QTimer>

class QDomNode;

//! class Modem
/*!
 * controls a analog modem via serial port to call a phone.
 */
class Modem : public QObject
{
    Q_OBJECT

public:
    //! enum State
    enum State {
        Close,
        Open,
        Calling,
        CallRejected,
        CallAccepted,
        NoCarrier
    };

    //! default constructor
    /*!
     * open and configure serial port to control a modem.
     * \param device name of the device (/dev/tty...)
     */
    Modem(const QByteArray& device = QByteArray());

    //! constructs a modem from a xml node
    Modem(const QDomNode& node);

    //! destructor
    /*!
     * close serial port.
     */
    ~Modem(void);

    //! calls a phone by number
    /*!
     * tries not longer than timeout
     * \param phoneNumber calls phone number
     * \param timeout timeout value
     */
    bool call(const QByteArray& phoneNumber, const unsigned int timeout = 0);

    //! reconfigure m_device
    /*!
     * you can reconfigure the modem device for example when it crashed.
     */
    void reconfigure(void) { this->configureDevice(m_deviceName); }

signals:
    //! signal stopped
    /*!
     * will be emited when call was finished or failed
     * \param state modem state after calling
     */
    void stopped(const Modem::State state);

private slots:
    //! reads character from device
    void readFromDevice(void);

private:
    //! configures device
    /*!
     * opens the serial port that controls the modem.
     * \param device device name of the serial port (/dev/tty...)
     */
    void configureDevice(const QByteArray& device);

    //! close device
    void closeDevice(void);

    //! reads until no more bytes are available
    void clearDeviceBuffer(void);

    int          m_device;
    QByteArray   m_deviceName;
    State        m_state;
    unsigned int m_timeout;
    QTime        m_timestamp;
    QTimer       m_timer;
    QByteArray   m_buffer;
};

#endif
