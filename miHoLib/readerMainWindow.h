#ifndef READERMAINWINDOW_H
#define READERMAINWINDOW_H

#include <QPushButton>
#include <QVector>
#include <QMainWindow>
#include "mainWindow.h"
#include "searchPage.h"
#include "personalPage.h"
#include "Reader.h"
#include "User.h"


class readerMainWindow : public MainWindow
{
    Q_OBJECT
public:
    explicit readerMainWindow(User *u,QWidget *parent = nullptr);
    ~readerMainWindow()=default;

protected:
    QString getQssFilePath() const override;

private:
    User* currentUser;
    // QWidget* navWidget;
    // QStackedWidget* stackedWidget;          // 用getter代替
    void setupNavigation() override;           // 创建导航栏按钮
    void setupPages() override;                // 创建页面
    
    // 页面成员变量
    PersonalPage* personalPage;
    SearchPage* searchPage;

    
    // 处理借阅/归还操作
    void handleBorrowRequest(const QString& isbn);
    void handleReturnRequest(const QString& bookTitle);
};

#endif // READERMAINWINDOW_H
