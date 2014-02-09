#-------------------------------------------------
#
# Project created by QtCreator 2013-01-12T01:55:38
#
#-------------------------------------------------

QT       += core gui webkit

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets serialport

TARGET = qkide
TEMPLATE = app

#QMAKE_LFLAGS += '-Wl,-rpath,\'../shared/lib/\',-z,origin'
QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/../shared/lib/\',-z,origin'
#QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/\',-z,origin'

DEFINES += QT_NO_DEBUG_OUTPUT

INCLUDEPATH += core
INCLUDEPATH += gui
INCLUDEPATH += gui/widgets
INCLUDEPATH += editor

INCLUDEPATH += ../qkcore
LIBS += -lqkcore
QMAKE_LIBDIR += ../qkcore/release

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
    gui/editor/codeparser.cpp

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
    gui/editor/codeparser.h

FORMS    += qkide.ui \
    gui/editor/findreplacedialog.ui \
    core/optionsdialog.ui

RESOURCES += \
    resources/qkide_fonts.qrc \
    resources/qkide_img.qrc \
    resources/qkide_html.qrc \
    resources/qkide_syntax.qrc \
    resources/qkide_styles.qrc \
    resources/qkide_templates.qrc

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
  deploy.commands = ./deploy.sh
}
macx {

}
QMAKE_EXTRA_TARGETS += deploy
POST_TARGETDEPS += deploy
