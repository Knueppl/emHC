TEMPLATE = lib
VERSION = 0.0.3

HEADERS = ../src/IOHandler.h \
          ../src/Pipe.h \
          ../src/PipePublisher.h \
          ../src/PipeSubscriber.h \
          ../src/ports/Modem.h \
          ../src/alert/AlertHandler.h \
          ../src/ports/PortFactory.h \
          ../src/ports/Port.h \
          ../src/ports/PortMultiplexer.h

SOURCES = ../src/Pipe.cpp \
          ../src/PipePublisher.cpp \
          ../src/PipeSubscriber.cpp \
          ../src/IOHandler.cpp \
          ../src/ports/Modem.cpp \
          ../src/alert/AlertHandler.cpp \
          ../src/ports/PortFactory.cpp \
          ../src/ports/Port.cpp \
          ../src/ports/PortMultiplexer.cpp

INCLUDEPATH += ../src/ports
INCLUDEPATH += ../src/alert
INCLUDEPATH += ../src

QT -= gui
QT += xml
