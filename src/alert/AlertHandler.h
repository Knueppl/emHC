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

#ifndef __ALERT_HANDLER__
#define __ALERT_HANDLER__

#include "Modem.h"

#include <QObject>
#include <QByteArray>
#include <QVector>
#include <QTextStream>

class QDomNode;

//! class AlertHandler
/*!
 * calls phone by phone untill one of them rejects the call.
 */
class AlertHandler : public QObject
{
    Q_OBJECT

public:
    //! constructor
    /*!
     * \param node xml node with necessary parameters
     * \param parent QObject parent
     */
    AlertHandler(const QDomNode& node, QObject* parent = 0);

    //! destructor
    ~AlertHandler(void);

    //! friend function to print state to stream
    friend QTextStream& operator<<(QTextStream& stream, const AlertHandler& handler);

    //! friend function to print state in a cstring
    friend QByteArray& operator<<(QByteArray& cstring, const AlertHandler& handler);

public slots:
    //! starts to calling people
    /*!
     * starts with the first phone number in the vector m_phones. If nobody takes the call it will be tried next one in the vector.
     */
    void startAlertRoutine(void);

private slots:
    //! starts next phone call
    /*!
     * this method will be called by Modem class when the call was finished.
     * \param state return state of the call
     */
    void finishedCall(const Modem::State state);

private:
    //! add a phone number to internal vector
    /*!
     * reads phone number from xml node and add it to the phone number vector.
     * \param node xml node "phone"
     */
    void addPhoneNumber(const QDomNode& node);

    Modem*                        m_modem;
    QVector<QByteArray>           m_phones;
    QVector<QByteArray>::iterator m_calling;
};

#endif
