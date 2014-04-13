#include "projectpreferencesdialog.h"
#include "ui_projectpreferencesdialog.h"

ProjectPreferencesDialog::ProjectPreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProjectPreferencesDialog)
{
    ui->setupUi(this);
}

ProjectPreferencesDialog::~ProjectPreferencesDialog()
{
    delete ui;
}
