#include "Reader.h"
#include <sstream>

Reader::Reader(std::string acc, std::string pwd, const int count, std::string name, std::string major)
    : User(std::move(acc), std::move(pwd)), borrowedCount(count), name(name) , major(major) {}

User::UserType Reader::getType() const {
    return UserType::READER;
}

std::string Reader::serialize() const {
    std::string base = User::serialize();
    std::ostringstream oss;
    oss << base << "READER," << borrowedCount << "," << getName() << "," << getMajor();
    return oss.str();
}

void Reader::incrementBorrowed() { borrowedCount++; }

void Reader::decrementBorrowed() { if(borrowedCount > 0) borrowedCount--; }

int Reader::getBorrowedCount() const { return borrowedCount; }
