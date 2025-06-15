#include "StyleScaler.h"
#include <QFile>
#include <QRegularExpression>

void StyleScaler::applyScaledStyle(QWidget* target, const QString& qssFilePath, qreal scale, int baseFontSize)
{
    QString style = loadAndScaleQSS(qssFilePath, scale, baseFontSize);
    target->setStyleSheet(style);
}

QString StyleScaler::loadAndScaleQSS(const QString& qssFilePath, qreal scale, int baseFontSize)
{
    QFile qssFile(qssFilePath);
    if (!qssFile.open(QFile::ReadOnly))
        return "";

    QString style = QString::fromUtf8(qssFile.readAll());
    qssFile.close();

    int scaledFontSize = int(baseFontSize * scale);

    if (style.contains("font-size"))
    {
        static QRegularExpression fs(R"(font-size\s*:\s*\d+px)");
        style.replace(fs,QString("font-size: %1px").arg(scaledFontSize));
    }
    else
    {
        style += QString("\nQWidget { font-size: %1px; }").arg(scaledFontSize);
    }

    return style;
}

