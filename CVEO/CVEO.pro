#-------------------------------------------------
#
# Project created by QtCreator 2013-10-25T19:02:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS -= -fno-keep-inline-dllexport
QMAKE_CXXFLAGS += -std=c++0x -fpermissive

QMAKE_LFLAGS += -static -static-libgcc
#QMAKE_LFLAGS += -static -static-libgcc -enable-stdcall-fixup -Wl,-enable-auto-import -Wl,-enable-runtime-pseudo-reloc

TARGET = CVEO
#CONFIG   += console
#CONFIG   -= app_bundle

#CONFIG	+= qt warn_on release static staticlib
CONFIG	+= static staticlib

TEMPLATE = app

SOURCES += main.cpp\
				CVEO.cpp \
    EngineO.cpp \
    ../Common/Time.cpp \
    ../Common/Telepathy.cpp \
    ../Common/StringTools.cpp \
    ../Common/StringTokenizer.cpp \
    ../Common/Process.cpp \
    ../Common/InternalProtocolSeeker.cpp \
    ../Common/File.cpp \
    ../Common/Debug.cpp \
    ../Common/CodeConverter.cpp \
    AdapterO.cpp \
    ../Common/Thread.cpp

HEADERS  += CVEO.hpp \
    EngineO.hpp \
    ../Common/Time.hpp \
    ../Common/Telepathy.hpp \
    ../Common/SystemDependency.hpp \
    ../Common/StringTools.hpp \
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
    AdapterO.hpp \
    CVEODependent.hpp \
    ../Common/Thread.hpp

FORMS    += CVEO.ui

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

## Include Path for OpenCVE.
INCLUDEPATH += . \
			# Qt Open GL ES2 Root
#			+= $$QT_OPENGLES2_ROOT \
			# OPEMCVE Root
#			+= $$OPENCVE_ROOT/CVES \
#			+= $$OPENCVE_ROOT/CVEC \
			+= $$OPENCVE_ROOT/Common \
			# OPENCV Root
			+= $$OPENCV_PATH/build/include \
			+= $$OPENCV_PATH/include/opencv

# Import libws2_32
LIBS +=	\
	-lws2_32 \
	-lpsapi \
	-lAdvAPI32 \
	-lrpcrt4
##LIBS += -L$$MINGW_PATH/lib \

win32 {
# Import Library for Debug
win32:CONFIG(debug, debug|release): \
#	LIBS +=	-L$$QT_MINGW_PATH/lib \
#	-lQt5Cored \
#	-lQt5Guid \
#	-lQt5Widgetsd
	LIBS += -L$$OPENCV_MSVC12_PATH/lib/Debug \
	-lopencv_calib3d246d \
	-lopencv_contrib246d \
	-lopencv_core246d \
	-lopencv_features2d246d \
	-lopencv_flann246d \
	-lopencv_gpu246d \
	-lopencv_haartraining_engined \
	-lopencv_highgui246d \
	-lopencv_imgproc246d \
	-lopencv_legacy246d \
	-lopencv_ml246d \
	-lopencv_nonfree246d \
	-lopencv_objdetect246d \
	-lopencv_photo246d \
	-lopencv_stitching246d \
	-lopencv_superres246d \
	-lopencv_ts246d \
	-lopencv_video246d \
	-lopencv_videostab246d
# Import Library for Release
win32:CONFIG(release, debug|release): \
#	LIBS +=	-L$$QT_MINGW_PATH/lib \
#	-lQt5Core \
#	-lQt5Gui \
#	-lQt5Widgets

	LIBS += -L$$OPENCV_MSVC12_PATH/lib/Release \
	-lopencv_calib3d246 \
	-lopencv_contrib246 \
	-lopencv_core246 \
	-lopencv_features2d246 \
	-lopencv_flann246 \
	-lopencv_gpu246 \
	-lopencv_haartraining_engine \
	-lopencv_highgui246 \
	-lopencv_imgproc246 \
	-lopencv_legacy246 \
	-lopencv_ml246 \
	-lopencv_nonfree246 \
	-lopencv_objdetect246 \
	-lopencv_photo246 \
	-lopencv_stitching246 \
	-lopencv_superres246 \
	-lopencv_ts246 \
	-lopencv_video246 \
	-lopencv_videostab246
}
else {

}
