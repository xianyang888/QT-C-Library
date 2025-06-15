#include "homepage.h"
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QFont>
#include <QPalette>
#include <QDateTime>
#include <QScrollArea>
#include "ResourceLoader.h"

HomePage::HomePage(const QString& adminName, QWidget* parent)
    : QWidget(parent), m_adminName(adminName), overdueRateLabel(nullptr), 
    top10Table(nullptr),
    loanManager(LoanManager::getInstance()),
    bookManager(BookManager::getInstance())
{
    setupUI();
    updateStats();
}

void HomePage::setupUI()
{
    this->setObjectName("homePage");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // 欢迎标题区域
    QFrame* welcomeFrame = new QFrame();
    welcomeFrame->setObjectName("welcomeFrame");

    QVBoxLayout* welcomeLayout = new QVBoxLayout(welcomeFrame);

    QLabel* welcomeTitle = new QLabel(QString(u8"欢迎回来，%1！").arg(m_adminName));
    welcomeTitle->setObjectName("welcomeTitle");
    welcomeTitle->setAlignment(Qt::AlignCenter); // 水平 + 垂直居中

    QLabel* timeLabel = new QLabel(QDateTime::currentDateTime().toString(u8"yyyy年MM月dd日"));
    timeLabel->setObjectName("timeLabel");
    timeLabel->setAlignment(Qt::AlignCenter);

    QLabel* welcomeDesc = new QLabel(u8"图书管理系统为您提供全面的数据统计和管理功能");
    welcomeDesc->setObjectName("welcomeDesc");
    welcomeDesc->setAlignment(Qt::AlignCenter);

    welcomeLayout->addWidget(welcomeTitle);
    welcomeLayout->addWidget(timeLabel);
    welcomeLayout->addWidget(welcomeDesc);

    mainLayout->addWidget(welcomeFrame);

    // 统计报告区域
    QLabel* statsTitle = new QLabel(u8"📊 月度借阅统计报告");
    statsTitle->setObjectName("statsTitle");
    mainLayout->addWidget(statsTitle);

    QHBoxLayout* cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(20);

    // 逾期率卡片
    QFrame* overdueCard = new QFrame();
    overdueCard->setObjectName("overdueCard");
    overdueCard->setFixedHeight(250);

    QVBoxLayout* overdueLayout = new QVBoxLayout(overdueCard);

    QLabel* overdueTitle = new QLabel(u8"📈 逾期率统计");
    overdueTitle->setObjectName("cardTitle");

    overdueRateLabel = new QLabel("0.0%");
    overdueRateLabel->setObjectName("overdueRateLabel");

    QLabel* overdueDesc = new QLabel(u8"本月图书逾期情况");
    overdueDesc->setObjectName("cardDesc");

    overdueLayout->addWidget(overdueTitle);
    overdueLayout->addWidget(overdueRateLabel, 1);
    overdueLayout->addWidget(overdueDesc);

    // TOP10卡片
    QFrame* top10Card = new QFrame();
    top10Card->setObjectName("top10Card");

    QVBoxLayout* top10Layout = new QVBoxLayout(top10Card);

    QLabel* top10Title = new QLabel(u8"🏆 热门图书TOP10");
    top10Title->setObjectName("cardTitle");

    top10Table = new QTableWidget(0, 3);
    top10Table->setObjectName("top10Table");
    top10Table->setHorizontalHeaderLabels({u8"排名", u8"书名", u8"借阅次数"});

    top10Table->horizontalHeader()->setStretchLastSection(true);
    top10Table->verticalHeader()->setVisible(false);
    top10Table->setSelectionBehavior(QAbstractItemView::SelectRows);
    top10Table->setAlternatingRowColors(true);
    top10Table->setShowGrid(false);

    top10Table->setColumnWidth(0, 60);
    top10Table->setColumnWidth(1, 200);

    top10Layout->addWidget(top10Title);
    top10Layout->addWidget(top10Table, 1);

    cardsLayout->addWidget(overdueCard, 1);
    cardsLayout->addWidget(top10Card, 2);

    mainLayout->addLayout(cardsLayout);
    mainLayout->addStretch();

    ResourceLoader::applyQSS(this, ":/style/res/style/homePage.qss");
}


