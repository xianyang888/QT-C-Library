#include "Login.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFont>
#include <QPixmap>
#include <QMouseEvent>
#include <QEvent>
#include <QDebug>
#include <QObject>
#include <QApplication>
#include <QCursor>
#include <QMessageBox>


#include "ExitHelper.h"
#include "ResourceLoader.h"
#include "adminMainWindow.h"
#include "readerMainWindow.h"
#include "Admin.h"
#include "Reader.h"
#include "UserManager.h"
#include "userAdd_register.h"

// ============== 视图层实现 ==============
loginDialog::loginDialog(QWidget *parent)
    : QDialog{parent}, mousePressed(false), m_currentUser(nullptr)
{
    // 创建请求层组件
    m_loginRequest = new LoginRequest(this);
    
    setupUi();
    
    // UI事件连接
    connect(closeButton, &QPushButton::clicked, this, [this]() {
        if (ExitHelper::confirmExit(this)) {
            QApplication::quit();
        }
    });
    
    connect(loginButton, &QPushButton::clicked, this, &loginDialog::onLoginButtonClicked);
    connect(registerButton, &QPushButton::clicked, this, &loginDialog::onRegisterButtonClicked);
    connect(forgotButton, &QPushButton::clicked, this, &loginDialog::handleForgot);
    
    // 请求层信号连接
    connect(m_loginRequest, &LoginRequest::loginSuccess, this, &loginDialog::onLoginSuccess);
    connect(m_loginRequest, &LoginRequest::loginFailed, this, &loginDialog::onLoginFailed);
    connect(m_loginRequest, &LoginRequest::registerSuccess, this, &loginDialog::onRegisterSuccess);
    connect(m_loginRequest, &LoginRequest::registerFailed, this, &loginDialog::onRegisterFailed);
}

