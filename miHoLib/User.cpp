#include "User.h"
#include "Admin.h"
#include "Reader.h"
#include <sstream>

User::User(const std::string& acc, const std::string& pwd)
    : account(acc), password(pwd) {}

std::string User::serialize() const {
    std::ostringstream oss;
    oss << account << "," << password << ",";
    return oss.str();
}

User* User::deserialize(const std::string& data) {
    std::istringstream iss(data);
    std::string acc, pwd, type;

    // 提取前三个字段
    std::getline(iss, acc, ',');
    std::getline(iss, pwd, ',');
    std::getline(iss, type, ',');

    if (type == "ADMIN") {
        return new Admin(acc, pwd);
    } else if (type == "READER") {
        std::string borrowedStr, name, major;

        std::getline(iss, borrowedStr, ',');
        std::getline(iss, name, ',');
        std::getline(iss, major, ',');

        int borrowedCount = std::stoi(borrowedStr);
        Reader* reader = new Reader(acc, pwd, borrowedCount, name, major);
        return reader;
    }

    return nullptr;
}


std::string User::getAccount() const { return account; }
std::string User::getPassword() const { return password; }


bool User::verifyPassword(const std::string& input) const {
    return password == input;
}
