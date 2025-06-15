#ifndef STYLESCALER_H
#define STYLESCALER_H

#include <QString>
#include <QWidget>

class StyleScaler
{
public:
    static void applyScaledStyle(QWidget* target, const QString& qssFilePath, qreal scale, int baseFontSize = 16);

private:
    static QString loadAndScaleQSS(const QString& qssFilePath, qreal scale, int baseFontSize);
};

#endif // STYLESCALER_H

