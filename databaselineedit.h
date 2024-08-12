#ifndef DATABASELINEEDIT_H
#define DATABASELINEEDIT_H

#include <QLineEdit>
#include <QMouseEvent>
#include <QDebug>

class DatabaseLineEdit : public QLineEdit {
    Q_OBJECT

public:
    explicit DatabaseLineEdit(QWidget *parent = nullptr) : QLineEdit(parent) {}

protected:
    void mousePressEvent(QMouseEvent *event) override {
        QLineEdit::mousePressEvent(event);
        if (event->button() == Qt::LeftButton) {
            emit clicked();
        }
    }

signals:
    void clicked();
};

#endif // DATABASELINEEDIT_H
