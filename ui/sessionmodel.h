#ifndef SESSIONMODEL_H
#define SESSIONMODEL_H

#include <QAbstractTableModel>
#include <QSqlDatabase>

class QFont;

class SessionModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_DISABLE_COPY(SessionModel)

public:
    enum class Role {
        Sort = Qt::UserRole,
    };
    Q_ENUM(Role)

    explicit SessionModel(QObject* parent = nullptr);
    ~SessionModel();

    int rowCount(const QModelIndex & = QModelIndex()) const override;

    int columnCount(const QModelIndex & = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    QVariant headerData(int id, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    Q_INVOKABLE int columnWidth(int c, const QFont *font = nullptr);
    Q_INVOKABLE void update();

private:
    QVector<QVector<QString>> _table;
    QVector<QString> _header;
    QVector<int> _columnWidths;
    QSqlDatabase _db;
    QHash<int, QByteArray> _roleNames;
};

#endif // SESSIONMODEL_H
