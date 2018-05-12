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

macx{
    CONFIG -= app_bundle
}

SOURCES += \
        main.cpp \
    tokenizer.cpp \
    mere_math.cpp \
    parser.cpp \
    interpreter.cpp \
    environment.cpp \
    expr.cpp \
    stmt.cpp \
    object.cpp \
    merecallable.cpp \
    tlogger.cpp \
    natives.cpp \
    runtimeerror.cpp \
    cmd.cpp

HEADERS += \
    expr.h \
    tokenizer.h \
    mere_math.h \
    parser.h \
    interpreter.h \
    environment.h \
    runtimeerror.h \
    stmt.h \
    merecallable.h \
    astprinter.hpp \
    object.h \
    tlogger.h \
    natives.h \
    cmd.h \
    sourceeditor.hpp \
    data_storage.hpp \
    token.hpp \
    parseerror.hpp \
    config.hpp
