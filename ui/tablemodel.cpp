#include "tablemodel.h"
#include <QSqlQuery>
#include<QDebug>

TableModel::TableModel(QObject *parent)
    : QAbstractTableModel{parent}
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("../sessions_stat.db");
    table.append({"user", "create_date", "update_date", "is_active", "src_endpoint",
                  "dst_endpoint", "bytes_sent", "bytes_recv"});
    if (!db.open()) {
        qDebug() << "there was an error during opening db";
    }
    update();
}

TableModel::~TableModel()
{
    db.close();
}

int TableModel::rowCount(const QModelIndex &) const
{
    return table.size();
}

int TableModel::columnCount(const QModelIndex &) const
{
    return table.at(0).size();
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    switch(role) {
    case TableDataRole:
    {
        return table.at(index.row()).at(index.column());
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

void TableModel::update()
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    table.resize(1);
    QSqlQuery query(db);
    query.exec("SELECT * from sessions ORDER BY id DESC");
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

        table.append(row);
    }
    endInsertRows();
}
