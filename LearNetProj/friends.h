#ifndef FRIENDS_H
#define FRIENDS_H

#include <QDialog>

namespace Ui {
class Friends;
}

class Friends : public QDialog
{
    Q_OBJECT

public:
    explicit Friends(QWidget *parent = 0);
    ~Friends();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_clicked();

private:
    Ui::Friends *ui;
};

#endif // FRIENDS_H
