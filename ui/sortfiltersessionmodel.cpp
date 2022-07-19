#include "sortfiltersessionmodel.h"

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

QModelIndex SortFilterSessionModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    return QSortFilterProxyModel::mapFromSource(sourceIndex);
}

QModelIndex SortFilterSessionModel::mapToSource(const QModelIndex &proxyIndex) const
{
    return QSortFilterProxyModel::mapToSource(proxyIndex);
}
