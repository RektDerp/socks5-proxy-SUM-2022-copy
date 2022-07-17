#include "tablemodel.h"
#include <QSqlQuery>
#include<QDebug>

TableModel::TableModel(QObject *parent)
    : QAbstractTableModel{parent}
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("../sessions_stat.db");
    table.append({"id", "user", "is_active", "type", "src_addr",
                  "src_port", "dst_addr", "dst_port", "bytes_sent", "bytes_recv"});
    if (!db.open()) {
        qDebug() << "there was an error during opening db";
    }
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
        int index = 0;
        QVector<QString> row;
        for (int i = 0; i < 10; i++) {
            QString value = query.value(index++).toString();
            row.append(value);
        }
        table.append(row);
    }
    endInsertRows();
}
