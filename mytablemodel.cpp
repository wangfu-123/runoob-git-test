#include "mytablemodel.h"

MyTableModel::MyTableModel()
{

}

int MyTableModel::rowCount(const QModelIndex &parent) const
{
    if (QSqlTableModel::rowCount(parent) < DEFAULT_ROW_COUNT && !m_isTwenty) //不足二十行时将行数设置为二十行
    {
        return DEFAULT_ROW_COUNT;
    }
    return QSqlTableModel::rowCount(parent) + 1;
}

int MyTableModel::columnCount(const QModelIndex &parent) const
{
    return QSqlTableModel::columnCount(parent);
}
