#ifndef __ALERT_HANDLER__
#define __ALERT_HANDLER__

#include "Modem.h"

#include <QObject>
#include <QByteArray>
#include <QVector>

class QDomNode;

class AlertHandler : public QObject
{
    Q_OBJECT

public:
    AlertHandler(const QDomNode& node, QObject* parent = 0);
    ~AlertHandler(void);

public slots:
    void startAlertRoutine(void);

private slots:
    void finishedCall(const Modem::State state);

private:
    void addPhoneNumber(const QDomNode& node);

    Modem* m_modem;
    QVector<QByteArray> m_phones;
    QVector<QByteArray>::iterator m_calling;
};

#endif
