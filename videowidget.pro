TEMPLATE = app
TARGET = videowidget

QT += multimedia multimediawidgets
QT       += charts

HEADERS = \
    videoplayer.h

SOURCES = \
    main.cpp \
    videoplayer.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/multimediawidgets/videowidget
INSTALLS += target

QT+=widgets

FORMS += \
    videoplayer.ui
