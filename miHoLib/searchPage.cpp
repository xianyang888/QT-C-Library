#include "searchPage.h"
#include "BookManager.h"
#include "LoanManager.h"
#include "Book.h"
#include "Loan.h"
#include "Reader.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QSplitter>
#include <QScrollArea>
#include <QFrame>
#include <QDateTime>
#include <algorithm>

SearchPage::SearchPage(User*u,QWidget* parent)
    : QWidget(parent),
      currentUser(u),
      m_bookManager(BookManager::getInstance()),
      m_loanManager(LoanManager::getInstance())
{
    setupUI();
}

void SearchPage::setupUI()
{
    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // 创建滚动区域
    QScrollArea* scrollArea = new QScrollArea();
    QWidget* scrollWidget = new QWidget();
    QVBoxLayout* scrollLayout = new QVBoxLayout(scrollWidget);
    
    // 标题
    QLabel* titleLabel = new QLabel(u8"图书馆综合查询系统");
    titleLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 24px;"
        "    font-weight: bold;"
        "    color: #2c3e50;"
        "    padding: 10px;"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "                                stop:0 #3498db, stop:1 #2980b9);"
        "    color: white;"
        "    border-radius: 8px;"
        "    margin-bottom: 10px;"
        "}"
    );
    titleLabel->setAlignment(Qt::AlignCenter);
    scrollLayout->addWidget(titleLabel);
    
    // 创建搜索区域的分组框
    QGroupBox* searchGroup = new QGroupBox(u8"搜索选项");
    searchGroup->setStyleSheet(
        "QGroupBox {"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    color: #34495e;"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 8px;"
        "    margin-top: 10px;"
        "    padding-top: 10px;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    left: 10px;"
        "    padding: 0 5px 0 5px;"
        "}"
    );
    
    QGridLayout* searchLayout = new QGridLayout(searchGroup);
    searchLayout->setSpacing(15);
    
    // ISBN搜索区域
    QFrame* isbnFrame = createSearchFrame();
    QHBoxLayout* isbnLayout = new QHBoxLayout(isbnFrame);
    QLabel* isbnLabel = new QLabel(u8"ISBN搜索:");
    isbnLabel->setMinimumWidth(100);
    isbnEdit = new QLineEdit();
    isbnEdit->setPlaceholderText(u8"请输入图书ISBN号码");
    isbnSearchBtn = new QPushButton(u8"搜索");
    styleButton(isbnSearchBtn, "#3498db");
    
    isbnLayout->addWidget(isbnLabel);
    isbnLayout->addWidget(isbnEdit, 1);
    isbnLayout->addWidget(isbnSearchBtn);
    
    // 书名搜索区域
    QFrame* titleFrame = createSearchFrame();
    QHBoxLayout* titleLayout = new QHBoxLayout(titleFrame);
    QLabel* titleLabel2 = new QLabel(u8"书名搜索:");
    titleLabel2->setMinimumWidth(100);
    titleEdit = new QLineEdit();
    titleEdit->setPlaceholderText(u8"请输入图书名称（支持模糊搜索）");
    titleSearchBtn = new QPushButton(u8"搜索");
    styleButton(titleSearchBtn, "#27ae60");
    
    titleLayout->addWidget(titleLabel2);
    titleLayout->addWidget(titleEdit, 1);
    titleLayout->addWidget(titleSearchBtn);
    
    // 作者/出版社搜索区域
    QFrame* authorFrame = createSearchFrame();
    QVBoxLayout* authorMainLayout = new QVBoxLayout(authorFrame);
    
    QHBoxLayout* authorLayout = new QHBoxLayout();
    QLabel* authorLabel = new QLabel(u8"作者:");
    authorLabel->setMinimumWidth(100);
    authorEdit = new QLineEdit();
    authorEdit->setPlaceholderText(u8"请输入作者姓名");
    authorLayout->addWidget(authorLabel);
    authorLayout->addWidget(authorEdit, 1);
    
    QHBoxLayout* publisherLayout = new QHBoxLayout();
    QLabel* publisherLabel = new QLabel(u8"出版社:");
    publisherLabel->setMinimumWidth(100);
    publisherEdit = new QLineEdit();
    publisherEdit->setPlaceholderText(u8"请输入出版社名称");
    publisherLayout->addWidget(publisherLabel);
    publisherLayout->addWidget(publisherEdit, 1);
    
    QHBoxLayout* authorBtnLayout = new QHBoxLayout();
    authorPublisherSearchBtn = new QPushButton(u8"联合搜索");
    styleButton(authorPublisherSearchBtn, "#e67e22");
    authorBtnLayout->addStretch();
    authorBtnLayout->addWidget(authorPublisherSearchBtn);
    
    authorMainLayout->addLayout(authorLayout);
    authorMainLayout->addLayout(publisherLayout);
    authorMainLayout->addLayout(authorBtnLayout);
    
    // 特殊查询区域
    QFrame* specialFrame = createSearchFrame();
    QHBoxLayout* specialLayout = new QHBoxLayout(specialFrame);
    QLabel* specialLabel = new QLabel(u8"特殊查询:");
    specialLabel->setMinimumWidth(100);
    overdueBooksBtn = new QPushButton(u8"查看逾期图书");
    styleButton(overdueBooksBtn, "#e74c3c");
    
    specialLayout->addWidget(specialLabel);
    specialLayout->addWidget(overdueBooksBtn);
    specialLayout->addStretch();
    
    // 添加到搜索布局
    searchLayout->addWidget(isbnFrame, 0, 0);
    searchLayout->addWidget(titleFrame, 1, 0);
    searchLayout->addWidget(authorFrame, 2, 0);
    searchLayout->addWidget(specialFrame, 3, 0);
    
    scrollLayout->addWidget(searchGroup);
    
    // 结果显示区域
    QGroupBox* resultGroup = new QGroupBox(u8"搜索结果");
    resultGroup->setStyleSheet(
        "QGroupBox {"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    color: #34495e;"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 8px;"
        "    margin-top: 10px;"
        "    padding-top: 10px;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    left: 10px;"
        "    padding: 0 5px 0 5px;"
        "}"
    );
    
    QVBoxLayout* resultLayout = new QVBoxLayout(resultGroup);
    
    // 创建结果表格
    resultTable = new QTableWidget();
    resultTable->setAlternatingRowColors(true);
    resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    resultTable->setSelectionMode(QAbstractItemView::SingleSelection);
    resultTable->setSortingEnabled(true);
    resultTable->setMinimumHeight(400);
    
    // 设置表格样式
    resultTable->setStyleSheet(
        "QTableWidget {"
        "    gridline-color: #bdc3c7;"
        "    background-color: white;"
        "    alternate-background-color: #ecf0f1;"
        "    selection-background-color: #3498db;"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 4px;"
        "}"
        "QTableWidget::item {"
        "    padding: 8px;"
        "    border-bottom: 1px solid #ecf0f1;"
        "}"
        "QTableWidget::item:selected {"
        "    background-color: #3498db;"
        "    color: white;"
        "}"
        "QHeaderView::section {"
        "    background-color: #34495e;"
        "    color: white;"
        "    padding: 10px;"
        "    border: none;"
        "    font-weight: bold;"
        "}"
    );
    
    resultLayout->addWidget(resultTable);
    
    // 操作按钮区域
    QFrame* actionFrame = new QFrame();
    actionFrame->setStyleSheet(
        "QFrame {"
        "    background-color: #f8f9fa;"
        "    border: 1px solid #dee2e6;"
        "    border-radius: 6px;"
        "    padding: 10px;"
        "    margin: 5px;"
        "}"
    );
    
    QHBoxLayout* actionLayout = new QHBoxLayout(actionFrame);
    
    QLabel* actionLabel = new QLabel(u8"图书操作:");
    actionLabel->setStyleSheet("font-weight: bold; color: #34495e;");
    
    borrowBtn = new QPushButton(u8"借阅图书");
    styleButton(borrowBtn, "#27ae60");
    
    returnBtn = new QPushButton(u8"归还图书");
    styleButton(returnBtn, "#e74c3c");
    
    actionLayout->addWidget(actionLabel);
    actionLayout->addWidget(borrowBtn);
    actionLayout->addWidget(returnBtn);
    actionLayout->addStretch();
    
    resultLayout->addWidget(actionFrame);
    scrollLayout->addWidget(resultGroup);
    
    // 设置滚动区域
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    mainLayout->addWidget(scrollArea);
    
    // 连接信号槽
    connect(isbnSearchBtn, &QPushButton::clicked, this, &SearchPage::onSearchByISBN);
    connect(titleSearchBtn, &QPushButton::clicked, this, &SearchPage::onSearchByTitle);
    connect(authorPublisherSearchBtn, &QPushButton::clicked, this, &SearchPage::onSearchByAuthorPublisher);
    connect(overdueBooksBtn, &QPushButton::clicked, this, &SearchPage::onShowOverdueBooks);
    connect(borrowBtn, &QPushButton::clicked, this, &SearchPage::onBorrowBook);
    connect(returnBtn, &QPushButton::clicked, this, &SearchPage::onReturnBook);
    
    // 回车键搜索
    connect(isbnEdit, &QLineEdit::returnPressed, this, &SearchPage::onSearchByISBN);
    connect(titleEdit, &QLineEdit::returnPressed, this, &SearchPage::onSearchByTitle);
    connect(authorEdit, &QLineEdit::returnPressed, this, &SearchPage::onSearchByAuthorPublisher);
    connect(publisherEdit, &QLineEdit::returnPressed, this, &SearchPage::onSearchByAuthorPublisher);
}

