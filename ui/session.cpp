#include "session.h"
#include <QSqlQuery>
#include <QVariant>

namespace proxy { namespace ui {
    SessionDao::SessionDao()
    {
        _db = QSqlDatabase::addDatabase("QSQLITE");
        _db.setDatabaseName(DB_PATH);

        if (!_db.open()) {
            throw std::runtime_error("There was an error during opening database");
        }
    }

    SessionDao::~SessionDao()
    {
        _db.close();
    }

    void SessionDao::update()
    {
        _data.clear();
        QSqlQuery query(_db);
        query.exec("SELECT * from sessions");
        while (query.next()) {
            QVector<QString> row;
            int i = 1;
            // QString id = query.value(i++).toString();
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

            _data.append(row);
        }
    }

    const QVector<QVector<QString>>& SessionDao::data() const
    {
        return _data;
    }
    const QString& SessionDao::get(int row, int column) const
    {
        return _data[row][column];
    }
    int SessionDao::size() const
    {
        return _data.size();
    }
}} // namespace proxy ui
