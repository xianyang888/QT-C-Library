#ifndef LOANMANAGER_H
#define LOANMANAGER_H

#include "Loan.h"
#include "BookManager.h"
#include "UserManager.h"
#include <vector>
#include <string>

class LoanManager {
private:
    // 构造函数和析构函数
    LoanManager();
    std::vector<Loan*> loans;
    BookManager& bookManager;
    UserManager& userManager;
    const std::string loanFilepath = "../../res/data/loans.dat";
    
    void loadLoans();
    void saveLoans();

public:
    ~LoanManager();
    
    BookManager& getBookManager()const{return bookManager;}
    UserManager& getUserManager()const{return userManager;}
    static LoanManager& getInstance(){
        static LoanManager instance;
        return instance;
    }
    // 借书
    bool borrowBook(const std::string& isbn, const std::string& readerAccount, int loanDays = 30);
    
    // 还书
    bool returnBook(const std::string& isbn, const std::string& readerAccount);
    
    // 获取所有借阅记录
    const std::vector<Loan*>& getAllLoans() const;
    
    // 获取读者的借阅记录
    std::vector<Loan*> getLoansByReader(const std::string& readerAccount) const;
    
    // 获取图书的借阅记录
    std::vector<Loan*> getLoansByBook(const std::string& isbn) const;
    
    // 获取当前借出的记录
    std::vector<Loan*> getCurrentLoans() const;
    
    // 获取逾期记录
    std::vector<Loan*> getOverdueLoans() const;
    
    // 更新所有借阅状态
    void updateAllLoanStatus();
    
    // 计算读者的总罚金
    double calculateReaderTotalFine(const std::string& readerAccount) const;
};

#endif // LOANMANAGER_H
