QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Admin.cpp \
    Book.cpp \
    BookManager.cpp \
    Loan.cpp \
    LoanManager.cpp \
    Login.cpp \
    Reader.cpp \
    ResourceLoader.cpp \
    StyleScaler.cpp \
    User.cpp \
    UserManager.cpp \
    adminMainWindow.cpp \
    bookPage.cpp \
    homepage.cpp \
    loanPage.cpp \
    main.cpp \
    mainWindow.cpp \
    personalPage.cpp \
    readerMainWindow.cpp \
    searchPage.cpp \
    tablePage.cpp \
    userAdd_register.cpp \
    userEditDialog.cpp \
    userPage.cpp

HEADERS += \
    Admin.h \
    Book.h \
    BookManager.h \
    ExitHelper.h \
    Loan.h \
    LoanManager.h \
    Login.h \
    Reader.h \
    ResourceLoader.h \
    StyleScaler.h \
    User.h \
    UserManager.h \
    adminMainWindow.h \
    bookPage.h \
    homePage.h \
    loanPage.h \
    mainWindow.h \
    personalPage.h \
    readerMainWindow.h \
    searchPage.h \
    tablePage.h \
    userAdd_register.h \
    userEditDialog.h \
    userPage.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
