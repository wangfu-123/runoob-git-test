#include "systemset.h"
#include "ui_systemset.h"
#include "senddata.h"

SystemSet::SystemSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemSet)
{
    ui->setupUi(this);
    createDataBase();
    sendDataThread = new QThread();
    m_tcpServer = new QTcpServer();
    SendData *socketSend = new SendData();
    socketSend->moveToThread(sendDataThread);
    connect(this, SIGNAL(socketSendData_sign(QByteArray)), socketSend, SLOT(sendMsg(QByteArray)));
    connect(this, SIGNAL(connectSocket(qintptr)), socketSend, SLOT(connectSocket(qintptr)));
    connect(this, SIGNAL(socketDisconnect()), socketSend, SLOT(socketDisconnect()));
    connect(socketSend, SIGNAL(Tcp_socketChange(QAbstractSocket::SocketState)), SLOT(onSocketStateChange(QAbstractSocket::SocketState)));
    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    connect(sendDataThread, &QThread::finished, socketSend, &QObject::deleteLater);    //线程结束释放工作类
    connect(sendDataThread, &QThread::finished, sendDataThread, &QObject::deleteLater);//线程结束释放线程指针
    connect(this, SIGNAL(changeWidget(int)), m_addwidget, SLOT(changeWidget(int)));
    connect(this, SIGNAL(addWidgetLanguageChange()), m_addwidget, SLOT(addWidgetLanguageChange()));
    connect(m_addwidget, SIGNAL(addData(QList<QString>)), this, SLOT(addData(QList<QString>)));
    connect(ui->plot_graph, SIGNAL(mouseMove(QMouseEvent *)), this, SLOT(mouseMove(QMouseEvent *))); //游标跟随鼠标
    readDataBase();
    initCustomPlot();
    emit ui->comboBox_language->currentIndexChanged(Chinese); //默认中文
    sendDataThread->start();
}

SystemSet::~SystemSet()
{

    m_db.close();
    sendDataThread->quit();
    sendDataThread->wait();
    delete m_tcpServer;
    delete m_translator;
    delete m_addwidget;
    delete m_myDelegate;
    delete m_mytable_model;
    delete ui;
}

void SystemSet::createDataBase()
{
    m_db = QSqlDatabase::addDatabase("QMYSQL", "dataBaseConnect");
    m_db.setHostName("127.0.0.1");  //连接本地主机
    m_db.setPort(3306);
    m_db.setUserName("root");
    m_db.setPassword("123456");
    m_db = QSqlDatabase::database("dataBaseConnect");
    QString db_str = QString("CREATE DATABASE IF NOT EXISTS wangfu_test");
    m_db.exec(db_str);
    m_db.setDatabaseName("wangfu_test");
    m_db.open();
    QString query_str = QString("create table SystemSet(default_sample_type int,"
                                "default_sample_cup int,"
                                "sample_needle_clean boolean,"
                                "auto_serum_index boolean,"
                                "sample_validity_period int,"
                                "interval_test_num int,"
                                "below_reference_range int,"
                                "above_reference_range int,"
                                "bio_reagent int,"
                                "ISE_reagent int,"
                                "ISE_Boot_perfusion_num int,"
                                "alarm_volume int)");
    if(!m_db.tables().contains("SystemSet"))
    {
        m_db.exec(query_str);
    }

    query_str = QString("create table state_system(inspection_system int, state varchar(255))");
    if(!m_db.tables().contains("state_system"))
    {
        m_db.exec(query_str);
    }

    query_str = QString("create table state_count(name varchar(255), num int)");
    if(!m_db.tables().contains("state_count"))
    {
        m_db.exec(query_str);
    }

    query_str = QString("create table state_temperature(name varchar(255), measured_value varchar(255), valid_range varchar(255))");
    if(!m_db.tables().contains("state_temperature"))
    {
        m_db.exec(query_str);
    }

    query_str = QString("create table state_power(name varchar(255), measured_value varchar(255), valid_range varchar(255))");
    if(!m_db.tables().contains("state_power"))
    {
        m_db.exec(query_str);
    }

    query_str = QString("create table state_liquid_path(name varchar(255), state varchar(255))");
    if(!m_db.tables().contains("state_liquid_path"))
    {
        m_db.exec(query_str);
    }

    query_str = QString("create table state_lower_computer(name varchar(255), state varchar(255))");
    if(!m_db.tables().contains("state_lower_computer"))
    {
        m_db.exec(query_str);
    }

    query_str = QString("create table state_reaction_vessel(model varchar(255), num int, state varchar(255), UNIQUE (model))");
    if(!m_db.tables().contains("state_reaction_vessel"))
    {
        m_db.exec(query_str);
    }
}

