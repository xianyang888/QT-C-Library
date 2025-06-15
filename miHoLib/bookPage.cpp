#include "bookPage.h"
#include "qdatetime.h"
#include <QLineEdit>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QInputDialog>
#include <QCheckBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QScrollArea>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDebug>
#include <QFormLayout>
#include <QMessageBox>
#include <QProgressDialog>
#include <QApplication>

BookPage::BookPage(QWidget *parent)
    : tablePage(parent),
    bookManager(BookManager::getInstance())
{
    initBookUI();
    setupTableColumns();
    fillTableData();
}

void BookPage::initBookUI()
{
    // 在表格上方添加图书特有的控件
    QHBoxLayout *topLayout = new QHBoxLayout(this);
    categoryCombo = new QComboBox(this);
    categoryCombo->addItems({u8"所有分类", u8"文学", u8"理学", u8"工学", u8"历史",u8"计算机",u8"艺术"});

    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText(u8"搜索书名或作者");

    batchUpdateBtn = new QPushButton(u8"批量更新库存");
    connect(batchUpdateBtn, &QPushButton::clicked, this, &BookPage::onBatchUpdateInventory);
    
    // 添加导入/导出CSV按钮
    importCsvBtn = new QPushButton(u8"导入CSV");
    connect(importCsvBtn, &QPushButton::clicked, this, &BookPage::onImportCsv);
    
    exportCsvBtn = new QPushButton(u8"导出CSV");
    connect(exportCsvBtn, &QPushButton::clicked, this, &BookPage::onExportCsv);

    topLayout->addWidget(new QLabel(u8"分类:"));
    topLayout->addWidget(categoryCombo);
    topLayout->addWidget(searchEdit);
    topLayout->addStretch();
    topLayout->addWidget(importCsvBtn);
    topLayout->addWidget(exportCsvBtn);
    topLayout->addWidget(batchUpdateBtn);
    
    // 连接搜索和分类筛选的信号
    connect(searchEdit, &QLineEdit::returnPressed, [this]() {
        currentPage = 1;  // 重置到第一页
        fillTableData();
    });
    
    connect(categoryCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int) {
        currentPage = 1;  // 重置到第一页
        fillTableData();
    });

    //插入到主布局中
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(layout());
    if (mainLayout) {
        mainLayout->insertLayout(0, topLayout);
    }
    //这段代码是在运行时获取主布局并安全转换为垂直布局指针，然后把一个子布局动态插入到顶部，非常适用于需要动态构造 UI 结构的场景（如插件式界面或控制栏插入）。
}

void BookPage::setupTableColumns()
{
    tableWidget->setColumnCount(9);
    QStringList headers = {u8"书名", u8"作者", u8"ISBN", u8"分类", u8"库存量", u8"可借数量", u8"状态", u8"修改", u8"删除"};
    tableWidget->setHorizontalHeaderLabels(headers);

    // 设置列宽策略
    auto header = tableWidget->horizontalHeader();

    // 设置所有列默认先自适应内容
    header->setSectionResizeMode(QHeaderView::ResizeToContents);

    // 设置“书名”和“作者”为 Stretch：占据剩余空间（适合长文本）
    header->setSectionResizeMode(0, QHeaderView::Stretch); // 书名
    header->setSectionResizeMode(1, QHeaderView::Stretch); // 作者

    // 设置“修改”和“删除”为固定宽度，避免占据过多空间
    header->setSectionResizeMode(7, QHeaderView::Fixed);
    tableWidget->setColumnWidth(7, 60);

    header->setSectionResizeMode(8, QHeaderView::Fixed);
    tableWidget->setColumnWidth(8, 60);

    // 末列不自动扩展（我们已指定 Stretch）
    header->setStretchLastSection(false);
}

