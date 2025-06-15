#include "mainWindow.h"
#include "Login.h"
#include "adminMainWindow.h"
#include "readerMainWindow.h"
#include "User.h"
#include <filesystem>
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    loginDialog login;
    std::cout << "Working dir: " << std::filesystem::current_path() << std::endl;
    
    if(login.exec() == QDialog::Accepted)
    {
        // 获取当前登录用户
        User* currentUser = login.getCurrentUser();
        
        if(currentUser != nullptr)
        {
            // 根据用户类型显示不同的主窗口
            if(currentUser->getType() == User::UserType::ADMIN)
            {
                // 管理员窗口
                adminMainWindow* adminWindow = new adminMainWindow();
                adminWindow->show();
            }
            else if(currentUser->getType() == User::UserType::READER)
            {
                // 读者窗口
                readerMainWindow* readerWindow = new readerMainWindow(currentUser);
                readerWindow->show();
            }
            
            return a.exec();
        }
    }
    
    return 0;
}