void SystemSet::readDataBase()
{
    QString query_str = QString("select * from state_system");
    QSqlQuery query = QSqlQuery(m_db);
    query.exec(query_str);
    int i = 0;
    while(query.next())
    {
        if(i >= (DEFAULT_ROW_COUNT - 1))
        {
            ui->tableWidget_state_system->insertRow(ui->tableWidget_state_system->rowCount());
        }
        ui->tableWidget_state_system->setItem(i, 0, new QTableWidgetItem(m_addwidget->getIndexText(query.value("inspection_system").toInt())));
        ui->tableWidget_state_system->setItem(i, 1, new QTableWidgetItem(query.value("state").toString()));
        m_systemIndexList.append(query.value("inspection_system").toInt());
        i++;
    }

    query_str = QString("select * from state_count");
    query.exec(query_str);
    i = 0;
    while(query.next())
    {
        if(i >= (DEFAULT_ROW_COUNT - 1))
        {
            ui->tableWidget_state_count->insertRow(ui->tableWidget_state_count->rowCount());
        }
        ui->tableWidget_state_count->setItem(i, 0, new QTableWidgetItem(query.value("name").toString()));
        ui->tableWidget_state_count->setItem(i, 1, new QTableWidgetItem(query.value("num").toString()));
        i++;
    }

    query_str = QString("select * from state_temperature");
    query.exec(query_str);
    i = 0;
    while(query.next())
    {
        if(i >= (DEFAULT_ROW_COUNT - 1))
        {
            ui->tableWidget_state_temperature->insertRow(ui->tableWidget_state_temperature->rowCount());
        }
        ui->tableWidget_state_temperature->setItem(i, 0, new QTableWidgetItem(query.value("name").toString()));
        ui->tableWidget_state_temperature->setItem(i, 1, new QTableWidgetItem(query.value("measured_value").toString()));
        ui->tableWidget_state_temperature->setItem(i, 2, new QTableWidgetItem(query.value("valid_range").toString()));
        i++;
    }

    query_str = QString("select * from state_power");
    query.exec(query_str);
    i = 0;
    while(query.next())
    {
        if(i >= (DEFAULT_ROW_COUNT - 1))
        {
            ui->tableWidget__state_power->insertRow(ui->tableWidget__state_power->rowCount());
        }
        ui->tableWidget__state_power->setItem(i, 0, new QTableWidgetItem(query.value("name").toString()));
        ui->tableWidget__state_power->setItem(i, 1, new QTableWidgetItem(query.value("measured_value").toString()));
        ui->tableWidget__state_power->setItem(i, 2, new QTableWidgetItem(query.value("valid_range").toString()));
        i++;
    }

    query_str = QString("select * from state_liquid_path");
    query.exec(query_str);
    i = 0;
    while(query.next())
    {
        if(i >= (DEFAULT_ROW_COUNT - 1))
        {
            ui->tableWidget_state_liquid->insertRow(ui->tableWidget_state_liquid->rowCount());
        }
        ui->tableWidget_state_liquid->setItem(i, 0, new QTableWidgetItem(query.value("name").toString()));
        ui->tableWidget_state_liquid->setItem(i, 1, new QTableWidgetItem(query.value("state").toString()));
        i++;
    }

    query_str = QString("select * from state_lower_computer");
    query.exec(query_str);
    i = 0;
    while(query.next())
    {
        if(i >= (DEFAULT_ROW_COUNT - 1))
        {
            ui->tableWidget_state_lower->insertRow(ui->tableWidget_state_lower->rowCount());
        }
        ui->tableWidget_state_lower->setItem(i, 0, new QTableWidgetItem(query.value("name").toString()));
        ui->tableWidget_state_lower->setItem(i, 1, new QTableWidgetItem(query.value("state").toString()));
        i++;
    }

    query_str = QString("select * from SystemSet");
    if(query.exec(query_str) && query.first())
    {
        ui->comboBox_sample_type->setCurrentIndex(query.value("default_sample_type").toInt());
        ui->comboBox_sample_cup->setCurrentIndex(query.value("default_sample_cup").toInt());
        ui->checkBox_needle_clean->setChecked(query.value("sample_needle_clean").toBool());
        ui->checkBox_serum_index->setChecked(query.value("auto_serum_index").toBool());
        ui->lineEdit_validity_period->setText(query.value("sample_validity_period").toString());
        ui->lineEdit_test_num->setText(query.value("interval_test_num").toString());
        ui->lineEdit_below_range->setText(query.value("below_reference_range").toString());
        ui->lineEdit_above_range->setText(query.value("above_reference_range").toString());
        ui->lineEdit_bio_reagent->setText(query.value("bio_reagent").toString());
        ui->lineEdit_ISE_reagent->setText(query.value("ISE_reagent").toString());
        ui->lineEdit_perfusion_num->setText(query.value("ISE_Boot_perfusion_num").toString());
        ui->Slider_alarm_volume->setValue(query.value("alarm_volume").toInt());
    }
    else
    {
        qDebug() << "failed:" << query.lastError();
    }
    m_mytable_model = new MyTableModel(this, m_db);
    connect(m_mytable_model, SIGNAL(dataChanged(const QModelIndex, const QModelIndex, const QVector<int>)),
            this, SLOT(dataChange(const QModelIndex, const QModelIndex, const QVector<int>)));
    m_mytable_model->setTable("state_reaction_vessel");
    m_mytable_model->select();
    ui->tableView_reaction_vessel->setModel(m_mytable_model);
    ui->tableView_reaction_vessel->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed); //设置表头宽度固定
    ui->tableView_reaction_vessel->horizontalHeader()->setSectionsClickable(false); //设置表头不可选
    ui->tableView_reaction_vessel->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter); //设置表头水平左对齐，垂直居中
    ui->tableView_reaction_vessel->setItemDelegate(m_myDelegate);
    ui->tableView_reaction_vessel->show();
}

