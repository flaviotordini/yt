INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

# include($$PWD/yt3/yt3.pri)
# include($$PWD/invidious/invidious.pri)
include($$PWD/ytjs/ytjs.pri)

HEADERS += $$files($$PWD/*.h, false)
SOURCES += $$files($$PWD/*.cpp, false)
