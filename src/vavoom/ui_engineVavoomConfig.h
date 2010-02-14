/********************************************************************************
** Form generated from reading ui file 'engineVavoomConfig.ui'
**
** Created: Sat Feb 13 21:57:02 2010
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_ENGINEVAVOOMCONFIG_H
#define UI_ENGINEVAVOOMCONFIG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_EngineVavoomConfigBox
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QHBoxLayout *horizontalLayout;
    QLineEdit *leClientBinaryPath;
    QPushButton *btnBrowseClientBinary;
    QVBoxLayout *verticalLayout_5;
    QLabel *label_4;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *leServerBinaryPath;
    QPushButton *btnBrowseServerBinary;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_3;
    QLineEdit *leCustomParameters;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_2;
    QLineEdit *leMasterserverAddress;
    QSpacerItem *verticalSpacer;

    void setupUi(QGroupBox *EngineVavoomConfigBox)
    {
        if (EngineVavoomConfigBox->objectName().isEmpty())
            EngineVavoomConfigBox->setObjectName(QString::fromUtf8("EngineVavoomConfigBox"));
        EngineVavoomConfigBox->resize(374, 252);
        verticalLayout_2 = new QVBoxLayout(EngineVavoomConfigBox);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
        label = new QLabel(EngineVavoomConfigBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setTextFormat(Qt::AutoText);

        verticalLayout->addWidget(label);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        leClientBinaryPath = new QLineEdit(EngineVavoomConfigBox);
        leClientBinaryPath->setObjectName(QString::fromUtf8("leClientBinaryPath"));

        horizontalLayout->addWidget(leClientBinaryPath);

        btnBrowseClientBinary = new QPushButton(EngineVavoomConfigBox);
        btnBrowseClientBinary->setObjectName(QString::fromUtf8("btnBrowseClientBinary"));
        btnBrowseClientBinary->setMaximumSize(QSize(32, 16777215));

        horizontalLayout->addWidget(btnBrowseClientBinary);


        verticalLayout->addLayout(horizontalLayout);


        verticalLayout_2->addLayout(verticalLayout);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalLayout_5->setSizeConstraint(QLayout::SetMinAndMaxSize);
        label_4 = new QLabel(EngineVavoomConfigBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setTextFormat(Qt::AutoText);

        verticalLayout_5->addWidget(label_4);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        leServerBinaryPath = new QLineEdit(EngineVavoomConfigBox);
        leServerBinaryPath->setObjectName(QString::fromUtf8("leServerBinaryPath"));

        horizontalLayout_2->addWidget(leServerBinaryPath);

        btnBrowseServerBinary = new QPushButton(EngineVavoomConfigBox);
        btnBrowseServerBinary->setObjectName(QString::fromUtf8("btnBrowseServerBinary"));
        btnBrowseServerBinary->setMaximumSize(QSize(32, 16777215));

        horizontalLayout_2->addWidget(btnBrowseServerBinary);


        verticalLayout_5->addLayout(horizontalLayout_2);


        verticalLayout_2->addLayout(verticalLayout_5);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        label_3 = new QLabel(EngineVavoomConfigBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout_4->addWidget(label_3);

        leCustomParameters = new QLineEdit(EngineVavoomConfigBox);
        leCustomParameters->setObjectName(QString::fromUtf8("leCustomParameters"));

        verticalLayout_4->addWidget(leCustomParameters);


        verticalLayout_2->addLayout(verticalLayout_4);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        label_2 = new QLabel(EngineVavoomConfigBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout_3->addWidget(label_2);

        leMasterserverAddress = new QLineEdit(EngineVavoomConfigBox);
        leMasterserverAddress->setObjectName(QString::fromUtf8("leMasterserverAddress"));

        verticalLayout_3->addWidget(leMasterserverAddress);


        verticalLayout_2->addLayout(verticalLayout_3);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);


        retranslateUi(EngineVavoomConfigBox);

        QMetaObject::connectSlotsByName(EngineVavoomConfigBox);
    } // setupUi

    void retranslateUi(QGroupBox *EngineVavoomConfigBox)
    {
        EngineVavoomConfigBox->setWindowTitle(QApplication::translate("EngineVavoomConfigBox", "GroupBox", 0, QApplication::UnicodeUTF8));
        EngineVavoomConfigBox->setTitle(QApplication::translate("EngineVavoomConfigBox", "Vavoom Configuration", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("EngineVavoomConfigBox", "Path to Vavoom client binary:", 0, QApplication::UnicodeUTF8));
        btnBrowseClientBinary->setText(QApplication::translate("EngineVavoomConfigBox", "...", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("EngineVavoomConfigBox", "Path to Vavoom server binary:", 0, QApplication::UnicodeUTF8));
        btnBrowseServerBinary->setText(QApplication::translate("EngineVavoomConfigBox", "...", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("EngineVavoomConfigBox", "Custom Parameters:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("EngineVavoomConfigBox", "Masterserver address:", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(EngineVavoomConfigBox);
    } // retranslateUi

};

namespace Ui {
    class EngineVavoomConfigBox: public Ui_EngineVavoomConfigBox {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ENGINEVAVOOMCONFIG_H