QFrame* SearchPage::createSearchFrame()
{
    QFrame* frame = new QFrame();
    frame->setFrameStyle(QFrame::Box);
    frame->setStyleSheet(
        "QFrame {"
        "    background-color: #f8f9fa;"
        "    border: 1px solid #dee2e6;"
        "    border-radius: 6px;"
        "    padding: 10px;"
        "    margin: 5px;"
        "}"
    );
    return frame;
}

void SearchPage::styleButton(QPushButton* button, const QString& color)
{
    button->setStyleSheet(
        QString("QPushButton {"
                "    background-color: %1;"
                "    color: white;"
                "    border: none;"
                "    padding: 8px 16px;"
                "    border-radius: 4px;"
                "    font-weight: bold;"
                "    min-width: 80px;"
                "}"
                "QPushButton:hover {"
                "    background-color: %2;"
                "}"
                "QPushButton:pressed {"
                "    background-color: %3;"
                "}").arg(color, adjustColor(color, -20), adjustColor(color, -40))
    );
}

QString SearchPage::adjustColor(const QString& color, int adjustment)
{
    // 简单的颜色调整函数
    QColor qcolor(color);
    int r = qBound(0, qcolor.red() + adjustment, 255);
    int g = qBound(0, qcolor.green() + adjustment, 255);
    int b = qBound(0, qcolor.blue() + adjustment, 255);
    return QString("rgb(%1, %2, %3)").arg(r).arg(g).arg(b);
}