void SystemSet::updateStateSystemTable()
{
    for(int i = 0; i < m_systemIndexList.size(); i++)
    {
        ui->tableWidget_state_system->item(i, 0)->setText(m_addwidget->getIndexText(m_systemIndexList.at(i)));
    }
}

void SystemSet::initCustomPlot()
{
    ui->plot_graph->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectLegend);
    ui->plot_graph->axisRect()->setRangeDrag(Qt::Horizontal);
    ui->plot_graph->axisRect()->setRangeZoom(Qt::Horizontal);

    ui->plot_graph->addGraph();
    ui->plot_graph->graph(0)->setPen(QColor(0, 0, 255));
    ui->plot_graph->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->plot_graph->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDiamond, 10));

    ui->plot_graph->xAxis->setSubTicks(false);
    ui->plot_graph->yAxis->setSubTicks(false);
    ui->plot_graph->xAxis2->setVisible(true);
    ui->plot_graph->yAxis2->setVisible(true);
    ui->plot_graph->xAxis2->setTicks(false);
    ui->plot_graph->yAxis2->setTicks(false);
    ui->plot_graph->rescaleAxes();

    m_tracer = new QCPItemTracer(ui->plot_graph);
    m_tracer->setPen(QPen(Qt::black));
    m_tracer->setBrush(QBrush(Qt::black));
    m_tracer->setStyle(QCPItemTracer::tsCrosshair);
    m_tracer->setSize(10.0);
}

void SystemSet::setData(QCustomPlot *plot, double data)
{
    static double s_max_temp = data;
    static double s_min_temp = data;
    static double s_x_maxrange = 1;
    bool ok = false;
    if(data > s_max_temp)
    {
        s_max_temp = data;
        ok = true;
    }

    if(data < s_min_temp)
    {
        s_min_temp = data;
        ok = true;
    }

    double range_offset = (s_max_temp - s_min_temp) * 3.0 / 14; //轴范围偏移量
    if(ok)
    {
        if(s_min_temp < s_max_temp)
        {
            plot->yAxis->setRange(s_min_temp - range_offset, s_max_temp + range_offset);
        }
    }
    plot->graph(0)->addData(s_x_maxrange, data);

    if(qFuzzyCompare(s_max_temp, s_min_temp)) //第一个数据时最小值等于最大值，进行特殊处理
    {
        plot->yAxis->setRange(s_max_temp - 1, s_max_temp + 1);
        m_tracer->setGraph(plot->graph(0));
    }

    plot->xAxis->setRange(0, s_x_maxrange + 1);
    s_x_maxrange++;
    plot->replot();
}

