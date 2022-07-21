#ifndef _SORT_FILTER_SESSION_MODEL_H_
#define _SORT_FILTER_SESSION_MODEL_H_

#include "sessionmodel.h"
#include <QSortFilterProxyModel>
#include <QObject>
#include <QDate>

class SortFilterSessionModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(SessionModel* sessionModel READ sessionModel CONSTANT)
    Q_PROPERTY(QString userFilter READ userFilter WRITE setUserFilter)
    Q_PROPERTY(QDate createDateFilter READ createDateFilter WRITE setCreateDateFilter)
    Q_PROPERTY(QDate updateDateFilter READ updateDateFilter WRITE setUpdateDateFilter)
    Q_PROPERTY(QDate fromDateFilter READ fromDateFilter WRITE setFromDateFilter)
    Q_PROPERTY(QDate toDateFilter READ toDateFilter WRITE setToDateFilter)
    Q_CLASSINFO("DefaultProperty", "data")
public:
    explicit SortFilterSessionModel(QObject *parent = nullptr);

    SessionModel* sessionModel() {
        return &_sessionModel;
    }

    Q_INVOKABLE void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
    Q_INVOKABLE int columnWidth(int c, const QFont* font = nullptr);
    Q_INVOKABLE int tableWidth(const QFont* font = nullptr);
    Q_INVOKABLE Qt::SortOrder initialSortOrder(int column) const;

    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;

    // filters
    void setUserFilter(QString filterText);
    QString userFilter() {
        return _userFilter;
    }
    void setCreateDateFilter(QDate createDate);
    QDate createDateFilter() {
        return _createDateFilter;
    }
    void setUpdateDateFilter(QDate updateDate);
    QDate updateDateFilter() {
        return _updateDateFilter;
    }
    void setFromDateFilter(QDate fromDate);
    QDate fromDateFilter() {
        return _fromDateFilter;
    }
    void setToDateFilter(QDate toDate);
    QDate toDateFilter() {
        return _toDateFilter;
    }
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const  override;
private:
    SessionModel _sessionModel;
    // filters
    QString _userFilter;
    QDate _createDateFilter;
    QDate _updateDateFilter;
    QDate _fromDateFilter;
    QDate _toDateFilter;
};

#endif // _SORT_FILTER_SESSION_MODEL_H_
