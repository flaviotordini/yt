INCLUDEPATH += $$PWD

# include($$PWD/yt3/yt3.pri)
# include($$PWD/invidious/invidious.pri)
include($$PWD/ytjs/ytjs.pri)

HEADERS += $$files($$PWD/*.h)
SOURCES += $$files($$PWD/*.cpp)
