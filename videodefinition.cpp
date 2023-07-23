#include "videodefinition.h"

namespace {
const int kEmptyDefinitionCode = -1;

template <typename T, T (VideoDefinition::*Getter)() const>
const VideoDefinition &getDefinitionForImpl(T matchValue) {
    const auto &defs = VideoDefinition::getDefinitions();

    for (auto i = defs.rbegin(); i != defs.rend(); ++i) {
        if ((*i.*Getter)() == matchValue) return *i;
    }
    /*
    for (const VideoDefinition &def : defs) {
        if ((def.*Getter)() == matchValue) return def;
    }
    */
    static const VideoDefinition kEmptyDefinition(QString(), kEmptyDefinitionCode);
    return kEmptyDefinition;
}
} // namespace

const QVector<VideoDefinition> &VideoDefinition::getDefinitions() {
    // List preferred equivalent format last:
    // algo selects the last format with same name first
    static const QVector<VideoDefinition> definitions = {
            VideoDefinition("240p", 242),      VideoDefinition("240p", 133),
            VideoDefinition("360p", 243),      VideoDefinition("360p", 396),
            VideoDefinition("360p", 18, true), VideoDefinition("480p", 244),
            VideoDefinition("480p", 135),      VideoDefinition("720p", 247),
            VideoDefinition("720p", 136),      VideoDefinition("720p", 22, true),
            VideoDefinition("1080p", 248),     VideoDefinition("1080p", 137),
            VideoDefinition("1440p", 271),     VideoDefinition("2160p", 313),
    };
    return definitions;
}

const QVector<QString> &VideoDefinition::getDefinitionNames() {
    static const QVector<QString> names = {"480p", "720p", "1080p", "1440p", "2160p"};
    return names;
}

const VideoDefinition &VideoDefinition::forName(const QString &name) {
    return getDefinitionForImpl<const QString &, &VideoDefinition::getName>(name);
}

const VideoDefinition &VideoDefinition::forCode(int code) {
    return getDefinitionForImpl<int, &VideoDefinition::getCode>(code);
}

const VideoDefinition &VideoDefinition::maxVideoDefinition() {
    const QString name = QSettings().value("definition", "720p").toString();
    const VideoDefinition &definition = VideoDefinition::forName(name);
    return definition;
}

void VideoDefinition::setMaxVideoDefinition(const QString &name) {
    QSettings settings;
    settings.setValue("definition", name);
}

VideoDefinition::VideoDefinition(const QString &name, int code, bool hasAudioStream)
    : name(name), code(code), hasAudioStream(hasAudioStream) {}

bool VideoDefinition::isEmpty() const {
    return code == kEmptyDefinitionCode && name.isEmpty();
}
