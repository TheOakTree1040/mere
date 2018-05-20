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
        main.cpp \
    tokenizer.cpp \
    parser.cpp \
    interpreter.cpp \
    environment.cpp \
    expr.cpp \
    stmt.cpp \
    object.cpp \
    merecallable.cpp \
    tlogger.cpp \
    natives.cpp \
    shell.cpp \
    utils.cpp \
    core.cpp

HEADERS += \
    expr.h \
    tokenizer.h \
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
    sourceeditor.hpp \
    data_storage.hpp \
    token.hpp \
    parseerror.hpp \
    config.hpp \
    shell.h \
    utils.h \
    core.h

#QTPLUGIN.platforms = qminimal
