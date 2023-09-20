#include "addwidget.h"
#include "ui_addwidget.h"

addWidget::addWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::addWidget)
{
    ui->setupUi(this);
    this->setWindowModality(Qt::ApplicationModal);      //阻止其他页面响应
    ui->lineEdit_tcp_address->setText(getLocalIP());
}

addWidget::~addWidget()
{
    delete ui;
}

QString addWidget::getLocalIP()
{
    //获取本机IPv4地址
    QString hostName = QHostInfo::localHostName(); //本地主机名
    QHostInfo hostInfo = QHostInfo::fromName(hostName);
    QString localIP = "";
    QList<QHostAddress> addList = hostInfo.addresses();

    if(!addList.isEmpty())
    {
        for(int i = 0; i < addList.count(); i++)
        {
            QHostAddress aHost = addList.at(i);
            if(QAbstractSocket::IPv4Protocol == aHost.protocol())
            {
                localIP = aHost.toString();
                break;
            }
        }
    }
    return localIP;
}

QString addWidget::getIndexText(int index)
{
    return ui->comboBox_system->itemText(index);
}

int addWidget::getIndex(QString IndexText)
{
    return ui->comboBox_system->findText(IndexText);
}

void addWidget::changeWidget(int index)
{
    if(index == TCP)
    {
        this->setWindowTitle("TCP");
    }
    else
    {
        this->setWindowTitle("Add");
    }
    ui->stackedWidget->setCurrentIndex(index);
}

void addWidget::editClear()
{
    switch(ui->stackedWidget->currentIndex())
    {
        case COUNT:
            {
                ui->lineEdit_count_num->clear();
                ui->lineEdit_count_name->clear();
                break;
            }
        case TEMPERATURE:
            {
                ui->lineEdit_temperature_end->clear();
                ui->lineEdit_temperature_name->clear();
                ui->lineEdit_temperature_start->clear();
                ui->lineEdit_temperature_value->clear();
                break;
            }
        case POWER:
            {
                ui->lineEdit_power_end->clear();
                ui->lineEdit_power_name->clear();
                ui->lineEdit_power_start->clear();
                ui->lineEdit_power_value->clear();
                break;
            }
        case LIQUID:
            {
                ui->lineEdit_liquid_name->clear();
                break;
            }
        case LOWCOMPUTER:
            {
                ui->lineEdit_lower_name->clear();
                break;
            }
        default:
            break;
    }
}

void addWidget::addWidgetLanguageChange()
{
    ui->retranslateUi(this);
}

void addWidget::on_pushButton_2_clicked()
{
    bool ok = false;
    QList<QString> list_str;
    switch (ui->stackedWidget->currentIndex())
    {
        case SYSTEM:
            {
                list_str.append(ui->comboBox_system->currentText());
                list_str.append(ui->comboBox_system_state->currentText());
                break;
            }
        case COUNT:
            {
                ok = ui->lineEdit_count_name->text().isEmpty() && ui->lineEdit_count_num->text().isEmpty();
                list_str.append(ui->lineEdit_count_name->text());
                list_str.append(ui->lineEdit_count_num->text());
                break;
            }
        case TEMPERATURE:
            {
                ok = ui->lineEdit_temperature_name->text().isEmpty() && ui->lineEdit_temperature_value->text().isEmpty() &&
                     ui->lineEdit_temperature_start->text().isEmpty() && ui->lineEdit_temperature_end->text().isEmpty();
                list_str.append(ui->lineEdit_temperature_name->text());
                list_str.append(ui->lineEdit_temperature_value->text());
                list_str.append(ui->lineEdit_temperature_start->text());
                list_str.append(ui->lineEdit_temperature_end->text());
                break;
            }
        case POWER:
            {
                ok = ui->lineEdit_power_name->text().isEmpty() && ui->lineEdit_power_value->text().isEmpty() &&
                     ui->lineEdit_power_start->text().isEmpty() && ui->lineEdit_power_end->text().isEmpty();
                list_str.append(ui->lineEdit_power_name->text());
                list_str.append(ui->lineEdit_power_value->text());
                list_str.append(ui->lineEdit_power_start->text());
                list_str.append(ui->lineEdit_power_end->text());
                break;
            }
        case LIQUID:
            {
                ok = ui->lineEdit_liquid_name->text().isEmpty();
                list_str.append(ui->lineEdit_liquid_name->text());
                list_str.append(ui->comboBox_liquid_state->currentText());
                break;
            }
        case REACTION:
            {
                list_str.append(ui->comboBox_reaction_vessel->currentText());
                list_str.append(ui->spinBox_reaction_vessel->text());
                if(ui->checkBox_reactio_vessel->isChecked())
                {
                    list_str.append("OK");
                }
                else
                {
                    list_str.append("NOT OK");
                }
                break;
            }
        case LOWCOMPUTER:
            {
                ok = ui->lineEdit_lower_name->text().isEmpty();
                list_str.append(ui->lineEdit_lower_name->text());
                list_str.append(ui->comboBox_lower_state->currentText());
                break;
            }
        case TCP:
            {
                ok = ui->lineEdit_tcp_address->text().isEmpty();
                list_str.append(ui->lineEdit_tcp_address->text());
                list_str.append(ui->spinBox_port->text());
                list_str.append(QString("%1").arg(ui->comboBox_format->currentIndex()));
                break;
            }
        default:
            break;
    }
    if(ok)
    {
        QMessageBox::information(this, tr("Tips"), tr("Please fill in the information completely！"), QMessageBox::Ok);
    }
    else
    {
        editClear();        //清理输入框
        emit addData(list_str);
    }
}

void addWidget::on_pushButton_clicked()
{
    QList<QString> list_str;
    emit addData(list_str);
}

void addWidget::on_checkBox_reactio_vessel_stateChanged(int arg1)
{
    switch (arg1)
    {
        case 0:
            ui->checkBox_reactio_vessel->setText("NOT OK");
            break;
        case 2:
            ui->checkBox_reactio_vessel->setText("OK");
            break;
    }
}
