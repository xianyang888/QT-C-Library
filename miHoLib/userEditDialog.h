#ifndef USEREDITDIALOG_H
#define USEREDITDIALOG_H

#include <QDialog>
#include <QString>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>

class UserEditDialog : public QDialog
{
    Q_OBJECT

public:
    // 构造函数（预填信息）
    UserEditDialog(const QString& account,
                   const QString& name,
                   const QString& major,
                   const QString& userType,
                   QWidget* parent = nullptr);

    // 获取修改后的数据
    QString getName() const;
    QString getMajor() const;
    QString getUserType() const;

private slots:
    void onConfirmClicked();
    void onCancelClicked();

private:
    void setupUI();

    QString m_account;

    QLabel* accountLabel;
    QLineEdit* nameEdit;
    QLineEdit* majorEdit;
    QComboBox* typeCombo;

    QPushButton* confirmButton;
    QPushButton* cancelButton;
};

#endif // USEREDITDIALOG_H
