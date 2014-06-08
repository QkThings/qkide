#ifndef THEME_H
#define THEME_H

#include <QPalette>

class Theme
{
public:
    Theme();

    static bool generate(const QString &path, Theme *theme);

    QPalette globalPalette;
    QString globalStyle;
    QString editorStyle;
    QString syntax;
    QString htmlCss;

private:

};

#endif // THEME_H
