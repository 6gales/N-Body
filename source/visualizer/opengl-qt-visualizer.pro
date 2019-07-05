#-------------------------------------------------
#
# Project created by QtCreator 2017-07-31T11:52:51
#
#-------------------------------------------------

QT += core gui
CONFIG += c++14
CONFIG += file_copies

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

exists(local.pri) {
    include(local.pri)
}

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    ../client/client.cpp \
    ../client/clientParser.cpp \
    ../client/clientReader.cpp \
    Particle.cpp \
    Vector3D.cpp \
    computer.cpp \
    main_window.cpp \
    my_opengl_widget.cpp \
    particlesdeque.cpp \
    sequentialComputer.cpp

HEADERS  += \
    ../client/IOException.hpp \
    ../client/client.hpp \
    ../client/clientReader.hpp \
    Particle.h \
    Vector3D.h \
    abstractComputer.h \
    computer.h \
    main_window.h \
    my_opengl_widget.h \
    particlesdeque.h \
    sequentialComputer.h \


FORMS    += \
    main_window.ui

DISTFILES += \
    in.txt \
    shaders/basic.vert \
    shaders/basic.frag

COPIES += shaders

shaders.files = $$files(shaders/*)
shaders.path = $$OUT_PWD/shaders

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../local/lib/ -llibboost_system-vc142-mt-x64-1_70
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../local/lib/ -llibboost_system-vc142-mt-x64-1_70d
#else:unix: LIBS += -L$$PWD/../../../../local/lib/ -llibboost_system-vc142-mt-x64-1_70

INCLUDEPATH += C:\local\include
LIBS += -L"C:\local\lib" -lws2_32
#DEPENDPATH += $$PWD/../../../../local/include
