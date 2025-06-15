#include "loanPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDateTime>
#include <QSpinBox>
#include <QDebug>


BorrowPage::BorrowPage(QWidget *parent) 
: tablePage(parent)
, loanManager(LoanManager::getInstance())
, bookManager(BookManager::getInstance())
, userManager(UserManager::getInstance()) 
{
    initBorrowUI();
    setupTableColumns();
    setupConnections();
    fillTableData();
}

void BorrowPage::initBorrowUI() {
    // 顶部控件
    QHBoxLayout *topLayout = new QHBoxLayout();
    
    // 状态筛选下拉框
    QLabel* statusLabel = new QLabel(u8"借阅状态：");
    statusComboBox = new QComboBox();
    statusComboBox->addItem(u8"全部", -1);
    statusComboBox->addItem(u8"借出", static_cast<int>(Loan::Status::BORROWED));
    statusComboBox->addItem(u8"已还", static_cast<int>(Loan::Status::RETURNED));
    statusComboBox->addItem(u8"逾期", static_cast<int>(Loan::Status::OVERDUE));
    
    // 日期范围筛选
    QLabel* dateRangeLabel = new QLabel(u8"日期范围：");
    startDateEdit = new QDateEdit(QDate::currentDate().addMonths(-1));
    endDateEdit = new QDateEdit(QDate::currentDate());
    startDateEdit->setCalendarPopup(true);
    endDateEdit->setCalendarPopup(true);
    
    // 搜索框
    QLabel* searchLabel = new QLabel(u8"搜索：");
    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText(u8"输入ISBN或读者账号");
    
    // 添加到顶部布局
    topLayout->addWidget(statusLabel);
    topLayout->addWidget(statusComboBox);
    topLayout->addWidget(dateRangeLabel);
    topLayout->addWidget(startDateEdit);
    topLayout->addWidget(new QLabel(u8"至"));
    topLayout->addWidget(endDateEdit);
    topLayout->addWidget(searchLabel);
    topLayout->addWidget(searchEdit);
    topLayout->addStretch();
    
    // 借书和还书按钮
    QPushButton* borrowBtn = new QPushButton(u8"借书");
    QPushButton* returnBtn = new QPushButton(u8"还书");
    
    topLayout->addWidget(borrowBtn);
    topLayout->addWidget(returnBtn);
    
    // 插入到主布局中
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(layout());
    if (mainLayout) {
        mainLayout->insertLayout(0, topLayout);
    }
    
    // 连接信号和槽
    connect(borrowBtn, &QPushButton::clicked, this, &BorrowPage::showBorrowDialog);
    connect(returnBtn, &QPushButton::clicked, this, &BorrowPage::showReturnDialog);
}

void BorrowPage::setupTableColumns() {
    // 设置表格列
    QStringList headers;
    headers << u8"ISBN" << u8"图书名称" << u8"读者账号" << u8"读者姓名" 
            << u8"借阅日期" << u8"应还日期" << u8"实际归还日期" 
            << u8"状态" << u8"罚金" << u8"操作";
    
    tableWidget->setColumnCount(headers.size());
    tableWidget->setHorizontalHeaderLabels(headers);
    
    // 设置列宽
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents); // ISBN
    tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents); // 读者账号
    tableWidget->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents); // 状态
    tableWidget->horizontalHeader()->setSectionResizeMode(8, QHeaderView::ResizeToContents); // 罚金
    tableWidget->horizontalHeader()->setSectionResizeMode(9, QHeaderView::ResizeToContents); // 操作
}

