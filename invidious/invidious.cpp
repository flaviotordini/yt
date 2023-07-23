#include "invidious.h"

#include "cachedhttp.h"
#include "http.h"
#include "httputils.h"
#include "jsfunctions.h"
#include "throttledhttp.h"

#ifdef APP_EXTRA
#include "extra.h"
#endif

namespace {
QStringList fallbackServers{"https://invidious.snopyta.org"};
QStringList preferredServers;

void shuffle(QStringList &sl) {
    std::shuffle(sl.begin(), sl.end(), *QRandomGenerator::global());
}

} // namespace

Invidious &Invidious::instance() {
    static Invidious i;
    return i;
}

Http &Invidious::http() {
    static Http *h = [] {
        Http *http = new Http;
        http->addRequestHeader("User-Agent", HttpUtils::stealthUserAgent());
        http->setMaxRetries(0);
        return http;
    }();
    return *h;
}

Http &Invidious::cachedHttp() {
    static Http *h = [] {
        ThrottledHttp *throttledHttp = new ThrottledHttp(http());
        throttledHttp->setMilliseconds(500);

        CachedHttp *cachedHttp = new CachedHttp(*throttledHttp, "iv");
        cachedHttp->setMaxSeconds(3600);
        cachedHttp->setIgnoreHostname(true);

        cachedHttp->getValidators().insert("application/json", [](const auto &reply) -> bool {
            const auto body = reply.body();
            if (body.isEmpty() || body == "[]" || body == "{}") return false;
            return true;
        });

        return cachedHttp;
    }();
    return *h;
}

Invidious::Invidious(QObject *parent) : QObject(parent) {

}

void Invidious::initServers() {
    if (!servers.isEmpty()) shuffleServers();

    QString instanceApi = JsFunctions::instance()->string("ivInstances()");
    if (instanceApi.isEmpty()) {
        qDebug() << "No instances API url";
        return;
    }

    if (initializing) {
        qDebug() << "Already initializing";
        return;
    }
    initializing = true;
    servers.clear();

    preferredServers = JsFunctions::instance()->stringArray("ivPreferred()");
#ifdef APP_EXTRA
    preferredServers << Extra::extraFunctions()->stringArray("ivPreferred()");
#endif
    shuffle(preferredServers);
    servers << preferredServers;

    auto newFallbackServers = JsFunctions::instance()->stringArray("ivFallback()");
    if (!newFallbackServers.isEmpty()) {
        fallbackServers = newFallbackServers;
        shuffle(fallbackServers);
    }

    auto reply = cachedHttp().get(instanceApi);
    connect(reply, &HttpReply::finished, this, [this](auto &reply) {
        if (reply.isSuccessful()) {
            QSettings settings;
            QStringList keywords = settings.value("recentKeywords").toStringList();
            QString testKeyword = keywords.isEmpty() ? "test" : keywords.first();

            QJsonDocument doc = QJsonDocument::fromJson(reply.body());
            for (const auto &v : doc.array()) {
                auto serverArray = v.toArray();
                QString host = serverArray.first().toString();
                QJsonObject serverObj = serverArray.at(1).toObject();
                if (serverObj["type"] == "https") {
                    QString url = "https://" + host;
                    QUrl testUrl(url + "/api/v1/search?q=" + testKeyword);
                    auto reply = http().get(testUrl);
                    connect(reply, &HttpReply::finished, this, [this, url](auto &reply) {
                        if (reply.isSuccessful()) {
                            QJsonDocument doc = QJsonDocument::fromJson(reply.body());
                            if (doc.array().isEmpty()) {
                                qDebug() << "Empty results" << url;
                            } else {
                                qDebug() << "Adding" << url;
                                servers << url;
                                initializing = false;
                            }
                        }
                    });
                }
            }
        }
    });
}

void Invidious::shuffleServers() {
    shuffle(servers);
}

QString Invidious::baseUrl() {
    QString host;
    if (servers.isEmpty()) {
        if (preferredServers.isEmpty()) {
            if (!fallbackServers.isEmpty()) host = fallbackServers.first();
        } else
            host = preferredServers.first();
    } else
        host = servers.first();

    QString url = host + QLatin1String("/api/v1/");
    return url;
}

QUrl Invidious::method(const QString &name) {
    QString base = baseUrl();
    if (base.isEmpty()) return QUrl();
    QUrl url(baseUrl() + name);
    return url;
}
