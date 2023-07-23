#include "videoapi.h"

QString VideoAPI::videoIdFromUrl(const QString &url) {
    static const QVector<QRegularExpression> res = {QRegularExpression("^.*[\\?&]v=([^&#]+).*$"),
                                                    QRegularExpression("^.*://.*/([^&#\\?]+).*$"),
                                                    QRegularExpression("^.*/shorts/([^&#\\?/]+)$")};
    for (const auto &re : res) {
        auto match = re.match(url);
        if (match.hasMatch()) return match.captured(1);
    }
    return QString();
}

QTime VideoAPI::videoTimestampFromUrl(const QString &url) {
    QTime res(0, 0);

    // TODO: should we make this accept h/m/s in any order?
    //       timestamps returned by youtube always seem to be
    //       ordered.
    static QRegularExpression re(".*t=([0-9]*h)?([0-9]*m)?([0-9]*s)?.*");
    auto match = re.match(url);
    if (!match.hasMatch()) {
        return res;
    }

    const auto captured = match.capturedTexts();
    for (const QString &str : captured) {
        if (str.length() <= 1) continue;

        QString truncated = str;
        truncated.chop(1);

        bool ok = false;
        int value = truncated.toInt(&ok);
        if (!ok) continue;
        char unit = str.at(str.length() - 1).toLatin1();

        switch (unit) {
        case 'h':
            value *= 60 * 60; // hours -> seconds
            break;

        case 'm':
            value *= 60; // minutes -> seconds
            break;

        case 's':
            break;

        default:
            continue;
        }

        res = res.addSecs(value);
    }

    return res;
}
