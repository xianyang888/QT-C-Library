# miHoLib 图书馆管理系统

## 项目简介
miHoLib 是一个基于 Qt 框架开发的图书馆管理系统，支持图书、用户、借阅等多种业务功能，适用于高校或中小型图书馆的信息化管理。

## 主要功能
- **用户管理**：支持管理员和读者两种角色，分别拥有不同权限。
- **图书管理**：图书的增删改查、分类、库存管理。
- **借阅管理**：借书、还书、逾期管理、罚金计算。
- **个人中心**：用户信息修改、密码修改、当前借阅查询、归还操作。
- **多窗口界面**：包括登录、主界面、图书管理、借阅管理、个人中心等。
- **数据持久化**：支持数据文件存储（如 books.dat、loans.dat、reader.dat、admin.dat）。
- **界面美化**：支持自定义 QSS 皮肤、图标、字体等资源。

## 目录结构
```
miHoLib/
├── Admin.cpp/.h           # 管理员类实现
├── Book.cpp/.h            # 图书类实现
├── BookManager.cpp/.h     # 图书管理器
├── Loan.cpp/.h            # 借阅记录类
├── LoanManager.cpp/.h     # 借阅管理器
├── User.cpp/.h            # 用户基类
├── Reader.cpp/.h          # 读者类
├── UserManager.cpp/.h     # 用户管理器
├── main.cpp               # 程序入口
├── mainWindow.cpp/.h      # 主窗口
├── adminMainWindow.cpp/.h # 管理员主界面
├── readerMainWindow.cpp/.h# 读者主界面
├── personalPage.cpp/.h    # 个人中心页面
├── bookPage.cpp/.h        # 图书管理页面
├── loanPage.cpp/.h        # 借阅管理页面
├── searchPage.cpp/.h      # 图书搜索页面
├── tablePage.cpp/.h       # 通用表格页面
├── userAdd_register.cpp/.h# 用户注册/添加
├── userEditDialog.cpp/.h  # 用户信息编辑对话框
├── ResourceLoader.cpp/.h  # 资源加载
├── StyleScaler.cpp/.h     # 样式缩放
├── res/                   # 资源文件（数据、图片、样式等）
│   ├── data/              # 数据文件
│   ├── font/              # 字体
│   ├── icons/             # 图标
│   ├── pic/               # 图片
│   └── style/             # QSS 样式表
├── miHoLib.pro            # Qt 工程文件
└── README.md              # 项目说明
```

## 编译与运行
### 环境要求
- Qt 5.x/6.x（建议 5.12 及以上）
- C++11 及以上
- Windows（推荐）、支持跨平台

### 编译步骤
1. 使用 Qt Creator 打开 `miHoLib.pro` 工程文件。
2. 配置合适的 Qt 版本与编译器。(作者推荐使用 :`Qt 5.15.1 MSVC2019 64bit`)
3. 构建并运行项目。

### 数据文件说明
- `res/data/admin.dat`：管理员信息
- `res/data/reader.dat`：读者信息
- `res/data/books.dat`：图书信息
- `res/data/loans.dat`：借阅记录

首次运行可自动生成初始数据，或手动编辑上述文件。

## 主要类与模块说明
- **User / Admin / Reader**：用户体系，支持多态。
- **Book / BookManager**：图书及其管理，支持分类、库存、状态管理。
- **Loan / LoanManager**：借阅记录及管理，支持借阅、归还、逾期、罚金。
- **UserManager**：用户注册、登录、信息维护。
- **各窗口/页面类**：负责 UI 展示与交互。

## 继承关系
- `User` 为基类，`Admin` 和 `Reader` 继承自 `User`。
- `BookManager`、`LoanManager`、`UserManager` 采用单例模式。

## 资源与样式
- 所有界面资源、图标、字体、样式表均在 `res/` 目录下。
- 支持自定义皮肤与主题。

## 贡献与反馈
如需贡献代码或反馈问题，请通过 issue 或 pull request 联系维护者

**GitHub**：[个人Github](https://github.com/xianyang888)

### 如何贡献 

1. Fork本仓库 
2. 创建功能分支：`git checkout -b feature/new-function`
3. 提交更改：`git commit -m "添加新功能"` 
4. 推送分支：`git push origin feature/new-function` 
5. 提交Pull Request

## 其他说明  

**项目背景** 

本项目为本人作为2024级大一学生的《面向对象程序设计》课程设计作业，旨在实践面向对象编程思想（如封装、继承、多态等）。  

**特别提示**  

- 由于个人能力及课程要求限制，代码可能存在逻辑漏洞、优化空间或规范问题，欢迎指正！  
- 部分实现可能未完全符合工业级标准，仅供学习交流使用。  

## License

MIT License © 2025 xianyang888

本项目仅供学习与交流，禁止商业用途。