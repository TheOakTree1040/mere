
# Mere Interpreter
# Project creation: 03/21/2018 18:24:44
# TheOakCode.

QT       += core
#QT       += gui #define T_GUI

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets #define T_GUI

macx:TARGET = mere-darwin
win32:TARGET = mere-win32
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += STATIC

CONFIG += console
CONFIG += static
CONFIG += c++14
CONFIG -= import_plugins
macx:CONFIG -= app_bundle

INCLUDEPATH += $$PWD/include

SOURCES += \
    src/data_storage.cpp \
    src/environment.cpp \
    src/expr.cpp \
    src/interpreter.cpp \
    src/main.cpp \
    src/merecallable.cpp \
    src/natives.cpp \
    src/object.cpp \
    src/parser.cpp \
    src/runtimeerror.cpp \
    src/shell.cpp \
    src/stmt.cpp \
    src/tlogger.cpp \
    src/token.cpp \
    src/tokenizer.cpp \
    src/src.cpp \
    src/interpretationunit.cpp \
    src/resolver.cpp

HEADERS += \
    include/data_storage.h \
    include/environment.h \
    include/expr.h \
    include/interpreter.h \
    include/merecallable.h \
    include/natives.h \
    include/object.h \
    include/parser.h \
    include/runtimeerror.h \
    include/shell.h \
    include/stmt.h \
    include/tlogger.h \
    include/token.h \
    include/tokenizer.h \
    include/astprinter.h \
    include/config.h \
    include/parseerror.h \
    include/sourceeditor.h \
    include/src.h \
    include/interpretationunit.h \
    include/resolver.h

RESOURCES += \
    resrc/resources.qrc
