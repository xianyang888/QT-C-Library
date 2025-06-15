#ifndef USERPAGE_H
#define USERPAGE_H

#include "tablePage.h"
#include "userManager.h"
#include <QRadioButton>
#include <QWidget>
#include <QPushButton>

class UserPage : public tablePage
{
    Q_OBJECT
public:
    explicit UserPage(QWidget *parent = nullptr);
    ~UserPage() = default;

private:
    QPushButton *addUserBtn;
    UserManager& userManager;

    // 初始化UI特有部分
    void initUserPageUI();
    // 设置表格列
    void setupTableColumns() override;
    // 填充表格数据
    void fillTableData() override;
    // 添加操作按钮
    void add_Operation_Buttons(int row);

//增删改    
private slots:
    // 添加新用户
    void onAddUser();
    // 修改用户
    void onEditUser(const std::string& account);
    // 重置用户密码
    void onResetPassword(const std::string& account);
    // 删除用户
    void onDeleteUser(const std::string& account);
};
#endif // USERPAGE_H
