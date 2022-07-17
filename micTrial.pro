QT -= gui
QT += network
QT += multimedia

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        docs_filter.cpp \
        docs_filter.cpp \
        filter.cpp \
        main.cpp \
        myfilter.cpp \
        mymic.cpp \
        myserver.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    docs_filter.h \
    docs_filter.h \
    docs_heapblock.h \
    docs_referencecountedobject.h \
    filter.h \
    myfilter.h \
    mymic.h \
    myserver.h


HEADERS += \
    docs_filter.h \
    docs_referencecountedobject.h \
    myfilter.h \
    mymic.h \
    myserver.h


SOURCES += \
        docs_filter.cpp \
        main.cpp \
        myfilter.cpp \
        mymic.cpp \
        myserver.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    docs_filter.h \
    docs_filter.h \
    docs_referencecountedobject.h \
    myfilter.h \
    mymic.h \
    myserver.h


HEADERS += \
    docs_filter.h \
    docs_referencecountedobject.h \
    myfilter.h \
    mymic.h \
    myserver.h