QByteArray SystemSet::convertFormat(QString str)
{
    QByteArray arr;
    switch(m_dataFormat)
    {
        case 0:
            arr = str.toUtf8();
            break;
        case 1:
            {
                QTextCodec *gbk = QTextCodec::codecForName("GBK");
                arr = gbk->fromUnicode(str);
                break;
            }
        default:
            break;
    }
    arr.append('\n');
    return arr;
}

void SystemSet::on_btn_F8_save_clicked()
{
    QSqlQuery query = QSqlQuery(m_db);
    bool ok = ui->lineEdit_validity_period->text().isEmpty() | ui->lineEdit_test_num->text().isEmpty() |
              ui->lineEdit_below_range->text().isEmpty() | ui->lineEdit_above_range->text().isEmpty() |
              ui->lineEdit_bio_reagent->text().isEmpty() | ui->lineEdit_ISE_reagent->text().isEmpty() |
              ui->lineEdit_perfusion_num->text().isEmpty();
    if(ok)
    {
        QMessageBox::information(this, tr("Tips"), tr("Please fill in the information completely！"), QMessageBox::Ok);
    }
    else
    {
        QString query_str = QString("delete from SystemSet;");      //  清除旧数据
        query.exec(query_str);
        query.prepare("INSERT INTO SystemSet(default_sample_type, default_sample_cup, sample_needle_clean,"
                      "auto_serum_index, sample_validity_period, interval_test_num, below_reference_range,"
                      "above_reference_range, bio_reagent, ISE_reagent, ISE_Boot_perfusion_num, alarm_volume)"
                      "VALUES(:default_sample_type, :default_sample_cup, :sample_needle_clean,"
                      ":auto_serum_index, :sample_validity_period, :interval_test_num, :below_reference_range,"
                      ":above_reference_range, :bio_reagent, :ISE_reagent, :ISE_Boot_perfusion_num, :alarm_volume)");

        query.bindValue(":default_sample_type", ui->comboBox_sample_type->currentIndex());
        query.bindValue(":default_sample_cup", ui->comboBox_sample_cup->currentIndex());
        query.bindValue(":sample_needle_clean", ui->checkBox_needle_clean->isChecked());
        query.bindValue(":auto_serum_index", ui->checkBox_serum_index->isChecked());
        query.bindValue(":sample_validity_period", ui->lineEdit_validity_period->text());
        query.bindValue(":interval_test_num", ui->lineEdit_test_num->text());
        query.bindValue(":below_reference_range", ui->lineEdit_below_range->text());
        query.bindValue(":above_reference_range", ui->lineEdit_above_range->text());
        query.bindValue(":bio_reagent", ui->lineEdit_bio_reagent->text());
        query.bindValue(":ISE_reagent", ui->lineEdit_ISE_reagent->text());
        query.bindValue(":ISE_Boot_perfusion_num", ui->lineEdit_perfusion_num->text());
        query.bindValue(":alarm_volume", ui->Slider_alarm_volume->value());

        query_str = ui->comboBox_sample_type->currentText() + " ";
        query_str += ui->comboBox_sample_cup->currentText() + " ";
        query_str += QString("%1 ").arg(ui->checkBox_needle_clean->isChecked());
        query_str += QString("%1 ").arg(ui->checkBox_serum_index->isChecked());
        query_str += ui->lineEdit_validity_period->text() + " ";
        query_str += ui->lineEdit_test_num->text() + " ";
        query_str += ui->lineEdit_below_range->text() + " ";
        query_str += ui->lineEdit_above_range->text() + " ";
        query_str += ui->lineEdit_bio_reagent->text() + " ";
        query_str += ui->lineEdit_ISE_reagent->text() + " ";
        query_str += ui->lineEdit_perfusion_num->text() + " ";
        query_str += QString("%1").arg(ui->Slider_alarm_volume->value());

        if(query.exec())
        {
            if(m_socketState == QAbstractSocket::UnconnectedState)
            {
                QMessageBox::information(this, tr("Tips"), tr("The database is saved successfully！\nThe socket is not connected！"), QMessageBox::Ok);
            }
            else
            {
                emit socketSendData_sign(convertFormat(query_str));
                QMessageBox::information(this, tr("Tips"), tr("The database is saved successfully！\nThe data was sent successfully！"), QMessageBox::Ok);
            }
        }
        else
        {
            QMessageBox::information(this, tr("Tips"), tr("Database save failed！"), QMessageBox::Ok);
        }
    }
}

