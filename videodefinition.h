#ifndef VIDEODEFINITION_H
#define VIDEODEFINITION_H

#include <QtCore>

class VideoDefinition {
public:
    static const QVector<VideoDefinition> &getDefinitions();
    static const QVector<QString> &getDefinitionNames();
    static const VideoDefinition &forName(const QString &name);
    static const VideoDefinition &forCode(int code);

    static const VideoDefinition &maxVideoDefinition();
    static void setMaxVideoDefinition(const QString &name);

    VideoDefinition(const QString &name, int code, bool hasAudioStream = false);

    const QString &getName() const { return name; }
    int getCode() const { return code; }
    bool hasAudio() const { return hasAudioStream; }
    bool isEmpty() const;

    VideoDefinition &operator=(const VideoDefinition &);

private:
    const QString name;
    const int code;
    const bool hasAudioStream;
};

inline bool operator==(const VideoDefinition &lhs, const VideoDefinition &rhs) {
    return lhs.getCode() == rhs.getCode();
}

#endif // VIDEODEFINITION_H
