#ifndef _SORT_FILTER_SESSION_MODEL_H_
#define _SORT_FILTER_SESSION_MODEL_H_

#include "sessionmodel.h"
#include <QSortFilterProxyModel>
#include <QObject>

class SortFilterSessionModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(SessionModel* sessionModel READ sessionModel CONSTANT)
    Q_CLASSINFO("DefaultProperty", "data")
public:
    explicit SortFilterSessionModel(QObject *parent = nullptr);

    SessionModel* sessionModel() {
        return &_sessionModel;
    }
    Q_INVOKABLE void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
    Q_INVOKABLE int columnWidth(int c, const QFont* font = nullptr);

    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;

private:
    SessionModel _sessionModel;
};

#endif // _SORT_FILTER_SESSION_MODEL_H_
