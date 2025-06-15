#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow()=default;
protected:
    QWidget* navWidget;            // 左侧导航
    QStackedWidget* stackedWidget; // 右侧页面容器

    //获取器
    QWidget* getNavWidget() { return navWidget; }
    QStackedWidget* getStackedWidget() { return stackedWidget; }

    //搭建器
    virtual void setupNavigation()=0;        // 搭建左侧导航栏
    virtual void setupPages() = 0;             // 搭建右侧页面容器
    void setupBaseUI();                        // 搭建基础结构，供子类使用

    //其他
    void resizeEvent(QResizeEvent *event) override final;
    virtual QString getQssFilePath() const = 0;  // 🔸 纯虚函数，交由派生类实现
    QPushButton* createIconButton(const QChar& iconChar, const QString& text); // 🔸 创建带图标的按钮
};
#endif // MAINWINDOW_H
