TARGET       = hybrishrmadaptor

HEADERS += hybrishrmadaptor.h \
           hybrishrmadaptorplugin.h

SOURCES += hybrishrmadaptor.cpp \
           hybrishrmadaptorplugin.cpp
LIBS+= -L../../core -lhybrissensorfw-qt5

include( ../adaptor-config.pri )
config_hybris {
    PKGCONFIG += android-headers
}
