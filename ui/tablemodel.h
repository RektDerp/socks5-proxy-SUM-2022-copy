#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <qqml.h>
#include <QAbstractTableModel>
#include <QSqlDatabase>
#include <QSortFilterProxyModel>

class TableModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_DISABLE_COPY(TableModel)

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

//    Q_INVOKABLE virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override
//    {
//        qDebug("Sorting by column %d", column);
//        QSortFilterProxyModel::sort(column, order);
//    }
protected:
    void timerEvent(QTimerEvent* event) override;
    void update();

private:
    QVector<QVector<QString>> _table;
    QSqlDatabase _db;
    int _timerId;
};

#endif // TABLEMODEL_H
