#ifndef TABLEPAGE_H
#define TABLEPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QHeaderView>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

class tablePage : public QWidget
{
    Q_OBJECT
public:
    explicit tablePage(QWidget *parent = nullptr);
    virtual ~tablePage() = default;
protected:
    QTableWidget *tableWidget;
    QPushButton *firstPageBtn;
    QPushButton *prevPageBtn;
    QLabel *pageLabel;
    QPushButton *nextPageBtn;
    QPushButton *lastPageBtn;

//------------页数显示:派生类控制---------
    int currentPage = 1;
    int totalPages = 1;
    int pageCapacity = 20;  // 每页显示的行数

    // 初始化UI通用部分
    void initCommonUI();
    // 更新分页标签
    void updatePageLabel();
    // 设置表格列
    virtual void setupTableColumns() = 0;
    // 填充表格数据
    virtual void fillTableData() = 0;

protected slots:
    virtual void onFirstPage();
    virtual void onPrevPage();
    virtual void onNextPage();
    virtual void onLastPage();
};
#endif // TABLEPAGE_H

