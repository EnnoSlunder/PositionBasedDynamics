/********************************************************************************
** Form generated from reading UI file 'ControlWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTROLWIDGET_H
#define UI_CONTROLWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ControlWidget
{
public:
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout_2;
    QLineEdit *lineEdit;
    QLabel *label_5;
    QLabel *label_4;
    QLineEdit *lineEdit_2;
    QGridLayout *gridLayout;
    QDoubleSpinBox *doubleSpinBox_4;
    QLabel *label;
    QSpinBox *spinBox_2;
    QDoubleSpinBox *doubleSpinBox_5;
    QSpinBox *spinBox;
    QSpinBox *spinBox_4;
    QDoubleSpinBox *doubleSpinBox;
    QDoubleSpinBox *doubleSpinBox_2;
    QSpinBox *spinBox_3;
    QLabel *label_3;
    QLabel *label_2;
    QDoubleSpinBox *doubleSpinBox_3;
    QSpacerItem *verticalSpacer;
    QWidget *widget;
    QScrollBar *verticalScrollBar;
    QScrollBar *horizontalScrollBar;
    QSlider *horizontalSlider;
    QPushButton *pushButton;

    void setupUi(QWidget *ControlWidget)
    {
        if (ControlWidget->objectName().isEmpty())
            ControlWidget->setObjectName(QStringLiteral("ControlWidget"));
        ControlWidget->resize(743, 728);
        verticalLayoutWidget = new QWidget(ControlWidget);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 10, 541, 661));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        lineEdit = new QLineEdit(verticalLayoutWidget);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        gridLayout_2->addWidget(lineEdit, 0, 1, 1, 1);

        label_5 = new QLabel(verticalLayoutWidget);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout_2->addWidget(label_5, 2, 0, 1, 1);

        label_4 = new QLabel(verticalLayoutWidget);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout_2->addWidget(label_4, 0, 0, 1, 1);

        lineEdit_2 = new QLineEdit(verticalLayoutWidget);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));

        gridLayout_2->addWidget(lineEdit_2, 2, 1, 1, 1);


        verticalLayout->addLayout(gridLayout_2);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        doubleSpinBox_4 = new QDoubleSpinBox(verticalLayoutWidget);
        doubleSpinBox_4->setObjectName(QStringLiteral("doubleSpinBox_4"));

        gridLayout->addWidget(doubleSpinBox_4, 1, 4, 1, 1);

        label = new QLabel(verticalLayoutWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setTextFormat(Qt::AutoText);
        label->setScaledContents(false);
        label->setWordWrap(false);

        gridLayout->addWidget(label, 0, 1, 1, 1);

        spinBox_2 = new QSpinBox(verticalLayoutWidget);
        spinBox_2->setObjectName(QStringLiteral("spinBox_2"));

        gridLayout->addWidget(spinBox_2, 1, 3, 1, 1);

        doubleSpinBox_5 = new QDoubleSpinBox(verticalLayoutWidget);
        doubleSpinBox_5->setObjectName(QStringLiteral("doubleSpinBox_5"));

        gridLayout->addWidget(doubleSpinBox_5, 2, 4, 1, 1);

        spinBox = new QSpinBox(verticalLayoutWidget);
        spinBox->setObjectName(QStringLiteral("spinBox"));

        gridLayout->addWidget(spinBox, 0, 3, 1, 1);

        spinBox_4 = new QSpinBox(verticalLayoutWidget);
        spinBox_4->setObjectName(QStringLiteral("spinBox_4"));

        gridLayout->addWidget(spinBox_4, 2, 3, 1, 1);

        doubleSpinBox = new QDoubleSpinBox(verticalLayoutWidget);
        doubleSpinBox->setObjectName(QStringLiteral("doubleSpinBox"));

        gridLayout->addWidget(doubleSpinBox, 0, 2, 1, 1);

        doubleSpinBox_2 = new QDoubleSpinBox(verticalLayoutWidget);
        doubleSpinBox_2->setObjectName(QStringLiteral("doubleSpinBox_2"));

        gridLayout->addWidget(doubleSpinBox_2, 0, 4, 1, 1);

        spinBox_3 = new QSpinBox(verticalLayoutWidget);
        spinBox_3->setObjectName(QStringLiteral("spinBox_3"));

        gridLayout->addWidget(spinBox_3, 2, 2, 1, 1);

        label_3 = new QLabel(verticalLayoutWidget);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 2, 1, 1, 1);

        label_2 = new QLabel(verticalLayoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 1, 1, 1, 1);

        doubleSpinBox_3 = new QDoubleSpinBox(verticalLayoutWidget);
        doubleSpinBox_3->setObjectName(QStringLiteral("doubleSpinBox_3"));

        gridLayout->addWidget(doubleSpinBox_3, 1, 2, 1, 1);


        verticalLayout->addLayout(gridLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        widget = new QWidget(ControlWidget);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(570, 300, 161, 121));
        verticalScrollBar = new QScrollBar(widget);
        verticalScrollBar->setObjectName(QStringLiteral("verticalScrollBar"));
        verticalScrollBar->setGeometry(QRect(50, 10, 16, 160));
        verticalScrollBar->setOrientation(Qt::Vertical);
        horizontalScrollBar = new QScrollBar(widget);
        horizontalScrollBar->setObjectName(QStringLiteral("horizontalScrollBar"));
        horizontalScrollBar->setGeometry(QRect(90, 60, 160, 16));
        horizontalScrollBar->setOrientation(Qt::Horizontal);
        horizontalSlider = new QSlider(widget);
        horizontalSlider->setObjectName(QStringLiteral("horizontalSlider"));
        horizontalSlider->setGeometry(QRect(120, 30, 160, 22));
        horizontalSlider->setOrientation(Qt::Horizontal);
        pushButton = new QPushButton(widget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(20, 50, 114, 32));

        retranslateUi(ControlWidget);

        QMetaObject::connectSlotsByName(ControlWidget);
    } // setupUi

    void retranslateUi(QWidget *ControlWidget)
    {
        ControlWidget->setWindowTitle(QApplication::translate("ControlWidget", "Form", nullptr));
        label_5->setText(QApplication::translate("ControlWidget", "TextLabel", nullptr));
        label_4->setText(QApplication::translate("ControlWidget", "TextLabel", nullptr));
        label->setText(QApplication::translate("ControlWidget", "awsdasd", nullptr));
        label_3->setText(QApplication::translate("ControlWidget", "TextLabel", nullptr));
        label_2->setText(QApplication::translate("ControlWidget", "TextLabel", nullptr));
        pushButton->setText(QApplication::translate("ControlWidget", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ControlWidget: public Ui_ControlWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTROLWIDGET_H
