#include <QApplication>
#include <QSettings>
#include <QFile>
#include <QStyle>
#include <QStyleFactory>
#include <QFontDatabase>
#include "qkide.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationDomain(QK_IDE_DOMAIN_STR);
    a.setApplicationName(QK_IDE_NAME_STR);

    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, a.applicationDirPath());
    QSettings::setDefaultFormat(QSettings::IniFormat);

    QFontDatabase::addApplicationFont("://fonts/Monaco.ttf");
    QFontDatabase::addApplicationFont("://fonts/Inconsolata.otf");
    QFontDatabase::addApplicationFont("://fonts/EnvyCodeR.ttf");
    QFontDatabase::addApplicationFont("://fonts/Saxmono.ttf");
    QFontDatabase::addApplicationFont("://fonts/SourceCodePro.otf");
    QFontDatabase::addApplicationFont("://fonts/LiberationMono.ttf");
    QFontDatabase::addApplicationFont("://fonts/Ubuntu-R.ttf");
    QFontDatabase::addApplicationFont("://fonts/UbuntuMono-R.ttf");
    QFontDatabase::addApplicationFont("://fonts/DroidSansMono.ttf");
    QFontDatabase::addApplicationFont("://fonts/VeraMono.ttf");
    QFontDatabase::addApplicationFont("://fonts/DejaVuSansMono.ttf");
    QFontDatabase::addApplicationFont("://fonts/Roboto-Regular.ttf");
    QFontDatabase::addApplicationFont("://fonts/Roboto-Thin.ttf");

    QStyle *style = QStyleFactory::create("Fusion");
    a.setStyle(style);

    QFile styleFile(":/styles/main.qss");
    styleFile.open(QFile::ReadOnly);
    a.setStyleSheet(QString(styleFile.readAll()));

    QkIDE w;
    w.show();
    
    return a.exec();
}