void SearchPage::onSearchByISBN()
{
    QString isbn = isbnEdit->text().trimmed();
    if (isbn.isEmpty()) {
        QMessageBox::warning(this, u8"输入错误", u8"请输入ISBN号码！");
        return;
    }
    
    Book* book = m_bookManager.findBookByISBN(isbn.toStdString());
    if (book) {
        std::vector<Book> books = {*book};
        displayBooks(books);
    } else {
        resultTable->setRowCount(0);
        QMessageBox::information(this, u8"搜索结果", u8"未找到指定ISBN的图书！");
    }
}

void SearchPage::onSearchByTitle()
{
    QString title = titleEdit->text().trimmed();
    if (title.isEmpty()) {
        QMessageBox::warning(this, u8"输入错误", u8"请输入图书名称！");
        return;
    }
    
    std::vector<Book*> bookPtrs = m_bookManager.searchBooks(title.toStdString());
    
    if (!bookPtrs.empty()) {
        std::vector<Book> books;
        for (Book* bookPtr : bookPtrs) {
            books.push_back(*bookPtr);
        }
        displayBooks(books);
    } else {
        resultTable->setRowCount(0);
        QMessageBox::information(this, u8"搜索结果", u8"未找到相关图书！");
    }
}

void SearchPage::onSearchByAuthorPublisher()
{
    QString author = authorEdit->text().trimmed();
    QString publisher = publisherEdit->text().trimmed();
    
    if (author.isEmpty() && publisher.isEmpty()) {
        QMessageBox::warning(this, u8"输入错误", u8"请至少输入作者或出版社信息！");
        return;
    }
    
    std::vector<Book*> bookPtrs = m_bookManager.searchBooksByAuthorAndPublisher(
        author.toStdString(), publisher.toStdString());
    
    if (!bookPtrs.empty()) {
        std::vector<Book> books;
        for (Book* bookPtr : bookPtrs) {
            books.push_back(*bookPtr);
        }
        displayBooks(books);
    } else {
        resultTable->setRowCount(0);
        QMessageBox::information(this, u8"搜索结果", u8"未找到相关图书！");
    }
}

