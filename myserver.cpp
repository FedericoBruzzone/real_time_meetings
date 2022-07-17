#include "myserver.h"

MyServer::MyServer()
{
}

void MyServer::startServer()
{
    int port = 12345;

    QString ipAddress = "161.97.158.57";

    if(!this->listen( QHostAddress(ipAddress),port ) )
    {
        qDebug() << "Could not start server";
    }
    else
    {
        qDebug() << "The server is running on\n\nIP: "<< ipAddress;
        qDebug() << "\nport: " << this->serverPort() << "\n\n";
    }
}

void MyServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << socketDescriptor << " Connecting...";

    MyMic *myMic = new MyMic(socketDescriptor);
    myMic->run();
}
