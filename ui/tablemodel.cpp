#include "tablemodel.h"
#include "qcoreevent.h"
#include <QSqlQuery>
#include <QDebug>

TableModel::TableModel(QObject *parent)
    : QAbstractTableModel{parent}
{
    _db = QSqlDatabase::addDatabase("QSQLITE");
    _db.setDatabaseName("../sessions_stat.db");
    _table.append({"user", "create_date", "update_date", "is_active", "src_endpoint",
                  "dst_endpoint", "bytes_sent", "bytes_recv"});
    if (!_db.open()) {
        qDebug() << "there was an error during opening db";
    }

    _timerId = startTimer(1000);
    update();
}

TableModel::~TableModel()
{
    _db.close();
}

int TableModel::rowCount(const QModelIndex &) const
{
    return _table.size();
}

int TableModel::columnCount(const QModelIndex &) const
{
    return _table.at(0).size();
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    switch(role) {
    case TableDataRole:
    {
        return _table.at(index.row()).at(index.column());
    }
    case HeadingRole:
    {
        return index.row() == 0;
    }
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> TableModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TableDataRole] = "tabledata";
    roles[HeadingRole] = "heading";
    return roles;
}

void TableModel::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == _timerId) {
        qDebug("update");
        update();
    }
}

void TableModel::update()
{
    beginResetModel();
    _table.resize(1);
    QSqlQuery query(_db);
    query.exec("SELECT * from sessions ORDER BY update_date DESC");
    while (query.next()) {
        QVector<QString> row;
        int i = 0;
        /*QString id = */query.value(i++).toString();
        QString user = query.value(i++).toString();
        QString create_date = query.value(i++).toString();
        QString update_date = query.value(i++).toString();
        QString is_active = query.value(i++).toString();
        QString src_addr = query.value(i++).toString();
        QString src_port = query.value(i++).toString();
        QString dst_addr = query.value(i++).toString();
        QString dst_port = query.value(i++).toString();
        QString bytes_sent = query.value(i++).toString();
        QString bytes_recv = query.value(i++).toString();
        row.append(user);
        row.append(create_date);
        row.append(update_date);
        row.append(is_active);
        row.append(src_addr + ":" + src_port);
        row.append(dst_addr + ":" + dst_port);
        row.append(bytes_sent);
        row.append(bytes_recv);

        _table.append(row);
    }
    endResetModel();
}

//void TableModel::sort(int column, Qt::SortOrder order)
//{
//    qDebug("Sorting by column %d", column);
//    QSortFilterProxyModel::sort(column, order);
//}
