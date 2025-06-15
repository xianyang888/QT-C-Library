#include "adminMainWindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QButtonGroup>
#include <QFile>
#include <QDebug>
#include "ResourceLoader.h"
#include <QPixmap>
#include "LoanManager.h"
#include "BookManager.h"
#include "User.h"
#include "Admin.h"
#include <map>
#include <algorithm>

adminMainWindow::adminMainWindow(QWidget* parent)
    : MainWindow(parent)
{
    setupBaseUI(); // 调用父类搭建结构
    setupNavigation();
    setupPages();
}


// 只修改setupPages方法
void adminMainWindow::setupPages()
{

    // 1. 导入子窗口
    homePage = new HomePage(u8"管理员", this); // 可以从登录信息获取真实管理员名称
    userPage = new UserPage(this);
    bookPage = new BookPage(this);
    loanPage = new BorrowPage(this);
    searchPage = new SearchPage(new Admin("1","1"),this);
    plusPage = new QWidget(this);
    aboutPage = new QWidget(this);    //TODO:

    // 2. 批量注册
    QVector<QWidget*> pages={
        homePage,
        userPage,
        bookPage,
        loanPage,
        searchPage,
        plusPage,
        aboutPage
    };
    // 3. 批量添加到stackedWidget
    for(QWidget* page:pages){
        stackedWidget->addWidget(page);
    }
}
//侧边栏
void adminMainWindow::setupNavigation()
{
    QVBoxLayout* layout = new QVBoxLayout(navWidget);
    QLabel* logo = ResourceLoader::loadAppLogo(200,navWidget);
    if (logo) {
        layout->addWidget(logo);
    }
    QPushButton* btnDashboard = createIconButton(QChar(0xf015), u8"主页");        // home
    QPushButton* btnUser = createIconButton(QChar(0xf0c0), u8"用户管理");    // users
    QPushButton* btnBook = createIconButton(QChar(0xf02d), u8"图书管理");    // book
    QPushButton* btnBorrow = createIconButton(QChar(0xf5da), u8"借阅管理");    // book-reader
    QPushButton* btnSearch = createIconButton(QChar(0xf002), u8"系统查询");    // search
    QPushButton* btnExtend = createIconButton(QChar(0xf067), u8"扩展功能");    // plus
    QPushButton* btnAbout = createIconButton(QChar(0xf05a), u8"关于");        // info-circle
    // 布局逻辑
    layout->addWidget(btnDashboard);
    layout->addWidget(btnUser);
    layout->addWidget(btnBook);
    layout->addWidget(btnBorrow);
    layout->addWidget(btnSearch);
    layout->addWidget(btnExtend);
    layout->addWidget(btnAbout);
    layout->addStretch();

    navWidget->setLayout(layout);

    // 合并逻辑单元
    QButtonGroup* sidebarGroup = new QButtonGroup(this);
    sidebarGroup->setExclusive(true);  // 只允许一个按钮处于 checked 状态
    sidebarGroup->addButton(btnDashboard,0);
    sidebarGroup->addButton(btnUser,1);
    sidebarGroup->addButton(btnBook,2);
    sidebarGroup->addButton(btnBorrow,3);
    sidebarGroup->addButton(btnSearch,4);
    sidebarGroup->addButton(btnExtend,5);
    sidebarGroup->addButton(btnAbout,6);

    // 初始选中
    btnDashboard->click();

    connect(sidebarGroup, QOverload<int>::of(&QButtonGroup::buttonClicked),this, [this](int id){
            getStackedWidget()->setCurrentIndex(id);});
}

QString adminMainWindow::getQssFilePath() const
{
    return ":/style/res/style/adminWindow.qss";  // 返回自己的 QSS 文件路径
}