void BorrowPage::fillTableData() {
    // 清空表格
    tableWidget->setRowCount(0);
    // 更新所有借阅状态
    loanManager.updateAllLoanStatus();
    
    // 获取借阅记录
    std::vector<Loan*> filteredLoans = loanManager.getAllLoans();
    
    // 应用状态筛选
    int statusFilter = statusComboBox->currentData().toInt();
    if (statusFilter >= 0) {
        std::vector<Loan*> statusFilteredLoans;
        for (const auto& loan : filteredLoans) {
            if (static_cast<int>(loan->getStatus()) == statusFilter) {
                statusFilteredLoans.push_back(loan);
            }
        }
        filteredLoans = statusFilteredLoans;
    }
    
    // 应用日期范围筛选
    QDateTime startDateTime = startDateEdit->dateTime();
    QDateTime endDateTime = endDateEdit->dateTime().addDays(1); // 包含结束日期
    
    std::vector<Loan*> dateFilteredLoans;
    for (const auto& loan : filteredLoans) {
        QDateTime borrowDateTime = QDateTime::fromSecsSinceEpoch(loan->getBorrowDate());
        if (borrowDateTime >= startDateTime && borrowDateTime <= endDateTime) {
            dateFilteredLoans.push_back(loan);
        }
    }
    filteredLoans = dateFilteredLoans;
    
    // 应用搜索筛选
    QString searchText = searchEdit->text().trimmed();
    if (!searchText.isEmpty()) {
        std::vector<Loan*> searchFilteredLoans;
        for (const auto& loan : filteredLoans) {
            if (QString::fromStdString(loan->getISBN()).contains(searchText, Qt::CaseInsensitive) ||
                QString::fromStdString(loan->getReaderAccount()).contains(searchText, Qt::CaseInsensitive)) {
                searchFilteredLoans.push_back(loan);
            }
        }
        filteredLoans = searchFilteredLoans;
    }
    
    // 填充表格
    tableWidget->setRowCount(filteredLoans.size());
    for (size_t i = 0; i < filteredLoans.size(); ++i) {
        Loan* loan = filteredLoans[i];
        Book* book = bookManager.findBookByISBN(loan->getISBN());
        
        // 获取读者信息
        std::string readerName = "";
        try {
            User* user = userManager.authenticate(loan->getReaderAccount(), "", User::UserType::READER);
            readerName = user->getName();
        } catch (...) {
            readerName = u8"未知";
        }
        
        // ISBN
        QTableWidgetItem* isbnItem = new QTableWidgetItem(QString::fromStdString(loan->getISBN()));
        isbnItem->setFlags(isbnItem->flags() & ~Qt::ItemIsEditable);
        tableWidget->setItem(i, 0, isbnItem);
        
        // 图书名称
        QString bookTitle = book ? QString::fromStdString(book->getTitle()) : u8"未知图书";
        QTableWidgetItem* titleItem = new QTableWidgetItem(bookTitle);
        titleItem->setFlags(titleItem->flags() & ~Qt::ItemIsEditable);
        tableWidget->setItem(i, 1, titleItem);
        
        // 读者账号
        QTableWidgetItem* accountItem = new QTableWidgetItem(QString::fromStdString(loan->getReaderAccount()));
        accountItem->setFlags(accountItem->flags() & ~Qt::ItemIsEditable);
        tableWidget->setItem(i, 2, accountItem);
        
        // 读者姓名
        QTableWidgetItem* nameItem = new QTableWidgetItem(QString::fromStdString(readerName));
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
        tableWidget->setItem(i, 3, nameItem);
        
        // 借阅日期
        QDateTime borrowDateTime = QDateTime::fromSecsSinceEpoch(loan->getBorrowDate());
        QTableWidgetItem* borrowDateItem = new QTableWidgetItem(borrowDateTime.toString("yyyy-MM-dd"));
        borrowDateItem->setFlags(borrowDateItem->flags() & ~Qt::ItemIsEditable);
        tableWidget->setItem(i, 4, borrowDateItem);
        
        // 应还日期
        QDateTime dueDateTime = QDateTime::fromSecsSinceEpoch(loan->getDueDate());
        QTableWidgetItem* dueDateItem = new QTableWidgetItem(dueDateTime.toString("yyyy-MM-dd"));
        dueDateItem->setFlags(dueDateItem->flags() & ~Qt::ItemIsEditable);
        tableWidget->setItem(i, 5, dueDateItem);
        
        // 实际归还日期
        QTableWidgetItem* returnDateItem;
        if (loan->getReturnDate() > 0) {
            QDateTime returnDateTime = QDateTime::fromSecsSinceEpoch(loan->getReturnDate());
            returnDateItem = new QTableWidgetItem(returnDateTime.toString("yyyy-MM-dd"));
        } else {
            returnDateItem = new QTableWidgetItem(u8"未归还");
        }
        returnDateItem->setFlags(returnDateItem->flags() & ~Qt::ItemIsEditable);
        tableWidget->setItem(i, 6, returnDateItem);
        
        // 状态
        QTableWidgetItem* statusItem = new QTableWidgetItem(QString::fromStdString(loan->getStatusString()));
        statusItem->setFlags(statusItem->flags() & ~Qt::ItemIsEditable);
        
        // 根据状态设置颜色
        if (loan->getStatus() == Loan::Status::OVERDUE) {
            statusItem->setForeground(Qt::red);
        } else if (loan->getStatus() == Loan::Status::RETURNED) {
            statusItem->setForeground(Qt::green);
        }
        
        tableWidget->setItem(i, 7, statusItem);
        
        // 罚金
        double fine = loan->calculateFine();
        QTableWidgetItem* fineItem = new QTableWidgetItem(QString::number(fine, 'f', 2));
        fineItem->setFlags(fineItem->flags() & ~Qt::ItemIsEditable);
        if (fine > 0) {
            fineItem->setForeground(Qt::red);
        }
        tableWidget->setItem(i, 8, fineItem);
        
        // 操作按钮
        QWidget* btnWidget = new QWidget();
        QHBoxLayout* btnLayout = new QHBoxLayout(btnWidget);
        btnLayout->setContentsMargins(0, 0, 0, 0);
        
        if (loan->getStatus() != Loan::Status::RETURNED) {
            // 还书按钮
            QPushButton* returnBtn = new QPushButton(u8"还书");
            returnBtn->setProperty("row", static_cast<int>(i));
            returnBtn->setProperty("isbn", QString::fromStdString(loan->getISBN()));
            returnBtn->setProperty("account", QString::fromStdString(loan->getReaderAccount()));
            connect(returnBtn, &QPushButton::clicked, this, &BorrowPage::onReturnBook);
            btnLayout->addWidget(returnBtn);
        } else {
            // 详情按钮
            QPushButton* detailBtn = new QPushButton(u8"详情");
            detailBtn->setProperty("row", static_cast<int>(i));
            connect(detailBtn, &QPushButton::clicked, [=]() {
                // 显示详情对话框
                QMessageBox::information(this, u8"借阅详情",
                    u8"ISBN: " + QString::fromStdString(loan->getISBN()) + "\n" +
                    u8"图书: " + bookTitle + "\n" +
                    u8"读者: " + QString::fromStdString(readerName) + "\n" +
                    u8"借阅日期: " + borrowDateTime.toString("yyyy-MM-dd") + "\n" +
                    u8"应还日期: " + dueDateTime.toString("yyyy-MM-dd") + "\n" +
                    u8"归还日期: " + (loan->getReturnDate() > 0 ? 
                                  QDateTime::fromSecsSinceEpoch(loan->getReturnDate()).toString("yyyy-MM-dd") : 
                                  u8"未归还") + "\n" +
                    u8"状态: " + QString::fromStdString(loan->getStatusString()) + "\n" +
                    u8"罚金: " + QString::number(fine, 'f', 2) + u8" 元"
                );
            });
            btnLayout->addWidget(detailBtn);
        }
        
        btnLayout->addStretch();
        tableWidget->setCellWidget(i, 9, btnWidget);
    }
}

