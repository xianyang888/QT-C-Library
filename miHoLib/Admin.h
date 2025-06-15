#ifndef ADMIN_H
#define ADMIN_H

#include "User.h"

class Admin : public User {
public:
    Admin(std::string acc, std::string pwd);

    UserType getType() const override;
    std::string getName() const override {return "Admin";};
    std::string getMajor() const override {return "Admin";};

    void setName(const std::string& ) override {};
    void setMajor(const std::string& ) override {};

    std::string serialize() const override;
};

#endif // ADMIN_H
