#ifndef READER_H
#define READER_H

#include "User.h"
#include "Book.h"
#include <string>
#include <set>

class Reader : public User {
private:
    std::string name;
    std::string major;
    //记录自己的借阅数量
    int borrowedCount=0;

public:
    Reader(std::string acc, std::string pwd, const int count, std::string name="Default", std::string major="Default");

    // 读者相关方法
    void setName(const std::string& name) { this->name = name; };
    void setMajor(const std::string& major){ this->major = major; };
    std::string getName() const override { return name; };
    std::string getMajor() const override { return major; };

    // 序列化相关方法
    UserType getType() const override;
    std::string serialize() const override;

    // 借阅相关方法
    bool borrowBook(Book* book);
    bool returnBook(Book* book);
    bool hasBorrowed(const std::string& isbn) const;


    void incrementBorrowed();
    void decrementBorrowed();
    int getBorrowedCount() const;

    

    

    

};

#endif // READER_H
