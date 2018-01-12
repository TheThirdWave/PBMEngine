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
    edgeobject.cpp \
    objcollection.cpp \
    objectcreator.cpp

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
    edgeobject.h \
    objcollection.h \
    objectcreator.h

LIBS += -lglut -lGLEW -lGL -lGLU -lm -lstdc++ -lpng -lrt

