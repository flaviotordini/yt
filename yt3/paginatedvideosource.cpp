#include "paginatedvideosource.h"

#include "yt3.h"
#include "yt3listparser.h"
#include "datautils.h"

#include "video.h"
#include "http.h"
#include "httputils.h"

PaginatedVideoSource::PaginatedVideoSource(QObject *parent) : VideoSource(parent)
  , tokenTimestamp(0)
  , reloadingToken(false)
  , currentMax(0)
  , currentStartIndex(0)
  , asyncDetails(false) { }

bool PaginatedVideoSource::hasMoreVideos() {
    qDebug() << __PRETTY_FUNCTION__ << nextPageToken;
    return !nextPageToken.isEmpty();
}

bool PaginatedVideoSource::maybeReloadToken(int max, int startIndex) {
    // kind of hackish. Thank the genius who came up with this stateful stuff
    // in a supposedly RESTful (aka stateless) API.

    if (nextPageToken.isEmpty()) {
        // previous request did not return a page token. Game over.
        // emit gotVideos(QVector<Video*>());
        emit finished(0);
        return true;
    }

    if (isPageTokenExpired()) {
        reloadingToken = true;
        currentMax = max;
        currentStartIndex = startIndex;
        reloadToken();
        return true;
    }
    return false;
}

bool PaginatedVideoSource::setPageToken(const QString &value) {
    tokenTimestamp = QDateTime::currentSecsSinceEpoch();
    nextPageToken = value;

    if (reloadingToken) {
        reloadingToken = false;
        loadVideos(currentMax, currentStartIndex);
        currentMax = currentStartIndex = 0;
        return true;
    }

    return false;
}

bool PaginatedVideoSource::isPageTokenExpired() {
    uint now = QDateTime::currentSecsSinceEpoch();
    return now - tokenTimestamp > 1800;
}

void PaginatedVideoSource::reloadToken() {
    qDebug() << "Reloading pageToken";
    QObject *reply = HttpUtils::yt().get(lastUrl);
    connect(reply, SIGNAL(data(QByteArray)), SLOT(parseResults(QByteArray)));
    connect(reply, SIGNAL(error(QString)), SLOT(requestError(QString)));
}

void PaginatedVideoSource::loadVideoDetails(const QVector<Video*> &videos) {
    this->videos = videos;
    QString videoIds;
    videoIds.reserve(videos.size()*12);
    videoMap.reserve(videos.size());
    for (Video *video : videos) {
        // TODO get video details from cache
        if (!videoIds.isEmpty()) videoIds += QLatin1Char(',');
        videoIds += video->getId();
        videoMap.insert(video->getId(), video);
    }

    if (videoIds.isEmpty()) {
        if (!asyncDetails) {
            emit gotVideos(videos);
            emit finished(videos.size());
        }
        return;
    }

    QUrl url = YT3::instance().method("videos");
    QUrlQuery q(url);
    q.addQueryItem(QStringLiteral("part"), QStringLiteral("contentDetails,statistics"));
    q.addQueryItem(QStringLiteral("id"), videoIds);
    url.setQuery(q);

    QObject *reply = HttpUtils::yt().get(url);
    connect(reply, SIGNAL(data(QByteArray)), SLOT(parseVideoDetails(QByteArray)));
    connect(reply, SIGNAL(error(QString)), SLOT(requestError(QString)));
}

void PaginatedVideoSource::parseVideoDetails(const QByteArray &bytes) {
    QJsonDocument doc = QJsonDocument::fromJson(bytes);
    QJsonObject obj = doc.object();

    QJsonValue items = obj["items"];
    if (items.isArray()) {
        const auto array = items.toArray();
        for (const QJsonValue &v : array) {
            if (!v.isObject()) continue;

            QJsonObject item = v.toObject();

            QString id = item["id"].toString();
            Video *video = videoMap.value(id);
            if (!video) {
                qWarning() << "No video for id" << id;
                continue;
            }

            QString isoPeriod = item["contentDetails"].toObject()["duration"].toString();
            int duration = DataUtils::parseIsoPeriod(isoPeriod);
            video->setDuration(duration);

            int viewCount = item["statistics"].toObject()["viewCount"].toString().toInt();
            video->setViewCount(viewCount);

            // TODO cache by etag?
        }
    }
    if (!asyncDetails) {
        emit gotVideos(videos);
        emit finished(videos.size());
    } else {
        emit gotDetails();
    }
}
