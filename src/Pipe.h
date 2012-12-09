#ifndef __PIPE__
#define __PIPE__

#include <QObject>
#include <QTimer>

class QByteArray;

class Pipe : public QObject
{
    Q_OBJECT

public:

    enum Type {
        None = 0,
        Text,
        Command
    };

    explicit Pipe(const QByteArray& keyFile, const unsigned int memSize = s_defaultMemSize, QObject* parent = 0);
    ~Pipe(void);

    bool isNull(void) const { return m_id < 0; }

    unsigned int memorySize(void) const { return m_memSize; }
    static unsigned int defaultMemSize(void) { return s_defaultMemSize; }
    static void setDefaultMemSize(const unsigned int size) { s_defaultMemSize = size; }

protected:
    int m_id;
    void* m_data;
    unsigned int m_memSize;

    static unsigned int s_defaultMemSize;
    static QTimer s_timer;
};

#endif
