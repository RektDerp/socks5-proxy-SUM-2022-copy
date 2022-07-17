#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <qqml.h>
#include <QAbstractTableModel>
#include <QSqlDatabase>

class TableModel : public QAbstractTableModel
{
    Q_OBJECT

    enum TableRoles {
        TableDataRole = Qt::UserRole + 1,
        HeadingRole
    };

public:
    explicit TableModel(QObject* parent = nullptr);
    ~TableModel();

    int rowCount(const QModelIndex & = QModelIndex()) const override;

    int columnCount(const QModelIndex & = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void update();

private:
    QVector<QVector<QString>> table;
    QSqlDatabase db;
};

#endif // TABLEMODEL_H
