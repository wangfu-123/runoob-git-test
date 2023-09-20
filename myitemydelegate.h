#ifndef MYITEMYDELEGATE_H
#define MYITEMYDELEGATE_H

#include <QObject>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QProgressBar>
#include <QStyledItemDelegate>
#include <QCheckBox>
#include <QPainter>

class MyItemyDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit MyItemyDelegate(QObject *parent = nullptr);

    /*当需要创建自定义交互编辑器时，需要实现下面这几个函数*/
    QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    /*当需要绘制样式时，需要重新实现paint()函数*/
    void paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

    int m_rowCount;

    enum Index_Column
    {
        MODEL,
        NUM,
        STATE
    };

};

#endif // MYITEMYDELEGATE_H

