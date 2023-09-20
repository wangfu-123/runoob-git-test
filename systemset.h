#ifndef SYSTEMSET_H
#define SYSTEMSET_H

#include <QWidget>
#include <QSqlDatabase>
#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <addwidget.h>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QTextCodec>
#include <QTranslator>
#include <QDir>
#include <myitemydelegate.h>
#include <QSqlTableModel>
#include <mytablemodel.h>
#include <QSqlRecord>
#include <qcustomplot.h>

namespace Ui
{
    class SystemSet;
}

class SystemSet : public QWidget
{
    Q_OBJECT

public:
    explicit SystemSet(QWidget *parent = nullptr);
    ~SystemSet();

    QSqlDatabase m_db;
    QTranslator *m_translator = new QTranslator;
    addWidget *m_addwidget = new addWidget;
    MyItemyDelegate *m_myDelegate = new MyItemyDelegate;
    MyTableModel *m_mytable_model;
    QTcpServer *m_tcpServer;           //TCP服务器
    QCPItemTracer *m_tracer;           //图标游标
    const int DEFAULT_ROW_COUNT = 20;  //默认行数

    QAbstractSocket::SocketState m_socketState = QAbstractSocket::UnconnectedState;
    int m_dataFormat;
    QList<int> m_systemIndexList;
    enum PageName
    {
        SYSTEM,
        COUNT,
        TEMPERATURE,
        POWER,
        LIQUID,
        REACTION,
        LOWCOMPUTER,
        TCP
    };

    enum Language
    {
        Chinese,
        English
    };

    void createDataBase();
    void readDataBase();
    void updateStateSystemTable();
    void initCustomPlot();
    void setData(QCustomPlot *plot, double data);
    QByteArray convertFormat(QString);

private slots:
    void on_btn_F8_save_clicked();
    void on_btn_state_add_clicked();
    void on_btn_state_delete_clicked();
    void on_btn_state_save_clicked();
    void addData(QList<QString> list_str);
    void onNewConnection();
    void onSocketStateChange(QAbstractSocket::SocketState socketState);
    void on_btn_F5_clicked();
    void on_comboBox_language_currentIndexChanged(int index);
    void dataChange(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
    void on_btn_graph_add_clicked();
    void mouseMove(QMouseEvent *e);

private:
    Ui::SystemSet *ui;
    QThread *sendDataThread;

signals:
    void changeWidget(int index);
    void connectSocket(qintptr ptr);
    void socketSendData_sign(QByteArray);
    void socketDisconnect();
    void addWidgetLanguageChange();

};

#endif // SYSTEMSET_H
