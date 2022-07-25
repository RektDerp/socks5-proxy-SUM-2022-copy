#include "sessionmodel.h"
#include "session.h"
#include "qcoreevent.h"
#include "qwindowdefs.h"
#include <QDebug>
#include <qguiapplication.h>
#include <QFontMetrics>

namespace proxy { namespace ui {
    SessionModel::SessionModel(QObject *parent)
        : QAbstractTableModel{parent}
    {
        _header.append({"Username", "Create date", "Update date", "Is active?", "Client [ip:port]",
                        "Server [ip:port]", "Sent bytes", "Recv bytes"});
        _columnWidths.resize(_header.size());
        update();
    }

    int SessionModel::rowCount(const QModelIndex &) const
    {
        return _sessions.size();
    }

    int SessionModel::columnCount(const QModelIndex &) const
    {
        return F_BYTES_RECV + 1;
    }

    QVariant SessionModel::data(const QModelIndex &index, int role) const
    {
        fields field = fields(index.column());
        if (role == Qt::DisplayRole) {
            QVariant ret = _sessions.get(index.row(), field);
            switch (field) {
            case F_USER:
            case F_IS_ACTIVE:
            case F_SRC_ENDPOINT:
            case F_DST_ENDPOINT:
            case F_CREATE_DATE:
            case F_UPDATE_DATE:
                return ret;
            case F_BYTES_SENT:
            case F_BYTES_RECV:
                return ret.toInt();
            default:
                return QVariant();
            }
        }
        return QVariant();
    }

    QHash<int, QByteArray> SessionModel::roleNames() const
    {
        return { {Qt::DisplayRole, "display"} };
    }

    QVariant SessionModel::headerData(int id, Qt::Orientation orientation, int role) const
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

    int SessionModel::columnWidth(int c)
    {
        if (c < 0 || c >= _columnWidths.size()) {
            return 0;
        }
        if (!_columnWidths[c]) {
            QFontMetrics fm = QFontMetrics(QGuiApplication::font());
            int ret = fm.horizontalAdvance(headerData(c, Qt::Horizontal).toString()) + 8;

            for (int r = 0; r < _sessions.size(); r++) {
                ret = qMax(ret, fm.horizontalAdvance(_sessions.get(r, c)));
            }
            _columnWidths[c] = ret + 10;
        }
        return _columnWidths[c];
    }

    int SessionModel::tableWidth()
    {
        int sum = 0;
        for (int c = 0; c < _columnWidths.size(); c++)
            sum += columnWidth(c);
        return sum;
    }

    void SessionModel::update()
    {
        beginResetModel();
        _columnWidths.clear();
        _columnWidths.resize(_header.size());
        _sessions.update();
        endResetModel();
    }
}} // namespace proxy ui
