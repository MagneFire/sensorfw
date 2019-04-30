CONFIG      += link_pkgconfig

TARGET       = hrmsensor

HEADERS += hrmsensor.h   \
           hrmsensor_a.h \
           hrmplugin.h

SOURCES += hrmsensor.cpp   \
           hrmsensor_a.cpp \
           hrmplugin.cpp

include( ../sensor-config.pri )

contextprovider {
    DEFINES += PROVIDE_CONTEXT_INFO
    PKGCONFIG += contextprovider-1.0
}

