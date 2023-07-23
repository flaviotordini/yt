#ifndef VIDEOAPI_H
#define VIDEOAPI_H

#include <QtCore>

class VideoAPI {
public:
    enum Impl { YT3, IV, JS };
    static Impl impl() { return JS; }

    static QString videoIdFromUrl(const QString &url);
    static QTime videoTimestampFromUrl(const QString &url);

private:
    VideoAPI() {}
};

#endif // VIDEOAPI_H
