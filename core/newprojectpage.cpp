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
    QFormLayout *projectInfoLayout, *targetInfoLayout;
    QVBoxLayout *mainLayout;
    QHBoxLayout *hBox;


    m_defaultProjectPath = path;
    //setTitle(tr("Project Information"));
    //setSubTitle(tr("Specify basic information about the class for which you "
                        //"want to generate skeleton source code files."));
    //setPixmap(QWizard::LogoPixmap, QPixmap(":/img/qk_small.png"));

    mainLayout = new QVBoxLayout(this);

    projectInfoLayout = new QFormLayout(this);
    projectNameEdit = new QLineEdit(this);
    projectInfoLayout->addRow(tr("Project name"), projectNameEdit);

    comboTargetName = new QComboBox(this);
    comboTargetVariant = new QComboBox(this);
    hBox = new QHBoxLayout(this);
    hBox->addWidget(comboTargetName);
    hBox->addWidget(comboTargetVariant);

//    projectInfoLayout->addRow(tr("Target"), hBox);

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

//    QFormLayout *targetLayout = new QFormLayout(this);
//    targetLayout->addRow(tr("Target"), hBox);

    mainLayout->addLayout(projectInfoLayout);
    mainLayout->addWidget(defaultPathCheck);
    mainLayout->addWidget(warningLabel);
//    mainLayout->addSpacerItem(new QSpacerItem(10,10,QSizePolicy::Minimum,QSizePolicy::Expanding));
//    mainLayout->addLayout(targetLayout);

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
        completePath = path + SLASH + name;
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


