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

#include "newprojectpage.h"
#include "../qkide_global.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFileDialog>
#include <QDir>

NewProjectPage::NewProjectPage(const QString &path, QWidget *parent) :
    QWizardPage(parent)
{
    QFormLayout *projectInfoLayout;
    QVBoxLayout *mainLayout;
    QHBoxLayout *hBox;


    m_defaultProjectPath = path;

    mainLayout = new QVBoxLayout(this);

    projectInfoLayout = new QFormLayout(this);
    projectNameEdit = new QLineEdit(this);
    projectInfoLayout->addRow(tr("Project name"), projectNameEdit);

    createInEdit = new QLineEdit;
    createInEdit->setReadOnly(true);
    browseButton = new QPushButton(tr("Browse..."));
    hBox = new QHBoxLayout(this);
    hBox->addWidget(createInEdit);
    hBox->addWidget(browseButton);
    projectInfoLayout->addRow(tr("Create in"), hBox);

    defaultPathCheck = new QCheckBox(tr("Use as default project location"));
    warningLabel = new QLabel("warning");
    warningLabel->setWordWrap(true);
    QPalette p;
    p.setColor(QPalette::Text, Qt::red);
    warningLabel->setPalette(p);
    warningLabel->hide();

    mainLayout->addLayout(projectInfoLayout);
    mainLayout->addWidget(defaultPathCheck);
    mainLayout->addWidget(warningLabel);

    setLayout(mainLayout);

    connect(projectNameEdit, SIGNAL(textChanged(QString)), this, SLOT(slotValidatePath()));
    connect(createInEdit, SIGNAL(textChanged(QString)), this, SLOT(slotValidatePath()));
    connect(browseButton, SIGNAL(clicked()), this, SLOT(slotBrowse()));

    registerField("projectName*", projectNameEdit);
    registerField("createIn*", createInEdit);
    registerField("saveDefaultProjectPath", defaultPathCheck);
}

void NewProjectPage::initializePage()
{
    createInEdit->setText(m_defaultProjectPath);
}

void NewProjectPage::slotBrowse()
{
    createInEdit->setText(QFileDialog::getExistingDirectory(this, tr("Choose Directory")));
}

void NewProjectPage::slotValidatePath()
{
    QString name = projectNameEdit->text();
    QString path = createInEdit->text();
    QString completePath;
    if(!name.isEmpty() && !path.isEmpty())
    {
        completePath = path + "/" + name;
        QDir projectDir(completePath);
        if(projectDir.exists()) {
            warningLabel->setText(tr("Warning!\n"
                                     "A project with the same name already exists on that location. "
                                     "If you continue, it will be overwritten. "
                                     "If that's not what you want, change the project name "
                                     "or its path."));
            warningLabel->show();
        }
        else {
            warningLabel->hide();
        }
    }
    else
        warningLabel->hide();
}