//核心代码 :填充表格
void BookPage::fillTableData()
{
    // 清空表格
    tableWidget->setRowCount(0);
   
    // 获取当前分类和搜索关键词
    std::string category = categoryCombo->currentText().toStdString();
    std::string keyword = searchEdit->text().toStdString();

    // 获取图书列表
    std::vector<Book*> filteredBooks = getFilteredBooks(category, keyword);

    // 计算总页数
    totalPages = (filteredBooks.size() + pageCapacity - 1) / pageCapacity;
    if (totalPages == 0) totalPages = 1;

    // 确保当前页在有效范围内
    if (currentPage > totalPages) currentPage = totalPages;
    if (currentPage < 1) currentPage = 1;

    // 计算当前页的起始和结束索引
    int startIndex = (currentPage - 1) * pageCapacity;
    int endIndex = std::min(startIndex + pageCapacity, static_cast<int>(filteredBooks.size()));

    // 填充表格数据
    for (int i = startIndex; i < endIndex; i++) {
        Book* book = filteredBooks[i];
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);

        // 设置图书信息
        tableWidget->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(book->getTitle())));
        tableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(book->getAuthor())));
        tableWidget->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(book->getISBN())));
        tableWidget->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(book->getCategory())));
        tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(book->getTotalCopies())));
        tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(book->getAvailableCopies())));
        tableWidget->setItem(row, 6, new QTableWidgetItem(QString::fromStdString(book->getStatusString())));

        // 添加修改按钮
        QPushButton* modifyBtn = new QPushButton(u8"修改");
        std::string isbn = book->getISBN(); // 捕获ISBN
        connect(modifyBtn, &QPushButton::clicked, [this, isbn]() {
            onModifyBook(isbn);
        });
        tableWidget->setCellWidget(row, 7, modifyBtn);

        // 添加删除按钮
        QPushButton* deleteBtn = new QPushButton(u8"删除");
        connect(deleteBtn, &QPushButton::clicked, [this, isbn]() {
            onDeleteBook(isbn);
        });
        tableWidget->setCellWidget(row, 8, deleteBtn);
    }

    // 更新分页标签
    updatePageLabel();
}


void BookPage::onModifyBook(const std::string& isbn) {
    // 根据ISBN查找图书
    Book* book = bookManager.findBookByISBN(isbn);
    if (!book) {
        QMessageBox::warning(this, u8"警告", u8"未找到指定的图书");
        return;
    }

    // 创建对话框
    QDialog dialog(this);
    dialog.setWindowTitle(u8"修改图书信息");
    dialog.setModal(true);

    // 创建控件
    QLineEdit* titleEdit = new QLineEdit(QString::fromStdString(book->getTitle()));
    QLineEdit* authorEdit = new QLineEdit(QString::fromStdString(book->getAuthor()));
    QComboBox* categoryBox = new QComboBox();
    QSpinBox* copiesSpin = new QSpinBox();
    copiesSpin->setMinimum(0);
    copiesSpin->setMaximum(100000);  // 自定义上限
    copiesSpin->setValue(book->getTotalCopies());

    // 填充分类下拉框（使用预定义的分类列表）
    QStringList categories = {u8"所有分类", u8"文学", u8"理学", u8"工学", u8"历史",u8"计算机",u8"艺术"};
    for (const QString& cat : categories) {
        categoryBox->addItem(cat);
    }
    categoryBox->setCurrentText(QString::fromStdString(book->getCategory()));

    // 布局
    QFormLayout* formLayout = new QFormLayout();
    formLayout->addRow(u8"书名:", titleEdit);
    formLayout->addRow(u8"作者:", authorEdit);
    formLayout->addRow(u8"分类:", categoryBox);
    formLayout->addRow(u8"库存:", copiesSpin);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);
    dialog.setLayout(mainLayout);

    // 显示对话框并处理结果
    if (dialog.exec() == QDialog::Accepted) {
        QString newTitle = titleEdit->text().trimmed();
        QString newAuthor = authorEdit->text().trimmed();
        QString newCategory = categoryBox->currentText();
        int newCopies = copiesSpin->value();

        // 输入验证
        if (newTitle.isEmpty()) {
            QMessageBox::warning(this, u8"警告", u8"书名不能为空");
            return;
        }
        if (newAuthor.isEmpty()) {
            QMessageBox::warning(this, u8"警告", u8"作者不能为空");
            return;
        }
        if (newCategory.isEmpty()) {
            QMessageBox::warning(this, u8"警告", u8"请选择分类");
            return;
        }

        try {
            // 更新图书信息和库存
            updateBookInfo(isbn, newTitle.toStdString(), newAuthor.toStdString(), newCategory.toStdString());
            updateBookCopies(isbn, newCopies);

            // 刷新表格显示
            fillTableData();
            QMessageBox::information(this, u8"成功", u8"图书信息已更新");
        } catch (const std::exception& e) {
            QMessageBox::critical(this, u8"错误", QString::fromUtf8(e.what()));
        }
    }
}



