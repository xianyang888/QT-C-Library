#include <QHeaderView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDebug>


#include "userPage.h"
#include "UserManager.h"
#include "Reader.h"
#include "Admin.h"
#include "userAdd_register.h"
#include "userEditDialog.h"

UserPage::UserPage(QWidget *parent)
    : tablePage(parent),userManager(UserManager::getInstance())
{
    initUserPageUI();
    setupTableColumns();
    fillTableData();
}

// 初始化UI
void UserPage::initUserPageUI()
{
    // 在表格上方添加用户特有的控件
    QHBoxLayout *topLayout = new QHBoxLayout();
    // 添加新增用户按钮
    addUserBtn = new QPushButton(u8"新增用户");
    topLayout->addStretch();
    topLayout->addWidget(addUserBtn);

    connect(addUserBtn, &QPushButton::clicked, this, &UserPage::onAddUser);
    
    // 插入到主布局中
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(layout());
    if (mainLayout) {
        mainLayout->insertLayout(0, topLayout);
    }
}

//设置当前页的表格布局
void UserPage::setupTableColumns()
{
    tableWidget->setColumnCount(6);
    QStringList headers = {u8"姓名", u8"学号", u8"专业", u8"修改", u8"重置密码", u8"删除"};
    tableWidget->setHorizontalHeaderLabels(headers);
}

// 填充用户数据
void UserPage::fillTableData()
{
    // 清空表格
    tableWidget->setRowCount(0);
    
    // 获取所有用户
    std::vector<User*> allUsers = userManager.getAllUsers();
    
    // 计算总页数
    totalPages = (allUsers.size() + pageCapacity - 1) / pageCapacity;    // 向上取整
    if (totalPages == 0) totalPages = 1;
    
    // 确保当前页在有效范围内
    if (currentPage > totalPages) currentPage = totalPages;
    if (currentPage < 1) currentPage = 1;
    
    // 计算当前页对应数据的起始和结束索引
    int startIndex = (currentPage - 1) * pageCapacity;
    int endIndex = std::min(startIndex + pageCapacity, static_cast<int>(allUsers.size()));  // 防止越界
    
//----------填充表格数据---------------------------------------------------
    for (int i = startIndex; i < endIndex; i++) {
        User* user = allUsers[i];
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        
        // 设置姓名
        QTableWidgetItem* nameItem = new QTableWidgetItem(QString::fromLocal8Bit(user->getName().c_str()));  //多态调用~~~
        tableWidget->setItem(row, 0, nameItem);
        
        // 设置学号
        QTableWidgetItem* idItem = new QTableWidgetItem(QString::fromStdString(user->getAccount()));
        tableWidget->setItem(row, 1, idItem);
        
        // 设置专业 
        QTableWidgetItem* majorItem = new QTableWidgetItem(QString::fromLocal8Bit(user->getMajor().c_str()));
        tableWidget->setItem(row, 2, majorItem);
        
        // 添加操作按钮
        add_Operation_Buttons(row);
    }
    
    // 更新分页标签
    updatePageLabel();
}

//添加操作按钮
void UserPage::add_Operation_Buttons(int row)
{
    // 获取用户账号
    std::string account = tableWidget->item(row, 1)->text().toStdString();
    
    // 修改按钮
    QPushButton *editBtn = new QPushButton(u8"修改");
    editBtn->setObjectName("editBtn");
    connect(editBtn, &QPushButton::clicked, [this, account]() {
        onEditUser(account);
    });
    
    // 重置密码按钮
    QPushButton *resetPwdBtn = new QPushButton(u8"重置");
    resetPwdBtn->setObjectName("resetPwdBtn");
    connect(resetPwdBtn, &QPushButton::clicked, [this, account]() {
        onResetPassword(account);
    });
    
    
    // 删除按钮
    QPushButton *deleteBtn = new QPushButton(u8"删除");
    deleteBtn->setObjectName("deleteBtn");
    connect(deleteBtn, &QPushButton::clicked, [this, account]() {
        onDeleteUser(account);
    });
    
    // 将按钮添加到表格中
    tableWidget->setCellWidget(row, 3, editBtn);
    tableWidget->setCellWidget(row, 4, resetPwdBtn);
    tableWidget->setCellWidget(row, 5, deleteBtn);
}


