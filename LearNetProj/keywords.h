#ifndef KEYWORDS_H
#define KEYWORDS_H

#include <QDialog>

namespace Ui {
class Keywords;
}

class Keywords : public QDialog
{
    Q_OBJECT

public:
    explicit Keywords(QWidget *parent = 0);
    ~Keywords();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::Keywords *ui;
};

#endif // KEYWORDS_H
