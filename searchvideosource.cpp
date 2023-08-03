#include "searchvideosource.h"

#include "searchparams.h"
#include "video.h"
#include "videoapi.h"

#ifdef YT_YT3
#include "ytsearch.h"
#endif

#ifdef YT_IV
#include "ivchannelsource.h"
#include "ivsearch.h"
#include "ivsinglevideosource.h"
#endif

#include "ytjschannelsource.h"
#include "ytjssearch.h"
#include "ytjssinglevideosource.h"

SearchVideoSource::SearchVideoSource(SearchParams *searchParams, QObject *parent)
    : VideoSource(parent), searchParams(searchParams) {}

void SearchVideoSource::loadVideos(int max, int startIndex) {
    if (!source) {
        aborted = false;

        if (VideoAPI::impl() == VideoAPI::JS) {
            if (searchParams->channelId().isEmpty()) {
                source = new YTJSSearch(searchParams);
            } else {
                source = new YTJSChannelSource(searchParams);
            }
        }
#ifdef YT_YT3
        else if (VideoAPI::impl() == VideoAPI::YT3) {
            YTSearch *ytSearch = new YTSearch(searchParams);
            ytSearch->setAsyncDetails(true);
            // --- connect(ytSearch, SIGNAL(gotDetails()), playlistModel, SLOT(emitDataChanged()));
            source = ytSearch;
        }
#endif
#ifdef YT_IV
        else if (VideoAPI::impl() == VideoAPI::IV) {
            if (searchParams->channelId().isEmpty()) {
                source = new IVSearch(searchParams);
            } else {
                source = new IVChannelSource(searchParams);
            }
        }
#endif
        connectSource(max, startIndex);
    }
    source->loadVideos(max, startIndex);
}

bool SearchVideoSource::hasMoreVideos() {
    if (source) return source->hasMoreVideos();
    return VideoSource::hasMoreVideos();
}

QString SearchVideoSource::getName() {
    if (source) return source->getName();
    return QString();
}

const QList<QAction *> &SearchVideoSource::getActions() {
    if (source) return source->getActions();
    return VideoSource::getActions();
}

int SearchVideoSource::maxResults() {
    if (source) return source->maxResults();
    return VideoSource::maxResults();
}

void SearchVideoSource::connectSource(int max, int startIndex) {
    connect(source, &VideoSource::nameChanged, this, &VideoSource::nameChanged);
    connect(source, &VideoSource::finished, this, &VideoSource::finished);
    connect(source, &VideoSource::gotVideos, this, [this](auto &videos) {
        if (aborted) return;
        emit gotVideos(videos);
    });
    connect(source, &VideoSource::error, this, [this, max, startIndex](auto msg) {
        qDebug() << source << msg;
        if (aborted) return;

#ifdef YT_IV
        if (QLatin1String(source->metaObject()->className()).startsWith(QLatin1String("YTJS"))) {
            qDebug() << "Falling back to IV";
            source->deleteLater();
            if (searchParams->channelId().isEmpty()) {
                source = new IVSearch(searchParams);
            } else {
                source = new IVChannelSource(searchParams);
            }
            connectSource(max, startIndex);
            source->loadVideos(max, startIndex);
            return;
        }
#endif
        emit error(msg);
    });
}
