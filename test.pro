QMAKE_CXXFLAGS += -std=c++0x
TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += main.cpp \
    CJRW.cpp \
    shadermanager.cpp \
    imagemanip.cpp \
    renderobject.cpp \
    modelmanager.cpp \
    physicsobject.cpp \
    physicsmanager.cpp \
    sphereobject.cpp \
    planeobject.cpp \
    function2d.cpp \
    linefunction.cpp \
    spherefunction.cpp \
    trigfunction.cpp \
    quadraticfunction.cpp \
    camera.cpp \
    polygonobject.cpp \
    particleobject.cpp \
    particlegenerator.cpp \
    kernel.cpp \
    lsegfunction.cpp \
    function3d.cpp \
    spherefunction3d.cpp \
    planefunction.cpp \
    quadraticfunction3d.cpp \
    shaders.cpp \
    lightbase.cpp

OTHER_FILES += \
    vertshade \
    FragShade \
    FragShade_2d \
    vertshade_2d

HEADERS += \
    CJRW.h \
    shadermanager.h \
    structpile.h \
    imagemanip.h \
    renderobject.h \
    modelmanager.h \
    physicsobject.h \
    physicsmanager.h \
    sphereobject.h \
    planeobject.h \
    function2d.h \
    linefunction.h \
    spherefunction.h \
    trigfunction.h \
    quadraticfunction.h \
    camera.h \
    polygonobject.h \
    particleobject.h \
    particlegenerator.h \
    kernel.h \
    kernel.h \
    lsegfunction.h \
    function3d.h \
    spherefunction3d.h \
    planefunction.h \
    quadraticfunction3d.h \
    shaders.h \
    lightbase.h \
    Headers.h

LIBS += -lglut -lGLEW -lGL -lGLU -lm -lstdc++ -lpng -lrt

