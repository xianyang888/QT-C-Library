#include "readerMainWindow.h"
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QLabel>
#include <QMessageBox>
#include "ResourceLoader.h"
#include "LoanManager.h"
#include "BookManager.h"

readerMainWindow::readerMainWindow(User* u,QWidget *parent)
 : MainWindow(parent),currentUser(u)
{
    setupBaseUI();      // 搭建基础 UI 结构
    setupNavigation();  // 设置导航栏按钮
    setupPages();       // 设置页面
}


QString readerMainWindow::getQssFilePath() const {
    return ":/style/res/style/readerWindow.qss";  
}

void readerMainWindow::setupNavigation()
{
    QVBoxLayout* navLayout = new QVBoxLayout(navWidget);
    QLabel* logo = ResourceLoader::loadAppLogo(200,navWidget);
    if (logo) {
        navLayout->addWidget(logo);
    }
    QStringList btnTexts = {u8"主页",u8"个人中心", u8"图书查询+借阅", u8"扩展功能", u8"关于"};
    QVector<QChar> icons = { 0xf015,   0xf007,      0xf002   ,    0xf067  ,   0xf05a}; 
                            //home,     user,       user-edit,  search,   extend,   about
    QButtonGroup* navBtnGroup = new QButtonGroup(navWidget);
    navBtnGroup->setExclusive(true);

    for (int i = 0; i < btnTexts.size(); ++i) {
        QPushButton* btn = createIconButton(icons[i], btnTexts[i]);
        navLayout->addWidget(btn);
        navBtnGroup->addButton(btn, i);
    }

    navLayout->addStretch();
    navBtnGroup->button(0)->setChecked(true); // 默认选中主页按钮

    connect(navBtnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, [this](int id) {
        stackedWidget->setCurrentIndex(id);
    });
}

void readerMainWindow::setupPages()
{
    // 创建页面实例
    searchPage = new SearchPage(currentUser,this);
    personalPage = new PersonalPage(currentUser, this);
    
    // 添加页面到stackedWidget
    // 按照导航按钮的顺序：主页(0), 个人中心(1), 信息修改(2), 图书查询(3), 扩展功能(4), 关于(5)
    stackedWidget->addWidget(new QWidget()); // 主页占位
    stackedWidget->addWidget(personalPage);  // 个人中心
    stackedWidget->addWidget(searchPage);    // 图书查询
    stackedWidget->addWidget(new QWidget()); // 扩展功能占位
    stackedWidget->addWidget(new QWidget()); // 关于占位
}

