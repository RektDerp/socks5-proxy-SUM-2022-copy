#include "sortfiltersessionmodel.h"
#include "session.h"

namespace proxy { namespace ui {
    SortFilterSessionModel::SortFilterSessionModel(QObject *parent)
        : QSortFilterProxyModel{parent},
          _bytesSentFilter(-1),
          _bytesRecvFilter(-1)
    {
        setSourceModel(&_sessionModel);
    }

    void SortFilterSessionModel::sort(int column, Qt::SortOrder order)
    {
        QSortFilterProxyModel::sort(column, order);
    }

    int SortFilterSessionModel::columnWidth(int c)
    {
        return _sessionModel.columnWidth(c);
    }

    int SortFilterSessionModel::tableWidth()
    {
        return _sessionModel.tableWidth();
    }

    Qt::SortOrder SortFilterSessionModel::initialSortOrder(int column) const
    {
        switch (column) {
        case F_USER:
        case F_SRC_ENDPOINT:
        case F_DST_ENDPOINT:
            return Qt::AscendingOrder;
        default:
            return Qt::DescendingOrder;
        }
    }

    QModelIndex SortFilterSessionModel::mapFromSource(const QModelIndex &sourceIndex) const
    {
        return QSortFilterProxyModel::mapFromSource(sourceIndex);
    }

    QModelIndex SortFilterSessionModel::mapToSource(const QModelIndex &proxyIndex) const
    {
        return QSortFilterProxyModel::mapToSource(proxyIndex);
    }

    void SortFilterSessionModel::setUserFilter(QString filterText)
    {
        if (_userFilter == filterText) {
            return;
        }
        _userFilter = filterText;
        invalidateFilter();
        emit filterChanged();
    }

    void SortFilterSessionModel::setCreateDateFilter(QDate createDate)
    {
        if (createDate.year() < 1970) {
            createDate = QDate();
        }
        if (_createDateFilter == createDate) {
            return;
        }
        _createDateFilter = createDate;
        invalidateFilter();
        emit filterChanged();
    }

    void SortFilterSessionModel::setUpdateDateFilter(QDate updateDate)
    {
        if (updateDate.year() < 1970) {
            updateDate = QDate();
        }
        if (_updateDateFilter == updateDate) {
            return;
        }
        _updateDateFilter = updateDate;
        invalidateFilter();
        emit filterChanged();
    }

    void SortFilterSessionModel::setFromDateFilter(QDate fromDate)
    {
        if (fromDate.year() < 1970) {
            fromDate = QDate();
        }
        if (_fromDateFilter == fromDate) {
            return;
        }
        _fromDateFilter = fromDate;
        invalidateFilter();
        emit filterChanged();
    }

    void SortFilterSessionModel::setToDateFilter(QDate toDate)
    {
        if (toDate.year() < 1970) {
            toDate = QDate();
        }
        if (_toDateFilter == toDate) {
            return;
        }
        _toDateFilter = toDate;
        invalidateFilter();
        emit filterChanged();
    }

    void SortFilterSessionModel::setIsActiveFilter(QString isActive)
    {
        if (_isActiveFilter == isActive) {
            return;
        }
        _isActiveFilter = isActive;
        invalidateFilter();
        emit filterChanged();
    }

    void SortFilterSessionModel::setSrcEndpointFilter(QString srcEndpoint)
    {
        if (_srcEndpointFilter == srcEndpoint) {
            return;
        }
        _srcEndpointFilter = srcEndpoint;
        invalidateFilter();
        emit filterChanged();
    }

    void SortFilterSessionModel::setDstEndpointFilter(QString dstEndpoint)
    {
        if (_dstEndpointFilter == dstEndpoint) {
            return;
        }
        _dstEndpointFilter = dstEndpoint;
        invalidateFilter();
        emit filterChanged();
    }

    void SortFilterSessionModel::setBytesSentFilter(int bytesSent)
    {
        if (_bytesSentFilter == bytesSent) {
            return;
        }
        _bytesSentFilter = bytesSent;
        invalidateFilter();
        emit filterChanged();
    }

    void SortFilterSessionModel::setBytesRecvFilter(int bytesRecv)
    {
        if (_bytesRecvFilter == bytesRecv) {
            return;
        }
        _bytesRecvFilter = bytesRecv;
        invalidateFilter();
        emit filterChanged();
    }

    bool SortFilterSessionModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const {
        int index = 0;
        QModelIndex userIndex = sourceModel()->index(sourceRow, index++, sourceParent);
        QString username = sourceModel()->data(userIndex).toString();
        QModelIndex createDateIndex = sourceModel()->index(sourceRow, index++, sourceParent);
        QDate createDate = sourceModel()->data(createDateIndex).toDate();
        QModelIndex updateDateIndex = sourceModel()->index(sourceRow, index++, sourceParent);
        QDate updateDate = sourceModel()->data(updateDateIndex).toDate();
        QModelIndex isActiveIndex = sourceModel()->index(sourceRow, index++, sourceParent);
        QString isActive = sourceModel()->data(isActiveIndex).toString();
        QModelIndex srcEndpointIndex = sourceModel()->index(sourceRow, index++, sourceParent);
        QString srcEndpoint = sourceModel()->data(srcEndpointIndex).toString();
        QModelIndex dstEndpointIndex = sourceModel()->index(sourceRow, index++, sourceParent);
        QString dstEndpoint = sourceModel()->data(dstEndpointIndex).toString();
        QModelIndex bytesSentIndex = sourceModel()->index(sourceRow, index++, sourceParent);
        int bytesSent = sourceModel()->data(bytesSentIndex).toInt();
        QModelIndex bytesRecvIndex = sourceModel()->index(sourceRow, index++, sourceParent);
        int bytesRecv = sourceModel()->data(bytesRecvIndex).toInt();
        return username.contains(_userFilter)
                && (!_createDateFilter.isValid() || createDate == _createDateFilter)
                && (!_updateDateFilter.isValid() || updateDate == _updateDateFilter)
                && (!_fromDateFilter.isValid()   || updateDate >= _fromDateFilter)
                && (!_toDateFilter.isValid()   || updateDate <= _toDateFilter)
                && isActive.contains(_isActiveFilter)
                && srcEndpoint.contains(_srcEndpointFilter)
                && dstEndpoint.contains(_dstEndpointFilter)
                && (_bytesSentFilter == -1 || bytesSent == _bytesSentFilter)
                && (_bytesRecvFilter == -1 || bytesRecv == _bytesRecvFilter);
    }
}} // namespace proxy ui
