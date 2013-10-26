#-------------------------------------------------
#
# Project created by QtCreator 2013-10-25T19:00:21
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = CVES
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    HandRecognition.cpp \
    EngineS.cpp \
    ChessRecognition.cpp \
    ChessGame.cpp \
    BlobLabeling.cpp \
    ../Common/Time.cpp \
    ../Common/Telepathy.cpp \
    ../Common/StringTokenizer.cpp \
    ../Common/InternalProtocolSeeker.cpp \
    ../Common/Debug.cpp \
    ../Common/CodeConverter.cpp

HEADERS += \
    HandRecognition.hpp \
    EngineS.hpp \
    ChessRecognition.hpp \
    ChessGame.hpp \
    BlobLabeling.hpp \
    ../Common/Time.hpp \
    ../Common/Telepathy.hpp \
    ../Common/SystemDependency.hpp \
    ../Common/StringTokenizer.hpp \
    ../Common/InternalProtocolSeeker.hpp \
    ../Common/InternalProtocol.hpp \
    ../Common/GlobalVariables.hpp \
    ../Common/ExtendedBlackBox.hpp \
    ../Common/Debug.hpp \
    ../Common/Common.hpp

#QT_PATH = ${QTDIR}
#PROJECT_PATH = ${INVI_PROJECT_ROOT}
#OPENCV_PATH = ${OPENCV_AROOT}

QT_PATH = C:/Qt/5.1.1/msvc2012
PROJECT_PATH = C:/invi/Projects
OPENCV_PATH = C:/invi/Dev_Bone/dev_libraries/opencv
OPENCV_QT_PATH = C:/invi/Dev_Bone/dev_libraries/OpenCV_QT
OPENCV_MSVC12_PATH = C:/invi/Dev_Bone/dev_libraries/OpenCV_MSVC12

QT_OPENGLES2_ROOT = $$QT_PATH/include/QtANGLE

OPENCVE_ROOT = $$PROJECT_PATH/OpenCVE

# Include Path.
INCLUDEPATH += . \
			# Qt Open GL ES2 Root
			+= $$QT_OPENGLES2_ROOT \
			# OPEMCVE Root
			+= $$OPENCVE_ROOT/CVES \
			+= $$OPENCVE_ROOT/CVEC \
			+= $$OPENCVE_ROOT/Common \
			# OPENCV Root
			+= $$OPENCV_PATH/build/include \
			+= $$OPENCV_PATH/include/opencv

win32 {
# Import Library for Debug
win32:CONFIG(debug, debug|release): \
		LIBS += -L$$OPENCV_MSVC12_PATH\\lib\\Debug \
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
else:win32:CONFIG(release, debug|release): \
		LIBS += -L$$OPENCV_MSVC12_PATH\\lib\\Release \
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
# OSX, UNIX, Linux 등의 Path.
}