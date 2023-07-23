#ifndef YTSTANDARDFEED_H
#define YTSTANDARDFEED_H

#include <QtNetwork>
#include "paginatedvideosource.h"

class YTStandardFeed : public PaginatedVideoSource {

    Q_OBJECT

public:
    YTStandardFeed(QObject *parent = 0);

    QString getFeedId() { return feedId; }
    void setFeedId(const QString &value) { feedId = value; }

    QString getRegionId() { return regionId; }
    void setRegionId(const QString &value) { regionId = value; }

    QString getCategory() { return category; }
    void setCategory(const QString &value) { category = value; }

    QString getLabel() { return label; }
    void setLabel(const QString &value) { label = value; }

    QString getTime() { return time; }
    void setTime(const QString &value) { time = value; }

    void loadVideos(int max, int startIndex);
    void abort();
    QString getName() { return label; }

private slots:
    void parseResults(QByteArray data);
    void requestError(const QString &message);

private:
    QString feedId;
    QString regionId;
    QString category;
    QString label;
    QString time;
    bool aborted;
};

#endif // YTSTANDARDFEED_H
