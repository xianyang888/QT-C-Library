#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDialog>
#include <QMouseEvent>
#include <QEvent>
#include <QDebug>
#include <QObject>
#include <QWheelEvent>
#include "User.h"

// 前向声明
class LoginRequest;

// 视图层 - 负责UI展示和事件处理
class loginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit loginDialog(QWidget *parent = nullptr);
    User* getCurrentUser() const { return m_currentUser; }

private:
    QLabel *logoLabel;
    QLabel *zstuLabel;
    QLineEdit *accountEdit;
    QLineEdit *passwordEdit;
    QLabel *noaccount;
    QPushButton *loginButton;
    QPushButton *registerButton;
    QPushButton *forgotButton;
    QPushButton *stdQuickLoginButton;
    QPushButton *closeButton;
    QPoint offset;
    bool mousePressed;
    User* m_currentUser;
    LoginRequest* m_loginRequest;  // 请求层组件
    
    void setupUi();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
    void onLoginButtonClicked();  // 处理登录按钮点击
    void onRegisterButtonClicked();  // 处理注册按钮点击
    void handleForgot();  // 处理忘记密码
    
    // 处理请求层的响应
    void onLoginSuccess(User* user);
    void onLoginFailed(const QString& errorMessage);
    void onRegisterSuccess();
    void onRegisterFailed(const QString& errorMessage);
};

// 请求层 - 负责请求调度和参数构造
class LoginRequest : public QObject
{
    Q_OBJECT
public:
    explicit LoginRequest(QObject *parent = nullptr);
    
    // 公开的请求方法
    void loginRequest(const QString& account, const QString& password);
    void registerRequest(const QString& account, const QString& password,
                        const QString& name, const QString& major);
    void forgetRequest(const QString& account);

signals:
    // 登录相关信号
    void loginSuccess(User* user);
    void loginFailed(const QString& errorMessage);
    
    // 注册相关信号
    void registerSuccess();
    void registerFailed(const QString& errorMessage);
    
    // 忘记密码相关信号
    void forgetSuccess();
    void forgetFailed(const QString& errorMessage);

private:
    class LoginManager* m_loginManager;  // 持有服务层引用
};

// 服务层 - 单例模式，执行实际业务逻辑
class LoginManager
{
private:
    LoginManager();
    LoginManager(const LoginManager&) = delete;
    LoginManager& operator=(const LoginManager&) = delete;

public:
    ~LoginManager();
    static LoginManager& getInstance();
    
    // 业务接口
    User* tryLogin(const QString& account, const QString& password);
    bool registerUser(const QString& account, const QString& password, 
                     const QString& name, const QString& major);
    bool resetPassword(const QString& account);
    
private:
    User* currentUser;
};

#endif // LOGINDIALOG_H