void loginDialog::setupUi()
{
    // 设置无边框窗口
    setWindowFlag(Qt::FramelessWindowHint);

    setWindowTitle(u8"miHoLib 登录");
    setFixedSize(680, 600);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(40, 20, 40, 10); // 调整四周边距（左、上、右、下）
    mainLayout->setSpacing(10); // 统一控件垂直间距为20像素

    // Header with close button
    QHBoxLayout* headerLayout = new QHBoxLayout();
    headerLayout->addStretch();
    closeButton = new QPushButton(this);
    closeButton->setFlat(true);
    closeButton->setIcon(QPixmap(":/icons/res/icons/close.svg"));
    closeButton->setIconSize(QSize(20, 20));
    closeButton->setFixedSize(30, 30);
    closeButton->setCursor(Qt::PointingHandCursor);
    closeButton->setToolTip(u8"关闭");
    headerLayout->addWidget(closeButton);
    mainLayout->addLayout(headerLayout);

    // Logo
    QHBoxLayout* logoLayout = new QHBoxLayout();
    logoLabel = new QLabel(this);
    QPixmap logoPixmap(":/icons/res/icons/mihoyo.png");
    logoLabel->setPixmap(logoPixmap.scaled(200, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logoLabel->setFixedSize(200, 120);
    logoLayout->addStretch();
    logoLayout->addWidget(logoLabel);
    logoLayout->addStretch();
    mainLayout->addLayout(logoLayout);

    // Account
    accountEdit = new QLineEdit(this);
    accountEdit->setPlaceholderText(u8"请输入账号/学号");
    accountEdit->setFixedSize(600, 70);
    mainLayout->addWidget(accountEdit);

    // Password
    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText(u8"请输入密码");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setFixedSize(600, 70);
    mainLayout->addWidget(passwordEdit);

    // Register / Forgot
    QHBoxLayout* linkLayout = new QHBoxLayout();
    linkLayout->addWidget(new QLabel(u8"没有账号?", this));
    registerButton = new QPushButton(u8"立即注册", this);
    forgotButton = new QPushButton(u8"忘记密码", this);
    registerButton->setFlat(true);
    forgotButton->setFlat(true);
    registerButton->setObjectName("twoButton");
    forgotButton->setObjectName("twoButton");
    linkLayout->addWidget(registerButton);
    linkLayout->addStretch();
    linkLayout->addWidget(forgotButton);
    mainLayout->addLayout(linkLayout);

    // Login button
    loginButton = new QPushButton(u8"进入在线图书馆(miHoLib)", this);
    loginButton->setObjectName("loginButton");
    loginButton->setFixedHeight(50);
    mainLayout->addWidget(loginButton);

    // Bottom layout
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();

    zstuLabel = new QLabel(this);
    QPixmap zstuPixmap(":/icons/res/icons/zstu.png");
    zstuLabel->setPixmap(zstuPixmap.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    zstuLabel->setFixedSize(32, 32);
    bottomLayout->addWidget(zstuLabel);

    QLabel* copyrightLabel = new QLabel(u8"©2025 miHoLib by 管智洋", this);
    bottomLayout->addWidget(copyrightLabel);
    bottomLayout->addStretch();
    mainLayout->addLayout(bottomLayout);
    setLayout(mainLayout);

    ResourceLoader::applyQSS(this, ":/style/res/style/login.qss");
}

// 视图层方法实现
void loginDialog::onLoginButtonClicked() {
    QString account = accountEdit->text();
    QString password = passwordEdit->text();
    
    // 输入验证
    if (account.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, u8"登录失败", u8"账号和密码不能为空！");
        return;
    }
    
    // 调用请求层发起登录请求
    m_loginRequest->loginRequest(account, password);
}

void loginDialog::onRegisterButtonClicked() {
    RegisterDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString account = dialog.getAccount();
        QString password = dialog.getPassword();
        QString name = dialog.getName();
        QString major = dialog.getMajor();
        // 校验字段
        if (account.isEmpty() || password.isEmpty() || name.isEmpty()) {
            QMessageBox::warning(this, u8"注册失败", u8"请填写完整信息！");
            return;
        }
        m_loginRequest->registerRequest(account, password, name, major);
    }
}

void loginDialog::onLoginSuccess(User* user) {
    m_currentUser = user;
    QString userType = (user->getType() == User::UserType::ADMIN) ? u8"管理员" : u8"读者";
    QMessageBox::information(this, u8"登录成功", 
        userType + u8":" + QString::fromStdString(user->getName()) + u8" 登录成功！");
    accept(); // 关闭登录窗口
}

void loginDialog::onLoginFailed(const QString& errorMessage) {
    QMessageBox::warning(this, u8"登录失败", errorMessage);
}

void loginDialog::onRegisterSuccess() {
    QMessageBox::information(this, u8"注册成功", u8"注册成功！");
}

void loginDialog::onRegisterFailed(const QString& errorMessage) {
    QMessageBox::warning(this, u8"注册失败", errorMessage);
}



void loginDialog::handleForgot() {
    QDialog forgotDialog(this);
    forgotDialog.setWindowTitle(u8"找回密码");
    forgotDialog.setFixedSize(400, 200);
    
    QVBoxLayout* layout = new QVBoxLayout(&forgotDialog);
    
    // 账号输入
    QLineEdit* accountEdit = new QLineEdit(&forgotDialog);
    accountEdit->setPlaceholderText(u8"请输入账号/学号");
    layout->addWidget(new QLabel(u8"账号:", &forgotDialog));
    layout->addWidget(accountEdit);
    
    // 提示信息
    QLabel* infoLabel = new QLabel(u8"*请联系图书馆管理员重置密码", &forgotDialog);
    infoLabel->setStyleSheet("color: #888;");
    layout->addWidget(infoLabel);
    
    // 按钮区域
    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* cancelBtn = new QPushButton(u8"取消", &forgotDialog);
    QPushButton* confirmBtn = new QPushButton(u8"提交", &forgotDialog);
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(confirmBtn);
    layout->addLayout(btnLayout);
    
    // 连接信号
    connect(cancelBtn, &QPushButton::clicked, &forgotDialog, &QDialog::reject);
    connect(confirmBtn, &QPushButton::clicked, [&]() {
        QString account = accountEdit->text();
        
        if (account.isEmpty()) {
            QMessageBox::warning(&forgotDialog, u8"提交失败", u8"请输入账号！");
            return;
        }
        
        QMessageBox::information(&forgotDialog, u8"提交成功", 
            u8"密码重置请求已提交，请联系管理员处理。\n联系方式：admin@miholib.com");
        forgotDialog.accept();
    });
    
    forgotDialog.exec();
}

// 拖动事件处理
void loginDialog::mousePressEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton) {
        offset = event->globalPos() - this->pos();
        mousePressed = true;
    }
}

