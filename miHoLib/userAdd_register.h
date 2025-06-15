#ifndef USERADD_H
#define USERADD_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>

class UserAddDialog : public QDialog {
    Q_OBJECT
public:
    explicit UserAddDialog(QWidget *parent = nullptr);
    virtual ~UserAddDialog() = default;

    //获取器
    QString getName() const{return nameEdit->text();};
    QString getAccount() const{return accountEdit->text();};
    QString getPassword() const{return passwordEdit->text();};
    QString getMajor() const{return majorEdit->text();};
    QString getUserType() const{return typeCombo->currentText();};
    

protected:
    QLineEdit* nameEdit;
    QLineEdit* accountEdit;
    QLineEdit* passwordEdit;
    QLineEdit* majorEdit;
    QComboBox* typeCombo; // 仅在管理员添加时使用

    QFormLayout* formLayout;
    QDialogButtonBox* buttonBox;

    
    virtual void setupUI();         // 基础布局
    virtual void applyCustomUI()=0;   // 子类可复写以隐藏/定制某些控件
};

class RegisterDialog : public UserAddDialog {
    Q_OBJECT
public:
    explicit RegisterDialog(QWidget* parent = nullptr);

protected:
    void applyCustomUI() override;  // 隐藏typeCombo，固定用户类型为"READER"
};

class AdminAddUserDialog : public UserAddDialog {
    Q_OBJECT
public:
    explicit AdminAddUserDialog(QWidget* parent = nullptr);

protected:
    void applyCustomUI() override;  // 显示typeCombo并添加选项
};


#endif // USERADD_H
