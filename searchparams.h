#ifndef SEARCHPARAMS_H
#define SEARCHPARAMS_H

#include <QtCore>

class SearchParams : public QObject {
    Q_OBJECT
    Q_PROPERTY(int sortBy READ sortBy WRITE setSortBy)
    Q_PROPERTY(int duration READ duration WRITE setDuration)
    Q_PROPERTY(int quality READ quality WRITE setQuality)
    Q_PROPERTY(int time READ time WRITE setTime)

public:
    enum SortBy { SortByRelevance = 0, SortByNewest, SortByViewCount, SortByRating };

    enum Duration { DurationAny = 0, DurationShort, DurationMedium, DurationLong };

    enum Quality { QualityAny = 0, QualityHD, Quality4K, QualityHDR };

    enum Time { TimeAny = 0, TimeToday, TimeWeek, TimeMonth, TimeYear };

    enum SafeSearch { None = 0, Moderate, Strict };

    SearchParams(QObject *parent = nullptr);

    const QString &keywords() const { return m_keywords; }
    void setKeywords(const QString &keywords) { m_keywords = keywords; }

    const QString &channelId() const { return m_channelId; }
    void setChannelId(const QString &value) { m_channelId = value; }

    int sortBy() const { return m_sortBy; }
    void setSortBy(int sortBy) { m_sortBy = sortBy; }

    int isTransient() const { return m_transient; }
    void setTransient(int transient) { m_transient = transient; }

    int duration() const { return m_duration; }
    void setDuration(int duration) { m_duration = duration; }

    int quality() const { return m_quality; }
    void setQuality(int quality) { m_quality = quality; }

    int time() const { return m_time; }
    void setTime(int time) { m_time = time; }

    uint publishedAfter() const { return m_publishedAfter; }
    void setPublishedAfter(uint value) { m_publishedAfter = value; }

    int safeSearch() const { return m_safeSearch; }
    void setSafeSearch(int safeSearch) { m_safeSearch = safeSearch; }

    bool operator==(const SearchParams &other) const {
        return m_keywords == other.keywords() && m_channelId == other.channelId();
    }

public slots:
    void setParam(const QString &name, const QVariant &value);

private:
    QString m_keywords;
    QString m_channelId;
    bool m_transient;
    int m_sortBy;
    int m_duration;
    int m_quality;
    int m_time;
    uint m_publishedAfter;
    int m_safeSearch;
};

#endif // SEARCHPARAMS_H
