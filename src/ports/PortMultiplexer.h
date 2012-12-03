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

#ifndef __PORT_MULTIPLEXER__
#define __PORT_MULTIPLEXER__

#include <QFile>
#include <QByteArray>

class QString;

//! controls omap multiplexer
class PortMultiplexer
{
public:

    //! enum Flags for setup port
    enum Flags {
        Input       = (1 << 5),
        Pullup      = (1 << 4),
        DisablePull = (1 << 3),
        Mask        = (Input | Pullup | DisablePull)
    };

    //! constructor
    /*!
     * search pinName in all multiplexer in static variable path
     * \param pinName name of pin, usually like gpio1_7
     * \param flags flags for options
     */
    PortMultiplexer(const QByteArray& pinName, const int flags = -1);
    //! destructor
    /*!
     * switch port to an input and disable pullup/pulldown
     */
    ~PortMultiplexer(void);

    //! get flags
    int flags(void) const                       { return m_flags; }
    //! set flags
    void setFlags(const int flags);
    //! get name of multiplexer
    QByteArray name(void) const                 { return m_name; }
    //! check if multiplexer works correctly
    bool works(void) const                      { return m_mode != None && m_file.isOpen(); }

    //! set path to multiplexer directory
    static void setPath(const QByteArray& path) { s_path = path; }
    //! get path to multiplexer directory
    static const QByteArray& path(void)         { return s_path; }

private:

    //! enum Mode
    /*!
     * Mode represents the position of the switcher in multiplexer
     */
    enum Mode {
        None  = -1,
        Mode0 = 0,
        Mode1 = 1,
        Mode2 = 2,
        Mode3 = 3,
        Mode4 = 4,
        Mode5 = 5,
        Mode6 = 6,
        Mode7 = 7
    };

    //! get name of multiplexer
    /*!
     * serch in directory that was set to static variable path
     * \param pinName name of the pin
     */
    QByteArray getMultiplexerName(const QByteArray& pinName);

    //! get mode
    /*!
     * mode is needed for the switcher position
     * \param pinName name of the pin
     */
    Mode getModeOfPort(const QByteArray& pinName);

    //! get switcher position
    /*!
     * search in multiplexer for the right switcher position
     * \param line data of multiplexer read from file
     * \param pinName name of the pin
     */
    int getSwitchPosition(const QString& line, const QString& pinName) const;

    //! name
    QByteArray m_name;
    //! mulitplexer file
    QFile m_file;
    //! flags
    int   m_flags;
    //! mode
    Mode  m_mode;

    //! static variable s_path
    /*!
     * this variable must store the path to the multiplexer directory
     */
    static QByteArray s_path;
};

#endif