void SystemSet::on_btn_state_add_clicked()
{
    emit changeWidget(ui->tabWidget_state->currentIndex());
    m_addwidget->show();
}

void SystemSet::on_btn_state_delete_clicked()
{
    switch (ui->tabWidget_state->currentIndex())    //判断界面索引
    {
        case SYSTEM:
            {
                if(!ui->tableWidget_state_system->selectedItems().isEmpty())        //判断有无选中项
                {
                    m_systemIndexList.removeAt(ui->tableWidget_state_system->currentRow());
                    ui->tableWidget_state_system->removeRow(ui->tableWidget_state_system->currentRow());
                    if(ui->tableWidget_state_system->rowCount() < DEFAULT_ROW_COUNT)
                    {
                        ui->tableWidget_state_system->insertRow(ui->tableWidget_state_system->rowCount());
                    }

                }
                break;
            }
        case COUNT:
            {
                if(!ui->tableWidget_state_count->selectedItems().isEmpty())
                {
                    ui->tableWidget_state_count->removeRow(ui->tableWidget_state_count->currentRow());
                    if(ui->tableWidget_state_count->rowCount() < DEFAULT_ROW_COUNT)
                    {
                        ui->tableWidget_state_count->insertRow(ui->tableWidget_state_count->rowCount());
                    }
                }
                break;
            }
        case TEMPERATURE:
            {
                if(!ui->tableWidget_state_temperature->selectedItems().isEmpty())
                {
                    ui->tableWidget_state_temperature->removeRow(ui->tableWidget_state_temperature->currentRow());
                    if(ui->tableWidget_state_temperature->rowCount() < DEFAULT_ROW_COUNT)
                    {
                        ui->tableWidget_state_temperature->insertRow(ui->tableWidget_state_temperature->rowCount());
                    }
                }
                break;
            }
        case POWER:
            {
                if(!ui->tableWidget__state_power->selectedItems().isEmpty())
                {
                    ui->tableWidget__state_power->removeRow(ui->tableWidget__state_power->currentRow());
                    if(ui->tableWidget__state_power->rowCount() < DEFAULT_ROW_COUNT)
                    {
                        ui->tableWidget__state_power->insertRow(ui->tableWidget__state_power->rowCount());
                    }
                }
                break;
            }
        case LIQUID:
            {
                if(!ui->tableWidget_state_liquid->selectedItems().isEmpty())
                {
                    ui->tableWidget_state_liquid->removeRow(ui->tableWidget_state_liquid->currentRow());
                    if(ui->tableWidget_state_liquid->rowCount() < DEFAULT_ROW_COUNT)
                    {
                        ui->tableWidget_state_liquid->insertRow(ui->tableWidget_state_liquid->rowCount());
                    }
                }
                break;
            }
        case REACTION:
            {
                if(ui->tableView_reaction_vessel->currentIndex().row() != -1)
                {
                    m_mytable_model->m_isTwenty = true;
                    m_mytable_model->removeRow(ui->tableView_reaction_vessel->currentIndex().row());
                    m_myDelegate->m_rowCount = m_mytable_model->rowCount();
                    m_mytable_model->m_isTwenty = false;
                    m_mytable_model->submitAll();
                    m_mytable_model->select();
                    ui->tableView_reaction_vessel->update();
                }

                break;
            }
        case LOWCOMPUTER:
            {
                if(!ui->tableWidget_state_lower->selectedItems().isEmpty())
                {
                    ui->tableWidget_state_lower->removeRow(ui->tableWidget_state_lower->currentRow());
                    if(ui->tableWidget_state_lower->rowCount() < DEFAULT_ROW_COUNT)
                    {
                        ui->tableWidget_state_lower->insertRow(ui->tableWidget_state_lower->rowCount());
                    }
                }
                break;
            }
        default:
            break;
    }
}

