INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

include($$PWD/invidious/invidious.pri)
include($$PWD/ytjs/ytjs.pri)

HEADERS += $$files($$PWD/*.h, false)
SOURCES += $$files($$PWD/*.cpp, false)
