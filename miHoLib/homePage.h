#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QFrame>
#include <QString>
#include <vector>
#include "LoanManager.h"
#include "BookManager.h"

struct BookStat {
    QString title;
    int borrowCount;
};

class HomePage : public QWidget {
    Q_OBJECT

public:
    explicit HomePage(const QString& adminName, QWidget* parent = nullptr);

    void setMonthlyStats(double overdueRate, const std::vector<BookStat>& topBooks);

private:
    void setupUI();
    QFrame* createCard(const QString& title, QWidget* contentWidget);

    QString m_adminName;

    LoanManager& loanManager;
    BookManager& bookManager;

    QLabel* overdueRateLabel;
    QTableWidget* top10Table;
    //刷新
    void updateStats();
};

#endif // HOMEPAGE_H
