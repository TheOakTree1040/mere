#---------------------#
# 2018-03-21 18:24:44 #
#---------------------#

QT       += core #
QT       -= gui

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mere
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += STATIC
CONFIG += console
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
    t.hpp \
    data_storage.hpp \
    token.hpp \
    parseerror.hpp
