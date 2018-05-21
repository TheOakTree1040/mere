#---------------------#
# 2018-03-21 18:24:44 #
#---------------------#

QT       += core #
#QT       += gui

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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

SOURCES += \
    src/core.cpp \
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
    src/tokenizer.cpp

HEADERS += \
    include/core.h \
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
    include/sourceeditor.h

INCLUDEPATH += $$PWD/include
#QTPLUGIN.platforms = qminimal

DISTFILES += \
    tests/test.mr
