TEMPLATE = app
DEPENDPATH += .
INCLUDEPATH += .

HEADERS += lib_initializer.h
SOURCES += lib_initializer.cpp main.cpp

TARGET=GsTLAppli_main

CONFIG      += plugin
INCLUDEPATH += $(QTDIR)/tools/designer/interfaces

contains( TEMPLATE, lib ) {
    DESTDIR = $$GSTLAPPLI_HOME/lib/$$CUR_PLATFORM
    OBJECTS_DIR = $$GSTLAPPLI_HOME/lib/$$CUR_PLATFORM/obj/main
}
contains( TEMPLATE, app ) {
    DESTDIR += $$GSTLAPPLI_HOME/bin/$$CUR_PLATFORM
    OBJECTS_DIR = $$GSTLAPPLI_HOME/bin/$$CUR_PLATFORM/obj/main
}


include( $$GSTLAPPLI_HOME/config.qmake )

exists( MMakefile ) {
   include( MMakefile )
}

