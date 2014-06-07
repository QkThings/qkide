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

//   QPalette darkPalette;
//   darkPalette.setColor(QPalette::Window, QColor(53,53,53));
//   darkPalette.setColor(QPalette::WindowText, Qt::white);
//   darkPalette.setColor(QPalette::Base, QColor(25,25,25));
//   darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
//   darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
//   darkPalette.setColor(QPalette::ToolTipText, Qt::white);
//   darkPalette.setColor(QPalette::Text, Qt::white);
//   darkPalette.setColor(QPalette::Button, QColor(53,53,53));
//   darkPalette.setColor(QPalette::ButtonText, Qt::white);
//   darkPalette.setColor(QPalette::BrightText, Qt::red);
//   darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

//   darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
//   darkPalette.setColor(QPalette::HighlightedText, Qt::black);
//   qApp->setPalette(darkPalette);


//   QString styleSheet;
//   styleSheet.append("QToolBar:horizontal {border: 1px solid transparent;}");
//   styleSheet.append("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
//   qApp->setStyleSheet(styleSheet);

    QkIDE w;
//    QFile styleFile(":/styles/main.qss");
//    styleFile.open(QFile::ReadOnly);
//    w.setStyleSheet(QString(styleFile.readAll()));

    w.show();
    
    return a.exec();
}
