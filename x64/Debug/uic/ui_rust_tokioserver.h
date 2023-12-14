/********************************************************************************
** Form generated from reading UI file 'rust_tokioserver.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RUST_TOKIOSERVER_H
#define UI_RUST_TOKIOSERVER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_rust_tokioserverClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *rust_tokioserverClass)
    {
        if (rust_tokioserverClass->objectName().isEmpty())
            rust_tokioserverClass->setObjectName("rust_tokioserverClass");
        rust_tokioserverClass->resize(600, 400);
        menuBar = new QMenuBar(rust_tokioserverClass);
        menuBar->setObjectName("menuBar");
        rust_tokioserverClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(rust_tokioserverClass);
        mainToolBar->setObjectName("mainToolBar");
        rust_tokioserverClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(rust_tokioserverClass);
        centralWidget->setObjectName("centralWidget");
        rust_tokioserverClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(rust_tokioserverClass);
        statusBar->setObjectName("statusBar");
        rust_tokioserverClass->setStatusBar(statusBar);

        retranslateUi(rust_tokioserverClass);

        QMetaObject::connectSlotsByName(rust_tokioserverClass);
    } // setupUi

    void retranslateUi(QMainWindow *rust_tokioserverClass)
    {
        rust_tokioserverClass->setWindowTitle(QCoreApplication::translate("rust_tokioserverClass", "rust_tokioserver", nullptr));
    } // retranslateUi

};

namespace Ui {
    class rust_tokioserverClass: public Ui_rust_tokioserverClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RUST_TOKIOSERVER_H
