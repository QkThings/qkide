#include "projectwizard.h"
#include "newprojectpage.h"

#include "../qkide.h"
#include "../QkIDE_global.h"

#include <QVariant>
#include <QDir>

ProjectWizard::ProjectWizard(QkIDE *ide, QWidget *parent) :
    QWizard(parent),
    m_ide(ide)
{
    addPage(new NewProjectPage(m_ide->m_projectDefaultLocation));

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
    defaultPath = field("defaultPath").toBool();

    QString completePath = createIn + SLASH + projectName;
    QDir projectPath(completePath);
    if(!projectPath.exists()) {
        projectPath.cdUp();
        projectPath.mkdir(projectName);
    }

    QDialog::accept();
}