void BookPage::onDeleteBook(const std::string& isbn) {
    // 确认删除
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, u8"确认删除", u8"确定要删除这本图书吗？此操作不可撤销。",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        try {
            bookManager.removeBook(isbn);
            fillTableData(); // 刷新表格
            QMessageBox::information(this, u8"成功", u8"图书已删除");
        } catch (const std::exception& e) {
            QMessageBox::critical(this, u8"错误", QString::fromUtf8(e.what()));
        }
    }
}

// 批量更新图书 KEY:
void BookPage::onBatchUpdateInventory() {
    const std::vector<Book*>& allBooks = bookManager.getAllBooks();

    if (allBooks.empty()) {
        QMessageBox::information(this, u8"提示", u8"没有可更新的图书");
        return;
    }

    // 创建批量更新对话框
    QDialog dialog(this);
    dialog.setWindowTitle(u8"批量更新库存");
    dialog.resize(800, 600);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    // 添加说明标签
    layout->addWidget(new QLabel(u8"选择要更新库存的图书:"));

    // 创建图书列表
    QWidget* scrollContent = new QWidget();
    QVBoxLayout* scrollLayout = new QVBoxLayout(scrollContent);

    std::vector<QCheckBox*> checkboxes;
    std::vector<std::string> isbns;

    // 添加“全选”复选框
    QCheckBox* selectAllCheckbox = new QCheckBox(u8"全选");
    selectAllCheckbox->setTristate(true);
    scrollLayout->addWidget(selectAllCheckbox);

    int cnt = 1;// 选框计数器
    // 添加图书复选框
    for (const auto& book : allBooks) {
        QString bookInfo = QString(u8"[%1] %2 - %3 (ISBN: %4, 当前库存: %5)")
            .arg(cnt++)
            .arg(QString::fromStdString(book->getTitle()))
            .arg(QString::fromStdString(book->getAuthor()))
            .arg(QString::fromStdString(book->getISBN()))
            .arg(book->getTotalCopies());

        QCheckBox* checkbox = new QCheckBox(bookInfo);
        checkbox->setTristate(false);
        scrollLayout->addWidget(checkbox);
        checkboxes.push_back(checkbox);
        isbns.push_back(book->getISBN());
    }
    // 构建完后，再添加反向同步逻辑(反向更新“全选”复选框状态)
    // ==== 关键修正：使用安全的更新lambda函数 ====
    auto updateSelectAllState = [selectAllCheckbox, checkboxes]() {
        int checkedCount = 0; // 计算选中图书数量
        int totalCount = checkboxes.size();
        for (auto cb : checkboxes) {
            if (cb->isChecked()) ++checkedCount;
        }

        // 关键：临时阻断全选信号
        selectAllCheckbox->blockSignals(true);
        // 安全更新全选状态
        if (checkedCount == 0) {
            selectAllCheckbox->setCheckState(Qt::Unchecked);
        } else if (checkedCount == totalCount) {
            selectAllCheckbox->setCheckState(Qt::Checked);
        } else {
            selectAllCheckbox->setCheckState(Qt::PartiallyChecked);
        }
        // 恢复全选信号
        selectAllCheckbox->blockSignals(false);
    };
    // 连接图书复选框
    for (auto checkbox : checkboxes) {
        connect(checkbox, &QCheckBox::stateChanged, updateSelectAllState);
    }
    // 连接全选复选框（使用安全捕获）
    connect(selectAllCheckbox, &QCheckBox::stateChanged,
        [checkboxes](int state) {
            Qt::CheckState checkState = static_cast<Qt::CheckState>(state);

            // 明确处理三种状态
            Qt::CheckState targetState;
            if (checkState == Qt::PartiallyChecked) {
                // 部分选中时视为全选（符合用户预期）
                targetState = Qt::Checked;
            } else {
                // 已选中或未选中时保持原状态
                targetState = checkState;
            }

            // 批量设置图书复选框
            for (auto checkbox : checkboxes) {
                checkbox->blockSignals(true);
                checkbox->setCheckState(targetState); // 直接使用targetState
                checkbox->blockSignals(false);
            }
        }
    );
    scrollLayout->addStretch();

    // 创建滚动区域
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(scrollContent);
    layout->addWidget(scrollArea);

    // 控制所有复选框的全选逻辑
    connect(selectAllCheckbox, &QCheckBox::stateChanged, [=](int state) {
        Qt::CheckState checkState = static_cast<Qt::CheckState>(state);
        for (auto checkbox : checkboxes) {
            checkbox->setCheckState(checkState);
        }
    });

    //// (简单)添加全选/取消全选按钮
    //QPushButton* selectAllBtn = new QPushButton(u8"全选");
    //QPushButton* deselectAllBtn = new QPushButton(u8"取消全选");
    //
    //QHBoxLayout* selectBtnLayout = new QHBoxLayout();
    //selectBtnLayout->addWidget(selectAllBtn);
    //selectBtnLayout->addWidget(deselectAllBtn);
    //layout->addLayout(selectBtnLayout);
    //
    //connect(selectAllBtn, &QPushButton::clicked, [&checkboxes]() {
    //    for (auto checkbox : checkboxes) {
    //        checkbox->setChecked(true);
    //    }
    //});
    //
    //connect(deselectAllBtn, &QPushButton::clicked, [&checkboxes]() {
    //    for (auto checkbox : checkboxes) {
    //        checkbox->setChecked(false);
    //    }
    //});


    // 添加库存输入
    QHBoxLayout* copiesLayout = new QHBoxLayout();
    copiesLayout->addWidget(new QLabel(u8"设置新库存数量:"));

    QSpinBox* copiesSpinBox = new QSpinBox();
    copiesSpinBox->setMinimum(0);
    copiesSpinBox->setMaximum(999);
    // copiesSpinBox->setRange(0, 999); // 一键设置范围为0到999
    copiesSpinBox->setWrapping(true); // 启用循环模式
    copiesSpinBox->setValue(1);
    copiesLayout->addWidget(copiesSpinBox);

    layout->addLayout(copiesLayout);

    // 底部按钮--确定和取消按钮
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttonBox);

    // 显示对话框
    if (dialog.exec() == QDialog::Accepted) {
        // 收集用户选中的图书ISBN
        std::vector<std::string> selectedISBNs;
        for (size_t i = 0; i < checkboxes.size(); ++i) {
            if (checkboxes[i]->isChecked()) {
                selectedISBNs.push_back(isbns[i]);
            }
        }

        if (selectedISBNs.empty()) {
            QMessageBox::information(this, u8"提示", u8"未选择任何图书");
            return;
        }

        // 执行批量更新操作
        try {
            bookManager.batchUpdateBookCopies(selectedISBNs, copiesSpinBox->value());
            fillTableData(); // 刷新表格显示

            // 显示成功消息
            QMessageBox::information(
                this, u8"成功",
                QString(u8"已成功更新 %1 本图书的库存").arg(selectedISBNs.size())
            );
        } catch (const std::exception& e) {
            // 显示错误消息
            QMessageBox::critical(this, u8"错误", QString::fromUtf8(e.what()));
        }
    }
}

