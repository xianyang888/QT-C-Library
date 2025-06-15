#include "ResourceLoader.h"
#include <QDebug>


bool ResourceLoader::fontAwesomeLoaded = false;
QString ResourceLoader::cachedFontFamily = "";

void ResourceLoader::loadAll()
{
    loadFontAwesome();
    // 后续如有图片/翻译资源等，也在这里集中加载
}

QString ResourceLoader::loadFontAwesome()
{
    if (fontAwesomeLoaded && !cachedFontFamily.isEmpty()) {
        return cachedFontFamily;
    }

    int id = QFontDatabase::addApplicationFont(":/font/res/font/fa-solid-900.ttf");
    if (id == -1) {
        qWarning() << u8"[ResourceLoader] Font Awesome 5 Free Solid 加载失败！";
        return "";
    }

    QStringList fontFamilies = QFontDatabase::applicationFontFamilies(id);
    if (!fontFamilies.isEmpty()) {
        cachedFontFamily = fontFamilies.first();
        qDebug() << "[Font] Font Awesome loaded:" << cachedFontFamily;
        fontAwesomeLoaded = true;
    }
    return cachedFontFamily;
}


QString ResourceLoader::loadQSS(const QString& qssPath) {
    QFile qss(qssPath);
    if (!qss.open(QFile::ReadOnly)) {
        qWarning() << "[ResourceLoader] 无法打开 QSS 文件:" << qssPath;
        return "";
    }
    return QLatin1String(qss.readAll());
}

void ResourceLoader::applyQSS(QWidget* target, const QString& qssPath) {
    QString style = loadQSS(qssPath);
    if (!style.isEmpty()) {
        target->setStyleSheet(style);
    }
}

QLabel* ResourceLoader::loadAppLogo(int size, QWidget* parent) {
    QPixmap logo(":/icons/res/icons/miHoLib.png");
    QPixmap scaledLogo = logo.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QLabel* logoLabel = new QLabel(parent);
    logoLabel->setPixmap(scaledLogo);
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setObjectName("logo");
    return logoLabel;
}



