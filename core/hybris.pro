QT += network

TEMPLATE = lib
TARGET = hybrissensorfw

include( ../common-config.pri )

CONFIG += link_pkgconfig
PKGCONFIG += android-headers

SENSORFW_INCLUDEPATHS = .. \
                        ../include \
                        ../filters \
                        ../datatypes

DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

QMAKE_LIBDIR_FLAGS += -lsensordatatypes-qt5

SOURCES += hybrisadaptor.cpp
HEADERS += hybrisadaptor.h
LIBS += -L$$[QT_INSTALL_LIBS] -L../datatypes

!contains(CONFIG,binder) {
    LIBS += -lhybris-common -lhardware
}

include(../common-install.pri)
target.path = $$SHAREDLIBPATH
INSTALLS += target