//==========BookPage业务逻辑方法===================

// 过滤搜索
std::vector<Book*> BookPage::getFilteredBooks(const std::string& category, const std::string& keyword) {
    if (!keyword.empty()) {
        // 有搜索关键词时，在搜索结果中进行分类筛选
        std::vector<Book*> searchResults = bookManager.searchBooks(keyword);
        if (category == u8"所有分类") {
            return searchResults;
        } else {
            // 在搜索结果中筛选指定分类
            std::vector<Book*> filteredResults;
            for (Book* book : searchResults) {
                if (book->getCategory() == category) {
                    filteredResults.push_back(book);
                }
            }
            return filteredResults;
        }
    } else {
        // 没有搜索关键词时，按分类筛选
        if (category == u8"所有分类") {
            const std::vector<Book*>& allBooks = bookManager.getAllBooks();
            return std::vector<Book*>(allBooks.begin(), allBooks.end());
        } else {
            return bookManager.getBooksByCategory(category);
        }
    }
}

// 更新图书信息
bool BookPage::updateBookInfo(const std::string& isbn, const std::string& newTitle,
    const std::string& newAuthor, const std::string& newCategory) {
    Book* book = bookManager.findBookByISBN(isbn);
    if (!book) return false;
    book->setTitle(newTitle);
    book->setAuthor(newAuthor);
    book->setCategory(newCategory);
    return true;
}

