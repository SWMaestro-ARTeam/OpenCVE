#-------------------------------------------------
#
# Project created by QtCreator 2013-10-25T19:02:33
#
#-------------------------------------------------

QT       += core

QT       -= gui

QMAKE_CXXFLAGS -= -fno-keep-inline-dllexport
QMAKE_CXXFLAGS += -std=c++0x -fpermissive
QMAKE_LFLAGS += -static -static-libgcc
#QMAKE_LFLAGS += -static -static-libgcc -enable-stdcall-fixup -Wl,-enable-auto-import -Wl,-enable-runtime-pseudo-reloc

TARGET = CVEO
CONFIG   += console
CONFIG   -= app_bundle

#CONFIG	+= qt warn_on release static staticlib
CONFIG	+= static staticlib

TEMPLATE = app


SOURCES += main.cpp


MINGW_PATH = C:/System_Emulator/MinGW
QT_PATH = C:/System_Development/Qt/5.1.1

QT_MINGW_PATH = $$QT_PATH/mingw48_32
QT_MSVC2012_32_PATH = $$QT_PATH/msvc2012
QT_MSVC2012_64_GL_PATH = $$QT_PATH/msvc2012_64_opengl

PROJECT_PATH = C:/invi/Projects
OPENCV_PATH = C:/invi/Dev_Bone/dev_libraries/opencv
OPENCV_QT_PATH = C:/invi/Dev_Bone/dev_libraries/OpenCV_QT
OPENCV_MSVC12_PATH = C:/invi/Dev_Bone/dev_libraries/OpenCV_MSVC12

# This path only used MSVC2012 32bit Ver.
QT_OPENGLES2_ROOT = $$QT_MSVC2012_32_PATH/include/QtANGLE