void BorrowPage::setupConnections() {
    connect(statusComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &BorrowPage::onFilterByStatus);
    
    connect(searchEdit, &QLineEdit::textChanged,
            this, &BorrowPage::onSearch);
    
    connect(startDateEdit, &QDateEdit::dateChanged,
            this, &BorrowPage::onDateRangeChanged);
    
    connect(endDateEdit, &QDateEdit::dateChanged,
            this, &BorrowPage::onDateRangeChanged);
}

void BorrowPage::showBorrowDialog() {
    QDialog dialog(this);
    dialog.setWindowTitle(u8"借书");
    dialog.setMinimumWidth(300);
    
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    
    // 表单布局
    QFormLayout* formLayout = new QFormLayout();
    
    // ISBN输入框
    QLineEdit* isbnEdit = new QLineEdit();
    formLayout->addRow(u8"ISBN:", isbnEdit);
    
    // 读者账号输入框
    QLineEdit* accountEdit = new QLineEdit();
    formLayout->addRow(u8"读者账号:", accountEdit);
    
    // 借阅天数
    QSpinBox* daysSpinBox = new QSpinBox();
    daysSpinBox->setMinimum(1);
    daysSpinBox->setMaximum(90);
    daysSpinBox->setValue(30);
    formLayout->addRow(u8"借阅天数:", daysSpinBox);
    
    layout->addLayout(formLayout);
    
    // 按钮
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttonBox);
    
    if (dialog.exec() == QDialog::Accepted) {
        QString isbn = isbnEdit->text().trimmed();
        QString account = accountEdit->text().trimmed();
        int days = daysSpinBox->value();
        
        if (isbn.isEmpty() || account.isEmpty()) {
            QMessageBox::warning(this, u8"错误", u8"ISBN和读者账号不能为空");
            return;
        }
        
        // 借书
        bool success = loanManager.borrowBook(isbn.toStdString(), account.toStdString(), days);
        if (success) {
            QMessageBox::information(this, u8"成功", u8"借书成功");
            fillTableData(); // 刷新表格
        } else {
            QMessageBox::warning(this, u8"错误", u8"借书失败，请检查ISBN和读者账号是否正确，以及图书是否可借");
        }
    }
}

