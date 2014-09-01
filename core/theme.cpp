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
#include "qkutils.h"
#include "qkide_global.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariantMap>
#include <QWidget>


Theme::Theme()
{

}

bool Theme::generate(const QString &path, Theme *theme)
{
    QJsonDocument doc = QkUtils::jsonFromFile(path);

    if(doc.isNull())
        return false;

    QVariantMap themeJson = doc.object().toVariantMap();

    theme->globalStyle = themeJson["globalStyle"].toString();
    theme->editorStyle = themeJson["editorStyle"].toString();
    theme->syntax = themeJson["syntax"].toString();

    QVariantMap paletteJson = themeJson["palette"].toJsonObject().toVariantMap();
    if(!paletteJson.isEmpty())
    {
        theme->globalPalette.setColor(QPalette::Window,     QColor(paletteJson["window"].toString()));
        theme->globalPalette.setColor(QPalette::WindowText, QColor(paletteJson["windowText"].toString()));
        theme->globalPalette.setColor(QPalette::Base, QColor(paletteJson["base"].toString()));
        theme->globalPalette.setColor(QPalette::AlternateBase, QColor(paletteJson["alternateBase"].toString()));
        theme->globalPalette.setColor(QPalette::ToolTipBase, QColor(paletteJson["toolTipBase"].toString()));
        theme->globalPalette.setColor(QPalette::ToolTipText, QColor(paletteJson["toolTipText"].toString()));
        theme->globalPalette.setColor(QPalette::Text, QColor(paletteJson["text"].toString()));
        theme->globalPalette.setColor(QPalette::Button, QColor(paletteJson["button"].toString()));
        theme->globalPalette.setColor(QPalette::ButtonText, QColor(paletteJson["buttonText"].toString()));
    }

    return true;
}
