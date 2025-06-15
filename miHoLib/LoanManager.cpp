#include "LoanManager.h"
#include "Reader.h"
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <QDebug>
#include <QString>
LoanManager::LoanManager()
    : bookManager(BookManager::getInstance()), userManager(UserManager::getInstance()) {
    loadLoans();
    qDebug() << "LoanManager initialized. Total loans: " << loans.size();
}

LoanManager::~LoanManager() {
    for(auto loan : loans) {
        delete loan;
    }
}

void LoanManager::loadLoans() {
    // 清空当前借阅列表
    for (auto loan : loans) {
        delete loan;
    }
    loans.clear();
    
    // 尝试打开文件
    std::ifstream loanFile(loanFilepath);
    if (!loanFile.is_open()) {
        // 如果文件不存在，创建空文件
        if (!std::filesystem::exists(loanFilepath)) {
            // 确保目录存在
            std::filesystem::path dirPath = std::filesystem::path(loanFilepath).parent_path();
            std::filesystem::create_directories(dirPath);
            
            // 创建空文件
            saveLoans();
        } else {
            qDebug() << u8"借阅数据文件打开失败";
        }
        return;
    }
    
    // 读取借阅数据+装填到容器中
    std::string line;
    while (std::getline(loanFile, line)) {
        Loan* loan = Loan::deserialize(line);
        if (loan) {
            loans.push_back(loan);
        }else {
            qDebug() << "Failed to deserialize loan: " << QString::fromStdString(line);
        }
    }
    loanFile.close();
    
    // 更新所有借阅状态
    updateAllLoanStatus();
}

void LoanManager::saveLoans() {
    // 确保目录存在
    std::filesystem::path dirPath = std::filesystem::path(loanFilepath).parent_path();
    std::filesystem::create_directories(dirPath);
    
    // 打开文件
    std::ofstream loanFile(loanFilepath);
    if (!loanFile.is_open()) {
        throw std::runtime_error(u8"借阅数据文件保存失败");
    }
    
    // 写入借阅数据
    for (const auto& loan : loans) {
        loanFile << loan->serialize() << "\n";
    }
    loanFile.close();
}

const std::vector<Loan*>& LoanManager::getAllLoans() const {
    return loans;
}

std::vector<Loan*> LoanManager::getLoansByReader(const std::string& readerAccount) const {
    std::vector<Loan*> result;
    for (const auto& loan : loans) {
        if (loan->getReaderAccount() == readerAccount) {
            result.push_back(loan);
        }
    }
    return result;
}

std::vector<Loan*> LoanManager::getLoansByBook(const std::string& isbn) const {
    std::vector<Loan*> result;
    for (const auto& loan : loans) {
        if (loan->getISBN() == isbn) {
            result.push_back(loan);
        }
    }
    return result;
}

std::vector<Loan*> LoanManager::getCurrentLoans() const {
    std::vector<Loan*> result;
    for (const auto& loan : loans) {
        if (loan->getStatus() != Loan::Status::RETURNED) {
            result.push_back(loan);
        }
    }
    return result;
}

std::vector<Loan*> LoanManager::getOverdueLoans() const {
    std::vector<Loan*> result;
    for (const auto& loan : loans) {
        if (loan->getStatus() == Loan::Status::OVERDUE) {
            result.push_back(loan);
        }
    }
    return result;
}

void LoanManager::updateAllLoanStatus() {
    for (auto& loan : loans) {
        loan->updateStatus();
    }
    saveLoans();
}

double LoanManager::calculateReaderTotalFine(const std::string& readerAccount) const {
    double totalFine = 0.0;
    for (const auto& loan : loans) {
        if (loan->getReaderAccount() == readerAccount) {
            totalFine += loan->calculateFine();
        }
    }
    return totalFine;
}

bool LoanManager::borrowBook(const std::string& isbn, const std::string& readerAccount, int loanDays) {
    // 检查图书是否存在
    Book* book = bookManager.findBookByISBN(isbn);
    if (!book) {
        qDebug() << "Book not found: " << QString::fromStdString(isbn);
        return false;
    }
    
    // 检查图书是否可借
    if (book->getAvailableCopies() <= 0) {
        qDebug() << "No available copies for book: " << QString::fromStdString(isbn);
        return false;
    }
    
    // 检查读者是否存在
    User* user = userManager.getUserByAccount(readerAccount);
    if (!user || user->getType() != User::UserType::READER) {
        qDebug() << "Reader not found: " << QString::fromStdString(readerAccount);
        return false;
    }
    
    // 检查读者是否已经借阅了这本书（未归还）
    for (const auto& loan : loans) {
        if (loan->getISBN() == isbn && 
            loan->getReaderAccount() == readerAccount && 
            loan->getStatus() != Loan::Status::RETURNED) {
            qDebug() << "Reader already borrowed this book: " << QString::fromStdString(isbn);
            return false;
        }
    }
    
    // 创建借阅记录
    time_t now = time(nullptr);
    time_t dueDate = now + (loanDays * 24 * 60 * 60); // 借阅天数转换为秒
    
    Loan* newLoan = new Loan(isbn, readerAccount, now, dueDate);
    loans.push_back(newLoan);
    
    // 减少图书可借数量
    book->decrementAvailableCopies();
    
    // 保存数据
    saveLoans();
    bookManager.saveBooks();
    
    qDebug() << "Book borrowed successfully: " << QString::fromStdString(isbn) 
             << " by " << QString::fromStdString(readerAccount);
    return true;
}

bool LoanManager::returnBook(const std::string& isbn, const std::string& readerAccount) {
    // 查找对应的借阅记录
    Loan* targetLoan = nullptr;
    for (auto& loan : loans) {
        if (loan->getISBN() == isbn && 
            loan->getReaderAccount() == readerAccount && 
            loan->getStatus() != Loan::Status::RETURNED) {
            targetLoan = loan;
            break;
        }
    }
    
    if (!targetLoan) {
        qDebug() << "No active loan found for book: " << QString::fromStdString(isbn) 
                 << " by reader: " << QString::fromStdString(readerAccount);
        return false;
    }
    
    // 检查图书是否存在
    Book* book = bookManager.findBookByISBN(isbn);
    if (!book) {
        qDebug() << "Book not found: " << QString::fromStdString(isbn);
        return false;
    }
    
    // 更新借阅记录
    time_t now = time(nullptr);
    targetLoan->setReturnDate(now);
    targetLoan->setStatus(Loan::Status::RETURNED);
    
    // 计算并设置罚金
    double fine = targetLoan->calculateFine();
    targetLoan->setFine(fine);
    
    // 增加图书可借数量
    book->incrementAvailableCopies();
    
    // 保存数据
    saveLoans();
    bookManager.saveBooks();
    
    qDebug() << "Book returned successfully: " << QString::fromStdString(isbn) 
             << " by " << QString::fromStdString(readerAccount)
             << " Fine: " << fine;
    return true;
}
