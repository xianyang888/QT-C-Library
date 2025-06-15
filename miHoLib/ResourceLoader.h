#ifndef RESOURCELOADER_H
#define RESOURCELOADER_H

#include <QString>
#include <QFontDatabase>
#include <QFile>
#include <QWidget>
#include <QPixmap>
#include <QIcon>
#include <QLabel>

class ResourceLoader
{
public:

    // 获取器
    QString getLogoPixmapPath() {return logoPixmap_path;}

    // 加载所有资源
    static void loadAll();

    // 字体加载
    static QString loadFontAwesome();   // 返回字体名称

    // QSS 加载
    static QString loadQSS(const QString& qssPath);

    // QSS 设置(职责分离)
    static void applyQSS(QWidget* target, const QString& qssPath);

    // 图标 / Logo 加载
    static QLabel* loadAppLogo(int size = 24, QWidget* parent = nullptr);    // 默认加载标准尺寸 Logo

private:
    static bool fontAwesomeLoaded;
    static QString cachedFontFamily;
    QString logoPixmap_path=":/icons/res/icons/miHoLib.png";
};

#endif // RESOURCELOADER_H
