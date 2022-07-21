#include "sortfiltersessionmodel.h"
#include "session.h"

SortFilterSessionModel::SortFilterSessionModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{
    setSourceModel(&_sessionModel);
}

void SortFilterSessionModel::sort(int column, Qt::SortOrder order)
{
    QSortFilterProxyModel::sort(column, order);
}

int SortFilterSessionModel::columnWidth(int c, const QFont *font)
{
    return _sessionModel.columnWidth(c, font);
}

int SortFilterSessionModel::tableWidth(const QFont *font)
{
    return _sessionModel.tableWidth(font);
}

Qt::SortOrder SortFilterSessionModel::initialSortOrder(int column) const
{
    if (column >= F_BYTES_SENT)
        return Qt::DescendingOrder;
    return Qt::AscendingOrder;
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
}

bool SortFilterSessionModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const {
    QModelIndex userIndex = sourceModel()->index(sourceRow, 0, sourceParent);
    QString username = sourceModel()->data(userIndex).toString();
    return username.contains(_userFilter);
}
