#ifndef MYSERVER_H
#define MYSERVER_H

#include <QtNetwork>
#include <QTcpServer>

#include <QtMultimedia/QAudioFormat>
#include <QtMultimedia/QAudioInput>
#include <QtMultimedia/QAudioDeviceInfo>
#include "mymic.h"

class MyServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit MyServer();
    void startServer();

signals:

public slots:

protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    QAudioInput *audioInput;
};

#endif // MYSERVER_H

