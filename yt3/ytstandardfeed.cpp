#include "ytstandardfeed.h"
#include "http.h"
#include "httputils.h"
#include "video.h"

#include "yt3.h"
#include "yt3listparser.h"

YTStandardFeed::YTStandardFeed(QObject *parent)
    : PaginatedVideoSource(parent),
      aborted(false) { }

void YTStandardFeed::loadVideos(int max, int startIndex) {
    aborted = false;

    QUrl url = YT3::instance().method("videos");

    QUrlQuery q(url);
    if (startIndex > 1) {
        if (maybeReloadToken(max, startIndex)) return;
        q.addQueryItem("pageToken", nextPageToken);
    }

    q.addQueryItem("part", "snippet,contentDetails,statistics");
    q.addQueryItem("chart", "mostPopular");

    if (!category.isEmpty())
        q.addQueryItem("videoCategoryId", category);

    if (!regionId.isEmpty())
        q.addQueryItem("regionCode", regionId);

    q.addQueryItem("maxResults", QString::number(max));

    url.setQuery(q);

    QObject *reply = HttpUtils::yt().get(url);
    qDebug() << url;
    connect(reply, SIGNAL(data(QByteArray)), SLOT(parseResults(QByteArray)));
    connect(reply, SIGNAL(error(QString)), SLOT(requestError(QString)));
}

void YTStandardFeed::parseResults(QByteArray data) {
    if (aborted) return;

    YT3ListParser parser(data);
    const QVector<Video*> &videos = parser.getVideos();

    bool tryingWithNewToken = setPageToken(parser.getNextPageToken());
    if (tryingWithNewToken) return;

    if (reloadingToken) {
        reloadingToken = false;
        loadVideos(currentMax, currentStartIndex);
        currentMax = currentStartIndex = 0;
        return;
    }

    emit gotVideos(videos);
    emit finished(videos.size());
}

void YTStandardFeed::abort() {
    aborted = true;
}

void YTStandardFeed::requestError(const QString &message) {
    emit error(message);
}
