#ifndef LOAN_H
#define LOAN_H

#include <string>
#include <ctime>

class Loan {
public:
    static constexpr double SECOND_PER_DAY = 24 * 60 * 60; // 秒数每天
    static constexpr double FINE_PER_DAY = 0.5; // 普通罚金每天
    enum class Status { BORROWED, RETURNED, OVERDUE };
    
    Loan();
    Loan(const std::string& isbn, const std::string& readerAccount, 
         time_t borrowDate, time_t dueDate);
    
    // 获取器
    std::string getISBN() const { return isbn; }
    std::string getReaderAccount() const { return readerAccount; }
    time_t getBorrowDate() const { return borrowDate; }
    time_t getDueDate() const { return dueDate; }
    time_t getReturnDate() const { return returnDate; }
    Status getStatus() const { return status; }
    double getFine() const { return fine; }
    
    // 设置器
    void setReturnDate(time_t date) { returnDate = date; }
    void setStatus(Status s) { status = s; }
    void setFine(double f) { fine = f; }
    
    // 计算罚金 (每天 0.5 元) plus<梯度罚款>
    double calculateFine() const;
    
    // 更新状态
    void updateStatus();
    
    // 序列化和反序列化
    std::string serialize() const;
    static Loan* deserialize(const std::string& data);
    
    // 获取状态的字符串表示
    std::string getStatusString() const;
    
private:
    std::string isbn;          // 图书ISBN
    std::string readerAccount; // 读者账号
    time_t borrowDate;         // 借阅日期
    time_t dueDate;            // 应还日期
    time_t returnDate;         // 实际归还日期
    Status status;             // 借阅状态
    double fine;               // 罚金
};

#endif // LOAN_H