void SystemSet::on_btn_state_save_clicked()
{
    QSqlQuery query = QSqlQuery(m_db);
    bool ok = true;
    QString query_str = QString("delete from state_system;"
                                "delete from state_count;"
                                "delete from state_temperature;"
                                "delete from state_power;"
                                "delete from state_liquid_path;"
                                "delete from state_lower_computer;");
    query.exec(query_str);      //清除老数据
    for(int i = 0; i < ui->tableWidget_state_system->rowCount(); i++)
    {
        if(ui->tableWidget_state_system->item(i, 0) != nullptr)
        {
            query.prepare("INSERT INTO state_system(inspection_system, state)"
                          "VALUES(:inspection_system, :state)");
            query.bindValue(":inspection_system", m_addwidget->getIndex(ui->tableWidget_state_system->item(i, 0)->text()));
            query.bindValue(":state", ui->tableWidget_state_system->item(i, 1)->text());
            ok &= query.exec();
        }
    }

    for(int i = 0; i < ui->tableWidget_state_count->rowCount(); i++)
    {
        if(ui->tableWidget_state_count->item(i, 0) != nullptr)
        {
            query.prepare("INSERT INTO state_count(name, num) VALUES(:name, :num)");
            query.bindValue(":name", ui->tableWidget_state_count->item(i, 0)->text());
            query.bindValue(":num", ui->tableWidget_state_count->item(i, 1)->text());
            ok &= query.exec();
        }
    }

    for(int i = 0; i < ui->tableWidget_state_temperature->rowCount(); i++)
    {
        if(ui->tableWidget_state_temperature->item(i, 0) != nullptr)
        {
            query.prepare("INSERT INTO state_temperature(name, measured_value, valid_range)"
                          "VALUES(:name, :measured_value, :valid_range)");
            query.bindValue(":name", ui->tableWidget_state_temperature->item(i, 0)->text());
            query.bindValue(":measured_value", ui->tableWidget_state_temperature->item(i, 1)->text());
            query.bindValue(":valid_range", ui->tableWidget_state_temperature->item(i, 2)->text());
            ok &= query.exec();
        }
    }

    for(int i = 0; i < ui->tableWidget__state_power->rowCount(); i++)
    {
        if(ui->tableWidget__state_power->item(i, 0) != nullptr)
        {
            query.prepare("INSERT INTO state_power(name, measured_value, valid_range)"
                          "VALUES(:name, :measured_value, :valid_range)");
            query.bindValue(":name", ui->tableWidget__state_power->item(i, 0)->text());
            query.bindValue(":measured_value", ui->tableWidget__state_power->item(i, 1)->text());
            query.bindValue(":valid_range", ui->tableWidget__state_power->item(i, 2)->text());
            ok &= query.exec();
        }
    }

    for(int i = 0; i < ui->tableWidget_state_liquid->rowCount(); i++)
    {
        if(ui->tableWidget_state_liquid->item(i, 0) != nullptr)
        {
            query.prepare("INSERT INTO state_liquid_path(name, state) VALUES(:name, :state)");
            query.bindValue(":name", ui->tableWidget_state_liquid->item(i, 0)->text());
            query.bindValue(":state", ui->tableWidget_state_liquid->item(i, 1)->text());
            ok &= query.exec();
        }
    }

    for(int i = 0; i < ui->tableWidget_state_lower->rowCount(); i++)
    {
        if(ui->tableWidget_state_lower->item(i, 0) != nullptr)
        {
            query.prepare("INSERT INTO state_lower_computer(name, state) VALUES(:name, :state)");
            query.bindValue(":name", ui->tableWidget_state_lower->item(i, 0)->text());
            query.bindValue(":state", ui->tableWidget_state_lower->item(i, 1)->text());
            ok &= query.exec();
        }
    }

    if(ok)
    {
        QMessageBox::information(this, tr("Tips"), tr("Save successfully！"), QMessageBox::Ok);
    }
    else
    {
        QMessageBox::information(this, tr("Tips"), tr("Save failed！"), QMessageBox::Ok);
    }
}

