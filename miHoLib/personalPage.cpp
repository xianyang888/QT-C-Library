#include "personalPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QInputDialog>
#include <QFont>
#include <QFrame>
#include <QSpacerItem>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QDateTime>
#include "LoanManager.h"
#include "BookManager.h"

PersonalPage::PersonalPage(User* currentReader, QWidget* parent)
    : QWidget(parent), m_reader(currentReader)
{
    initUI();
    refreshDisplay();
}

void PersonalPage::initUI()
{
    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    
    // 标题
    QLabel* titleLabel = new QLabel(u8"个人信息", this);
    QFont titleFont;
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #2c3e50; margin-bottom: 20px;");
    mainLayout->addWidget(titleLabel);
    
    // 个人信息卡片
    QGroupBox* infoGroup = new QGroupBox(u8"基本信息", this);
    infoGroup->setStyleSheet(
        "QGroupBox {"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    color: #34495e;"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 10px;"
        "    margin-top: 10px;"
        "    padding-top: 15px;"
        "    background-color: #ffffff;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    left: 15px;"
        "    padding: 0 10px 0 10px;"
        "    background-color: #ffffff;"
        "}"
    );
    
    QFormLayout* infoLayout = new QFormLayout(infoGroup);
    infoLayout->setSpacing(15);
    infoLayout->setLabelAlignment(Qt::AlignRight);
    
    // 创建信息标签
    QFont labelFont;
    labelFont.setPointSize(12);
    labelFont.setBold(true);
    
    QFont valueFont;
    valueFont.setPointSize(12);
    
    // 账号
    QLabel* accountLabel = new QLabel(u8"账号:", this);
    accountLabel->setFont(labelFont);
    accountValueLbl = new QLabel(this);
    accountValueLbl->setFont(valueFont);
    accountValueLbl->setStyleSheet("color: #2980b9; padding: 5px;");
    infoLayout->addRow(accountLabel, accountValueLbl);
    
    // 姓名
    QLabel* nameLabel = new QLabel(u8"姓名:", this);
    nameLabel->setFont(labelFont);
    nameValueLbl = new QLabel(this);
    nameValueLbl->setFont(valueFont);
    nameValueLbl->setStyleSheet("color: #27ae60; padding: 5px;");
    infoLayout->addRow(nameLabel, nameValueLbl);
    
    // 专业
    QLabel* majorLabel = new QLabel(u8"专业:", this);
    majorLabel->setFont(labelFont);
    majorValueLbl = new QLabel(this);
    majorValueLbl->setFont(valueFont);
    majorValueLbl->setStyleSheet("color: #8e44ad; padding: 5px;");
    infoLayout->addRow(majorLabel, majorValueLbl);
    
    // 借阅情况
    QLabel* borrowedLabel = new QLabel(u8"借阅情况:", this);
    borrowedLabel->setFont(labelFont);
    borrowedValueLbl = new QLabel(this);
    borrowedValueLbl->setFont(valueFont);
    borrowedValueLbl->setStyleSheet("color: #e74c3c; padding: 5px;");
    infoLayout->addRow(borrowedLabel, borrowedValueLbl);
    
    mainLayout->addWidget(infoGroup);
    
    // 操作按钮区域
    QGroupBox* actionGroup = new QGroupBox(u8"账户操作", this);
    actionGroup->setStyleSheet(
        "QGroupBox {"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    color: #34495e;"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 10px;"
        "    margin-top: 10px;"
        "    padding-top: 15px;"
        "    background-color: #ffffff;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    left: 15px;"
        "    padding: 0 10px 0 10px;"
        "    background-color: #ffffff;"
        "}"
    );
    
    QHBoxLayout* actionLayout = new QHBoxLayout(actionGroup);
    actionLayout->setSpacing(20);
    
    // 编辑信息按钮
    editBtn = new QPushButton(u8"编辑信息", this);
    editBtn->setFixedSize(120, 40);
    editBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #3498db;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #21618c;"
        "}"
    );
    
    // 修改密码按钮
    changePwdBtn = new QPushButton(u8"修改密码", this);
    changePwdBtn->setFixedSize(120, 40);
    changePwdBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #e67e22;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #d35400;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #a04000;"
        "}"
    );
    
    // 查看当前借阅按钮
    viewLoansBtn = new QPushButton(u8"当前借阅", this);
    viewLoansBtn->setFixedSize(120, 40);
    viewLoansBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #27ae60;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #229954;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #1e8449;"
        "}"
    );
    
    // 归还图书按钮
    returnBookBtn = new QPushButton(u8"归还图书", this);
    returnBookBtn->setFixedSize(120, 40);
    returnBookBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #e74c3c;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #c0392b;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #a93226;"
        "}"
    );
    
    actionLayout->addWidget(editBtn);
    actionLayout->addWidget(changePwdBtn);
    actionLayout->addWidget(viewLoansBtn);
    actionLayout->addWidget(returnBookBtn);
    actionLayout->addStretch();
    
    mainLayout->addWidget(actionGroup);
    
    // 借阅记录表格区域
    QGroupBox* loansGroup = new QGroupBox(u8"当前借阅记录", this);
    loansGroup->setStyleSheet(
        "QGroupBox {"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    color: #34495e;"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 10px;"
        "    margin-top: 10px;"
        "    padding-top: 15px;"
        "    background-color: #ffffff;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    left: 15px;"
        "    padding: 0 10px 0 10px;"
        "    background-color: #ffffff;"
        "}"
    );
    
    QVBoxLayout* loansLayout = new QVBoxLayout(loansGroup);
    
    // 设置借阅记录表格
    setupLoansTable();
    loansLayout->addWidget(loansTable);
    
    mainLayout->addWidget(loansGroup);
    
    // 添加弹性空间
    mainLayout->addStretch();
    
    // 连接信号和槽
    connect(editBtn, &QPushButton::clicked, this, &PersonalPage::onEditProfile);
    connect(changePwdBtn, &QPushButton::clicked, this, &PersonalPage::onPasswordChange);
    connect(viewLoansBtn, &QPushButton::clicked, this, &PersonalPage::onViewCurrentLoans);
    connect(returnBookBtn, &QPushButton::clicked, this, &PersonalPage::onReturnBook);
}

