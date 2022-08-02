#ifndef SESSION_H
#define SESSION_H
#include <QString>
#include <QSqlDatabase>
#include <QVector>

namespace proxy { namespace ui {
    enum fields {
        F_USER = 0,
        F_CREATE_DATE,
        F_UPDATE_DATE,
        F_IS_ACTIVE,
        F_SRC_ENDPOINT,
        F_DST_ENDPOINT,
        F_BYTES_SENT,
        F_BYTES_RECV
    };

#ifdef __linux__
    const char* const DB_PATH = "/tmp/sessions_stat.db";
#else
    const char* const DB_PATH = "./statistics.db";
#endif

    class SessionDao {
    public:
        SessionDao();
        ~SessionDao();
        void update();
        const QVector<QVector<QString>>& data() const;
        const QString& get(int row, int column) const;
        int size() const;
    private:
        QVector<QVector<QString>> _data;
        QSqlDatabase _db;
    };
}} // namespace proxy ui
#endif // SESSION_H
