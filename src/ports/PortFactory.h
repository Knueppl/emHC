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

#ifndef __PORT_FACTORY__
#define __PORT_FACTORY__

#include "Port.h"

#include <QProcess>

class QByteArray;
class QDomNode;

//! class PortFactory
/*!
 * builds Ports and starts the port timer and sets the path for ports and multiplexers
 */
class PortFactory
{
public:
    //! constructor
    /*!
     * set multiplexer and port path to the directories. starts the static port timer
     * \param multiplexerPath path to directory for omap multiplexer
     * \param portPath path to directory for gpio
     */
    PortFactory(const QByteArray& multiplexerPath, const QByteArray& portPath);

    //! destructor
    ~PortFactory(void);

    //! build new port from xml node
    Port* build(const QDomNode& node);

    //! build new port
    /*!
     * overloaded method build()
     */
    Port* build(const unsigned int portNumber, const QByteArray& portName, const QByteArray& pinName,
                const int flags = PortMultiplexer::Input | PortMultiplexer::Pullup);
};

#endif
