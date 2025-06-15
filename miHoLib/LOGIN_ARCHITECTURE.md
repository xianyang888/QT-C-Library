# 登录系统架构重构说明

## 概述
本次重构将原有的单一登录对话框重构为三层架构，实现了职责分离和更好的代码组织。

## 架构设计

### 1. 视图层 (loginDialog)
**职责：**
- UI展示和用户交互
- 输入验证
- 事件处理
- 调用请求层发起请求

**主要方法：**
- `onLoginButtonClicked()` - 处理登录按钮点击
- `onRegisterButtonClicked()` - 处理注册按钮点击
- `handleForgot()` - 处理忘记密码
- `onLoginSuccess(User*)` - 处理登录成功响应
- `onLoginFailed(QString)` - 处理登录失败响应

**特点：**
- 不直接处理业务逻辑
- 通过信号槽与请求层通信
- 负责用户体验和界面反馈

### 2. 请求层 (LoginRequest)
**职责：**
- 请求调度中心
- 参数构造和验证
- 将请求转发到服务层
- 处理服务层响应并发送信号

**主要方法：**
- `loginRequest(account, password)` - 发起登录请求
- `registerRequest(account, password, name, major, type)` - 发起注册请求
- `forgetRequest(account)` - 发起忘记密码请求

**信号：**
- `loginSuccess(User*)` / `loginFailed(QString)`
- `registerSuccess()` / `registerFailed(QString)`
- `forgetSuccess()` / `forgetFailed(QString)`

**特点：**
- 继承QObject，支持信号槽机制
- 持有LoginManager引用
- 异常处理和错误转换

### 3. 服务层 (LoginManager)
**职责：**
- 执行实际的业务逻辑
- 与数据层(UserManager)交互
- 用户认证和管理
- 单例模式确保全局唯一

**主要方法：**
- `tryLogin(account, password)` - 尝试用户登录
- `registerUser(account, password, name, major, type)` - 注册新用户
- `resetPassword(account)` - 重置密码

**特点：**
- 单例模式
- 纯业务逻辑，不依赖UI
- 可被其他模块复用

## 数据流向

```
用户操作 → loginDialog → LoginRequest → LoginManager → UserManager
                ↓              ↓              ↓
            UI更新 ←    信号响应   ←    业务处理   ←    数据操作
```

## 优势

1. **职责分离**：每层专注于自己的职责，代码更清晰
2. **可测试性**：业务逻辑与UI分离，便于单元测试
3. **可维护性**：修改某层逻辑不影响其他层
4. **可扩展性**：易于添加新的请求类型或UI组件
5. **复用性**：服务层可被其他模块使用

## 使用示例

```cpp
// 创建登录对话框
loginDialog dialog;

// 显示对话框
if (dialog.exec() == QDialog::Accepted) {
    User* currentUser = dialog.getCurrentUser();
    // 处理登录成功后的逻辑
}
```

## 扩展建议

1. **添加日志记录**：在请求层添加操作日志
2. **缓存机制**：在服务层添加用户信息缓存
3. **异步处理**：对于耗时操作使用异步处理
4. **配置管理**：将配置信息从硬编码中分离
5. **国际化支持**：添加多语言支持