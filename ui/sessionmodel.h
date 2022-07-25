#ifndef SESSIONMODEL_H
#define SESSIONMODEL_H
#include "session.h"
#include <QAbstractTableModel>

namespace proxy { namespace ui {
    class SessionModel : public QAbstractTableModel
    {
        Q_OBJECT
        Q_DISABLE_COPY(SessionModel)
    public:
        explicit SessionModel(QObject* parent = nullptr);
        ~SessionModel() = default;

        int rowCount(const QModelIndex & = QModelIndex()) const override;

        int columnCount(const QModelIndex & = QModelIndex()) const override;

        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

        QHash<int, QByteArray> roleNames() const override;

        QVariant headerData(int id, Qt::Orientation orientation,
                            int role = Qt::DisplayRole) const override;

        Q_INVOKABLE int columnWidth(int c);
        Q_INVOKABLE int tableWidth();
        Q_INVOKABLE void update();
    private:
        SessionDao _sessions;
        QVector<QString> _header;
        QVector<int> _columnWidths;
    };
}} // namespace proxy ui
#endif // SESSIONMODEL_H
