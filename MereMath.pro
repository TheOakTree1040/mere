#---------------------#
# 2018-03-21 18:24:44 #
#---------------------#

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cmm
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += STATIC
CONFIG += static
CONFIG += c++14

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
    tlogger.cpp

HEADERS += \
    expr.h \
    token.h \
    tokenizer.h \
    mere_math.h \
    sourceeditor.h \
    parser.h \
    interpreter.h \
    parseerror.h \
    environment.h \
    runtimeerror.h \
    stmt.h \
    merecallable.h \
    astprinter.hpp \
    object.h \
    cmdparser.h \
    data_storage.h \
    tlogger.h \
    t.h
