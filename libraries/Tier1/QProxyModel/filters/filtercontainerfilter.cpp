#include "filtercontainerfilter.h"

namespace QQsfpm {

void FilterContainerFilter::proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel)
{
    for (Filter* filter : std::as_const(m_filters))
        filter->proxyModelCompleted(proxyModel);
}

void FilterContainerFilter::onFilterAppended(Filter* filter)
{
    connect(filter, &Filter::invalidated, this, &FilterContainerFilter::invalidate);
    invalidate();
}

void FilterContainerFilter::onFilterRemoved(Filter* filter)
{
    Q_UNUSED(filter)
    invalidate();
}

void QQsfpm::FilterContainerFilter::onFiltersCleared()
{
    invalidate();
}

}
