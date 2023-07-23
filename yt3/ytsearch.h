#ifndef YTSEARCH_H
#define YTSEARCH_H

#include "paginatedvideosource.h"
#include <QtNetwork>

class SearchParams;
class Video;

class YTSearch : public PaginatedVideoSource {
    Q_OBJECT

public:
    YTSearch(SearchParams *params, QObject *parent = 0);
    void loadVideos(int max, int startIndex);
    void abort();
    QString getName();
    const QList<QAction *> &getActions();
    int maxResults() { return 50; }
    SearchParams *getSearchParams() const { return searchParams; }

    bool operator==(const YTSearch &other) const { return searchParams == other.getSearchParams(); }

private slots:
    void parseResults(const QByteArray &data);
    void requestError(const QString &message);

private:
    SearchParams *searchParams;
    bool aborted;
    QString name;
};

#endif // YTSEARCH_H
