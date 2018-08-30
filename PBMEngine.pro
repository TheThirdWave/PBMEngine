TEMPLATE = app
CONFIG += console
CONFIG -= qt

QMAKE_CXXFLAGS += -fopenmp

SOURCES += main.cpp \
    buffer2d.cpp \
    fluidmodel.cpp \
    sphmodel.cpp \
    particle.cpp \
    particlegrid.cpp \
    solidframe.cpp \
    stuffbuilder.cpp

OTHER_FILES += \
    FragS1.glsl \
    VertS1.glsl

LIBS += -L/usr/local/lib -lglfw3 -pthread -lGLEW -lGLU -lGL -lrt -lXrandr -lXxf86vm -lXi -lXinerama -lXcursor -lX11 -ldl -lm -lOpenImageIO -fopenmp

HEADERS += \
    buffer2d.h \
    CmdLineFind.h \
    structpile.h \
    fluidmodel.h \
    sphmodel.h \
    particle.h \
    particlegrid.h \
    solidframe.h \
    stuffbuilder.h

DISTFILES += \
    FragS2.glsl \
    VertS2.glsl