void PersonalPage::setupLoansTable()
{
    loansTable = new QTableWidget(this);
    
    // 设置列数和列标题
    loansTable->setColumnCount(6);
    QStringList headers;
    headers << u8"图书ISBN" << u8"图书名称" << u8"作者" << u8"借阅日期" << u8"应还日期" << u8"状态";
    loansTable->setHorizontalHeaderLabels(headers);
    
    // 设置表格样式
    loansTable->setStyleSheet(
        "QTableWidget {"
        "    gridline-color: #bdc3c7;"
        "    background-color: #ffffff;"
        "    alternate-background-color: #f8f9fa;"
        "    selection-background-color: #3498db;"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 5px;"
        "}"
        "QTableWidget::item {"
        "    padding: 8px;"
        "    border-bottom: 1px solid #ecf0f1;"
        "}"
        "QHeaderView::section {"
        "    background-color: #34495e;"
        "    color: white;"
        "    padding: 10px;"
        "    border: none;"
        "    font-weight: bold;"
        "}"
    );
    
    // 设置表格属性
    loansTable->setAlternatingRowColors(true);
    loansTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    loansTable->setSelectionMode(QAbstractItemView::SingleSelection);
    loansTable->setSortingEnabled(true);
    
    // 设置列宽
    loansTable->horizontalHeader()->setStretchLastSection(true);
    loansTable->setColumnWidth(0, 120); // ISBN
    loansTable->setColumnWidth(1, 200); // 图书名称
    loansTable->setColumnWidth(2, 120); // 作者
    loansTable->setColumnWidth(3, 100); // 借阅日期
    loansTable->setColumnWidth(4, 100); // 应还日期
    
    // 初始化表格数据
    refreshLoansTable();
}

