#ifndef SIGNUPWIND_H
#define SIGNUPWIND_H

#include <QDialog>

namespace Ui {
class SignUpWind;
}

class SignUpWind : public QDialog
{
    Q_OBJECT

public:
    explicit SignUpWind(QWidget *parent = 0);
    ~SignUpWind();

private slots:
    void on_pushButton_clicked();

private:
    Ui::SignUpWind *ui;
};

#endif // SIGNUPWIND_H
