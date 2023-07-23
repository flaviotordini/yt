#include "ytsearch.h"
#include "constants.h"
#include "http.h"
#include "httputils.h"
#include "searchparams.h"
#include "video.h"
#include "videoapi.h"

#include "datautils.h"
#include "mainwindow.h"
#include "yt3.h"
#include "yt3listparser.h"

namespace {

QString RFC3339toString(const QDateTime &dt) {
    return dt.toString(QStringLiteral("yyyy-MM-ddThh:mm:ssZ"));
}
}

YTSearch::YTSearch(SearchParams *searchParams, QObject *parent)
    : PaginatedVideoSource(parent), searchParams(searchParams) {
    searchParams->setParent(this);
}

void YTSearch::loadVideos(int max, int startIndex) {
    aborted = false;

    QUrl url = YT3::instance().method("search");

    QUrlQuery q(url);
    q.addQueryItem("part", "snippet");
    q.addQueryItem("type", "video");
    q.addQueryItem("maxResults", QString::number(max));

    if (startIndex > 1) {
        if (maybeReloadToken(max, startIndex)) return;
        q.addQueryItem("pageToken", nextPageToken);
    }

    // TODO interesting params
    // urlHelper.addQueryItem("videoSyndicated", "true");
    // urlHelper.addQueryItem("regionCode", "IT");
    // urlHelper.addQueryItem("videoType", "movie");

    if (!searchParams->keywords().isEmpty()) {
        if (searchParams->keywords().startsWith("http://") ||
            searchParams->keywords().startsWith("https://")) {
            q.addQueryItem("q", VideoAPI::videoIdFromUrl(searchParams->keywords()));
        } else
            q.addQueryItem("q", searchParams->keywords());
    }

    if (!searchParams->channelId().isEmpty())
        q.addQueryItem("channelId", searchParams->channelId());

    switch (searchParams->sortBy()) {
    case SearchParams::SortByNewest:
        q.addQueryItem("order", "date");
        break;
    case SearchParams::SortByViewCount:
        q.addQueryItem("order", "viewCount");
        break;
    case SearchParams::SortByRating:
        q.addQueryItem("order", "rating");
        break;
    }

    switch (searchParams->duration()) {
    case SearchParams::DurationShort:
        q.addQueryItem("videoDuration", "short");
        break;
    case SearchParams::DurationMedium:
        q.addQueryItem("videoDuration", "medium");
        break;
    case SearchParams::DurationLong:
        q.addQueryItem("videoDuration", "long");
        break;
    }

    switch (searchParams->time()) {
    case SearchParams::TimeToday:
        q.addQueryItem("publishedAfter",
                       RFC3339toString(QDateTime::currentDateTimeUtc().addSecs(-60 * 60 * 24)));
        break;
    case SearchParams::TimeWeek:
        q.addQueryItem("publishedAfter",
                       RFC3339toString(QDateTime::currentDateTimeUtc().addSecs(-60 * 60 * 24 * 7)));
        break;
    case SearchParams::TimeMonth:
        q.addQueryItem("publishedAfter", RFC3339toString(QDateTime::currentDateTimeUtc().addSecs(
                                                 -60 * 60 * 24 * 30)));
        break;
    }

    if (searchParams->publishedAfter()) {
        q.addQueryItem("publishedAfter", RFC3339toString(QDateTime::fromSecsSinceEpoch(
                                                 searchParams->publishedAfter(), Qt::UTC)));
    }

    switch (searchParams->quality()) {
    case SearchParams::QualityHD:
        q.addQueryItem("videoDefinition", "high");
        break;
    }

    switch (searchParams->safeSearch()) {
    case SearchParams::None:
        q.addQueryItem("safeSearch", "none");
        break;
    case SearchParams::Strict:
        q.addQueryItem("safeSearch", "strict");
        break;
    }

    url.setQuery(q);

    lastUrl = url;

    // qWarning() << "YT3 search" << url.toString();
    QObject *reply = HttpUtils::yt().get(url);
    connect(reply, SIGNAL(data(QByteArray)), SLOT(parseResults(QByteArray)));
    connect(reply, SIGNAL(error(QString)), SLOT(requestError(QString)));
}

void YTSearch::parseResults(const QByteArray &data) {
    if (aborted) return;

    YT3ListParser parser(data);
    const QVector<Video *> &videos = parser.getVideos();

    bool tryingWithNewToken = setPageToken(parser.getNextPageToken());
    if (tryingWithNewToken) return;

    if (name.isEmpty() && !searchParams->channelId().isEmpty()) {
        if (!videos.isEmpty()) {
            name = videos.at(0)->getChannelTitle();
            if (!searchParams->keywords().isEmpty()) {
                name += QLatin1String(": ") + searchParams->keywords();
            }
        }
        emit nameChanged(name);
    }

    if (asyncDetails) {
        emit gotVideos(videos);
        emit finished(videos.size());
    }
    loadVideoDetails(videos);
}

void YTSearch::abort() {
    aborted = true;
}

QString YTSearch::getName() {
    if (!name.isEmpty()) return name;
    if (!searchParams->keywords().isEmpty()) return searchParams->keywords();
    return QString();
}

void YTSearch::requestError(const QString &message) {
    QString msg = message;
    msg.remove(QRegularExpression("key=[^ &]+"));
    emit error(msg);
}

const QList<QAction *> &YTSearch::getActions() {
    static const QList<QAction *> channelActions = {
            MainWindow::instance()->getAction("subscribeChannel")};
    if (searchParams->channelId().isEmpty()) {
        static const QList<QAction *> noActions;
        return noActions;
    }
    return channelActions;
}
