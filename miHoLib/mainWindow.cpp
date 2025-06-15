#include "mainWindow.h"
#include <QHBoxLayout>
#include "StyleScaler.h"
#include "ResourceLoader.h"
#include <QSizePolicy>
#include <QPushButton>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    stackedWidget = new QStackedWidget(this);
    navWidget = new QWidget(this);
    navWidget->setObjectName("navWidget");
}
// 基础UI设置
void MainWindow::setupBaseUI()
{
    resize(1200,800);
    //1:5界面比例
    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->addWidget(navWidget,1);
    mainLayout->addWidget(stackedWidget,5); 

    //设置布局
    QWidget* central = new QWidget(this);
    central->setLayout(mainLayout);
    setCentralWidget(central);
    
    //导入并应用qss 
    QString qssFile = getQssFilePath();                 // 交由派生类决定路径 多态~~~
    ResourceLoader::applyQSS(this,qssFile);
}
// 重写缩放事件
void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);

    int baseWidth = 1200;
    int baseHeight = 800;

    qreal widthScale = qreal(this->width()) / baseWidth;
    qreal heightScale = qreal(this->height()) / baseHeight;
    qreal scale = 0.7 * widthScale + 0.3 * heightScale;

    if (scale < 0.8) scale = 0.8;
    if (scale > 1.5) scale = 1.5;

    QString qssFile = getQssFilePath();  // 交由派生类决定路径
    if (!qssFile.isEmpty())
        StyleScaler::applyScaledStyle(this, qssFile, scale);
}

QPushButton* MainWindow::createIconButton(const QChar& iconChar, const QString& text)
{
    QPushButton* button = new QPushButton();
    button->setCheckable(true);
    button->setObjectName("sidebarButton");
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QHBoxLayout* layout = new QHBoxLayout(button);
    layout->setContentsMargins(12, 4, 12, 4);
    layout->setSpacing(10);  // 控制图标和文字之间的间距

    // 图标标签
    QLabel* iconLabel = new QLabel(QString(iconChar));
    iconLabel->setObjectName("iconLabel");

    QString fontFamily = ResourceLoader::loadFontAwesome();
    QFont iconFont(fontFamily);
    iconFont.setWeight(QFont::Black);
    iconFont.setPointSize(14);
    iconLabel->setFont(iconFont);

    iconLabel->setFixedWidth(30);  // 图标区域统一宽度
    iconLabel->setAlignment(Qt::AlignCenter);

    // 文字标签
    QLabel* textLabel = new QLabel(text);
    textLabel->setObjectName("textLabel");
    QFont textFont("Microsoft YaHei", 14);
    textLabel->setFont(textFont);
    textLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);  // 靠左对齐，便于多按钮统一排布

    layout->addWidget(iconLabel);
    layout->addWidget(textLabel, 1);  // 设置拉伸因子使文本自然占满剩余空间

    return button;
}