// 添加新用户
void UserPage::onAddUser()
{
    AdminAddUserDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString name = dialog.getName();
        QString account = dialog.getAccount();
        QString password = dialog.getPassword();
        QString major = dialog.getMajor();
        QString type = dialog.getUserType();

        // 根据类型构建对象
        User* newUser = nullptr;
        if (type == "READER")
            newUser = new Reader(account.toStdString(), password.toStdString(),0, name.toStdString(), major.toStdString());
        else if (type == "ADMIN")
            newUser = new Admin(account.toStdString(), password.toStdString());

        // 添加到 UserManager
        if (newUser) {
            userManager.addUser(newUser);
            QMessageBox::information(this, "成功", "用户添加成功！");
            fillTableData(); // 刷新列表
        } else {
            QMessageBox::warning(this, "失败", "添加用户失败！");
            delete newUser;
        }
    }
}


void UserPage::onEditUser(const std::string& account)
{
    // 获取用户信息
    User* user = userManager.getUserByAccount(account);
    if (!user) {
        QMessageBox::warning(this, u8"错误", u8"未找到指定用户！");
        return;
    }
    
    // 获取用户详细信息
    QString name = QString::fromStdString(user->getName());
    QString major;
    QString userType;
    
    // 根据用户类型获取专业信息
    Reader* reader = dynamic_cast<Reader*>(user);
    Admin* admin = dynamic_cast<Admin*>(user);
    
    if (reader) {
        major = QString::fromStdString(reader->getMajor());
        userType = "READER";
    } else if (admin) {
        major = "ADMIN"; 
        userType = "ADMIN";
    } else {
        QMessageBox::warning(this, u8"错误", u8"用户类型未知！");
        return;
    }
    
    // 创建编辑对话框
    UserEditDialog dialog(QString::fromStdString(account), name, major, userType, this);
    
    // 显示对话框并处理结果
    if (dialog.exec() == QDialog::Accepted) {
        // 获取修改后的数据
        QString newName = dialog.getName();
        QString newMajor = dialog.getMajor();
        QString newUserType = dialog.getUserType();
        
        // 验证数据
        if (newName.isEmpty() || newMajor.isEmpty()) {
            QMessageBox::warning(this, u8"错误", u8"姓名和专业不能为空！");
            return;
        }
        
        try {
            // 更新用户信息
            user->setName(newName.toStdString());
            
            // 如果是读者，更新专业信息
            if (reader && newUserType == "READER") {
                reader->setMajor(newMajor.toStdString());
            }
            // 如果用户类型发生变化，需要特殊处理
            else if ((reader && newUserType == "ADMIN") || (admin && newUserType == "READER")) {
                QMessageBox::information(this, u8"提示", u8"用户类型变更功能暂未实现，仅更新基本信息");
            }
            
            // 保存到文件
            userManager.saveUsers();
            
            // 刷新表格显示
            fillTableData();
            
            QMessageBox::information(this, u8"成功", u8"用户信息修改成功！");
            
        } catch (const std::exception& e) {
            QMessageBox::critical(this, u8"错误", 
                QString(u8"修改用户信息失败：%1").arg(e.what()));
        }
    }
}


// 重置用户密码
void UserPage::onResetPassword(const std::string& account)
{
    //TODO:
    // 确认对话框
    QMessageBox::StandardButton reply = QMessageBox::question(this, u8"确认", 
        QString(u8"确定要重置用户 %1 的密码吗？").arg(QString::fromStdString(account)),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::No) {
        return;
    }
}
// 删除用户
void UserPage::onDeleteUser(const std::string& account)
{
    //TODO:
    QMessageBox::StandardButton reply = QMessageBox::question(this, u8"确认", 
        QString(u8"确定要删除用户 %1 吗？此操作不可恢复！").arg(QString::fromStdString(account)),
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
        return;
    }
}

