#include "senddata.h"

SendData::SendData(QObject *parent) : QObject (parent)
{
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    qRegisterMetaType<qintptr>("qintptr");
}

void SendData::sendMsg(QByteArray arr)
{
    m_tcpSocket->write(arr);
}

void SendData::connectSocket(qintptr ptr)
{
    m_tcpSocket = new QTcpSocket();
    m_tcpSocket->setSocketDescriptor(ptr);
    m_isConnected = true;
    connect(m_tcpSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(socketChange(QAbstractSocket::SocketState)));
    socketChange(m_tcpSocket->state());
}

void SendData::socketChange(QAbstractSocket::SocketState socketState)
{
    if(socketState == QAbstractSocket::UnconnectedState)
    {
        m_tcpSocket->deleteLater();
    }
    emit Tcp_socketChange(socketState);
}

void SendData::socketDisconnect()
{
    if(m_isConnected)
    {
        m_tcpSocket->disconnectFromHost();
        m_isConnected = false;
        m_tcpSocket->deleteLater();
    }
}


