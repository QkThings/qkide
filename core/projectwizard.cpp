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

#include "projectwizard.h"
#include "newprojectpage.h"

#include "../qkide.h"
#include "../qkide_global.h"

#include <QVariant>
#include <QDir>

ProjectWizard::ProjectWizard(QkIDE *ide, QWidget *parent) :
    QWizard(parent),
    m_ide(ide)
{
    addPage(new NewProjectPage(ide->m_projectDefaultLocation));

    setPixmap(QWizard::BannerPixmap, QPixmap(":/images/banner.png"));
    setPixmap(QWizard::BackgroundPixmap, QPixmap(":/images/background.png"));

    setWizardStyle(QWizard::ModernStyle);
    setFixedSize(400, 200);

    setWindowTitle(tr("New Project"));
}

void ProjectWizard::accept()
{
    projectName = field("projectName").toString();
    createIn = field("createIn").toString();
    saveDefaultPath = field("saveDefaultProjectPath").toBool();

    QString completePath = createIn + "/" + projectName;
    QDir projectPath(completePath);
    if(!projectPath.exists())
    {
        projectPath.cdUp();
        projectPath.mkdir(projectName);
    }

    QDialog::accept();
}
