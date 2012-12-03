TEMPLATE = lib
VERSION = 0.0.1

HEADERS = ../src/IOHandler.h \
          ../src/ports/Modem.h \
          ../src/alert/AlertHandler.h

SOURCES = ../src/IOHandler.cpp \
          ../src/ports/Modem.cpp \
          ../src/alert/AlertHandler.cpp

INCLUDEPATH += ../src/ports
INCLUDEPATH += ../src/alert
INCLUDEPATH += ../src

QT -= gui
QT += xml
