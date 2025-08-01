#ifndef FILTERSORTER_H
#define FILTERSORTER_H

#include "sorter.h"
#include "filters/filtercontainer.h"

namespace QQsfpm {

class FilterSorter : public QQsfpm::Sorter, public QQsfpm::FilterContainer
{
    Q_OBJECT
    Q_INTERFACES(QQsfpm::FilterContainer)

    Q_PROPERTY(QQmlListProperty<QQsfpm::Filter> filters READ filtersListProperty CONSTANT FINAL)
    Q_CLASSINFO("DefaultProperty", "filters")

public:
    using QQsfpm::Sorter::Sorter;

protected:
    int compare(const QModelIndex &sourceLeft, const QModelIndex &sourceRight, const QQmlSortFilterProxyModel &proxyModel) const override;

private:
    void proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel) override;
    void onFilterAppended(Filter *filter) override;
    void onFilterRemoved(Filter *filter) override;
    void onFiltersCleared() override;

    bool indexIsAccepted(const QModelIndex &sourceIndex, const QQmlSortFilterProxyModel &proxyModel) const;
};

}

#endif // FILTERSORTER_H
