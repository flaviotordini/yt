#ifndef YTSINGLEVIDEOSOURCE_H
#define YTSINGLEVIDEOSOURCE_H

#include <QtNetwork>
#include "paginatedvideosource.h"

class YTSingleVideoSource : public PaginatedVideoSource {

    Q_OBJECT

public:
    YTSingleVideoSource(QObject *parent = 0);
    void loadVideos(int max, int startIndex);
    void abort();
    QString getName();

    void setVideoId(const QString &value) { videoId = value; }
    void setVideo(Video *video);

private slots:
    void parseResults(QByteArray data);
    void requestError(const QString &message);

private:
    Video *video;
    QString videoId;
    bool aborted;
    int startIndex;
    int max;
    QString name;
};

#endif // YTSINGLEVIDEOSOURCE_H
