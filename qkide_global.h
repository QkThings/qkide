/*
 * QkThings LICENSE
 * The open source framework and modular platform for smart devices.
 * Copyright (C) 2014 <http://qkthings.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QKIDE_GLOBAL_H
#define QKIDE_GLOBAL_H

#include <QtGlobal>
#include <QFont>

class QString;

#ifdef Q_OS_WIN
const QString GNUWIN_DIR = "/resources/gnuwin";
#endif

const QString DEFAULT_THEME = "white.theme";

//const QString SLASH = "/";
const QString QK_IDE_DOMAIN_STR = "qkthings";
const QString QK_IDE_NAME_STR = "QkIDE";
const QString QK_IDE_C_DEF_STR = "my_board.c";
const QString QK_IDE_H_DEF_STR = "my_board.h";

const QString EDITOR_FONT_NAME = "DejaVuSansMono";
const int     EDITOR_FONT_SIZE = 9;

const QString CTAGS_EXE = "/resources/tools/ctags/bin/ctags";
const QString EMB_DIR = "/resources/embedded";
const QString THEME_DIR = "/resources/theme";
const QString TOOLCHAIN_DIR = EMB_DIR + "/toolchain";

const QString TEMP_DIR = "/temp";
const QString TAGS_DIR = TEMP_DIR + "/tags";

//TODO These should be imported from a json file

const QString QKPROGRAM_DIR = EMB_DIR + "/qkprogram";
const QString QKPROGRAM_INC_DIR = QKPROGRAM_DIR + "/include";
const QString QKPROGRAM_LIB_DIR = QKPROGRAM_DIR + "/lib";
const QString QKPROGRAM_DOC_DIR = QKPROGRAM_DIR + "/doc";

const QString QKPERIPHERAL_DIR = EMB_DIR + "/qkperipheral";
const QString QKPERIPHERAL_DOC_DIR = QKPERIPHERAL_DIR + "/doc";

const QString QKDSP_DIR = EMB_DIR + "/qkdsp";
const QString QKDSP_DOC_DIR = QKDSP_DIR + "/doc";



#endif // QKIDE_GLOBAL_H
