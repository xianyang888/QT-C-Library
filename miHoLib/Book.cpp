#include "Book.h"
#include <sstream>

Book::Book() : status(Status::AVAILABLE), totalCopies(1), availableCopies(1) {}

Book::Book(const std::string& title, const std::string& author, 
           const std::string& isbn, const std::string& category, 
           const std::string& publisher,int totalCopies)
    : title(title), author(author), isbn(isbn), category(category), publisher(publisher),
      status(Status::AVAILABLE), borrower(""), totalCopies(totalCopies), availableCopies(totalCopies) {}

Book::Status Book::computeStatus() const {
    if (totalCopies == 0) {
        return Status::MAINTENANCE;
    } else if (availableCopies == 0) {
        return Status::BORROWED;
    } else {
        return Status::AVAILABLE;
    }
}


//================
//批量修改副本数量
void Book::setTotalCopies(int copies) {
    if (copies < 0) copies = 0;

    int borrowedCount = totalCopies - availableCopies;
    totalCopies = copies;

    // 可借副本 = 新总数 - 已借出数，且不能为负
    availableCopies = std::max(0, totalCopies - borrowedCount);
}

//增加可借数量
void Book::incrementAvailableCopies() {
    if (availableCopies < totalCopies) {
        availableCopies++;
    }
}

//减少可借数量
void Book::decrementAvailableCopies() {
    if (availableCopies > 0) {
        availableCopies--;
    }
}


//=============序列化和反序列化====================
std::string Book::serialize() const {
    std::ostringstream oss;
    oss << title << "," << author << "," << isbn << "," << category << ","
        << publisher << "," << static_cast<int>(status) << "," << borrower << ","
        << totalCopies << "," << availableCopies;
    return oss.str();
}

// 修改反序列化方法
Book* Book::deserialize(const std::string& data) {
    std::istringstream iss(data);
    std::string title, author, isbn, category, publisher, statusStr, borrower, totalCopiesStr, availableCopiesStr;
    
    std::getline(iss, title, ',');
    std::getline(iss, author, ',');
    std::getline(iss, isbn, ',');
    std::getline(iss, category, ',');
    std::getline(iss, publisher, ',');
    std::getline(iss, statusStr, ',');
    std::getline(iss, borrower, ',');
    
    Book* book = new Book(title, author, isbn, category, publisher);
    
    try {
        int statusInt = std::stoi(statusStr);
        book->status = static_cast<Status>(statusInt);
        
        // 读取库存量和可借阅数量
        if (std::getline(iss, totalCopiesStr, ',')) {
            book->totalCopies = std::stoi(totalCopiesStr);
        }
        
        if (std::getline(iss, availableCopiesStr)) {
            book->availableCopies = std::stoi(availableCopiesStr);
        } else {
            // 兼容旧数据，如果没有可借阅数量字段，则设置为总库存
            book->availableCopies = book->totalCopies;
        }
    } catch (const std::exception& e) {
        book->status = Status::AVAILABLE;
        book->totalCopies = 1;
        book->availableCopies = 1;
    }
    
    book->borrower = borrower;
    return book;
}
//状态转字符串
std::string Book::getStatusString() const {
    switch (status) {
        case Status::AVAILABLE:
            return u8"可借阅";
        case Status::BORROWED:
            return u8"已借出";
        case Status::MAINTENANCE:
            return u8"维护中";
        default:
            return u8"未知";
    }
}
