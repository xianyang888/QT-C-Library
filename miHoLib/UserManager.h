// UserManager.h
#ifndef USERMANAGER_H
#define USERMANAGER_H

#include "User.h"
#include <vector>
#include <string>

class UserManager {
private:
    // 单例模式
    UserManager();
    
    // 禁用拷贝构造和赋值操作
    UserManager(const UserManager&) = delete;
    UserManager& operator=(const UserManager&) = delete;


    std::vector<User*> users;
    // 不同类型用户的文件路径
    const std::string adminFilepath = "../../res/data/admin.dat";
    const std::string readerFilepath = "../../res/data/reader.dat";

    void loadUsers();
    void loadAdmins();
    void loadReaders();
    void initializeAdmin(); 
    void saveAdmins();
    void saveReaders();


public:
    ~UserManager(); //自动析构
    // 获取单例实例的静态方法
    static UserManager& getInstance();

    User* authenticate(const std::string& account,
                       const std::string& password,
                       User::UserType expectedType) const;


//------增删改查----------------------------------------
    void saveUsers();

    // 添加新用户
    void addUser(User* newUser);

    // 删除用户
    void removeUser(const std::string& account);

    // 修改用户信息
    void updateUserInfo(const std::string& account,
        const std::string& newName,
        const std::string& newMajor);
    // 修改密码
    void changePassword(const std::string& account, 
                        const std::string& newPassword);


//-------获取器-----------------------------------------
    // 获取所有用户
    const std::vector<User*>& getAllUsers() const;
    // 获取特定类型的用户
    std::vector<User*> getUsersByType(User::UserType type) const;
    // 获得指定账号的用户
    User* getUserByAccount(const std::string& account) const;
};

#endif // USERMANAGER_H
