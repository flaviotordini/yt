#ifndef VIDEO_H
#define VIDEO_H

#include <QtCore>
#include <QtGui>

#include "ytthumb.h"

class YTJSVideo;
class VariantPromise;

class Video : public QObject {
    Q_OBJECT

public:
    Video();
    ~Video();
    Video *clone();

    enum License { LicenseYouTube = 1, LicenseCC };
    Q_ENUM(License)

    const QString &getTitle() const { return title; }
    void setTitle(const QString &value) { title = value; }

    const QString &getDescription() const { return description; }
    void setDescription(const QString &value) { description = value; }

    const QString &getChannelTitle() const { return channelTitle; }
    void setChannelTitle(const QString &value) { channelTitle = value; }

    const QString &getChannelId() const { return channelId; }
    void setChannelId(const QString &value) { channelId = value; }

    const QString &getWebpage();
    void setWebpage(const QString &value);

    int getDuration() const { return duration; }
    void setDuration(int value);
    const QString &getFormattedDuration() const { return formattedDuration; }

    int getViewCount() const { return viewCount; }
    void setViewCount(int value);
    const QString &getFormattedViewCount() const { return formattedViewCount; }

    const QDateTime &getPublished() const { return published; }
    void setPublished(const QDateTime &value);
    const QString &getFormattedPublished() const { return formattedPublished; }

    int getDefinitionCode() const { return definitionCode; }

    void loadStreamUrl();
    const QString &getStreamUrl() { return streamUrl; }
    bool isLoadingStreamUrl() const;
    void abortLoadStreamUrl();

    const QString &getId() const { return id; }
    void setId(const QString &value) { id = value; }

    License getLicense() const { return license; }
    void setLicense(License value) { license = value; }

    const auto &getThumbs() const { return thumbs; }
    void addThumb(int width, int height, QString url);
    VariantPromise &loadThumb(QSize size, qreal pixelRatio);

signals:
    void gotStreamUrl(const QString &videoUrl, const QString &audioUrl);
    void errorStreamUrl(const QString &message);
    void changed();

private slots:
    void streamUrlLoaded(const QString &streamUrl, const QString &audioUrl);

private:
    void loadStreamUrlJS();

    QString title;
    QString description;
    QString channelTitle;
    QString channelId;
    QString webpage;
    QString streamUrl;
    int duration;
    QString formattedDuration;

    QDateTime published;
    QString formattedPublished;
    int viewCount;
    QString formattedViewCount;
    License license;
    QString id;
    int definitionCode;

    YTJSVideo *ytjsVideo;

    QVector<YTThumb> thumbs;
    bool thumbsNeedSorting = false;
};

// This is required in order to use QPointer<Video> as a QVariant
// as used by the Model/View playlist
typedef QPointer<Video> VideoPointer;
Q_DECLARE_METATYPE(VideoPointer)

#endif // VIDEO_H