void SystemSet::addData(QList<QString> list_str)
{
    if(list_str.size() > 0 && ui->tabWidget->currentIndex() == 1)
    {
        int curRowCount = 0;    //有数据的行数
        switch (ui->tabWidget_state->currentIndex())
        {
            case SYSTEM:
                {
                    for(int i = 0; i < ui->tableWidget_state_system->rowCount(); i++)
                    {
                        curRowCount = i;
                        if(ui->tableWidget_state_system->item(i, 0) == nullptr)
                        {
                            break;
                        }
                    }
                    if(curRowCount >= (DEFAULT_ROW_COUNT - 1))
                    {
                        ui->tableWidget_state_system->insertRow(ui->tableWidget_state_system->rowCount());
                    }
                    ui->tableWidget_state_system->setItem(curRowCount, 0, new QTableWidgetItem(list_str.at(0)));
                    ui->tableWidget_state_system->setItem(curRowCount, 1, new QTableWidgetItem(list_str.at(1)));
                    m_systemIndexList.append(m_addwidget->getIndex(list_str.at(0)));
                    break;
                }
            case COUNT:
                {
                    for(int i = 0; i < ui->tableWidget_state_count->rowCount(); i++)
                    {
                        curRowCount = i;
                        if(ui->tableWidget_state_count->item(i, 0) == nullptr)
                        {
                            break;
                        }
                    }
                    if(curRowCount >= (DEFAULT_ROW_COUNT - 1))
                    {
                        ui->tableWidget_state_count->insertRow(ui->tableWidget_state_count->rowCount());
                    }
                    ui->tableWidget_state_count->setItem(curRowCount, 0, new QTableWidgetItem(list_str.at(0)));
                    ui->tableWidget_state_count->setItem(curRowCount, 1, new QTableWidgetItem(list_str.at(1)));
                    break;
                }
            case TEMPERATURE:
                {
                    for(int i = 0; i < ui->tableWidget_state_temperature->rowCount(); i++)
                    {
                        curRowCount = i;
                        if(ui->tableWidget_state_temperature->item(i, 0) == nullptr)
                        {
                            break;
                        }
                    }
                    if(curRowCount >= (DEFAULT_ROW_COUNT - 1))
                    {
                        ui->tableWidget_state_temperature->insertRow(ui->tableWidget_state_temperature->rowCount());
                    }
                    ui->tableWidget_state_temperature->setItem(curRowCount, 0, new QTableWidgetItem(list_str.at(0)));
                    ui->tableWidget_state_temperature->setItem(curRowCount, 1, new QTableWidgetItem(list_str.at(1) + "℃"));
                    ui->tableWidget_state_temperature->setItem(curRowCount, 2, new QTableWidgetItem(QString("%1℃-%2℃").arg(list_str.at(2)).arg(list_str.at(3))));
                    break;
                }
            case POWER:
                {
                    for(int i = 0; i < ui->tableWidget__state_power->rowCount(); i++)
                    {
                        curRowCount = i;
                        if(ui->tableWidget__state_power->item(i, 0) == nullptr)
                        {
                            break;
                        }
                    }
                    if(curRowCount >= (DEFAULT_ROW_COUNT - 1))
                    {
                        ui->tableWidget__state_power->insertRow(ui->tableWidget__state_power->rowCount());
                    }
                    ui->tableWidget__state_power->setItem(curRowCount, 0, new QTableWidgetItem(list_str.at(0)));
                    ui->tableWidget__state_power->setItem(curRowCount, 1, new QTableWidgetItem(list_str.at(1) + "V"));
                    ui->tableWidget__state_power->setItem(curRowCount, 2, new QTableWidgetItem(QString("%1V-%2V").arg(list_str.at(2)).arg(list_str.at(3))));
                    break;
                }
            case LIQUID:
                {
                    for(int i = 0; i < ui->tableWidget_state_liquid->rowCount(); i++)
                    {
                        curRowCount = i;
                        if(ui->tableWidget_state_liquid->item(i, 0) == nullptr)
                        {
                            break;
                        }
                    }
                    if(curRowCount >= (DEFAULT_ROW_COUNT - 1))
                    {
                        ui->tableWidget_state_liquid->insertRow(ui->tableWidget_state_liquid->rowCount());
                    }
                    ui->tableWidget_state_liquid->setItem(curRowCount, 0, new QTableWidgetItem(list_str.at(0)));
                    ui->tableWidget_state_liquid->setItem(curRowCount, 1, new QTableWidgetItem(list_str.at(1)));
                    break;
                }
            case REACTION:
                {
                    m_mytable_model->m_isTwenty = true;
                    QSqlRecord record = m_mytable_model->record();
                    record.setValue(0, list_str.at(0));
                    record.setValue(1, list_str.at(1));
                    record.setValue(2, list_str.at(2));
                    m_mytable_model->insertRecord(m_mytable_model->rowCount() - 1, record);
                    m_myDelegate->m_rowCount = m_mytable_model->rowCount();
                    m_mytable_model->m_isTwenty = false;
                    m_mytable_model->submitAll();
                    break;
                }
            case LOWCOMPUTER:
                {
                    for(int i = 0; i < ui->tableWidget_state_lower->rowCount(); i++)
                    {
                        curRowCount = i;
                        if(ui->tableWidget_state_lower->item(i, 0) == nullptr)
                        {
                            break;
                        }
                    }
                    if(curRowCount >= (DEFAULT_ROW_COUNT - 1))
                    {
                        ui->tableWidget_state_lower->insertRow(ui->tableWidget_state_lower->rowCount());
                    }
                    ui->tableWidget_state_lower->setItem(curRowCount, 0, new QTableWidgetItem(list_str.at(0)));
                    ui->tableWidget_state_lower->setItem(curRowCount, 1, new QTableWidgetItem(list_str.at(1)));
                    break;
                }
            default:
                break;
        }
    }
    else if(list_str.size() > 0 && ui->tabWidget->currentIndex() == 0)
    {
        if((m_tcpServer->serverAddress().toString() != list_str.at(0)) | (m_tcpServer->serverPort() != list_str.at(1).toUShort()))
        {
            //更换端口号或IP地址重新监听
            m_tcpServer->close();
            QHostAddress addr(list_str.at(0));
            m_tcpServer->listen(addr, list_str.at(1).toUShort());
            emit socketDisconnect();    //socket主动断开连接
            ui->label_tcp->setText(tr("Socket_State：Listening"));
        }
        m_dataFormat = list_str.at(2).toInt();
    }
    m_addwidget->hide();
}

