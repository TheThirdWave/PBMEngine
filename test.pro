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
    planeobject.cpp \
    function2d.cpp \
    linefunction.cpp \
    spherefunction.cpp \
    trigfunction.cpp \
    quadraticfunction.cpp \
    polygonobject.cpp \
    kernel.cpp \
    lsegfunction.cpp \
    function3d.cpp \
    spherefunction3d.cpp \
    planefunction.cpp \
    quadraticfunction3d.cpp \
    shaders.cpp \
    lightbase.cpp \
    directionallight.cpp \
    pointlight.cpp \
    spotlight.cpp \
    arealight.cpp \
    infinitysphere.cpp \
    trianglefunction.cpp \
    trianglemesh.cpp

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
    planeobject.h \
    function2d.h \
    linefunction.h \
    spherefunction.h \
    trigfunction.h \
    quadraticfunction.h \
    polygonobject.h \
    kernel.h \
    kernel.h \
    lsegfunction.h \
    function3d.h \
    spherefunction3d.h \
    planefunction.h \
    quadraticfunction3d.h \
    shaders.h \
    lightbase.h \
    directionallight.h \
    pointlight.h \
    spotlight.h \
    arealight.h \
    infinitysphere.h \
    trianglefunction.h \
    trianglemesh.h

LIBS += -lglut -lGLEW -lGL -lGLU -lm -lstdc++ -lpng -lrt