void SearchPage::onShowOverdueBooks()
{
    std::vector<Loan*> overdueLoanPtrs = m_loanManager.getOverdueLoans();
    
    if (!overdueLoanPtrs.empty()) {
        std::vector<Loan> overdueLoans;
        for (Loan* loanPtr : overdueLoanPtrs) {
            overdueLoans.push_back(*loanPtr);
        }
        displayLoans(overdueLoans);
    } else {
        resultTable->setRowCount(0);
        QMessageBox::information(this, u8"搜索结果", u8"当前没有逾期图书！");
    }
}

void SearchPage::displayBooks(const std::vector<Book>& books)
{
    // 设置表格列
    QStringList headers = {u8"ISBN", u8"书名", u8"作者", u8"分类", u8"出版社", u8"总数量", u8"可借数量", u8"状态"};
    resultTable->setColumnCount(headers.size());
    resultTable->setHorizontalHeaderLabels(headers);
    resultTable->setRowCount(books.size());
    
    // 填充数据
    for (size_t i = 0; i < books.size(); ++i) {
        const Book& book = books[i];
        
        resultTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(book.getISBN())));
        resultTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(book.getTitle())));
        resultTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(book.getAuthor())));
        resultTable->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(book.getCategory())));
        resultTable->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(book.getPublisher())));
        resultTable->setItem(i, 5, new QTableWidgetItem(QString::number(book.getTotalCopies())));
        resultTable->setItem(i, 6, new QTableWidgetItem(QString::number(book.getAvailableCopies())));
        
        // 状态显示
        QString statusText;
        QColor statusColor;
        switch (book.getStatus()) {
            case Book::Status::AVAILABLE:
                statusText = u8"可借";
                statusColor = QColor("#27ae60");
                break;
            case Book::Status::BORROWED:
                statusText = u8"已借出";
                statusColor = QColor("#f39c12");
                break;
            case Book::Status::MAINTENANCE:
                statusText = u8"维护中";
                statusColor = QColor("#e74c3c");
                break;
        }
        
        QTableWidgetItem* statusItem = new QTableWidgetItem(statusText);
        statusItem->setForeground(statusColor);
        statusItem->setFont(QFont("", -1, QFont::Bold));
        resultTable->setItem(i, 7, statusItem);
    }
    
    // 调整列宽
    resultTable->resizeColumnsToContents();
    resultTable->horizontalHeader()->setStretchLastSection(true);
}

