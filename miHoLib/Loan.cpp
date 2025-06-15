#include "Loan.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <QDebug>

Loan::Loan() : borrowDate(0), dueDate(0), returnDate(0), status(Status::BORROWED), fine(0.0) {}

Loan::Loan(const std::string& isbn, const std::string& readerAccount, 
           time_t borrowDate, time_t dueDate)
    : isbn(isbn), readerAccount(readerAccount), borrowDate(borrowDate), dueDate(dueDate),
      returnDate(0), status(Status::BORROWED), fine(0.0) {}


double Loan::calculateFine() const {
    // 统一结束时间
    time_t endTime = (status == Status::RETURNED) ? returnDate : time(nullptr);

    // 计算逾期秒数
    time_t secondsOverdue = endTime - dueDate;

    if (secondsOverdue <= 0) {
        return 0.0; // 未逾期，罚款为0
    }

    // 计算逾期天数
    int daysOverdue = static_cast<int>((secondsOverdue + SECOND_PER_DAY - 1) / SECOND_PER_DAY); // 向上取整
    double fineTotal = 0.0;

    if (daysOverdue <= 3) {
        fineTotal = daysOverdue * FINE_PER_DAY;
    }
    else if (daysOverdue <= 7) {
        fineTotal = 3 * FINE_PER_DAY + (daysOverdue - 3) * 1.0;
    }
    else {
        fineTotal = 3 * FINE_PER_DAY + 4 * 1.0 + (daysOverdue - 7) * 2.0;
    }

    return fineTotal;
}

void Loan::updateStatus() {
    if (status == Status::RETURNED) {
        return; // 已归还的记录不需要更新状态
    }
    
    time_t now = time(nullptr);
    if (now > dueDate) {
        status = Status::OVERDUE;
        fine = calculateFine();
    } // 更新状态为逾期+自动计算罚款
}

std::string Loan::serialize() const {
    std::ostringstream oss;
    oss << isbn << "," << readerAccount << ","
        << borrowDate << "," << dueDate << ",";

    // 处理未归还图书的情况
    if (returnDate == 0) {
        oss << "";  // 未归还时输出空字符串
    }
    else {
        oss << returnDate;
    }

    oss << "," << static_cast<int>(status) << "," << fine;
    return oss.str();
}


// 修正后的反序列化函数
Loan* Loan::deserialize(const std::string& data) {
    std::istringstream iss(data);
    std::string token;
    std::vector<std::string> tokens;

    // 拆分逗号分隔的字段
    while (std::getline(iss, token, ',')) {
        tokens.push_back(token);
    }

    // 检查字段数量是否足够
    if (tokens.size() < 7) {
        qDebug() << "Error: Invalid data format";
        return nullptr;
    }

    try {
        // 解析基础字段
        std::string isbn = tokens[0];
        std::string readerAccount = tokens[1];
        time_t borrowDate = std::stoll(tokens[2]);
        time_t dueDate = std::stoll(tokens[3]);

        // 创建Loan对象
        Loan* loan = new Loan(isbn, readerAccount, borrowDate, dueDate);

        // 处理returnDate（空字符串表示未归还）
        loan->returnDate = tokens[4].empty() ? 0 : std::stoll(tokens[4]);

        // 解析状态和罚金（但不要直接使用文件中的状态）
        int fileStatus = std::stoi(tokens[5]);
        loan->fine = std::stod(tokens[6]);

        // 根据当前时间验证并更新状态
        time_t now = time(nullptr);
        if (loan->returnDate != 0) {
            // 已归还图书：使用文件中的状态
            loan->status = static_cast<Status>(fileStatus);
        }
        else {
            // 未归还图书：根据当前时间确定状态
            if (now > dueDate) {
                loan->status = Status::OVERDUE;
                loan->fine = loan->calculateFine(); // 重新计算罚金
            }
            else {
                loan->status = Status::BORROWED;
                loan->fine = 0.0; // 确保罚金为0
            }
        }
        return loan;
    }
    catch (const std::exception& e) {
        qDebug() << "Error: " << e.what();
        return nullptr;
    }
}

// 状态ui转为中文显示
std::string Loan::getStatusString() const {
    switch (status) {
        case Status::BORROWED: return u8"借出";
        case Status::RETURNED: return u8"已还";
        case Status::OVERDUE: return u8"逾期";
        default: return u8"未知";
    }
}