void PersonalPage::refreshDisplay()
{
    if (!m_reader) {
        return;
    }
    
    // 显示基本信息
    accountValueLbl->setText(QString::fromStdString(m_reader->getAccount()));
    nameValueLbl->setText(QString::fromUtf8(m_reader->getName().c_str()));
    majorValueLbl->setText(QString::fromUtf8(m_reader->getMajor().c_str()));

    
    // 获取借阅情况
    LoanManager& loanManager = LoanManager::getInstance();
    auto loans = loanManager.getLoansByReader(m_reader->getAccount());
    
    // 计算当前借阅数量
    int currentBorrowCount = 0;
    for (const auto& loan : loans) {
        if (loan->getStatus()==Loan::Status::BORROWED) {
            currentBorrowCount++;
        }
    }
    
    // 显示借阅情况（假设最大借阅数为5）
    int maxBorrowLimit = 5;
    QString borrowText = QString(u8"%1 / %2 本").arg(currentBorrowCount).arg(maxBorrowLimit);
    
    if (currentBorrowCount >= maxBorrowLimit) {
        borrowedValueLbl->setStyleSheet("color: #e74c3c; font-weight: bold; padding: 5px;");
        borrowText += u8" (已达上限)";
    } else if (currentBorrowCount > maxBorrowLimit * 0.8) {
        borrowedValueLbl->setStyleSheet("color: #f39c12; font-weight: bold; padding: 5px;");
        borrowText += u8" (接近上限)";
    } else {
        borrowedValueLbl->setStyleSheet("color: #27ae60; padding: 5px;");
    }
    
    borrowedValueLbl->setText(borrowText);
    
    // 刷新借阅记录表格
    refreshLoansTable();
}

void PersonalPage::refreshLoansTable()
{
    if (!m_reader || !loansTable) {
        return;
    }
    
    // 获取当前用户的借阅记录
    LoanManager& loanManager = LoanManager::getInstance();
    BookManager& bookManager = BookManager::getInstance();
    auto loans = loanManager.getLoansByReader(m_reader->getAccount());
    
    // 过滤出当前借阅的记录
    std::vector<Loan*> currentLoans;
    for (const auto& loan : loans) {
        if (loan->getStatus() == Loan::Status::BORROWED || loan->getStatus() == Loan::Status::OVERDUE) {
            currentLoans.push_back(loan);
        }
    }
    
    // 设置表格行数
    loansTable->setRowCount(currentLoans.size());
    
    // 填充表格数据
    for (int i = 0; i < currentLoans.size(); ++i) {
        Loan* loan = currentLoans[i];
        
        // ISBN
        QTableWidgetItem* isbnItem = new QTableWidgetItem(QString::fromStdString(loan->getISBN()));
        isbnItem->setFlags(isbnItem->flags() & ~Qt::ItemIsEditable);
        loansTable->setItem(i, 0, isbnItem);
        
        // 获取图书信息
        Book* book = bookManager.findBookByISBN(loan->getISBN());
        QString bookTitle = book ? QString::fromStdString(book->getTitle()) : u8"未知图书";
        QString bookAuthor = book ? QString::fromStdString(book->getAuthor()) : u8"未知作者";
        
        // 图书名称
        QTableWidgetItem* titleItem = new QTableWidgetItem(bookTitle);
        titleItem->setFlags(titleItem->flags() & ~Qt::ItemIsEditable);
        loansTable->setItem(i, 1, titleItem);
        
        // 作者
        QTableWidgetItem* authorItem = new QTableWidgetItem(bookAuthor);
        authorItem->setFlags(authorItem->flags() & ~Qt::ItemIsEditable);
        loansTable->setItem(i, 2, authorItem);
        
        // 借阅日期
        QDateTime borrowDateTime = QDateTime::fromSecsSinceEpoch(loan->getBorrowDate());
        QTableWidgetItem* borrowDateItem = new QTableWidgetItem(borrowDateTime.toString("yyyy-MM-dd"));
        borrowDateItem->setFlags(borrowDateItem->flags() & ~Qt::ItemIsEditable);
        loansTable->setItem(i, 3, borrowDateItem);
        
        // 应还日期
        QDateTime dueDateTime = QDateTime::fromSecsSinceEpoch(loan->getDueDate());
        QTableWidgetItem* dueDateItem = new QTableWidgetItem(dueDateTime.toString("yyyy-MM-dd"));
        dueDateItem->setFlags(dueDateItem->flags() & ~Qt::ItemIsEditable);
        loansTable->setItem(i, 4, dueDateItem);
        
        // 状态
        QString statusText;
        QString statusColor;
        if (loan->getStatus() == Loan::Status::BORROWED) {
            statusText = u8"已借阅";
            statusColor = "color: #27ae60;";
        } else if (loan->getStatus() == Loan::Status::OVERDUE) {
            statusText = u8"已逾期";
            statusColor = "color: #e74c3c; font-weight: bold;";
        }
        
        QTableWidgetItem* statusItem = new QTableWidgetItem(statusText);
        statusItem->setFlags(statusItem->flags() & ~Qt::ItemIsEditable);
        statusItem->setData(Qt::UserRole, QString::fromStdString(loan->getISBN())); // 存储ISBN用于归还操作
        loansTable->setItem(i, 5, statusItem);
        
        // 设置状态颜色
        for (int col = 0; col < 6; ++col) {
            if (loan->getStatus() == Loan::Status::OVERDUE) {
                loansTable->item(i, col)->setBackground(QColor(255, 235, 235)); // 浅红色背景
            }
        }
    }
}

