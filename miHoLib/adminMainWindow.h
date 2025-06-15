#ifndef ADMINMAINWINDOW_H
#define ADMINMAINWINDOW_H

#include "mainWindow.h"
#include <QStackedWidget>

//pages
#include "homepage.h"
#include "userPage.h"
#include "bookPage.h"
#include "loanPage.h"
#include "searchPage.h"


class adminMainWindow : public MainWindow
{
    Q_OBJECT
public:
    explicit adminMainWindow(QWidget *parent = nullptr);
    ~adminMainWindow()override =default;

protected:
    QString getQssFilePath() const override;  // 实现虚函数

private: 
    // 搭建器(重写)
    void setupPages() override;
    void setupNavigation() override;
    
private:
    HomePage* homePage;
    UserPage* userPage;
    BookPage* bookPage;
    BorrowPage* loanPage;
    SearchPage* searchPage;
    QWidget* plusPage;
    QWidget* aboutPage;
};

#endif // ADMINMAINWINDOW_H
