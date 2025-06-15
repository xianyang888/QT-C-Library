#ifndef BOOKPAGE_H
#define BOOKPAGE_H

#include "tablePage.h"
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include "BookManager.h"

#include <vector>
#include <string>

class BookPage : public tablePage {
    Q_OBJECT

public:
    explicit BookPage(QWidget *parent = nullptr);

private:
    // UI 初始化与设置
    void initBookUI();
    void setupTableColumns() override;
    void fillTableData() override;

    // 交互操作
    void onModifyBook(const std::string& isbn);
    void onDeleteBook(const std::string& isbn);
    void onBatchUpdateInventory();
    
    // CSV导入导出功能
    void onImportCsv();
    void onExportCsv();
    bool importBooksFromCsv(const QString& filePath);
    bool exportBooksToCsv(const QString& filePath);

    // 业务逻辑方法（原BookService功能）
    std::vector<Book*> getFilteredBooks(const std::string& category, const std::string& keyword);
    bool updateBookInfo(const std::string& isbn, const std::string& newTitle,
                        const std::string& newAuthor, const std::string& newCategory);
    bool updateBookCopies(const std::string& isbn, int newCopies);

private:
    // 控件
    QComboBox* categoryCombo;
    QLineEdit* searchEdit;
    QPushButton* batchUpdateBtn;
    QPushButton* importCsvBtn;
    QPushButton* exportCsvBtn;

    // 业务类
    BookManager& bookManager;

    // 分页
    int currentPage = 1;
    int totalPages = 1;
    const int pageCapacity = 10;
};

#endif // BOOKPAGE_H
