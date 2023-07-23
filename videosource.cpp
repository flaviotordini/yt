#include "videosource.h"

void VideoSource::setParam(const QString &name, const QVariant &value) {
    bool success = setProperty(name.toUtf8(), value);
    if (!success) qWarning() << "Failed to set property" << name << value.toString();
}
