// UserManager.cpp
#include "UserManager.h"
#include "User.h"
#include "Admin.h"
#include "Reader.h"
#include <fstream>
#include <stdexcept>

#include <iostream>
#include <filesystem>
#include <QDebug>

UserManager& UserManager::getInstance() {
    static UserManager instance;//C++11线程安全
    return instance;
}

UserManager::UserManager() {
    loadUsers();
    initializeAdmin();
    qDebug() << "UserManager initialized. Total users: " << users.size();
}

UserManager::~UserManager() {
    for(auto user : users) {
        delete user;
    }
}
void UserManager::loadAdmins() {
    std::ifstream adminFile(adminFilepath);
    if (!adminFile.is_open()) {
        throw std::runtime_error(u8"管理员数据文件打开失败");
    }
    
    std::string line;
    while (std::getline(adminFile, line)) {
        User* user = User::deserialize(line);
        if (user) {
            if (user->getType() == User::UserType::ADMIN) {
                users.push_back(user);
            } else {
                delete user;  // 类型不匹配，释放内存
            }
        }
    }
    adminFile.close();
}

void UserManager::loadReaders() {
    std::ifstream readerFile(readerFilepath);
    if (!readerFile.is_open()) {
        throw std::runtime_error(u8"读者数据文件打开失败");
    }
    
    std::string line;
    while (std::getline(readerFile, line)) {
        User* user = User::deserialize(line);
        if (user) {
            if (user->getType() == User::UserType::READER) {
                users.push_back(user);
            } else {
                delete user;  // 类型不匹配，释放内存
            }
        }
    }
    readerFile.close();
}

void UserManager::loadUsers() {
    // 清空当前用户列表
    for (auto user : users) {
        delete user;
    }
    users.clear();

    bool adminLoaded = false;
    bool readerLoaded = false;
    
    try {
        loadAdmins();
        adminLoaded = true;
    } catch (const std::runtime_error& e) {
        // 记录或忽略管理员加载失败
        qDebug() << u8"管理员数据文件加载失败：" << e.what();
    }
    
    try {
        loadReaders();
        readerLoaded = true;
    } catch (const std::runtime_error& e) {
        // 记录或忽略读者加载失败
        qDebug() << u8"读者数据文件加载失败：" << e.what();
    }
    
    // 如果两个文件都不存在，创建空文件
    if (!adminLoaded && !readerLoaded &&
        !std::filesystem::exists(adminFilepath) && 
        !std::filesystem::exists(readerFilepath)) {
        saveUsers();  // 创建空文件
    }
}
// 初始化管理员账号
void UserManager::initializeAdmin() {
    
    // 检查是否存在管理员账户
    bool adminExists = false;
    for (auto& user : users) {
        if (user->getType() == User::UserType::ADMIN) {
            adminExists = true;
            break;
        }
    }
    
    // 不存在则创建默认管理员
    if (!adminExists) {
        Admin* defaultAdmin = new Admin("admin", "admin123");
        users.push_back(defaultAdmin);
        saveUsers();
        
        // TODO 安全日志记录
        // Logger::logSecurityEvent("Default admin account created");
    }
}
void UserManager::saveAdmins() {
    std::ofstream adminFile(adminFilepath);
    if (!adminFile.is_open()) {
        throw std::runtime_error(u8"管理员数据文件保存失败");
    }
    
    for (const auto& user : users) {
        if (user->getType() == User::UserType::ADMIN) {
            adminFile << user->serialize() << "\n";  // 多态序列化~~~
        }
    }
    adminFile.close();
}

void UserManager::saveReaders() {
    std::ofstream readerFile(readerFilepath);
    if (!readerFile.is_open()) {
        throw std::runtime_error(u8"读者数据文件保存失败");
    }
    
    for (const auto& user : users) {
        if (user->getType() == User::UserType::READER) {
            readerFile << user->serialize() << "\n";  // 多态序列化~~~
        }
    }
    readerFile.close();
}

void UserManager::saveUsers() {
    try {
        saveAdmins();
    } catch (const std::runtime_error& e) {
        // 记录错误但继续尝试保存读者
        qDebug() << u8"管理员数据文件保存失败：" << e.what();
    }
    
    try {
        saveReaders();
    } catch (const std::runtime_error& e) {
        // 记录错误
        qDebug() << u8"读者数据文件保存失败：" << e.what();
    }
    
    // 如果两个保存都失败，重新抛出异常
    if (std::ifstream(adminFilepath).fail() && 
        std::ifstream(readerFilepath).fail()) {
        throw std::runtime_error(u8"所有用户数据保存失败");
    }
}

User* UserManager::authenticate(const std::string& account,
                                const std::string& password,
                                User::UserType expectedType) const
{
    for(const auto& user : users) {
        if(user->getAccount() == account) { 
            // 检查用户类型 
            if(user->getType() != expectedType) {
                throw std::invalid_argument(u8"用户类型不匹配");
            }

            // 验证密码
            if(user->verifyPassword(password)) {
                return user;
            }
            throw std::invalid_argument(u8"密码错误");
        }
    }
    throw std::invalid_argument(u8"账号不存在");
}

// 添加新用户
void UserManager::addUser(User* newUser) {
    for (auto user : users) {
        if (user->getAccount() == newUser->getAccount()) {
            delete newUser; // 清理
            throw User::DuplicateAccountException();
        }
    }
    users.push_back(newUser);
    saveUsers();
}

// 删除用户
void UserManager::removeUser(const std::string& account) {
    for (auto it = users.begin(); it != users.end(); ++it) {
        if ((*it)->getAccount() == account) {
            delete *it;
            users.erase(it);
            saveUsers();
            return;
        }
    }
    throw std::invalid_argument(u8"用户不存在");
}

// 修改用户信息
void UserManager::updateUserInfo(const std::string& account,
    const std::string& newName,
    const std::string& newMajor) {
    // 查找用户
    User* user = getUserByAccount(account);
    if (!user) {
        throw std::invalid_argument(u8"用户不存在");
    }

    // 检查用户类型是否支持修改（可扩展，如禁止修改管理员专业）
    if (user->getType() == User::UserType::ADMIN && !newMajor.empty()) {
        throw std::logic_error(u8"管理员不允许设置专业");
    }

    // 更新信息
    user->setName(newName);
    if (!newMajor.empty()) { // 允许为空（如管理员无需专业）
        user->setMajor(newMajor);
    }

    // 保存数据
    saveUsers();
}

// 修改密码
void UserManager::changePassword(const std::string& account, 
                                 const std::string& newPassword) {
    for (auto& user : users) {
        if (user->getAccount() == account) {
            user->setPassword(newPassword);
            saveUsers();
            return;
        }
    }
    throw std::invalid_argument(u8"用户不存在");
}

// 获取所有用户
const std::vector<User*>& UserManager::getAllUsers() const {
    return users;
}

// 获取特定类型的用户
std::vector<User*> UserManager::getUsersByType(User::UserType type) const {
    std::vector<User*> result;
    for (const auto& user : users) {
        if (user->getType() == type) {
            result.push_back(user);
        }
    }
    return result;
}

User* UserManager::getUserByAccount(const std::string& account) const
{
    User* result = nullptr;
    for (const auto& user : users) {
        if (user->getAccount() == account) {
            result = user;
            break;
        }
    }
    return result;
}
