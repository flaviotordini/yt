#include "searchparams.h"

SearchParams::SearchParams(QObject *parent) : QObject(parent) {
    m_transient = false;
    m_sortBy = SortByRelevance;
    m_duration = DurationAny;
    m_quality = QualityAny;
    m_time = TimeAny;
    m_publishedAfter = 0;
    m_safeSearch = Moderate;
}

void SearchParams::setParam(const QString &name, const QVariant &value) {
    bool success = setProperty(name.toUtf8(), value);
    if (!success) qWarning() << "Failed to set property" << name << value.toString();
}
