#ifndef PAGINATEDVIDEOSOURCE_H
#define PAGINATEDVIDEOSOURCE_H

#include "videosource.h"

class PaginatedVideoSource : public VideoSource {

    Q_OBJECT

public:
    PaginatedVideoSource(QObject *parent = 0);
    virtual bool hasMoreVideos();

    bool maybeReloadToken(int max, int startIndex);
    bool setPageToken(const QString &value);
    bool isPageTokenExpired();
    void reloadToken();
    void setAsyncDetails(bool value) { asyncDetails = value; }
    void loadVideoDetails(const QVector<Video*> &videos);

signals:
    void gotDetails();

protected slots:
    void parseVideoDetails(const QByteArray &bytes);

protected:
    QString nextPageToken;
    uint tokenTimestamp;
    QUrl lastUrl;
    int currentMax;
    int currentStartIndex;
    bool reloadingToken;
    QVector<Video*> videos;
    QHash<QString, Video*> videoMap;
    bool asyncDetails;

};

#endif // PAGINATEDVIDEOSOURCE_H
