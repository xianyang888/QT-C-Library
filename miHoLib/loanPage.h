#ifndef BORROWPAGE_H
#define BORROWPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QDateEdit>
#include "LoanManager.h"
#include "BookManager.h"
#include "UserManager.h"
#include "tablePage.h"

class BorrowPage : public tablePage {
    Q_OBJECT

public:
    explicit BorrowPage(QWidget *parent = nullptr);

protected:
    void setupTableColumns() override;
    void fillTableData() override;

private:
    LoanManager& loanManager;
    BookManager& bookManager;
    UserManager& userManager;
    
    QComboBox* statusComboBox;
    QLineEdit* searchEdit;
    QDateEdit* startDateEdit;
    QDateEdit* endDateEdit;
    
    void initBorrowUI();
    void setupConnections();
    
    // 借书对话框
    void showBorrowDialog();
    
    // 还书对话框
    void showReturnDialog();
    
    // 刷新表格数据
    void refreshTable();
    
private slots:
    void onBorrowBook();
    void onReturnBook();
    void onFilterByStatus(int index);
    void onSearch(const QString& keyword);
    void onDateRangeChanged();
};

#endif // BORROWPAGE_H
