#ifndef __MSG_PIPE__
#define __MSG_PIPE__

#include <QObject>
#include <QTimer>

class QByteArray;

class MsgPipe : public QObject
{
    Q_OBJECT

public:

    enum Type {
        None = 0,
        Text = 1
    };

    explicit MsgPipe(const QByteArray& keyFile, const Type type, const unsigned int bufSize = s_defaultBufSize);
    ~MsgPipe(void);

    bool isNull(void) const { return m_msgID < 0; }

    bool isText(void) const { return m_type == Text; }
    QByteArray text(void) const;

    unsigned int bufferSize(void) const { return m_bufSize; }
    static unsigned int defaultBufferSize(void) { return s_defaultBufSize; }
    static void setDefaultBufferSize(const unsigned int size) { s_defaultBufSize = size; }

public slots:
    bool send(const QByteArray& msg);

signals:
    void messageReceived(const MsgPipe* pipe);

private slots:
    void checkQueue(void);

private:
    bool allocBuffer(const Type type, const unsigned int size);
    void freeBuffer(void);

    int m_msgID;
    Type m_type;
    void* m_data;
    unsigned int m_bufSize;
    unsigned int m_memSize;

    static unsigned int s_defaultBufSize;
    static QTimer s_timer;
};

#endif