void PersonalPage::onViewCurrentLoans()
{
    // 刷新借阅记录表格
    refreshLoansTable();
    
    // 显示消息
    LoanManager& loanManager = LoanManager::getInstance();
    auto loans = loanManager.getLoansByReader(m_reader->getAccount());
    
    int currentBorrowCount = 0;
    int overdueCount = 0;
    for (const auto& loan : loans) {
        if (loan->getStatus() == Loan::Status::BORROWED) {
            currentBorrowCount++;
        } else if (loan->getStatus() == Loan::Status::OVERDUE) {
            overdueCount++;
            currentBorrowCount++;
        }
    }
    
    QString message = QString(u8"当前借阅：%1 本").arg(currentBorrowCount);
    if (overdueCount > 0) {
        message += QString(u8"\n其中逾期：%1 本").arg(overdueCount);
    }
    
    QMessageBox::information(this, u8"当前借阅情况", message);
}

void PersonalPage::onReturnBook()
{
    // 检查是否有选中的行
    int currentRow = loansTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, u8"提示", u8"请先选择要归还的图书！");
        return;
    }
    
    // 获取选中行的ISBN
    QTableWidgetItem* statusItem = loansTable->item(currentRow, 5);
    if (!statusItem) {
        QMessageBox::warning(this, u8"错误", u8"无法获取图书信息！");
        return;
    }
    
    QString isbn = statusItem->data(Qt::UserRole).toString();
    if (isbn.isEmpty()) {
        QMessageBox::warning(this, u8"错误", u8"无法获取图书ISBN！");
        return;
    }
    
    // 获取图书信息用于确认
    BookManager& bookManager = BookManager::getInstance();
    Book* book = bookManager.findBookByISBN(isbn.toStdString());
    QString bookTitle = book ? QString::fromStdString(book->getTitle()) : u8"未知图书";
    
    // 确认归还
    int ret = QMessageBox::question(this, u8"确认归还", 
        QString(u8"确定要归还《%1》吗？").arg(bookTitle),
        QMessageBox::Yes | QMessageBox::No, 
        QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        try {
            // 执行归还操作
            LoanManager& loanManager = LoanManager::getInstance();
            bool success = loanManager.returnBook(isbn.toStdString(), m_reader->getAccount());
            
            if (success) {
                QMessageBox::information(this, u8"成功", u8"图书归还成功！");
                
                // 刷新显示
                refreshDisplay();
            } else {
                QMessageBox::warning(this, u8"失败", u8"图书归还失败，请检查借阅记录！");
            }
            
        } catch (const std::exception& e) {
            QMessageBox::critical(this, u8"错误", 
                QString(u8"归还图书时发生错误：%1").arg(e.what()));
        }
    }
}

