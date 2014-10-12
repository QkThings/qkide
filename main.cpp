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

#include <QApplication>
#include <QSettings>
#include <QFile>
#include <QStyle>
#include <QStyleFactory>
#include <QFontDatabase>
#include <QSplashScreen>
#include "qkide.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationDomain(QK_IDE_DOMAIN_STR);
    a.setApplicationName(QK_IDE_NAME_STR);

    QPixmap pixmap(":/splash.png");
    QSplashScreen splash(pixmap);
    splash.show();
    a.processEvents();

    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, a.applicationDirPath());
    QSettings::setDefaultFormat(QSettings::IniFormat);

    QFontDatabase::addApplicationFont("://fonts/Monaco.ttf");
    QFontDatabase::addApplicationFont("://fonts/Ubuntu-R.ttf");
    QFontDatabase::addApplicationFont("://fonts/Roboto-Regular.ttf");
    QFontDatabase::addApplicationFont("://fonts/Roboto-Thin.ttf");
    QFontDatabase::addApplicationFont("://fonts/DejaVuSansMono.ttf");

    QStyle *style = QStyleFactory::create("Fusion");
    a.setStyle(style);

    QkIDE w;
    splash.hide();
    w.show();
    
    return a.exec();
}
