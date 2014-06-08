#-------------------------------------------------
#
# Project created by QtCreator 2013-01-12T01:55:38
#
#-------------------------------------------------

QT       += core gui webkit

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets serialport

TARGET = qkide
TEMPLATE = app

QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/../shared/lib/\',-z,origin'

#DEFINES += QT_NO_DEBUG_OUTPUT

INCLUDEPATH += ../utils
INCLUDEPATH += core
INCLUDEPATH += editor
INCLUDEPATH += gui
INCLUDEPATH += gui/widgets
INCLUDEPATH += gui/browser

INCLUDEPATH += ../qkcore
QMAKE_LIBDIR += ../qkcore/release
LIBS += -L../qkcore/release -lqkcore

#INCLUDEPATH += ../qkconnect
#QMAKE_LIBDIR += ../qkconnect/release
#LIBS += -L../qkconnect/release -lqkconnect

INCLUDEPATH += ../qkwidget
QMAKE_LIBDIR += ../qkwidget/release
LIBS += -L../qkwidget/release -lqkwidget

SOURCES += main.cpp\
        qkide.cpp \
    gui/editor/editor.cpp \
    gui/editor/pagetab.cpp \
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
    gui/editor/codeparser.cpp \
    ../utils/qkutils.cpp \
    core/projectpreferencesdialog.cpp \
    gui/editor/codetip.cpp \
    core/theme.cpp \
    core/project.cpp

HEADERS  += qkide.h \
    qkide_global.h \
    gui/editor/pagetab.h \
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
    gui/editor/codeparser.h \
    ../utils/qkutils.h \
    core/projectpreferencesdialog.h \
    gui/editor/codetip.h \
    core/theme.h \
    core/project.h

FORMS    += qkide.ui \
    gui/editor/findreplacedialog.ui \
    core/optionsdialog.ui \
    core/projectpreferencesdialog.ui \
    gui/editor/codetip.ui

RESOURCES += \
    resources/qkide_fonts.qrc \
    resources/qkide_img.qrc \
    resources/qkide_html.qrc \
    resources/qkide_syntax.qrc \
    resources/qkide_templates.qrc \
    resources/qkide_style.qrc

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
  deploy.commands = python deploy.py
}
unix {
  deploy.commands = python deploy.py
}
macx {

}
QMAKE_EXTRA_TARGETS += deploy
POST_TARGETDEPS += deploy