void BorrowPage::showReturnDialog() {
    QDialog dialog(this);
    dialog.setWindowTitle(u8"还书");
    dialog.setMinimumWidth(300);
    
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    
    // 表单布局
    QFormLayout* formLayout = new QFormLayout();
    
    // ISBN输入框
    QLineEdit* isbnEdit = new QLineEdit();
    formLayout->addRow(u8"ISBN:", isbnEdit);
    
    // 读者账号输入框
    QLineEdit* accountEdit = new QLineEdit();
    formLayout->addRow(u8"读者账号:", accountEdit);
    
    layout->addLayout(formLayout);
    
    // 按钮
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttonBox);
    
    if (dialog.exec() == QDialog::Accepted) {
        QString isbn = isbnEdit->text().trimmed();
        QString account = accountEdit->text().trimmed();
        
        if (isbn.isEmpty() || account.isEmpty()) {
            QMessageBox::warning(this, u8"错误", u8"ISBN和读者账号不能为空");
            return;
        }
        
        // 还书
        bool success = loanManager.returnBook(isbn.toStdString(), account.toStdString());
        if (success) {
            // 计算罚金
            double fine = 0.0;
            for (const auto& loan : loanManager.getLoansByBook(isbn.toStdString())) {
                if (loan->getReaderAccount() == account.toStdString() && 
                    loan->getStatus() == Loan::Status::RETURNED) {
                    fine = loan->getFine();
                    break;
                }
            }
            
            QString message = u8"还书成功";
            if (fine > 0) {
                message += u8"，需缴纳罚金 " + QString::number(fine, 'f', 2) + u8" 元";
            }
            
            QMessageBox::information(this, u8"成功", message);
            fillTableData(); // 刷新表格
        } else {
            QMessageBox::warning(this, u8"错误", u8"还书失败，请检查ISBN和读者账号是否正确，以及图书是否已借出");
        }
    }
}

void BorrowPage::onBorrowBook() {
    showBorrowDialog();
}

void BorrowPage::onReturnBook() {
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    
    QString isbn = btn->property("isbn").toString();
    QString account = btn->property("account").toString();
    
    // 还书
    bool success = loanManager.returnBook(isbn.toStdString(), account.toStdString());
    if (success) {
        // 计算罚金
        double fine = 0.0;
        for (const auto& loan : loanManager.getLoansByBook(isbn.toStdString())) {
            if (loan->getReaderAccount() == account.toStdString() && 
                loan->getStatus() == Loan::Status::RETURNED) {
                fine = loan->getFine();
                break;
            }
        }
        
        QString message = u8"还书成功";
        if (fine > 0) {
            message += u8"，需缴纳罚金 " + QString::number(fine, 'f', 2) + u8" 元";
        }
        
        QMessageBox::information(this, u8"成功", message);
        fillTableData(); // 刷新表格
    } else {
        QMessageBox::warning(this, u8"错误", u8"还书失败");
    }
}

void BorrowPage::onFilterByStatus(int index) {
    fillTableData();
}

void BorrowPage::onSearch(const QString& keyword) {
    fillTableData();
}

void BorrowPage::onDateRangeChanged() {
    fillTableData();
}

void BorrowPage::refreshTable() {
    fillTableData();
}
