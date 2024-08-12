#ifndef CUSTOMHAND_H
#define CUSTOMHAND_H

#include "qlabel.h"
#include "qradiobutton.h"
#include <QWidget>
#include <QButtonGroup>

class MyButtonGroup : public QButtonGroup
{
    Q_OBJECT

public:
    MyButtonGroup(QWidget *parent = 0) : QButtonGroup(parent)
    {
        connect(this, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));
        connect(this, &QButtonGroup::buttonToggled, this, &MyButtonGroup::onButtonToggled);
    }

    void ChangeExclusive(bool state)
    {
        setExclusive(state);
        exclusive_state = state;

    }

public slots:
    void onButtonClicked(QAbstractButton *button)
    {
        Q_UNUSED(button);
    }

    void onButtonToggled(QAbstractButton *button,bool state)
    {
        if(state == true)
        {
            button->setAutoExclusive(exclusive_state);
            button->group()->setExclusive(exclusive_state);
        }
    }

private:
    bool exclusive_state = false;

};
class CustomHand : public QWidget
{
public:

    CustomHand(QWidget *parent = nullptr) : QWidget(parent)
    {

    }

    void setBackgroudLabel(QLabel *label)
    {
        backgroundLabel = label;

        backgroundLabel->setPixmap(QPixmap(":/img/@3x/bg_hand@3x.png"));
        backgroundLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

        fingerRightThumb = new QRadioButton (backgroundLabel);
        fingerRightIndex = new QRadioButton (backgroundLabel);
        fingerRightMiddle = new QRadioButton (backgroundLabel);
        fingerRightRing = new QRadioButton (backgroundLabel);
        fingerRightLittle = new QRadioButton (backgroundLabel);

        fingerLeftThumb = new QRadioButton (backgroundLabel);
        fingerLeftIndex = new QRadioButton (backgroundLabel);
        fingerLeftMiddle = new QRadioButton (backgroundLabel);
        fingerLeftRing = new QRadioButton (backgroundLabel);
        fingerLeftLittle = new QRadioButton (backgroundLabel);


        // Agregar cada radio button a su propio grupo
        group1->addButton(fingerRightThumb,1);
        group1->addButton(fingerRightIndex,2);
        group1->addButton(fingerRightMiddle,3);
        group1->addButton(fingerRightRing,4);
        group1->addButton(fingerRightLittle,5);

        group1->addButton(fingerLeftThumb,6);
        group1->addButton(fingerLeftIndex,7);
        group1->addButton(fingerLeftMiddle,8);
        group1->addButton(fingerLeftRing,9);
        group1->addButton(fingerLeftLittle,10);

        fingerRightThumb->setChecked(true);
        fingerRightIndex->setChecked(true);
        fingerRightMiddle->setChecked(true);
        fingerRightRing->setChecked(true);
        fingerRightLittle->setChecked(true);


        fingerLeftThumb->setChecked(true);
        fingerLeftIndex->setChecked(true);
        fingerLeftMiddle->setChecked(true);
        fingerLeftRing->setChecked(true);
        fingerLeftLittle->setChecked(true);

        // Establecer las posiciones relativas de los radio buttons
        relativePos1 = QPointF(0.5, 0.5); // Debe ser la posición relativa para el dedo correspondiente
        // ... Establecer las posiciones relativas para los demás botones
        updateButtonPositions();
    }
protected:
    void resizeEvent(QResizeEvent *event) override
    {
        Q_UNUSED(event);
        updateButtonPositions();
    }

public:
    QLabel *backgroundLabel;

    void updateButtonPositions()
    {
        qDebug() << "updateButtonPositions";

        int absoluteX = static_cast<int>(backgroundLabel->width() * relativePos1.x());
        int absoluteY = static_cast<int>(backgroundLabel->height() * relativePos1.y());

        fingerLeftLittle->move(absoluteX - 230 ,absoluteY - 50);
        fingerLeftRing->move(absoluteX - 190 ,absoluteY - 95);
        fingerLeftMiddle->move(absoluteX - 145 ,absoluteY - 110);
        fingerLeftIndex->move(absoluteX - 100 ,absoluteY - 80);
        fingerLeftThumb->move(absoluteX - 42 ,absoluteY + 38);

        fingerRightLittle->move(absoluteX + 215 ,absoluteY - 50);
        fingerRightRing->move(absoluteX + 170 ,absoluteY - 95);
        fingerRightMiddle->move(absoluteX + 125 ,absoluteY - 110);
        fingerRightIndex->move(absoluteX + 80 ,absoluteY - 80);
        fingerRightThumb->move(absoluteX + 22 ,absoluteY + 38);
    }
    QRadioButton  *fingerRightThumb;
    QRadioButton  *fingerRightIndex;
    QRadioButton  *fingerRightMiddle;
    QRadioButton  *fingerRightRing;
    QRadioButton  *fingerRightLittle;

    QRadioButton  *fingerLeftThumb;
    QRadioButton  *fingerLeftIndex;
    QRadioButton  *fingerLeftMiddle;
    QRadioButton  *fingerLeftRing;
    QRadioButton  *fingerLeftLittle;

    // Crear grupos de botones para cada radio button
    MyButtonGroup *group1 = new MyButtonGroup;


    QPointF relativePos1;
    // ... Más posiciones relativas para los otros dedos
};

#endif // CUSTOMHAND_H