void SearchPage::onBorrowBook()
{
    // 检查是否选中了行
    int currentRow = resultTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, u8"选择错误", u8"请先选择要借阅的图书！");
        return;
    }
    
    // 检查当前用户是否为读者
    Reader* reader = dynamic_cast<Reader*>(currentUser);
    if (!reader) {
        QMessageBox::warning(this, u8"权限错误", u8"只有读者可以借阅图书！");
        return;
    }
    
    // 获取选中图书的ISBN
    QTableWidgetItem* isbnItem = resultTable->item(currentRow, 0);
    if (!isbnItem) {
        QMessageBox::warning(this, u8"数据错误", u8"无法获取图书信息！");
        return;
    }
    
    QString isbn = isbnItem->text();
    std::string isbnStr = isbn.toStdString();
    
    // 检查图书是否存在
    Book* book = m_bookManager.findBookByISBN(isbnStr);
    if (!book) {
        QMessageBox::warning(this, u8"图书错误", u8"图书不存在！");
        return;
    }
    
    // 检查图书是否可借
    if (book->getAvailableCopies() <= 0) {
        QMessageBox::warning(this, u8"借阅失败", u8"该图书当前无可借副本！");
        return;
    }
    
    // 检查读者是否已经借阅了这本书
    std::vector<Loan*> readerLoans = m_loanManager.getLoansByReader(reader->getAccount());
    for (Loan* loan : readerLoans) {
        if (loan->getISBN() == isbnStr && loan->getStatus() == Loan::Status::BORROWED) {
            QMessageBox::warning(this, u8"借阅失败", u8"您已经借阅了这本图书！");
            return;
        }
    }
    
    // 检查读者借阅数量限制（假设最多借5本）
    if (reader->getBorrowedCount() >= 5) {
        QMessageBox::warning(this, u8"借阅失败", u8"您的借阅数量已达上限（5本）！");
        return;
    }
    
    // 确认借阅
    QString bookTitle = QString::fromStdString(book->getTitle());
    int ret = QMessageBox::question(this, u8"确认借阅", 
                                   QString(u8"确定要借阅《%1》吗？\n借阅期限：30天").arg(bookTitle),
                                   QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        // 执行借阅操作
        bool success = m_loanManager.borrowBook(isbnStr, reader->getAccount(), 30);
        
        if (success) {
            QMessageBox::information(this, u8"借阅成功", 
                                   QString(u8"成功借阅《%1》！\n请在30天内归还。").arg(bookTitle));
            
            // 刷新显示（重新搜索当前结果）
            if (!isbnEdit->text().isEmpty()) {
                onSearchByISBN();
            } else if (!titleEdit->text().isEmpty()) {
                onSearchByTitle();
            } else if (!authorEdit->text().isEmpty() || !publisherEdit->text().isEmpty()) {
                onSearchByAuthorPublisher();
            }
        } else {
            QMessageBox::critical(this, u8"借阅失败", u8"借阅操作失败，请稍后重试！");
        }
    }
}

void SearchPage::onReturnBook()
{
    // 检查是否选中了行
    int currentRow = resultTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, u8"选择错误", u8"请先选择要归还的图书！");
        return;
    }
    
    // 检查当前用户是否为读者
    Reader* reader = dynamic_cast<Reader*>(currentUser);
    if (!reader) {
        QMessageBox::warning(this, u8"权限错误", u8"只有读者可以归还图书！");
        return;
    }
    
    // 获取选中图书的ISBN
    QTableWidgetItem* isbnItem = resultTable->item(currentRow, 0);
    if (!isbnItem) {
        QMessageBox::warning(this, u8"数据错误", u8"无法获取图书信息！");
        return;
    }
    
    QString isbn = isbnItem->text();
    std::string isbnStr = isbn.toStdString();
    
    // 检查读者是否借阅了这本书
    std::vector<Loan*> readerLoans = m_loanManager.getLoansByReader(reader->getAccount());
    bool hasBorrowed = false;
    Loan* targetLoan = nullptr;
    
    for (Loan* loan : readerLoans) {
        if (loan->getISBN() == isbnStr && loan->getStatus() == Loan::Status::BORROWED) {
            hasBorrowed = true;
            targetLoan = loan;
            break;
        }
    }
    
    if (!hasBorrowed) {
        QMessageBox::warning(this, u8"归还失败", u8"您没有借阅这本图书！");
        return;
    }
    
    // 获取图书信息
    Book* book = m_bookManager.findBookByISBN(isbnStr);
    QString bookTitle = book ? QString::fromStdString(book->getTitle()) : u8"未知图书";
    
    // 检查是否逾期并显示罚金信息
    QString confirmMessage = QString(u8"确定要归还《%1》吗？").arg(bookTitle);
    if (targetLoan && targetLoan->getStatus() == Loan::Status::OVERDUE) {
        double fine = targetLoan->getFine();
        confirmMessage += QString(u8"\n\n注意：该图书已逾期，需要支付罚金：¥%1").arg(fine, 0, 'f', 2);
    }
    
    // 确认归还
    int ret = QMessageBox::question(this, u8"确认归还", confirmMessage,
                                   QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        // 执行归还操作
        bool success = m_loanManager.returnBook(isbnStr, reader->getAccount());
        
        if (success) {
            QString successMessage = QString(u8"成功归还《%1》！").arg(bookTitle);
            if (targetLoan && targetLoan->getFine() > 0) {
                successMessage += QString(u8"\n罚金：¥%1").arg(targetLoan->getFine(), 0, 'f', 2);
            }
            QMessageBox::information(this, u8"归还成功", successMessage);
            
            // 刷新显示（重新搜索当前结果）
            if (!isbnEdit->text().isEmpty()) {
                onSearchByISBN();
            } else if (!titleEdit->text().isEmpty()) {
                onSearchByTitle();
            } else if (!authorEdit->text().isEmpty() || !publisherEdit->text().isEmpty()) {
                onSearchByAuthorPublisher();
            }
        } else {
            QMessageBox::critical(this, u8"归还失败", u8"归还操作失败，请稍后重试！");
        }
    }
}

