#include "ytsinglevideosource.h"
#include "http.h"
#include "httputils.h"
#include "video.h"

#include "yt3.h"
#include "yt3listparser.h"

YTSingleVideoSource::YTSingleVideoSource(QObject *parent)
    : PaginatedVideoSource(parent), video(nullptr), startIndex(0), max(0) {}

void YTSingleVideoSource::loadVideos(int max, int startIndex) {
    aborted = false;
    this->startIndex = startIndex;
    this->max = max;

    QUrl url;

    if (startIndex == 1) {
        if (video) {
            QVector<Video *> videos;
            videos << video->clone();
            if (name.isEmpty()) {
                name = videos.at(0)->getTitle();
                qDebug() << "Emitting name changed" << name;
                emit nameChanged(name);
            }
            emit gotVideos(videos);
            loadVideos(max - 1, 2);
            return;
        }

        url = YT3::instance().method("videos");
        QUrlQuery q(url);
        q.addQueryItem("part", "snippet");
        q.addQueryItem("id", videoId);
        url.setQuery(q);

    } else {
        url = YT3::instance().method("search");
        QUrlQuery q(url);
        q.addQueryItem("part", "snippet");
        q.addQueryItem("type", "video");
        q.addQueryItem("relatedToVideoId", videoId);
        q.addQueryItem("maxResults", QString::number(max));
        if (startIndex > 2) {
            if (maybeReloadToken(max, startIndex)) return;
            q.addQueryItem("pageToken", nextPageToken);
        }
        url.setQuery(q);
    }

    lastUrl = url;

    QObject *reply = HttpUtils::yt().get(url);
    connect(reply, SIGNAL(data(QByteArray)), SLOT(parseResults(QByteArray)));
    connect(reply, SIGNAL(error(QString)), SLOT(requestError(QString)));
}

void YTSingleVideoSource::parseResults(QByteArray data) {
    if (aborted) return;

    YT3ListParser parser(data);
    const QVector<Video *> &videos = parser.getVideos();

    bool tryingWithNewToken = setPageToken(parser.getNextPageToken());
    if (tryingWithNewToken) return;

    if (asyncDetails) {
        emit gotVideos(videos);
        if (startIndex == 2)
            emit finished(videos.size() + 1);
        else
            emit finished(videos.size());
    }
    loadVideoDetails(videos);
}

void YTSingleVideoSource::abort() {
    aborted = true;
}

QString YTSingleVideoSource::getName() {
    return name;
}

void YTSingleVideoSource::setVideo(Video *video) {
    this->video = video;
    videoId = video->getId();
}

void YTSingleVideoSource::requestError(const QString &message) {
    emit error(message);
}