void SystemSet::onNewConnection()
{
    emit connectSocket(m_tcpServer->nextPendingConnection()->socketDescriptor());
}

void SystemSet::onSocketStateChange(QAbstractSocket::SocketState socketState)
{
    m_socketState = socketState;
    switch(socketState)
    {
        case QAbstractSocket::UnconnectedState:
            ui->label_tcp->setText(tr("Socket_State：UnconnectedState"));
            break;
        case QAbstractSocket::HostLookupState:
            ui->label_tcp->setText(tr("Socket_State：HostLookupState"));
            break;
        case QAbstractSocket::ConnectingState:
            ui->label_tcp->setText(tr("Socket_State：ConnectingState"));
            break;

        case QAbstractSocket::ConnectedState:
            ui->label_tcp->setText(tr("Socket_State：ConnectedState"));
            break;

        case QAbstractSocket::BoundState:
            ui->label_tcp->setText(tr("Socket_State：BoundState"));
            break;

        case QAbstractSocket::ClosingState:
            ui->label_tcp->setText(tr("Socket_State：ClosingState"));
            break;
        case QAbstractSocket::ListeningState:
            ui->label_tcp->setText(tr("Socket_State：ListeningState"));
            break;
    }
}

void SystemSet::on_btn_F5_clicked()
{
    emit changeWidget(TCP);  //切换到TCP地址界面
    m_addwidget->show();
}

void SystemSet::on_comboBox_language_currentIndexChanged(int index)
{
    QString filePath;
    switch(index)
    {
        case Chinese:
            filePath = QDir::currentPath() + "/LNG_CN.qm";
            break;
        case English:
            filePath = QDir::currentPath() + "/LNG_EN.qm";
            break;
        default:
            break;
    }
    m_translator->load(filePath);

    qApp->installTranslator(m_translator);
    ui->retranslateUi(this);
    m_mytable_model->setHeaderData(0, Qt::Horizontal, QObject::tr("Model"));
    m_mytable_model->setHeaderData(1, Qt::Horizontal, QObject::tr("Num"));
    m_mytable_model->setHeaderData(2, Qt::Horizontal, QObject::tr("State"));
    emit addWidgetLanguageChange();
    updateStateSystemTable();
    onSocketStateChange(m_socketState);
}

void SystemSet::dataChange(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);
    Q_UNUSED(roles);
    m_mytable_model->submitAll();
    m_mytable_model->select();
    ui->tableView_reaction_vessel->update();
}

void SystemSet::on_btn_graph_add_clicked()
{
    if(!ui->lineEdit_graph_data->text().isEmpty())
    {
        setData(ui->plot_graph, ui->lineEdit_graph_data->text().toDouble());
    }
}

void SystemSet::mouseMove(QMouseEvent *e)
{
    if(!ui->plot_graph->graph(0)->data()->isEmpty())
    {
        double x = ui->plot_graph->xAxis->pixelToCoord(e->pos().x());
        m_tracer->setGraphKey(qRound(x));
        m_tracer->setInterpolating(true);
        m_tracer->updatePosition();
        ui->plot_graph->replot();
        ui->label_graph_xy->setText(QString("x:%1-y:%2").arg(m_tracer->position->key()).arg(m_tracer->position->value()));
    }
}