void SearchPage::displayLoans(const std::vector<Loan>& loans)
{
    // 设置表格列
    QStringList headers = {u8"ISBN", u8"图书名称", u8"借阅者", u8"借阅日期", u8"应还日期", u8"实际归还日期", u8"状态", u8"罚金"};
    resultTable->setColumnCount(headers.size());
    resultTable->setHorizontalHeaderLabels(headers);
    resultTable->setRowCount(loans.size());
    
    // 填充数据
    for (size_t i = 0; i < loans.size(); ++i) {
        const Loan& loan = loans[i];
        
        resultTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(loan.getISBN())));
        
        // 获取图书名称
        Book* book = m_bookManager.findBookByISBN(loan.getISBN());
        QString bookTitle = book ? QString::fromStdString(book->getTitle()) : u8"未知";
        resultTable->setItem(i, 1, new QTableWidgetItem(bookTitle));
        
        resultTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(loan.getReaderAccount())));
        
        // 日期格式化
        QDateTime borrowDate = QDateTime::fromSecsSinceEpoch(loan.getBorrowDate());
        QDateTime dueDate = QDateTime::fromSecsSinceEpoch(loan.getDueDate());
        
        resultTable->setItem(i, 3, new QTableWidgetItem(borrowDate.toString("yyyy-MM-dd")));
        resultTable->setItem(i, 4, new QTableWidgetItem(dueDate.toString("yyyy-MM-dd")));
        
        // 实际归还日期
        if (loan.getReturnDate() > 0) {
            QDateTime returnDate = QDateTime::fromSecsSinceEpoch(loan.getReturnDate());
            resultTable->setItem(i, 5, new QTableWidgetItem(returnDate.toString("yyyy-MM-dd")));
        } else {
            resultTable->setItem(i, 5, new QTableWidgetItem(u8"未归还"));
        }
        
        // 状态显示
        QString statusText;
        QColor statusColor;
        switch (loan.getStatus()) {
            case Loan::Status::BORROWED:
                statusText = u8"借阅中";
                statusColor = QColor("#3498db");
                break;
            case Loan::Status::RETURNED:
                statusText = u8"已归还";
                statusColor = QColor("#27ae60");
                break;
            case Loan::Status::OVERDUE:
                statusText = u8"逾期";
                statusColor = QColor("#e74c3c");
                break;
        }
        
        QTableWidgetItem* statusItem = new QTableWidgetItem(statusText);
        statusItem->setForeground(statusColor);
        statusItem->setFont(QFont("", -1, QFont::Bold));
        resultTable->setItem(i, 6, statusItem);
        
        // 罚金
        double fine = loan.getFine();
        QTableWidgetItem* fineItem = new QTableWidgetItem(QString("¥%1").arg(fine, 0, 'f', 2));
        if (fine > 0) {
            fineItem->setForeground(QColor("#e74c3c"));
            fineItem->setFont(QFont("", -1, QFont::Bold));
        }
        resultTable->setItem(i, 7, fineItem);
    }
    
    // 调整列宽
    resultTable->resizeColumnsToContents();
    resultTable->horizontalHeader()->setStretchLastSection(true);
}
