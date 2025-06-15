// ExitHelper.h
#ifndef EXITHELPER_H
#define EXITHELPER_H

#include <QMessageBox>
#include <QPixmap>
#include <QICon>
#include <QWidget>

class ExitHelper {
public:
    static bool confirmExit(QWidget *parent = nullptr) {
        QPixmap question_img(":/pic/res/pic/pm_question.png");

        QMessageBox box(parent);   //重点 : 没定
        box.setWindowTitle(u8"退出");
        box.setText(u8"您确定要退出 miHoLib 吗？");
        box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        box.setDefaultButton(QMessageBox::No);
        box.setIconPixmap(question_img.scaled(150,150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        box.setWindowIcon(QIcon(":/icons/res/icons/question.png"));

        return box.exec() == QMessageBox::Yes;
    }
};

#endif // EXITHELPER_H
