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

#ifndef __PORT__
#define __PORT__

#include "PortMultiplexer.h"

#include <QObject>
#include <QByteArray>
#include <QFile>
#include <QTimer>

class QDomNode;

//! class Port
/*!
 * this class represents a gpio digital port. Port will configure the omap multiplexer to support the right functionality
 */
class Port : public QObject
{
    Q_OBJECT

public:

    //! emum Type
    enum Type {
        Input,
        Output
    };

    //! constructor
    /*!
     * construct and configure gpio port
     * \param portNumber number of port, for example gpio1_6 = 32 * 1 + 6 = 38
     * \param portName name of port. give your own name
     * \param pinName name of pin, usually like gpio1_6
     * \param flags set options of port
     * \param parent QObject parent
     */
    Port(const unsigned int portNumber, const QByteArray& portName, const QByteArray& pinName,
         const int flags = PortMultiplexer::Input | PortMultiplexer::Pullup, QObject* parent = 0);

    //! constructor
    /*!
     * construct and configure gpio port from a xml node
     * \param node xml node <port>
     * \param parent QObject parent
     */
    Port(const QDomNode& node, QObject* parent = 0);

    //! destructor
    /*!
     * unexport port directory and set multiplexer to input and disable pullup/pulldown
     */
    ~Port(void);

    //! get flags
    int flags(void) const                 { return m_multiplexer.flags(); }

    //! check if Port works correctly
    bool works(void) const                { return m_multiplexer.works() && m_directionFile.isOpen() && m_valueFile.isOpen(); }

    //! get number of port
    unsigned int portNumber(void) const   { return m_portNumber; }

    //! get name of port
    const QByteArray portName(void) const { return m_portName; }

    //! get current value
    bool value(void) const                { return m_value; }

    //! get if pullup/pulldown is disabled
    bool pullDisabled(void) const         { return m_multiplexer.flags() & PortMultiplexer::DisablePull; }

    //! disbale pullup/pulldown
    /*!
     * \param disable if true disable otherwise enable
     */
    void disablePull(const bool disable = true);

    //! enable pullup/pulldown
    /*!
      \param enable if ture enable pullup otherwise enable pulldown
    */
    void enablePullup(const bool enable = true);

    //! get if pullup is enabled
    /*!
     * \return true if pullup is enabled otherwide false if pulldown is enabled
     */
    bool pullupEnabled(void) const        { return m_multiplexer.flags() & PortMultiplexer::Pullup; }

    //! set type
    /*!
     * configure port as input or output
     * \param type input or output
     */
    void setType(const Type type);

    //! get type of port
    /*!
     * \return type of port
     */
    Type type(void) const                 { if (m_multiplexer.flags() & PortMultiplexer::Input) return Input; return Output; }

    //! set holdtime for positive edge
    void setHoldTimeHigh(const unsigned int ticks);

    //! get holdtime for positive edge
    unsigned int holdTimeHigh(void) const { return m_holdTimeHigh; }

    //! set holdtime for negative edge
    void setHoldTimeLow(const unsigned int ticks);

    //! get holdtime for negative edge
    unsigned int holdTimeLow(void) const  { return m_holdTimeLow; }

    //! set the path to the gpio directory
    static void setPath(const QByteArray& path) { s_path = path; }

    //! get the path to the gpio directory
    static QByteArray& path(void)         { return s_path; }

    //! start the port timer
    /*!
     * \param intervall intervall of one tick
     */
    static void startTimer(const unsigned int intervall) { s_timer.start(intervall); }

    //! stop the port timer
    static void stopTimer(void)           { s_timer.stop(); }

public slots:
    //! set value of port
    /*!
     * if port is from type input this function hasn't no effect
     * \param value set port value to given
     */
    void setValue(const bool value);

private slots:
    //! time tick
    /*!
     * this function will called every timeout of port timer
     */
    void tick(void);

signals:
    //! signal value changed
    /*!
     * will be emitted when port value changed
     * \param value the current value of port
     */
    void valueChanged(bool value);

private:
    //! init method
    /*!
     * configure port
     */
    bool init(void);

    //! get flags from xml node
    int getFlagsFromNode(const QDomNode& node);

    //! get port name from xml node
    QByteArray getPortNameFromNode(const QDomNode& node);

    //! get pin name from xml node
    QByteArray getPinNameFromNode(const QDomNode& node);

    //! get port number from xml node
    unsigned int getPortNumberFromNode(const QDomNode& node);

    //! get holdtime for positive edge from xml node
    unsigned int getHoldTimeHighFromNode(const QDomNode& node);

    //! get holdtime for negative edge from xml node
    unsigned int getHoldTimeLowFromNode(const QDomNode& node);

    //! port number
    unsigned int m_portNumber;
    //! port name
    QByteArray m_portName;
    //! pin name
    QByteArray m_pinName;
    //! direction file from gpio
    QFile m_directionFile;
    //! value file from gpio
    QFile m_valueFile;
    //! value
    bool m_value;
    //! holdtime for positive edge
    unsigned int m_holdTimeHigh;
    //! holdtime for negative edge
    unsigned int m_holdTimeLow;
    //! timer ticks counter
    unsigned int m_tickCounter;
    //! omap multiplexer
    PortMultiplexer m_multiplexer;

    //! holds the path to gpio directory
    static QByteArray s_path;
    //! port timer
    static QTimer s_timer;
};

#endif
