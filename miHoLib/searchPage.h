#ifndef SEARCHPAGE_H
#define SEARCHPAGE_H

#include "Book.h"
#include "Loan.h"
#include "User.h"
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <vector>

class BookManager;
class LoanManager;

class SearchPage : public QWidget {
    Q_OBJECT

public:
    explicit SearchPage(User*u,QWidget* parent = nullptr);

private slots:
    void onSearchByISBN();
    void onSearchByTitle();
    void onSearchByAuthorPublisher();
    void onShowOverdueBooks();
    void onBorrowBook();
    void onReturnBook();

private:
    User* currentUser;
    // 数据依赖
    BookManager& m_bookManager;
    LoanManager& m_loanManager;

    // 控件区域
    QLineEdit* isbnEdit;
    QPushButton* isbnSearchBtn;

    QLineEdit* titleEdit;
    QPushButton* titleSearchBtn;

    QLineEdit* authorEdit;
    QLineEdit* publisherEdit;
    QPushButton* authorPublisherSearchBtn;

    QPushButton* overdueBooksBtn;

    QTableWidget* resultTable;
    QPushButton* borrowBtn;
    QPushButton* returnBtn;

    // 布局
    void setupUI();
    void displayBooks(const std::vector<Book>& books);
    void displayLoans(const std::vector<Loan>& loans);
    
    // UI辅助方法
    QFrame* createSearchFrame();
    void styleButton(QPushButton* button, const QString& color);
    QString adjustColor(const QString& color, int adjustment);
};

#endif // SEARCHPAGE_H
