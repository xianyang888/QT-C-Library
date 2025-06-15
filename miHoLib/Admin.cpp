#include "Admin.h"

Admin::Admin(std::string acc, std::string pwd)
    : User(std::move(acc), std::move(pwd)) {}

User::UserType Admin::getType() const {
    return UserType::ADMIN;
}

std::string Admin::serialize() const {
    std::string base = User::serialize();
    return base + "ADMIN,";
}
