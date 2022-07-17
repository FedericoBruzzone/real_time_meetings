#ifndef MYMIC_H
#define MYMIC_H

#include <QThread>
#include <QTcpSocket>
#include <QDebug>
#include <QAudioInput>
#include <QDataStream>
#include <QObject>
#include <QIODevice>

class MyMic: public QObject
{
    Q_OBJECT
public:
    explicit MyMic( int ID);
    void run();

signals:
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void sendData();
    void disconnected();
    void onfinish();

private:
    QTcpSocket *socket;
    QAudioInput *mic;
    QIODevice *dev;
    qintptr socketDescriptor;
};
#endif // MYMIC_H

