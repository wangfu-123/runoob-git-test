#ifndef SENDDATA_H
#define SENDDATA_H

#include <QObject>
#include <QTcpSocket>

class SendData : public QObject
{
    Q_OBJECT
public:
    explicit SendData(QObject *parent = nullptr);
    bool m_isConnected = false;
    QTcpSocket *m_tcpSocket; //TCP通讯的Socket

public slots:
    void sendMsg(QByteArray arr);
    void connectSocket(qintptr ptr);
    void socketChange(QAbstractSocket::SocketState socketState);
    void socketDisconnect();

signals:
    void Tcp_socketChange(QAbstractSocket::SocketState socketState);
};

#endif // SENDDATA_H
