#-------------------------------------------------
#
# Project created by QtCreator 2013-10-25T19:01:52
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = CVEC
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    UCICommandSesker.cpp \
    Option.cpp \
    EngineC.cpp \
    ../Common/Time.cpp \
    ../Common/Telepathy.cpp \
    ../Common/StringTokenizer.cpp \
    ../Common/Process.cpp \
    ../Common/InternalProtocolSeeker.cpp \
    ../Common/File.cpp \
    ../Common/Debug.cpp \
    ../Common/CodeConverter.cpp

HEADERS += \
    UCICommandSeeker.hpp \
    UCICommand.hpp \
    Option.hpp \
    EngineC.hpp \
    ../Common/Time.hpp \
    ../Common/Telepathy.hpp \
    ../Common/SystemDependency.hpp \
    ../Common/StringTokenizer.hpp \
    ../Common/Process.hpp \
    ../Common/InternalProtocolSeeker.hpp \
    ../Common/InternalProtocol.hpp \
    ../Common/GlobalVariables.hpp \
    ../Common/File.hpp \
    ../Common/ExtendedBlackBox.hpp \
    ../Common/Debug.hpp \
    ../Common/Common.hpp \
    ../Common/CodeConverter.hpp \
    CVECDependent.hpp

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

OPENCVE_ROOT = $$PROJECT_PATH/OpenCVE

# Include Path.
INCLUDEPATH += . \
			# Qt Open GL ES2 Root
#			+= $$QT_OPENGLES2_ROOT \
			# OPEMCVE Root
#			+= $$OPENCVE_ROOT/CVES \
#			+= $$OPENCVE_ROOT/CVEC \
			+= $$OPENCVE_ROOT/Common
			# OPENCV Root
#			+= $$OPENCV_PATH/build/include \
#			+= $$OPENCV_PATH/include/opencv

# Import libws2_32
LIBS +=	\
	-lws2_32 \
	-lpsapi
#LIBS += -L$$MINGW_PATH/lib \

