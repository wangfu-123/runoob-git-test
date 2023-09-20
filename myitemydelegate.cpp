#include "Myitemydelegate.h"
#include <QApplication>
#include <QMouseEvent>
#include <QDebug>

MyItemyDelegate::MyItemyDelegate(QObject *parent )
    : QStyledItemDelegate(parent)
{

}

QWidget * MyItemyDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() == MODEL) //型号
    {
        if(index.data().isValid())
        {
            QComboBox *tableComBox = new QComboBox(parent);
            tableComBox->addItem(tr("ISE"));
            tableComBox->addItem(tr("Standard"));
            tableComBox->addItem(tr("Microcup"));
            return tableComBox;
        }
        else
        {
            return nullptr;
        }

    }
    else if(index.column() == NUM) //数量
    {
        if(index.data().isValid())
        {
            QSpinBox *tableSpinBox = new QSpinBox(parent);
            tableSpinBox->setFrame(false);
            tableSpinBox->setMinimum(0);
            tableSpinBox->setMaximum(100);
            return tableSpinBox;
        }
        else
        {
            return nullptr;
        }

    }
    else if(index.column() == STATE) //状态
    {
        return nullptr;
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void MyItemyDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.column() == MODEL) //型号
    {
        QString value = index.model()->data(index, Qt::EditRole).toString();
        QComboBox *comboBox = static_cast<QComboBox *>(editor);
        int tindex = comboBox->findText(value);
        comboBox->setCurrentIndex(tindex);
    }
    else if(index.column() == NUM) //数量
    {
        int value = index.model()->data(index, Qt::EditRole).toInt();
        QSpinBox *spinBox = static_cast<QSpinBox *>(editor);
        spinBox->setValue(value);
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void MyItemyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(index.column() == MODEL) //型号
    {
        QComboBox* comboBox = static_cast<QComboBox*>(editor);
        QString text = comboBox->currentText();
        model->setData(index, text, Qt::EditRole);
    }
    else if(index.column() == NUM) //数量
    {
        QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
        spinBox->interpretText();
        int value = spinBox->value();
        model->setData(index, value, Qt::EditRole);
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void MyItemyDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}

void MyItemyDelegate::paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.isValid() && (index.column() == STATE)) //状态
    {
        QStyleOptionButton style;
        QCheckBox box;
        style.rect = option.rect;
        if(index.data().toString() == "OK")
        {
            style.state = QStyle::State_On;
            style.text = "OK";
            box.style()->drawControl(QStyle::CE_CheckBox, &style, painter, &box);
        }
        else if(index.data().toString() == "NOT OK")
        {
            style.state = QStyle::State_Off;
            style.text = "NOT OK";
            box.style()->drawControl(QStyle::CE_CheckBox, &style, painter, &box);
        }
        else
        {
            QStyledItemDelegate::paint(painter, option, index);
        }
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

bool MyItemyDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_UNUSED(model);

    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    if(option.rect.contains(mouseEvent->pos()))
    {
        if((event->type() == QEvent::MouseButtonPress) && (index.column() == STATE))
        {
            if(index.data().toString() == "OK")
            {
                model->setData(index, "NOT OK");
            }
            else if(index.data().toString() == "NOT OK")
            {
                model->setData(index, "OK");
            }

        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