void loginDialog::mouseMoveEvent(QMouseEvent* event)
{
    if (mousePressed) {
        move(event->globalPos() - offset);
    }
}

void loginDialog::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    mousePressed = false;
}

// ============== 请求层实现 ==============
LoginRequest::LoginRequest(QObject *parent)
    : QObject(parent)
{
    m_loginManager = &LoginManager::getInstance();
}

void LoginRequest::loginRequest(const QString& account, const QString& password) {
    try {
        User* user = m_loginManager->tryLogin(account, password);
        if (user) {
            emit loginSuccess(user);
        } else {
            emit loginFailed(u8"账号或密码错误");
        }
    } catch (const std::exception& e) {
        emit loginFailed(u8"登录过程中发生错误：" + QString::fromStdString(e.what()));
    }
}

void LoginRequest::registerRequest(const QString& account, const QString& password, 
                                 const QString& name, const QString& major) {
    try {
        bool success = m_loginManager->registerUser(account, password, name, major);
        if (success) {
            emit registerSuccess();
        } else {
            emit registerFailed(u8"注册失败，账号可能已存在");
        }
    } catch (const std::exception& e) {
        emit registerFailed(u8"注册过程中发生错误：" + QString::fromStdString(e.what()));
    }
}

void LoginRequest::forgetRequest(const QString& account) {
    try {
        bool success = m_loginManager->resetPassword(account);
        if (success) {
            emit forgetSuccess();
        } else {
            emit forgetFailed(u8"账号不存在");
        }
    } catch (const std::exception& e) {
        emit forgetFailed(u8"重置密码过程中发生错误：" + QString::fromStdString(e.what()));
    }
}

// ============== 服务层实现 ==============
LoginManager::LoginManager() : currentUser(nullptr) {}

LoginManager::~LoginManager() {
    // 注意：不要在这里删除currentUser，因为它可能被其他地方使用
}

LoginManager& LoginManager::getInstance() {
    static LoginManager instance;
    return instance;
}

User* LoginManager::tryLogin(const QString& account, const QString& password) {
    UserManager& userManager = UserManager::getInstance();
    
    try {
        // 先尝试管理员登录
        User* user = userManager.authenticate(
            account.toStdString(), 
            password.toStdString(), 
            User::UserType::ADMIN
        );
        currentUser = user;
        return user;
    } catch (const std::invalid_argument&) {
        try {
            // 再尝试读者登录
            User* user = userManager.authenticate(
                account.toStdString(), 
                password.toStdString(), 
                User::UserType::READER
            );
            currentUser = user;
            return user;
        } catch (const std::invalid_argument&) {
            return nullptr;
        }
    }
}

bool LoginManager::registerUser(const QString& account, const QString& password,
                                const QString& name, const QString& major) {
    try {
        UserManager& userManager = UserManager::getInstance();
        User* newUser = new Reader(account.toStdString(), password.toStdString(), 0,
                                   name.toStdString(), major.toStdString());

        if(newUser)userManager.addUser(newUser);
        return true;
    } catch (const User::DuplicateAccountException&) {
        // 处理重复账户异常
        return false;
    } catch (const std::exception&) {
        // 处理其他异常（包括内存分配失败等）
        return false;
    }
}
bool LoginManager::resetPassword(const QString& account) {
    // 这里可以实现密码重置逻辑
    // 目前只是一个占位实现
    Q_UNUSED(account);
    return true; // 假设总是成功
}


