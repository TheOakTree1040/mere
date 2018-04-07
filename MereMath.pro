#-------------------------------------------------
#
# Project created by QtCreator 2018-03-21T18:24:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cmm
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += STATIC
CONFIG += static

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
    tokenizer.cpp \
    mere_math.cpp \
    parser.cpp \
    interpreter.cpp \
    environment.cpp \
    expr.cpp \
    stmt.cpp \
    object.cpp

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
    project.h \
    data_storage.h
