#ifndef ADDWIDGET_H
#define ADDWIDGET_H

#include <QWidget>
#include <QMessageBox>
#include <QTcpServer>
#include <QHostInfo>

namespace Ui
{
    class addWidget;
}

class addWidget : public QWidget
{
    Q_OBJECT

public:
    explicit addWidget(QWidget *parent = nullptr);
    ~addWidget();
    QString getLocalIP();//获取本机IP地址
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

    QString getIndexText(int index);
    int getIndex(QString IndexText);

public slots:
    void changeWidget(int index);
    void editClear();
    void addWidgetLanguageChange();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

signals:
    void addData(QList<QString> list_str);

private slots:
    void on_checkBox_reactio_vessel_stateChanged(int arg1);

private:
    Ui::addWidget *ui;
};

#endif // ADDWIDGET_H
