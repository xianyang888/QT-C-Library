#ifndef PERSONALPAGE_H
#define PERSONALPAGE_H

#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QFormLayout>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "User.h"
#include "Reader.h"
#include "UserManager.h"
#include "userEditDialog.h"
#include "LoanManager.h"

class PersonalPage : public QWidget
{
    Q_OBJECT
public:
    explicit PersonalPage(User* u,QWidget* parent = nullptr);


private slots:
    void onEditProfile();          // 点击"编辑信息"按钮
    void onPasswordChange();       // 点击"修改密码"按钮
    void onViewCurrentLoans();     // 查看当前借阅
    void onReturnBook();           // 归还图书

private:
    // === 数据 ===
    User* m_reader;
    // === UI 部件 ===
    QLabel* accountValueLbl;
    QLabel* nameValueLbl;
    QLabel* majorValueLbl;
    QLabel* borrowedValueLbl;      // 当前借阅数量 / 上限

    QPushButton* editBtn;
    QPushButton* changePwdBtn;
    QPushButton* viewLoansBtn;     // 查看当前借阅按钮
    QPushButton* returnBookBtn;    // 归还图书按钮
    
    QTableWidget* loansTable;      // 借阅记录表格

    // === 布局与显示 ===
    void initUI();
    void refreshDisplay();         // 把 Reader 数据刷新到界面
    void refreshLoansTable();      // 刷新借阅记录表格

    // === 辅助 ===
    void openPasswordDialog();     // 弹出修改密码对话框
    void setupLoansTable();        // 设置借阅记录表格
};

#endif // PERSONALPAGE_H
