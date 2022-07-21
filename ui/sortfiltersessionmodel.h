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
    Q_PROPERTY(QString userFilter READ userFilter WRITE setUserFilter NOTIFY filterChanged)
    Q_PROPERTY(QDate createDateFilter READ createDateFilter WRITE setCreateDateFilter NOTIFY filterChanged)
    Q_PROPERTY(QDate updateDateFilter READ updateDateFilter WRITE setUpdateDateFilter NOTIFY filterChanged)
    Q_PROPERTY(QDate fromDateFilter READ fromDateFilter WRITE setFromDateFilter NOTIFY filterChanged)
    Q_PROPERTY(QDate toDateFilter READ toDateFilter WRITE setToDateFilter NOTIFY filterChanged)
    Q_PROPERTY(QString isActiveFilter READ isActiveFilter WRITE setIsActiveFilter NOTIFY filterChanged)
    Q_PROPERTY(QString srcEndpointFilter READ srcEndpointFilter WRITE setSrcEndpointFilter NOTIFY filterChanged)
    Q_PROPERTY(QString dstEndpointFilter READ dstEndpointFilter WRITE setDstEndpointFilter NOTIFY filterChanged)
    Q_PROPERTY(int bytesSentFilter READ bytesSentFilter WRITE setBytesSentFilter NOTIFY filterChanged)
    Q_PROPERTY(int bytesRecvFilter READ bytesRecvFilter WRITE setBytesRecvFilter NOTIFY filterChanged)
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
    void setIsActiveFilter(QString isActive);
    QString isActiveFilter() {
        return _isActiveFilter;
    }
    void setSrcEndpointFilter(QString srcEndpoint);
    QString srcEndpointFilter() {
        return _srcEndpointFilter;
    }
    void setDstEndpointFilter(QString dstEndpoint);
    QString dstEndpointFilter() {
        return _dstEndpointFilter;
    }
    void setBytesSentFilter(int bytesSent);
    int bytesSentFilter() {
        return _bytesSentFilter;
    }
    void setBytesRecvFilter(int bytesRecv);
    int bytesRecvFilter() {
        return _bytesRecvFilter;
    }
signals:
    void filterChanged();
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
    QString _isActiveFilter;
    QString _srcEndpointFilter;
    QString _dstEndpointFilter;
    int _bytesSentFilter;
    int _bytesRecvFilter;
};

#endif // _SORT_FILTER_SESSION_MODEL_H_
