/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.12.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCharts/QChartView>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_Principal;
    QHBoxLayout *fila1;
    QGroupBox *grupoConexion;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *lineEditIp;
    QLabel *label_2;
    QSpinBox *spinPuerto;
    QPushButton *botonConectar;
    QLabel *etiquetaEstado;
    QGroupBox *grupoServo;
    QHBoxLayout *horizontalLayout_2;
    QSlider *deslizadorAngulo;
    QLabel *etiquetaPedido;
    QHBoxLayout *fila2;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_4;
    QPushButton *btnCuchau;
    QPushButton *btnMiedo;
    QPushButton *btnNoNo;
    QPushButton *btnDireccionales;
    QGroupBox *grupoMovimiento;
    QGridLayout *gridLayout_2;
    QPushButton *pushButton_4;
    QPushButton *pushButton_2;
    QPushButton *pushButton;
    QPushButton *pushButton_3;
    QGroupBox *grupoDistancia;
    QHBoxLayout *horizontalLayout_3;
    QChartView *graficaPot;
    QListWidget *listaEventos;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1280, 720);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout_Principal = new QVBoxLayout(centralwidget);
        verticalLayout_Principal->setObjectName("verticalLayout_Principal");
        verticalLayout_Principal->setContentsMargins(5, 5, 5, 5);
        fila1 = new QHBoxLayout();
        fila1->setObjectName("fila1");
        grupoConexion = new QGroupBox(centralwidget);
        grupoConexion->setObjectName("grupoConexion");
        grupoConexion->setMaximumHeight(70);
        horizontalLayout = new QHBoxLayout(grupoConexion);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(-1, 0, -1, 0);
        label = new QLabel(grupoConexion);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);

        lineEditIp = new QLineEdit(grupoConexion);
        lineEditIp->setObjectName("lineEditIp");

        horizontalLayout->addWidget(lineEditIp);

        label_2 = new QLabel(grupoConexion);
        label_2->setObjectName("label_2");

        horizontalLayout->addWidget(label_2);

        spinPuerto = new QSpinBox(grupoConexion);
        spinPuerto->setObjectName("spinPuerto");
        spinPuerto->setMaximum(65535);
        spinPuerto->setValue(81);

        horizontalLayout->addWidget(spinPuerto);

        botonConectar = new QPushButton(grupoConexion);
        botonConectar->setObjectName("botonConectar");

        horizontalLayout->addWidget(botonConectar);

        etiquetaEstado = new QLabel(grupoConexion);
        etiquetaEstado->setObjectName("etiquetaEstado");

        horizontalLayout->addWidget(etiquetaEstado);


        fila1->addWidget(grupoConexion);

        grupoServo = new QGroupBox(centralwidget);
        grupoServo->setObjectName("grupoServo");
        grupoServo->setMaximumHeight(70);
        horizontalLayout_2 = new QHBoxLayout(grupoServo);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(-1, 0, -1, 0);
        deslizadorAngulo = new QSlider(grupoServo);
        deslizadorAngulo->setObjectName("deslizadorAngulo");
        deslizadorAngulo->setMaximum(180);
        deslizadorAngulo->setValue(90);
        deslizadorAngulo->setOrientation(Qt::Orientation::Horizontal);

        horizontalLayout_2->addWidget(deslizadorAngulo);

        etiquetaPedido = new QLabel(grupoServo);
        etiquetaPedido->setObjectName("etiquetaPedido");

        horizontalLayout_2->addWidget(etiquetaPedido);


        fila1->addWidget(grupoServo);


        verticalLayout_Principal->addLayout(fila1);

        fila2 = new QHBoxLayout();
        fila2->setObjectName("fila2");
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName("groupBox");
        gridLayout_4 = new QGridLayout(groupBox);
        gridLayout_4->setObjectName("gridLayout_4");
        gridLayout_4->setContentsMargins(-1, 2, -1, 2);
        btnCuchau = new QPushButton(groupBox);
        btnCuchau->setObjectName("btnCuchau");
        btnCuchau->setMinimumSize(QSize(0, 50));

        gridLayout_4->addWidget(btnCuchau, 0, 0, 1, 1);

        btnMiedo = new QPushButton(groupBox);
        btnMiedo->setObjectName("btnMiedo");
        btnMiedo->setMinimumSize(QSize(0, 50));

        gridLayout_4->addWidget(btnMiedo, 0, 1, 1, 1);

        btnNoNo = new QPushButton(groupBox);
        btnNoNo->setObjectName("btnNoNo");
        btnNoNo->setMinimumSize(QSize(0, 50));

        gridLayout_4->addWidget(btnNoNo, 1, 0, 1, 1);

        btnDireccionales = new QPushButton(groupBox);
        btnDireccionales->setObjectName("btnDireccionales");
        btnDireccionales->setMinimumSize(QSize(0, 50));

        gridLayout_4->addWidget(btnDireccionales, 1, 1, 1, 1);


        fila2->addWidget(groupBox);

        grupoMovimiento = new QGroupBox(centralwidget);
        grupoMovimiento->setObjectName("grupoMovimiento");
        gridLayout_2 = new QGridLayout(grupoMovimiento);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(-1, 2, -1, 2);
        pushButton_4 = new QPushButton(grupoMovimiento);
        pushButton_4->setObjectName("pushButton_4");
        pushButton_4->setMinimumSize(QSize(0, 50));

        gridLayout_2->addWidget(pushButton_4, 0, 1, 1, 1);

        pushButton_2 = new QPushButton(grupoMovimiento);
        pushButton_2->setObjectName("pushButton_2");
        pushButton_2->setMinimumSize(QSize(0, 50));

        gridLayout_2->addWidget(pushButton_2, 1, 0, 1, 1);

        pushButton = new QPushButton(grupoMovimiento);
        pushButton->setObjectName("pushButton");
        pushButton->setMinimumSize(QSize(0, 50));

        gridLayout_2->addWidget(pushButton, 1, 2, 1, 1);

        pushButton_3 = new QPushButton(grupoMovimiento);
        pushButton_3->setObjectName("pushButton_3");
        pushButton_3->setMinimumSize(QSize(0, 50));

        gridLayout_2->addWidget(pushButton_3, 2, 1, 1, 1);


        fila2->addWidget(grupoMovimiento);


        verticalLayout_Principal->addLayout(fila2);

        grupoDistancia = new QGroupBox(centralwidget);
        grupoDistancia->setObjectName("grupoDistancia");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(grupoDistancia->sizePolicy().hasHeightForWidth());
        grupoDistancia->setSizePolicy(sizePolicy);
        horizontalLayout_3 = new QHBoxLayout(grupoDistancia);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(-1, 0, -1, 0);
        graficaPot = new QChartView(grupoDistancia);
        graficaPot->setObjectName("graficaPot");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(3);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(graficaPot->sizePolicy().hasHeightForWidth());
        graficaPot->setSizePolicy(sizePolicy1);
        graficaPot->setMinimumSize(QSize(0, 100));

        horizontalLayout_3->addWidget(graficaPot);

        listaEventos = new QListWidget(grupoDistancia);
        listaEventos->setObjectName("listaEventos");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy2.setHorizontalStretch(1);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(listaEventos->sizePolicy().hasHeightForWidth());
        listaEventos->setSizePolicy(sizePolicy2);
        listaEventos->setMinimumSize(QSize(150, 0));

        horizontalLayout_3->addWidget(listaEventos);


        verticalLayout_Principal->addWidget(grupoDistancia);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "P3 - Aar\303\263n Emiliano Aguayo Casta\303\261eda 220530227", nullptr));
        grupoConexion->setTitle(QCoreApplication::translate("MainWindow", "Conexi\303\263n", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "IP:", nullptr));
        lineEditIp->setText(QCoreApplication::translate("MainWindow", "192.168.1.100", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Puerto:", nullptr));
        botonConectar->setText(QCoreApplication::translate("MainWindow", "Conectar", nullptr));
        etiquetaEstado->setText(QCoreApplication::translate("MainWindow", "Desconectado", nullptr));
        grupoServo->setTitle(QCoreApplication::translate("MainWindow", "Servo", nullptr));
        etiquetaPedido->setText(QCoreApplication::translate("MainWindow", "90\302\260", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "Emotes", nullptr));
        btnCuchau->setText(QCoreApplication::translate("MainWindow", "CUCHAU", nullptr));
        btnMiedo->setText(QCoreApplication::translate("MainWindow", "MIEDO", nullptr));
        btnNoNo->setText(QCoreApplication::translate("MainWindow", "NO-NO", nullptr));
        btnDireccionales->setText(QCoreApplication::translate("MainWindow", "DIRECCIONALES", nullptr));
        grupoMovimiento->setTitle(QCoreApplication::translate("MainWindow", "Movimiento", nullptr));
        pushButton_4->setText(QCoreApplication::translate("MainWindow", "Adelante", nullptr));
        pushButton_2->setText(QCoreApplication::translate("MainWindow", "Izquierda", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "Derecha", nullptr));
        pushButton_3->setText(QCoreApplication::translate("MainWindow", "Atr\303\241s", nullptr));
        grupoDistancia->setTitle(QCoreApplication::translate("MainWindow", "Distancia", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
