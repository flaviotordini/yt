#ifndef VIDEOSOURCE_H
#define VIDEOSOURCE_H

#include <QAction>
#include <QtCore>

#include "video.h"

class VideoSource : public QObject {
    Q_OBJECT

public:
    VideoSource(QObject *parent = 0) : QObject(parent) {}
    virtual void loadVideos(int max, int startIndex) = 0;
    virtual bool hasMoreVideos() { return true; }
    virtual void abort() = 0;
    virtual QString getName() = 0;
    virtual const QList<QAction *> &getActions() {
        static const QList<QAction *> noActions;
        return noActions;
    }
    virtual int maxResults() { return 0; }

public slots:
    void setParam(const QString &name, const QVariant &value);

signals:
    void gotVideos(const QVector<Video *> &videos);
    void finished(int total);
    void error(QString message);
    void nameChanged(QString name);
};

#endif // VIDEOSOURCE_H
