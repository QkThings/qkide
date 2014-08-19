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

    QStyle *style = QStyleFactory::create("Fusion");
    a.setStyle(style);

    QkIDE w;
    splash.hide();
    w.show();
    
    return a.exec();
}
