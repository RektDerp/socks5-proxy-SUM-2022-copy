#ifndef _SORT_FILTER_SESSION_MODEL_H_
#define _SORT_FILTER_SESSION_MODEL_H_

#include "sessionmodel.h"
#include <QSortFilterProxyModel>
#include <QObject>

class SortFilterSessionModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(SessionModel* sessionModel READ sessionModel CONSTANT)
    Q_PROPERTY(QString filterText READ filterText WRITE setFilterText NOTIFY filterTextChanged)
    Q_CLASSINFO("DefaultProperty", "data")
public:
    explicit SortFilterSessionModel(QObject *parent = nullptr);

    SessionModel* sessionModel() {
        return &_sessionModel;
    }
    QString filterText() {
        return _filterText;
    }
    void setFilterText(QString filterText);
    Q_INVOKABLE void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
    Q_INVOKABLE int columnWidth(int c, const QFont* font = nullptr);
    Q_INVOKABLE int tableWidth(const QFont* font = nullptr);
    Q_INVOKABLE Qt::SortOrder initialSortOrder(int column) const;

    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
signals:
    void filterTextChanged(QString filterText);
private:
    SessionModel _sessionModel;
    QString _filterText;
};

#endif // _SORT_FILTER_SESSION_MODEL_H_
