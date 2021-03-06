#ifndef DEVICETABLEDELEGATE_H
#define DEVICETABLEDELEGATE_H

#include <customtypes.h>

class DeviceTableDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit DeviceTableDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    QStringListModel *fillDeviceModelComboBox() const;
};

#endif // DEVICETABLEDELEGATE_H
