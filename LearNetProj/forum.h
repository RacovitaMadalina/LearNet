#ifndef FORUM_H
#define FORUM_H

#include <QDialog>

namespace Ui {
class Forum;
}

class Forum : public QDialog
{
    Q_OBJECT

public:
    explicit Forum(QWidget *parent = 0);
    ~Forum();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::Forum *ui;
};

#endif // FORUM_H
