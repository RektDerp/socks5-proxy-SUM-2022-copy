#include "tablemodel.h"
#include "qcoreevent.h"
#include "qwindowdefs.h"
#include <QSqlQuery>
#include <QDebug>
#include <qguiapplication.h>
#include <QFont>
#include <QFontMetrics>

TableModel::TableModel(QObject *parent)
    : QAbstractTableModel{parent}
{
    _header.append({"user", "create_date", "update_date", "is_active", "src_endpoint",
                    "dst_endpoint", "bytes_sent", "bytes_recv"});
    _columnWidths.resize(_header.size());
    _db = QSqlDatabase::addDatabase("QSQLITE");
    _db.setDatabaseName("../sessions_stat.db");
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

QVariant TableModel::data(const QModelIndex &index, int /*role*/) const
{
    return _table.at(index.row()).at(index.column());
}

QHash<int, QByteArray> TableModel::roleNames() const
{
    return { {Qt::DisplayRole, "display"} };
}

QVariant TableModel::headerData(int id, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        if (id > _header.size() || id < 0)
            return QVariant();
        return _header[id];
    } else {
        return QString();
    }
}

int TableModel::columnWidth(int c, const QFont *font)
{
    if (!_columnWidths[c]) {
        QFontMetrics defaultFontMetrics = QFontMetrics(QGuiApplication::font());
        QFontMetrics fm = (font ? QFontMetrics(*font) : defaultFontMetrics);
        int ret = fm.horizontalAdvance(headerData(c, Qt::Horizontal).toString()) + 10;

        for (int r = 0; r < _table.size(); r++) {
            ret = qMax(ret, fm.horizontalAdvance(_table[r][c]));
        }
        _columnWidths[c] = ret + 10;
    }

    return _columnWidths[c];
}

void TableModel::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == _timerId) {
        update();
    }
}

void TableModel::update()
{
    beginResetModel();
    _table.clear();
    _columnWidths.clear();
    _columnWidths.resize(_header.size());
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