void PersonalPage::onEditProfile()
{
    if (!m_reader) {
        QMessageBox::warning(this, u8"错误", u8"读者信息不存在！");
        return;
    }
    
    // 创建编辑对话框
    UserEditDialog dialog(
        QString::fromStdString(m_reader->getAccount()),
        QString::fromStdString(m_reader->getName()),
        QString::fromStdString(m_reader->getMajor()),
        "读者",
        this
    );
    
    // 显示对话框并处理结果
    if (dialog.exec() == QDialog::Accepted) {
        // 获取修改后的数据
        QString newName = dialog.getName();
        QString newMajor = dialog.getMajor();
        
        // 验证数据
        if (newName.isEmpty() || newMajor.isEmpty()) {
            QMessageBox::warning(this, u8"错误", u8"姓名和专业不能为空！");
            return;
        }
        
        try {
            // 更新读者信息
            m_reader->setName(newName.toStdString());
            m_reader->setMajor(newMajor.toStdString());
            
            // 保存到文件
            UserManager& userManager = UserManager::getInstance();
            userManager.saveUsers();
            
            // 刷新显示
            refreshDisplay();
            
            QMessageBox::information(this, u8"成功", u8"个人信息修改成功！");
            
        } catch (const std::exception& e) {
            QMessageBox::critical(this, u8"错误", 
                QString(u8"修改个人信息失败：%1").arg(e.what()));
        }
    }
}

void PersonalPage::onPasswordChange()
{
    openPasswordDialog();
}

void PersonalPage::openPasswordDialog()
{
    if (!m_reader) {
        QMessageBox::warning(this, u8"错误", u8"读者信息不存在！");
        return;
    }
    
    // 获取当前密码
    bool ok;
    QString currentPassword = QInputDialog::getText(this, u8"验证身份", 
        u8"请输入当前密码:", QLineEdit::Password, "", &ok);
    
    if (!ok || currentPassword.isEmpty()) {
        return;
    }
    
    // 验证当前密码
    if (currentPassword.toStdString() != m_reader->getPassword()) {
        QMessageBox::warning(this, u8"错误", u8"当前密码不正确！");
        return;
    }
    
    // 获取新密码
    QString newPassword = QInputDialog::getText(this, u8"修改密码", 
        u8"请输入新密码:", QLineEdit::Password, "", &ok);
    
    if (!ok || newPassword.isEmpty()) {
        return;
    }
    
    // 验证新密码长度
    if (newPassword.length() < 6) {
        QMessageBox::warning(this, u8"错误", u8"密码长度不能少于6位！");
        return;
    }
    
    // 确认新密码
    QString confirmPassword = QInputDialog::getText(this, u8"确认密码", 
        u8"请再次输入新密码:", QLineEdit::Password, "", &ok);
    
    if (!ok || confirmPassword.isEmpty()) {
        return;
    }
    
    if (newPassword != confirmPassword) {
        QMessageBox::warning(this, u8"错误", u8"两次输入的密码不一致！");
        return;
    }
    
    // 确认修改
    int ret = QMessageBox::question(this, u8"确认修改", 
        u8"确定要修改密码吗？", 
        QMessageBox::Yes | QMessageBox::No, 
        QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        try {
            // 更新密码
            m_reader->setPassword(newPassword.toStdString());
            
            // 保存到文件
            UserManager& userManager = UserManager::getInstance();
            userManager.saveUsers();
            
            QMessageBox::information(this, u8"成功", u8"密码修改成功！");
            
        } catch (const std::exception& e) {
            QMessageBox::critical(this, u8"错误", 
                QString(u8"修改密码失败：%1").arg(e.what()));
        }
    }
}
