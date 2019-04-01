#ifndef SEARCHWINDOW_H
#define SEARCHWINDOW_H

#include <QDialog>

namespace Ui {
class SearchWindow;
}

class SearchWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SearchWindow(QWidget *parent = 0);
    ~SearchWindow();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::SearchWindow *ui;
};

#endif // SEARCHWINDOW_H
