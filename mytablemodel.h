#ifndef MYTABLEMODEL_H
#define MYTABLEMODEL_H

#include <QObject>
#include <QSqlTableModel>
#include <QDebug>

class MyTableModel : public QSqlTableModel
{
    Q_OBJECT
public:
    MyTableModel();
    MyTableModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase()) : QSqlTableModel(parent, db) {}
    int rowCount(const QModelIndex &parent = QModelIndex())const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    const int DEFAULT_ROW_COUNT = 20;  //默认行数
    bool m_isTwenty = false;  //标志位，判断数据是否有二十行
};

#endif // MYTABLEMODEL_H
