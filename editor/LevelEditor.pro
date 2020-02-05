#-------------------------------------------------
#
# Project created by QtCreator 2016-06-10T02:09:51
#
#-------------------------------------------------

QT       += core gui opengl
CONFIG += c++14
RC_ICONS = epsilon.ico
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LevelEditor
TEMPLATE = app
#LIBS += -L "C:\Users\Imanol\Documents\Code\C++\common-libs\glew\lib/" -L"C:\Users\Imanol\Documents\Code\C++\common-libs\SOIL\lib/" -lglew32.dll -lSOIL
#LIBS += -L"C:\Users\Imanol\Documents\Code\C++\common-libs\SOIL\lib/" -lSOIL

#LIBS += -pthread
LIBS += -lopengl32
LIBS += -lglu32

#DESTDIR=bin #Target file directory
OBJECTS_DIR=obj #Intermediate object files directory
MOC_DIR=moc #Intermediate moc files directory

SOURCES += src/leveleditor.cpp \
    src/Log.cpp \
    src/main.cpp \
    src/oglwidget.cpp \
    src/Shader.cpp \
    src/BSP.cpp \
    src/BSPFace.cpp \
    src/Frustum.cpp \
    src/Mesh.cpp \
    src/Model.cpp \
    src/terrain.cpp \
    src/Camera.cpp \
               framelesswindow/framelesswindow.cpp \
               framelesswindow/windowdragger.cpp \
               DarkStyle.cpp \
    GL/glad.c \
    image.cpp \
    src/texture.cpp

INCLUDEPATH += "C:\Users\Imanol\Documents\Code\C++\common-libs\glm-master" \
               "C:\Users\Imanol\Documents\Code\C++\common-libs\SOIL\src" \
                "framelesswindow"
#"C:\Users\Imanol\Documents\Code\C++\common-libs\glew\include" \

HEADERS  += include/leveleditor.h \
    include/Log.hpp \
    include/gizmo_geometry.hpp \
    include/gizmos.hpp \
    include/oglwidget.h \
    include/Shader.h \
    include/BSP.h \
    include/BSPFace.h \
    include/CBitset.h \
    include/Frustum.h \
    include/BSPfile.h \
    include/Mesh.h \
    include/Model.h \
    include/emlFormat.h \
    include/Types.h \
    include/terrain.h \
    include/camera.h \
    include/helpers.h \
    include/defaultgrid.h \
    include/picking.h \
    include/skybox.h \
    include/boundingbox.h \
               framelesswindow/framelesswindow.h \
               framelesswindow/windowdragger.h \
               DarkStyle.h \
    include/cpu.hpp \
    GL/glad.h \
    GL/khrplatform.h \
    image.hpp \
    include/texture.hpp \
    include/threadpool.hpp \
    stbi.hpp \
    threadmanager.hpp

FORMS    += leveleditor.ui \
    framelesswindow/framelesswindow.ui

RESOURCES += \
    darkstyle.qrc \
    framelesswindow.qrc