void HomePage::setMonthlyStats(double overdueRate, const std::vector<BookStat>& topBooks)
{
    // 更新逾期率
    if (overdueRateLabel) {
        overdueRateLabel->setText(QString("%1%").arg(overdueRate, 0, 'f', 1));
        
        // 根据逾期率设置颜色
        QString color;
        if (overdueRate < 5.0) {
            color = "#27ae60"; // 绿色 - 良好
        } else if (overdueRate < 15.0) {
            color = "#f39c12"; // 橙色 - 一般
        } else {
            color = "#e74c3c"; // 红色 - 需要关注
        }
        
        overdueRateLabel->setStyleSheet(QString("color: %1;").arg(color));
    }
    
    // 更新TOP10表格
    if (top10Table) {
        top10Table->setRowCount(0);
        
        int rank = 1;
        for (const auto& book : topBooks) {
            if (rank > 10) break;
            
            int row = top10Table->rowCount();
            top10Table->insertRow(row);
            
            // 排名
            QTableWidgetItem* rankItem = new QTableWidgetItem(QString::number(rank));
            rankItem->setTextAlignment(Qt::AlignCenter);
            rankItem->setFlags(rankItem->flags() & ~Qt::ItemIsEditable);
            
            // 根据排名设置颜色
            if (rank <= 3) {
                QFont boldFont = rankItem->font();
                boldFont.setBold(true);
                rankItem->setFont(boldFont);
                
                if (rank == 1) rankItem->setForeground(QColor("#FFD700")); // 金色
                else if (rank == 2) rankItem->setForeground(QColor("#C0C0C0")); // 银色
                else rankItem->setForeground(QColor("#CD7F32")); // 铜色
            }
            
            // 书名
            QTableWidgetItem* titleItem = new QTableWidgetItem(book.title);
            titleItem->setFlags(titleItem->flags() & ~Qt::ItemIsEditable);
            
            // 借阅次数
            QTableWidgetItem* countItem = new QTableWidgetItem(QString::number(book.borrowCount));
            countItem->setTextAlignment(Qt::AlignCenter);
            countItem->setFlags(countItem->flags() & ~Qt::ItemIsEditable);
            
            top10Table->setItem(row, 0, rankItem);
            top10Table->setItem(row, 1, titleItem);
            top10Table->setItem(row, 2, countItem);
            
            rank++;
        }
        
        // 如果没有数据，显示提示
        if (topBooks.empty()) {
            top10Table->insertRow(0);
            QTableWidgetItem* noDataItem = new QTableWidgetItem(u8"暂无借阅数据");
            noDataItem->setTextAlignment(Qt::AlignCenter);
            noDataItem->setFlags(noDataItem->flags() & ~Qt::ItemIsEditable);
            noDataItem->setForeground(QColor("#999"));
            top10Table->setItem(0, 1, noDataItem);
            top10Table->setSpan(0, 0, 1, 3);
        }
    }
}

QFrame* HomePage::createCard(const QString& title, QWidget* contentWidget)
{
    QFrame* card = new QFrame();
    card->setObjectName("card");
    
    QVBoxLayout* layout = new QVBoxLayout(card);
    
    QLabel* titleLabel = new QLabel(title);
    QFont titleFont;
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setObjectName("cardTitle");
    
    layout->addWidget(titleLabel);
    if (contentWidget) {
        layout->addWidget(contentWidget);
    }
    
    return card;
}

void HomePage::updateStats()
{
    // 计算逾期率
    auto allLoans = loanManager.getAllLoans();
    auto overdueLoans = loanManager.getOverdueLoans();
    
    double overdueRate = 0.0;
    if (!allLoans.empty()) {
        overdueRate = (static_cast<double>(overdueLoans.size()) / allLoans.size()) * 100.0;
    }
    
    // 统计每本书的借阅次数
    std::map<std::string, int> bookBorrowCount;
    std::map<std::string, std::string> isbnToTitle;
    
    // 获取所有图书信息
    auto allBooks = bookManager.getAllBooks();
    for (const auto& book : allBooks) {
        isbnToTitle[book->getISBN()] = book->getTitle();
        bookBorrowCount[book->getISBN()] = 0;
    }
    
    // 统计借阅次数
    for (const auto& loan : allLoans) {
        if (loan) {
            bookBorrowCount[loan->getISBN()]++;
        }
    }
    
    // 创建借阅统计向量并排序
    std::vector<std::pair<std::string, int>> borrowStats;
    for (const auto& pair : bookBorrowCount) {
        if (pair.second > 0) { // 只包含有借阅记录的图书
            borrowStats.push_back(pair);
        }
    }
    
    // 按借阅次数降序排序
    std::sort(borrowStats.begin(), borrowStats.end(), 
              [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
                  return a.second > b.second;
              });
    
    // 转换为BookStat格式（取前10个）
    std::vector<BookStat> topBooks;
    int count = 0;
    for (const auto& stat : borrowStats) {
        if (count >= 10) break;
        
        BookStat bookStat;
        auto titleIt = isbnToTitle.find(stat.first);
        if (titleIt != isbnToTitle.end()) {
            bookStat.title = QString::fromStdString(titleIt->second);
        } else {
            bookStat.title = QString::fromStdString(stat.first); // 如果找不到标题，使用ISBN
        }
        bookStat.borrowCount = stat.second;
        topBooks.push_back(bookStat);
        count++;
    }
    
    // 更新homepage的统计数据
    setMonthlyStats(overdueRate, topBooks);
}