// 更新图书库存
bool BookPage::updateBookCopies(const std::string& isbn, int newCopies) {
    Book* book = bookManager.findBookByISBN(isbn);
    if (!book) return false;
    bookManager.updateBookCopies(isbn, newCopies);
    return true;
}

void BookPage::onImportCsv() {
    QString fileName = QFileDialog::getOpenFileName(
        this,
        u8"导入图书数据",
        "",
        "CSV Files (*.csv)"
    );
    
    if (fileName.isEmpty()) {
        return;
    }
    
    // 确认导入操作
    int ret = QMessageBox::question(
        this,
        u8"确认导入",
        u8"导入操作将添加新图书或更新现有图书信息。\n是否继续？",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (ret != QMessageBox::Yes) {
        return;
    }
    
    if (importBooksFromCsv(fileName)) {
        QMessageBox::information(this, u8"成功", u8"图书数据导入成功！");
        fillTableData(); // 刷新表格显示
    } else {
        QMessageBox::critical(this, u8"错误", u8"图书数据导入失败！");
    }
}

bool BookPage::importBooksFromCsv(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream in(&file);
    in.setCodec("UTF-8");
    
    // 跳过头部行
    if (!in.atEnd()) {
        in.readLine();
    }
    
    QStringList lines;
    while (!in.atEnd()) {
        lines.append(in.readLine());
    }
    
    if (lines.isEmpty()) {
        return true; // 空文件也算成功
    }
    
    QProgressDialog progress(u8"正在导入...", u8"取消", 0, lines.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    
    int successCount = 0;
    int errorCount = 0;
    QStringList errorMessages;
    
    for (int i = 0; i < lines.size(); ++i) {
        if (progress.wasCanceled()) {
            break;
        }
        
        const QString& line = lines[i].trimmed();
        if (line.isEmpty()) {
            continue;
        }
        
        // 解析CSV行（简单的逗号分割，不处理复杂的引号转义）
        QStringList fields = line.split(',');
        
        if (fields.size() < 7) {
            errorMessages.append(QString(u8"第%1行：字段数量不足").arg(i + 2));
            errorCount++;
            continue;
        }
        
        // 清理字段（移除可能的引号）
        for (QString& field : fields) {
            field = field.trimmed();
            if (field.startsWith('"') && field.endsWith('"')) {
                field = field.mid(1, field.length() - 2);
                field.replace('\"\"', '"'); // 处理转义的引号
            }
        }
        
        try {
            QString title = fields[0];
            QString author = fields[1];
            QString isbn = fields[2];
            QString category = fields[3];
            QString publisher = fields[4];
            int totalCopies = fields[5].toInt();
            int availableCopies = fields[6].toInt();
            
            // 验证必填字段
            if (title.isEmpty() || author.isEmpty() || isbn.isEmpty()) {
                errorMessages.append(QString(u8"第%1行：书名、作者或ISBN不能为空").arg(i + 2));
                errorCount++;
                continue;
            }
            
            // 验证数量
            if (totalCopies < 0 || availableCopies < 0 || availableCopies > totalCopies) {
                errorMessages.append(QString(u8"第%1行：库存数量无效").arg(i + 2));
                errorCount++;
                continue;
            }
            
            // 检查图书是否已存在
            Book* existingBook = bookManager.findBookByISBN(isbn.toStdString());
            if (existingBook) {
                // 更新现有图书
                existingBook->setTitle(title.toStdString());
                existingBook->setAuthor(author.toStdString());
                existingBook->setCategory(category.toStdString());
                existingBook->setPublisher(publisher.toStdString());
                existingBook->setTotalCopies(totalCopies);
                // 注意：可借数量需要根据业务逻辑处理
            } else {
                // 创建新图书
                Book* newBook = new Book(
                    title.toStdString(),
                    author.toStdString(),
                    isbn.toStdString(),
                    category.toStdString(),
                    publisher.toStdString()
                );
                newBook->setTotalCopies(totalCopies);
                QString title = QString::fromStdString(newBook->getTitle());
                QString author = QString::fromStdString(newBook->getAuthor());
                QString isbn = QString::fromStdString(newBook->getISBN());
                QString category = QString::fromStdString(newBook->getCategory());
                QString publisher = QString::fromStdString(newBook->getPublisher());
                QString totalCopies = QString::number(newBook->getTotalCopies());
                QString availableCopies = QString::number(newBook->getAvailableCopies());
                QString status = QString::fromStdString(newBook->getStatusString());
                bookManager.addBook(
                    title.toStdString(),
                    author.toStdString(),
                    isbn.toStdString(),
                    category.toStdString()
                    );
            }
            
            successCount++;
        } catch (const std::exception& e) {
            errorMessages.append(QString(u8"第%1行：数据格式错误").arg(i + 2));
            errorCount++;
        }
        
        progress.setValue(i + 1);
        QApplication::processEvents();
    }
    
    // 保存数据
    bookManager.saveBooks();
    
    // 显示导入结果
    QString resultMessage = QString(u8"导入完成！\n成功：%1条\n失败：%2条")
                           .arg(successCount).arg(errorCount);
    
    if (!errorMessages.isEmpty() && errorMessages.size() <= 10) {
        resultMessage += "\n\n错误详情：\n" + errorMessages.join("\n");
    } else if (errorMessages.size() > 10) {
        resultMessage += QString("\n\n错误详情（前10条）：\n") + 
                        errorMessages.mid(0, 10).join("\n") + 
                        QString("\n...还有%1条错误").arg(errorMessages.size() - 10);
    }
    
    QMessageBox::information(this, u8"导入结果", resultMessage);
    
    return errorCount == 0;
}

void BookPage::onExportCsv() {
    QString fileName = QFileDialog::getSaveFileName(
        this,
        u8"导出图书数据",
        QString("books_%1.csv").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")),
        "CSV Files (*.csv)"
    );
    
    if (fileName.isEmpty()) {
        return;
    }
    
    if (exportBooksToCsv(fileName)) {
        QMessageBox::information(this, u8"成功", u8"图书数据导出成功！");
    } else {
        QMessageBox::critical(this, u8"错误", u8"图书数据导出失败！");
    }
}

bool BookPage::exportBooksToCsv(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream out(&file);
    out.setCodec("UTF-8");
    
    // 写入CSV头部
    out << u8"书名,作者,ISBN,分类,出版社,总库存,可借数量,状态\n";
    
    // 获取所有图书数据
    const auto& books = bookManager.getAllBooks();
    
    QProgressDialog progress(u8"正在导出...", u8"取消", 0, books.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    
    for (int i = 0; i < books.size(); ++i) {
        if (progress.wasCanceled()) {
            return false;
        }
        
        const Book* book = books[i];
        // 处理CSV中的特殊字符（逗号、引号、换行符）
        auto escapeField = [](const QString& field) {
            QString escaped = field;
            if (escaped.contains(',') || escaped.contains('"') || escaped.contains('\n')) {
                escaped.replace('"', '\"\"');
                escaped = '"' + escaped + '"';
            }
            return escaped;
        };
        
        out << escapeField(QString::fromStdString(book->getTitle())) << ","
            << escapeField(QString::fromStdString(book->getAuthor())) << ","
            << escapeField(QString::fromStdString(book->getISBN())) << ","
            << escapeField(QString::fromStdString(book->getCategory())) << ","
            << escapeField(QString::fromStdString(book->getPublisher())) << ","
            << book->getTotalCopies() << ","
            << book->getAvailableCopies() << ","
            << escapeField(QString::fromStdString(book->getStatusString())) << "\n";
        
        progress.setValue(i + 1);
        QApplication::processEvents();
    }
    
    return true;
}
