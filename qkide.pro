#-------------------------------------------------
#
# Project created by QtCreator 2013-01-12T01:55:38
#
#-------------------------------------------------

QT       += core gui webkit

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets serialport

TARGET = qkide
TEMPLATE = app

INCLUDEPATH += core
INCLUDEPATH += gui
INCLUDEPATH += gui/widgets

INCLUDEPATH += ../qkcore

INCLUDEPATH += ../qkconnect
LIBS += -lqkconnect
QMAKE_LIBDIR += ../qkconnect/release

INCLUDEPATH += ../qkwidget
LIBS += -lqkwidget
QMAKE_LIBDIR += ../qkwidget/release

SOURCES += main.cpp\
        qkide.cpp \
    gui/editor/editor.cpp \
    gui/editor/pagetab.cpp \
    core/qkproject.cpp \
    core/projectwizard.cpp \
    gui/editor/page.cpp \
    gui/editor/highlighter.cpp \
    gui/editor/completer.cpp \
    gui/browser/browser.cpp \
    core/newprojectpage.cpp \
    gui/widgets/pminitablewidget.cpp \
    gui/widgets/ptextdock.cpp \
    gui/widgets/ptextedit.cpp \
    gui/editor/findreplacedialog.cpp \
    core/optionsdialog.cpp \
    core/projectpreferencesdialog.cpp

HEADERS  += qkide.h \
    qkide_global.h \
    gui/editor/pagetab.h \
    core/qkproject.h \
    core/projectwizard.h \
    gui/editor/editor.h \
    gui/editor/page.h \
    gui/editor/highlighter.h \
    gui/editor/completer.h \
    gui/browser/browser.h \
    core/newprojectpage.h \
    gui/widgets/pminitablewidget.h \
    gui/widgets/ptextdock.h \
    gui/widgets/ptextedit.h \
    gui/editor/findreplacedialog.h \
    core/optionsdialog.h \
    core/projectpreferencesdialog.h

FORMS    += qkide.ui \
    gui/editor/findreplacedialog.ui \
    core/optionsdialog.ui

RESOURCES += \
    resources/styles.qrc \
    resources/img.qrc \
    resources/templates.qrc \
    resources/syntax.qrc \
    resources/completer.qrc \
    resources/html.qrc

CONFIG(debug, debug|release) {
    DESTDIR = debug
} else {
    DESTDIR = release
}

OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui

win32 {
  deploy.commands = deploy.bat
}
unix {
  deploy.commands = deploy.sh
}
macx {

}
QMAKE_EXTRA_TARGETS += deploy
POST_TARGETDEPS += deploy
