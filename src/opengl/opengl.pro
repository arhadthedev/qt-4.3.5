TARGET     = QtOpenGL
QPRO_PWD   = $$PWD
QT         = core gui
DEFINES   += QT_BUILD_OPENGL_LIB
win32-msvc*|win32-icc:QMAKE_LFLAGS += /BASE:0x63000000
solaris-cc*:QMAKE_CXXFLAGS_RELEASE -= -O2


include(../qbase.pri)

!win32:!embedded:!mac:CONFIG	   += x11
contains(QT_CONFIG, opengl):CONFIG += opengl


HEADERS += qgl.h \
	   qgl_p.h \
	   qglcolormap.h \
	   qpaintengine_opengl_p.h \
	   qglpixelbuffer.h \
	   qglframebufferobject.h
SOURCES	+= qgl.cpp \
	   qglcolormap.cpp \
	   qpaintengine_opengl.cpp \
	   qglpixelbuffer.cpp \
	   qglframebufferobject.cpp \
           qglextensions.cpp
x11 {
    SOURCES += qgl_x11.cpp \
	       qglpixelbuffer_x11.cpp
 	contains(QT_CONFIG, fontconfig) {
 		include($$QT_SOURCE_TREE/config.tests/unix/freetype/freetype.pri)
	} else {
	    DEFINES *= QT_NO_FREETYPE
 	}
}

mac {
    SOURCES += qgl_mac.cpp \
	       qglpixelbuffer_mac.cpp
    LIBS += -framework Carbon
}
win32 {
    SOURCES += qgl_win.cpp \
	       qglpixelbuffer_win.cpp
}

embedded {
    SOURCES += qegl_qws.cpp \
               qgl_qws.cpp \
               qglpaintdevice_qws.cpp \
               qglpixelbuffer_qws.cpp \
               qglscreen_qws.cpp \
               qglwindowsurface_qws.cpp

    HEADERS += qegl_qws_p.h \
               qglpaintdevice_qws_p.h \
               qglscreen_qws.h \
               qglwindowsurface_qws_p.h

    contains(QT_CONFIG, fontconfig) {
        include($$QT_SOURCE_TREE/config.tests/unix/freetype/freetype.pri)
    } else {
       DEFINES *= QT_NO_FREETYPE
    }
}

QMAKE_LIBS += $$QMAKE_LIBS_OPENGL
