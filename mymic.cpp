#include "mymic.h"
#include <QCoreApplication>
#include <QBuffer>
MyMic::MyMic( int ID)
{
    this->socketDescriptor = ID;
    this->mic = nullptr;
    this->dev = nullptr;
}

void MyMic::run()
{
    qDebug() << " MyMic started";

    socket = new QTcpSocket();

    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        emit error(socket->error());
        return;
    }

    QAudioFormat format;

    format.setSampleRate(8000);
    //format.setSampleRate(20000);
    format.setChannelCount(1);
    format.setSampleSize(8);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    //format.setSampleType(QAudioFormat::SignedInt);
    format.setSampleType(QAudioFormat::UnSignedInt);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format))
    {
        qWarning()<<"default format not supported try to use nearest";
        format = info.nearestFormat(format);
    }

    this->mic = new QAudioInput(format, socket);
    this->mic->setBufferSize(8000 * 8);

    QBuffer mInputBuffer;
    mInputBuffer.open(QBuffer::ReadWrite);

    connect(this->dev = mic->start(), SIGNAL( readyRead() ), this, SLOT( sendData() ) , Qt::DirectConnection);

    qDebug() << socketDescriptor << " Microphone connected";
}

void MyMic::onfinish(){
    qDebug()<<"thread is finished!";
}

void MyMic::sendData()
{
    QByteArray Data = dev->readAll();

    socket->write(Data, Data.length());
    socket->waitForBytesWritten();
}

void MyMic::disconnected( )
{
    qDebug() << socketDescriptor << "Mic Disconnected";

    mic->stop();
    delete mic;

    socket->deleteLater();
    exit(0);
}
