#include "tablePage.h"

tablePage::tablePage(QWidget *parent)
    : QWidget(parent)
{
    initCommonUI();
}

void tablePage::initCommonUI()
{
    // 表格布局的初始化
    tableWidget = new QTableWidget(this);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑 ReadOnly
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//选择行为:行选择
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);//选择模式:单选
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->verticalHeader()->setVisible(false);
    
    // 底部分页布局
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    firstPageBtn = new QPushButton(u8"首页");
    prevPageBtn = new QPushButton(u8"上一页");
    pageLabel = new QLabel(u8"第 1 页 / 共 1 页");
    nextPageBtn = new QPushButton(u8"下一页");
    lastPageBtn = new QPushButton(u8"尾页");
    
    bottomLayout->addWidget(firstPageBtn);
    bottomLayout->addWidget(prevPageBtn);
    bottomLayout->addWidget(pageLabel);
    bottomLayout->addWidget(nextPageBtn);
    bottomLayout->addWidget(lastPageBtn);
    bottomLayout->setContentsMargins(0, 10, 0, 0);
    
    // 连接信号槽
    connect(firstPageBtn, &QPushButton::clicked, this, &tablePage::onFirstPage);
    connect(prevPageBtn, &QPushButton::clicked, this, &tablePage::onPrevPage);
    connect(nextPageBtn, &QPushButton::clicked, this, &tablePage::onNextPage);
    connect(lastPageBtn, &QPushButton::clicked, this, &tablePage::onLastPage);
    
    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tableWidget);
    mainLayout->addLayout(bottomLayout);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    setLayout(mainLayout);
}

//更新分页标签
void tablePage::updatePageLabel()
{
    pageLabel->setText(QString(u8"第 %1 页 / 共 %2 页").arg(currentPage).arg(totalPages));
    
    // 更新按钮状态
    firstPageBtn->setEnabled(currentPage > 1);
    prevPageBtn->setEnabled(currentPage > 1);
    nextPageBtn->setEnabled(currentPage < totalPages);
    lastPageBtn->setEnabled(currentPage < totalPages);
}
// 首页
void tablePage::onFirstPage()
{
    currentPage = 1;
    fillTableData();
}
// 上一页
void tablePage::onPrevPage()
{
    if (currentPage > 1) {
        currentPage--;
        fillTableData();
    }
}
// 下一页
void tablePage::onNextPage()
{
    if (currentPage < totalPages) {
        currentPage++;
        fillTableData();
    }
}
// 尾页
void tablePage::onLastPage()
{
    currentPage = totalPages;
    fillTableData();
}
