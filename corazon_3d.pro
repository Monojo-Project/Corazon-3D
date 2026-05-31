QT += core gui opengl

CONFIG += c++11

TARGET = corazon_3d
TEMPLATE = app

SOURCES += main.cpp

# OpenGL - compatible con Qt5 y Qt6
unix: LIBS += -lGL -lGLU -lm

# Incluir openglwidgets solo para Qt6
greaterThan(QT_MAJOR_VERSION, 5): QT += openglwidgets

QMAKE_CXXFLAGS += -Wall -O2 -std=c++11
