#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <vector>

class Book
{
public:
    enum class Status { AVAILABLE, BORROWED, MAINTENANCE };
    
    Book();
    Book(const std::string& title, const std::string& author, 
         const std::string& isbn, const std::string& category,
         const std::string& publisher = "", 
         int totalCopies = 1);
    //状态计算
    Status computeStatus() const;
    //借阅相关
    bool isAvailable() const { return availableCopies > 0; }

    // 获取器
    Status getStatus() const { return computeStatus(); }
    std::string getTitle() const { return title; }
    std::string getAuthor() const { return author; }
    std::string getISBN() const { return isbn; }
    std::string getCategory() const { return category; }
    std::string getPublisher() const { return publisher; }
    std::string getBorrower() const { return borrower; }
    int getTotalCopies() const { return totalCopies; }
    int getAvailableCopies() const { return availableCopies; }
    
    // 设置器
    void setTitle(const std::string& title) { this->title = title; }
    void setAuthor(const std::string& author) { this->author = author; }
    void setISBN(const std::string& isbn) { this->isbn = isbn; }
    void setCategory(const std::string& category) { this->category = category; }
    void setStatus(Status status) { this->status = status; }
    void setPublisher(const std::string& publisher) { this->publisher = publisher; }
    
    // 库存管理
    void setTotalCopies(int copies);
    void incrementAvailableCopies();
    void decrementAvailableCopies();

    
    // 序列化和反序列化
    std::string serialize() const;
    static Book* deserialize(const std::string& data);
    
    // 获取状态的字符串表示
    std::string getStatusString() const;
    
private:
    std::string title;
    std::string author;
    std::string isbn;
    std::string category;
    std::string publisher;
    Status status;
    std::string borrower;  // 借阅者账号
    int totalCopies;       // 总副本数/库存量
    int availableCopies;   // 可借阅数量
};

#endif // BOOK_H
