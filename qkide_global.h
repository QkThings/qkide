#ifndef QKIDE_GLOBAL_H
#define QKIDE_GLOBAL_H

#include <QtGlobal>
#include <QFont>

class QString;

const QString SLASH = "/";
const QString QK_IDE_DOMAIN_STR = "qkthings.com";
const QString QK_IDE_NAME_STR = "qkide";
const QString QK_IDE_C_DEF_STR = "my_board.c";
const QString QK_IDE_H_DEF_STR = "my_board.h";

const QString CTAGS_EXE = "/resources/tools/bin/ctags";
const QString TOOLCHAIN_DIR = "/resources/embedded/toolchain";
const QString QKPROGRAM_DIR = "/resources/embedded/qkprogram";
const QString QKPROGRAM_LIB_DIR = QKPROGRAM_DIR + "/lib";
const QString TEMP_DIR = "/temp";
const QString TAGS_DIR = TEMP_DIR + "/tags";

#ifdef Q_OS_WIN
const QString GNUWIN_DIR = "/resources/gnuwin";
#endif

#ifdef Q_OS_LINUX
const QFont QK_EDITOR_DEFAULT_FONT("Monospace",9);
#else
const QFont QK_EDITOR_DEFAULT_FONT("Consolas",10);
#endif

#endif // QKIDE_GLOBAL_H
