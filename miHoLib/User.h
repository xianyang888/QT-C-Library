#ifndef USER_H
#define USER_H

#include <string>
#include <stdexcept>

class User {
protected:
    std::string account;
    std::string password;

public:
    enum class UserType { ADMIN, READER };

    User(const std::string& acc, const std::string& pwd);
    virtual ~User() = default;

    // 序列化/反序列化方法
    virtual std::string serialize() const;
    static User* deserialize(const std::string& data);

    // 虚函数用于类型识别
    virtual UserType getType() const = 0;

    // 访问器
    virtual std::string getName() const = 0;
    virtual std::string getMajor() const = 0;
    std::string getAccount() const;
    std::string getPassword() const;

    // 验证密码
    bool verifyPassword(const std::string& input) const;

    void setAccount(const std::string& newAccount) {
        account = newAccount;
    }
    // 设置密码
    void setPassword(const std::string& newPassword) {
        password = newPassword;
    }
    virtual void setName(const std::string& name) = 0;
    virtual void setMajor(const std::string& major) = 0;

    // 异常类
    class DuplicateAccountException : public std::runtime_error {
    public:
        DuplicateAccountException() : std::runtime_error("Account already exists") {}
    };
};

#endif // USER_H
