/********************************************************************************
** Form generated from reading UI file 'leveleditor.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LEVELEDITOR_H
#define UI_LEVELEDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "include/oglwidget.h"

QT_BEGIN_NAMESPACE

class Ui_LevelEditor
{
public:
    QAction *actionOpen_Scene;
    QAction *actionSave_Scene;
    QAction *actionSave_Acene_As;
    QAction *actionExit;
    QAction *actionCompile;
    QAction *actionCompile_and_Run;
    QAction *actionCompile_2;
    QAction *actionCompile_and_Run_2;
    QAction *actionSettings;
    QAction *actionNew_Scene;
    QAction *actionLighting;
    QAction *actionTexture;
    QAction *actionTexture_Lighting;
    QAction *actionNormals;
    QAction *actionOrthogonal;
    QAction *actionPerspective;
    QAction *actionTop;
    QAction *actionBottom;
    QAction *actionRight;
    QAction *actionLeft;
    QAction *actionFront;
    QAction *actionBack;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QTabWidget *tabWidget_3;
    QWidget *tab_5;
    QGridLayout *gridLayout_12;
    QSplitter *splitter_2;
    QWidget *gridWidget_2;
    QFormLayout *formLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout_2;
    QGroupBox *groupBox;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QDoubleSpinBox *doubleSpinBox;
    QDoubleSpinBox *doubleSpinBox_11;
    QDoubleSpinBox *doubleSpinBox_3;
    QDoubleSpinBox *doubleSpinBox_4;
    QDoubleSpinBox *doubleSpinBox_5;
    QDoubleSpinBox *doubleSpinBox_6;
    QDoubleSpinBox *doubleSpinBox_7;
    QDoubleSpinBox *doubleSpinBox_8;
    QDoubleSpinBox *doubleSpinBox_9;
    QDoubleSpinBox *doubleSpinBox_10;
    QGroupBox *groupBox_11;
    QVBoxLayout *verticalLayout;
    QTreeView *treeView;
    QPushButton *pushButton;
    QPushButton *pushButton_4;
    QPushButton *pushButton_3;
    QWidget *tab_2;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_13;
    QListWidget *listWidget_2;
    QWidget *tab_3;
    QGroupBox *groupBox_3;
    QPlainTextEdit *plainTextEdit;
    QPushButton *pushButton_5;
    QLabel *label_5;
    QPushButton *pushButton_6;
    QWidget *tab_4;
    QGroupBox *groupBox_5;
    QSpinBox *spinBox_11;
    QLabel *label_6;
    QSpinBox *spinBox_12;
    QSpinBox *spinBox_13;
    QLabel *label_7;
    QPlainTextEdit *plainTextEdit_2;
    QPushButton *pushButton_7;
    QPushButton *pushButton_8;
    QLabel *label_8;
    QSpinBox *TerrainSizespbox;
    QLabel *label_9;
    QLabel *label_10;
    QDoubleSpinBox *Roughness_spbox;
    QLabel *label_11;
    QDoubleSpinBox *TerrainHeightspbox;
    QFrame *line;
    QPushButton *generate_terrain_btn;
    QGroupBox *groupBox_6;
    QPlainTextEdit *plainTextEdit_3;
    QPushButton *pushButton_9;
    QLabel *label_12;
    QGroupBox *groupBox_7;
    QSlider *horizontalSlider;
    QLabel *label_13;
    QWidget *tab_9;
    QGridLayout *gridLayout_8;
    QGroupBox *groupBox_2;
    QGroupBox *groupBox_10;
    QWidget *tab_10;
    QGridLayout *gridLayout_9;
    QGroupBox *groupBox_12;
    QWidget *widget;
    QGridLayout *gridLayout_3;
    QSplitter *splitter;
    OGLWidget *openGLWidget;
    QTabWidget *tabWidget_2;
    QWidget *tab_7;
    QWidget *tab_8;
    QGridLayout *gridLayout_5;
    QTextEdit *textEdit;
    QPushButton *pushButton_2;
    QWidget *gridWidget;
    QGridLayout *gridLayout_6;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout_11;
    QGroupBox *groupBox_8;
    QGridLayout *gridLayout_4;
    QTreeWidget *EntityTree;
    QTreeWidget *property_tree;
    QWidget *tab_6;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuView;
    QMenu *menuRendering_Mode;
    QMenu *menuProjection;
    QMenu *menuCamera;
    QMenu *menuComplements;
    QMenu *menuHelp;
    QMenu *menuCompile;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *LevelEditor)
    {
        if (LevelEditor->objectName().isEmpty())
            LevelEditor->setObjectName(QString::fromUtf8("LevelEditor"));
        LevelEditor->setWindowModality(Qt::ApplicationModal);
        LevelEditor->resize(1886, 847);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(LevelEditor->sizePolicy().hasHeightForWidth());
        LevelEditor->setSizePolicy(sizePolicy);
        LevelEditor->setContextMenuPolicy(Qt::DefaultContextMenu);
        QIcon icon;
        icon.addFile(QString::fromUtf8("../media/epsilon.ico"), QSize(), QIcon::Normal, QIcon::Off);
        LevelEditor->setWindowIcon(icon);
        LevelEditor->setAutoFillBackground(true);
        LevelEditor->setTabShape(QTabWidget::Triangular);
        actionOpen_Scene = new QAction(LevelEditor);
        actionOpen_Scene->setObjectName(QString::fromUtf8("actionOpen_Scene"));
        QIcon icon1;
        QString iconThemeName = QString::fromUtf8("open");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon1 = QIcon::fromTheme(iconThemeName);
        } else {
            icon1.addFile(QString::fromUtf8("."), QSize(), QIcon::Normal, QIcon::Off);
        }
        actionOpen_Scene->setIcon(icon1);
        actionSave_Scene = new QAction(LevelEditor);
        actionSave_Scene->setObjectName(QString::fromUtf8("actionSave_Scene"));
        actionSave_Acene_As = new QAction(LevelEditor);
        actionSave_Acene_As->setObjectName(QString::fromUtf8("actionSave_Acene_As"));
        actionExit = new QAction(LevelEditor);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionCompile = new QAction(LevelEditor);
        actionCompile->setObjectName(QString::fromUtf8("actionCompile"));
        actionCompile_and_Run = new QAction(LevelEditor);
        actionCompile_and_Run->setObjectName(QString::fromUtf8("actionCompile_and_Run"));
        actionCompile_2 = new QAction(LevelEditor);
        actionCompile_2->setObjectName(QString::fromUtf8("actionCompile_2"));
        actionCompile_and_Run_2 = new QAction(LevelEditor);
        actionCompile_and_Run_2->setObjectName(QString::fromUtf8("actionCompile_and_Run_2"));
        actionSettings = new QAction(LevelEditor);
        actionSettings->setObjectName(QString::fromUtf8("actionSettings"));
        actionNew_Scene = new QAction(LevelEditor);
        actionNew_Scene->setObjectName(QString::fromUtf8("actionNew_Scene"));
        actionLighting = new QAction(LevelEditor);
        actionLighting->setObjectName(QString::fromUtf8("actionLighting"));
        actionTexture = new QAction(LevelEditor);
        actionTexture->setObjectName(QString::fromUtf8("actionTexture"));
        actionTexture_Lighting = new QAction(LevelEditor);
        actionTexture_Lighting->setObjectName(QString::fromUtf8("actionTexture_Lighting"));
        actionNormals = new QAction(LevelEditor);
        actionNormals->setObjectName(QString::fromUtf8("actionNormals"));
        actionOrthogonal = new QAction(LevelEditor);
        actionOrthogonal->setObjectName(QString::fromUtf8("actionOrthogonal"));
        actionPerspective = new QAction(LevelEditor);
        actionPerspective->setObjectName(QString::fromUtf8("actionPerspective"));
        actionTop = new QAction(LevelEditor);
        actionTop->setObjectName(QString::fromUtf8("actionTop"));
        actionBottom = new QAction(LevelEditor);
        actionBottom->setObjectName(QString::fromUtf8("actionBottom"));
        actionRight = new QAction(LevelEditor);
        actionRight->setObjectName(QString::fromUtf8("actionRight"));
        actionLeft = new QAction(LevelEditor);
        actionLeft->setObjectName(QString::fromUtf8("actionLeft"));
        actionFront = new QAction(LevelEditor);
        actionFront->setObjectName(QString::fromUtf8("actionFront"));
        actionBack = new QAction(LevelEditor);
        actionBack->setObjectName(QString::fromUtf8("actionBack"));
        centralWidget = new QWidget(LevelEditor);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        tabWidget_3 = new QTabWidget(centralWidget);
        tabWidget_3->setObjectName(QString::fromUtf8("tabWidget_3"));
        tab_5 = new QWidget();
        tab_5->setObjectName(QString::fromUtf8("tab_5"));
        gridLayout_12 = new QGridLayout(tab_5);
        gridLayout_12->setSpacing(6);
        gridLayout_12->setContentsMargins(11, 11, 11, 11);
        gridLayout_12->setObjectName(QString::fromUtf8("gridLayout_12"));
        splitter_2 = new QSplitter(tab_5);
        splitter_2->setObjectName(QString::fromUtf8("splitter_2"));
        sizePolicy.setHeightForWidth(splitter_2->sizePolicy().hasHeightForWidth());
        splitter_2->setSizePolicy(sizePolicy);
        splitter_2->setOrientation(Qt::Horizontal);
        gridWidget_2 = new QWidget(splitter_2);
        gridWidget_2->setObjectName(QString::fromUtf8("gridWidget_2"));
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(gridWidget_2->sizePolicy().hasHeightForWidth());
        gridWidget_2->setSizePolicy(sizePolicy1);
        gridWidget_2->setMinimumSize(QSize(300, 0));
        gridWidget_2->setMaximumSize(QSize(300, 16777215));
        gridWidget_2->setBaseSize(QSize(300, 0));
        formLayout = new QFormLayout(gridWidget_2);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setHorizontalSpacing(0);
        formLayout->setVerticalSpacing(0);
        formLayout->setContentsMargins(0, 0, 0, 0);
        tabWidget = new QTabWidget(gridWidget_2);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setEnabled(true);
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(1);
        sizePolicy2.setVerticalStretch(1);
        sizePolicy2.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy2);
        tabWidget->setMinimumSize(QSize(0, 0));
        tabWidget->setMaximumSize(QSize(400, 16777215));
        tabWidget->setSizeIncrement(QSize(0, 0));
        tabWidget->setBaseSize(QSize(300, 0));
        tabWidget->setLayoutDirection(Qt::LeftToRight);
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        sizePolicy.setHeightForWidth(tab->sizePolicy().hasHeightForWidth());
        tab->setSizePolicy(sizePolicy);
        tab->setMaximumSize(QSize(1600000, 16777215));
        tab->setSizeIncrement(QSize(250, 0));
        tab->setBaseSize(QSize(0, 0));
        gridLayout_2 = new QGridLayout(tab);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        groupBox = new QGroupBox(tab);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy3);
        groupBox->setMaximumSize(QSize(16777215, 200));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 40, 61, 16));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 90, 121, 16));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 140, 61, 16));
        doubleSpinBox = new QDoubleSpinBox(groupBox);
        doubleSpinBox->setObjectName(QString::fromUtf8("doubleSpinBox"));
        doubleSpinBox->setGeometry(QRect(10, 110, 51, 22));
        doubleSpinBox->setValue(1.000000000000000);
        doubleSpinBox_11 = new QDoubleSpinBox(groupBox);
        doubleSpinBox_11->setObjectName(QString::fromUtf8("doubleSpinBox_11"));
        doubleSpinBox_11->setGeometry(QRect(70, 110, 51, 22));
        doubleSpinBox_3 = new QDoubleSpinBox(groupBox);
        doubleSpinBox_3->setObjectName(QString::fromUtf8("doubleSpinBox_3"));
        doubleSpinBox_3->setGeometry(QRect(130, 110, 51, 22));
        doubleSpinBox_4 = new QDoubleSpinBox(groupBox);
        doubleSpinBox_4->setObjectName(QString::fromUtf8("doubleSpinBox_4"));
        doubleSpinBox_4->setGeometry(QRect(90, 60, 61, 22));
        doubleSpinBox_4->setDecimals(5);
        doubleSpinBox_4->setMinimum(-10000000000000000000000.000000000000000);
        doubleSpinBox_4->setMaximum(100000000000000004384584304507619735463404765184.000000000000000);
        doubleSpinBox_4->setSingleStep(0.100000000000000);
        doubleSpinBox_5 = new QDoubleSpinBox(groupBox);
        doubleSpinBox_5->setObjectName(QString::fromUtf8("doubleSpinBox_5"));
        doubleSpinBox_5->setGeometry(QRect(10, 60, 61, 22));
        doubleSpinBox_5->setDecimals(5);
        doubleSpinBox_5->setMinimum(-999999999000000.000000000000000);
        doubleSpinBox_5->setMaximum(9999999000000000.000000000000000);
        doubleSpinBox_5->setSingleStep(0.100000000000000);
        doubleSpinBox_6 = new QDoubleSpinBox(groupBox);
        doubleSpinBox_6->setObjectName(QString::fromUtf8("doubleSpinBox_6"));
        doubleSpinBox_6->setGeometry(QRect(170, 60, 61, 22));
        doubleSpinBox_6->setDecimals(5);
        doubleSpinBox_6->setMinimum(-99999999999999999322094867436162797646170844194406400.000000000000000);
        doubleSpinBox_6->setMaximum(999999999999999959416724456350362731491996089648451439669739009806703922950954425516032.000000000000000);
        doubleSpinBox_6->setSingleStep(0.100000000000000);
        doubleSpinBox_7 = new QDoubleSpinBox(groupBox);
        doubleSpinBox_7->setObjectName(QString::fromUtf8("doubleSpinBox_7"));
        doubleSpinBox_7->setGeometry(QRect(90, 160, 61, 22));
        doubleSpinBox_7->setSingleStep(0.100000000000000);
        doubleSpinBox_7->setValue(1.000000000000000);
        doubleSpinBox_8 = new QDoubleSpinBox(groupBox);
        doubleSpinBox_8->setObjectName(QString::fromUtf8("doubleSpinBox_8"));
        doubleSpinBox_8->setGeometry(QRect(10, 160, 61, 22));
        doubleSpinBox_8->setSingleStep(0.100000000000000);
        doubleSpinBox_8->setValue(1.000000000000000);
        doubleSpinBox_9 = new QDoubleSpinBox(groupBox);
        doubleSpinBox_9->setObjectName(QString::fromUtf8("doubleSpinBox_9"));
        doubleSpinBox_9->setGeometry(QRect(170, 160, 61, 22));
        doubleSpinBox_9->setSingleStep(0.100000000000000);
        doubleSpinBox_9->setValue(1.000000000000000);
        doubleSpinBox_10 = new QDoubleSpinBox(groupBox);
        doubleSpinBox_10->setObjectName(QString::fromUtf8("doubleSpinBox_10"));
        doubleSpinBox_10->setGeometry(QRect(190, 110, 51, 22));

        gridLayout_2->addWidget(groupBox, 1, 0, 1, 1);

        groupBox_11 = new QGroupBox(tab);
        groupBox_11->setObjectName(QString::fromUtf8("groupBox_11"));
        sizePolicy3.setHeightForWidth(groupBox_11->sizePolicy().hasHeightForWidth());
        groupBox_11->setSizePolicy(sizePolicy3);
        groupBox_11->setBaseSize(QSize(0, 200));
        verticalLayout = new QVBoxLayout(groupBox_11);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetMaximumSize);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        treeView = new QTreeView(groupBox_11);
        treeView->setObjectName(QString::fromUtf8("treeView"));
        treeView->setMaximumSize(QSize(16777215, 400));

        verticalLayout->addWidget(treeView);

        pushButton = new QPushButton(groupBox_11);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        verticalLayout->addWidget(pushButton);

        pushButton_4 = new QPushButton(groupBox_11);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));

        verticalLayout->addWidget(pushButton_4);

        pushButton_3 = new QPushButton(groupBox_11);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));

        verticalLayout->addWidget(pushButton_3);


        gridLayout_2->addWidget(groupBox_11, 0, 0, 1, 1);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        groupBox_4 = new QGroupBox(tab_2);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        groupBox_4->setGeometry(QRect(0, 0, 241, 341));
        gridLayout_13 = new QGridLayout(groupBox_4);
        gridLayout_13->setSpacing(6);
        gridLayout_13->setContentsMargins(11, 11, 11, 11);
        gridLayout_13->setObjectName(QString::fromUtf8("gridLayout_13"));
        listWidget_2 = new QListWidget(groupBox_4);
        listWidget_2->setObjectName(QString::fromUtf8("listWidget_2"));

        gridLayout_13->addWidget(listWidget_2, 0, 0, 1, 1);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        groupBox_3 = new QGroupBox(tab_3);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(0, 0, 231, 131));
        plainTextEdit = new QPlainTextEdit(groupBox_3);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(10, 60, 171, 21));
        plainTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        plainTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        plainTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
        plainTextEdit->setReadOnly(true);
        pushButton_5 = new QPushButton(groupBox_3);
        pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));
        pushButton_5->setGeometry(QRect(190, 60, 21, 23));
        label_5 = new QLabel(groupBox_3);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(10, 40, 91, 16));
        pushButton_6 = new QPushButton(groupBox_3);
        pushButton_6->setObjectName(QString::fromUtf8("pushButton_6"));
        pushButton_6->setGeometry(QRect(70, 90, 75, 23));
        tabWidget->addTab(tab_3, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        groupBox_5 = new QGroupBox(tab_4);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        groupBox_5->setGeometry(QRect(0, 10, 231, 361));
        spinBox_11 = new QSpinBox(groupBox_5);
        spinBox_11->setObjectName(QString::fromUtf8("spinBox_11"));
        spinBox_11->setGeometry(QRect(110, 70, 42, 22));
        spinBox_11->setMinimum(-64000);
        spinBox_11->setMaximum(64000);
        label_6 = new QLabel(groupBox_5);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(10, 50, 61, 16));
        spinBox_12 = new QSpinBox(groupBox_5);
        spinBox_12->setObjectName(QString::fromUtf8("spinBox_12"));
        spinBox_12->setGeometry(QRect(60, 70, 42, 22));
        spinBox_12->setMinimum(-64000);
        spinBox_12->setMaximum(64000);
        spinBox_13 = new QSpinBox(groupBox_5);
        spinBox_13->setObjectName(QString::fromUtf8("spinBox_13"));
        spinBox_13->setGeometry(QRect(10, 70, 42, 22));
        spinBox_13->setMinimum(-64000);
        spinBox_13->setMaximum(64000);
        label_7 = new QLabel(groupBox_5);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(10, 100, 121, 16));
        plainTextEdit_2 = new QPlainTextEdit(groupBox_5);
        plainTextEdit_2->setObjectName(QString::fromUtf8("plainTextEdit_2"));
        plainTextEdit_2->setGeometry(QRect(10, 120, 171, 25));
        pushButton_7 = new QPushButton(groupBox_5);
        pushButton_7->setObjectName(QString::fromUtf8("pushButton_7"));
        pushButton_7->setGeometry(QRect(190, 122, 31, 31));
        pushButton_8 = new QPushButton(groupBox_5);
        pushButton_8->setObjectName(QString::fromUtf8("pushButton_8"));
        pushButton_8->setGeometry(QRect(70, 160, 75, 23));
        label_8 = new QLabel(groupBox_5);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(10, 200, 121, 16));
        TerrainSizespbox = new QSpinBox(groupBox_5);
        TerrainSizespbox->setObjectName(QString::fromUtf8("TerrainSizespbox"));
        TerrainSizespbox->setGeometry(QRect(121, 220, 91, 22));
        TerrainSizespbox->setMaximum(1000000000);
        TerrainSizespbox->setValue(128);
        label_9 = new QLabel(groupBox_5);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(10, 221, 47, 20));
        label_10 = new QLabel(groupBox_5);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(10, 250, 61, 21));
        Roughness_spbox = new QDoubleSpinBox(groupBox_5);
        Roughness_spbox->setObjectName(QString::fromUtf8("Roughness_spbox"));
        Roughness_spbox->setGeometry(QRect(121, 250, 91, 22));
        Roughness_spbox->setDecimals(7);
        Roughness_spbox->setValue(0.010000000000000);
        label_11 = new QLabel(groupBox_5);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(10, 280, 47, 13));
        TerrainHeightspbox = new QDoubleSpinBox(groupBox_5);
        TerrainHeightspbox->setObjectName(QString::fromUtf8("TerrainHeightspbox"));
        TerrainHeightspbox->setGeometry(QRect(120, 280, 91, 22));
        TerrainHeightspbox->setMaximum(1000000.000000000000000);
        TerrainHeightspbox->setValue(125.000000000000000);
        line = new QFrame(groupBox_5);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(10, 179, 201, 21));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        generate_terrain_btn = new QPushButton(groupBox_5);
        generate_terrain_btn->setObjectName(QString::fromUtf8("generate_terrain_btn"));
        generate_terrain_btn->setGeometry(QRect(70, 320, 75, 23));
        groupBox_6 = new QGroupBox(tab_4);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        groupBox_6->setGeometry(QRect(0, 480, 231, 161));
        plainTextEdit_3 = new QPlainTextEdit(groupBox_6);
        plainTextEdit_3->setObjectName(QString::fromUtf8("plainTextEdit_3"));
        plainTextEdit_3->setGeometry(QRect(10, 50, 161, 31));
        pushButton_9 = new QPushButton(groupBox_6);
        pushButton_9->setObjectName(QString::fromUtf8("pushButton_9"));
        pushButton_9->setGeometry(QRect(180, 50, 31, 31));
        label_12 = new QLabel(groupBox_6);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(10, 30, 81, 16));
        groupBox_7 = new QGroupBox(tab_4);
        groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
        groupBox_7->setGeometry(QRect(0, 379, 231, 91));
        horizontalSlider = new QSlider(groupBox_7);
        horizontalSlider->setObjectName(QString::fromUtf8("horizontalSlider"));
        horizontalSlider->setGeometry(QRect(10, 50, 211, 22));
        horizontalSlider->setMinimum(1);
        horizontalSlider->setOrientation(Qt::Horizontal);
        label_13 = new QLabel(groupBox_7);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setGeometry(QRect(10, 30, 91, 16));
        tabWidget->addTab(tab_4, QString());
        tab_9 = new QWidget();
        tab_9->setObjectName(QString::fromUtf8("tab_9"));
        gridLayout_8 = new QGridLayout(tab_9);
        gridLayout_8->setSpacing(6);
        gridLayout_8->setContentsMargins(11, 11, 11, 11);
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        groupBox_2 = new QGroupBox(tab_9);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));

        gridLayout_8->addWidget(groupBox_2, 0, 0, 1, 1);

        groupBox_10 = new QGroupBox(tab_9);
        groupBox_10->setObjectName(QString::fromUtf8("groupBox_10"));

        gridLayout_8->addWidget(groupBox_10, 1, 0, 1, 1);

        tabWidget->addTab(tab_9, QString());
        tab_10 = new QWidget();
        tab_10->setObjectName(QString::fromUtf8("tab_10"));
        gridLayout_9 = new QGridLayout(tab_10);
        gridLayout_9->setSpacing(6);
        gridLayout_9->setContentsMargins(11, 11, 11, 11);
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        groupBox_12 = new QGroupBox(tab_10);
        groupBox_12->setObjectName(QString::fromUtf8("groupBox_12"));

        gridLayout_9->addWidget(groupBox_12, 0, 0, 1, 1);

        tabWidget->addTab(tab_10, QString());

        formLayout->setWidget(0, QFormLayout::SpanningRole, tabWidget);

        splitter_2->addWidget(gridWidget_2);
        widget = new QWidget(splitter_2);
        widget->setObjectName(QString::fromUtf8("widget"));
        sizePolicy3.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy3);
        widget->setBaseSize(QSize(1500, 0));
        gridLayout_3 = new QGridLayout(widget);
        gridLayout_3->setSpacing(0);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setContentsMargins(0, 0, 0, 0);
        splitter = new QSplitter(widget);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(splitter->sizePolicy().hasHeightForWidth());
        splitter->setSizePolicy(sizePolicy4);
        splitter->setBaseSize(QSize(700, 0));
        splitter->setOrientation(Qt::Vertical);
        openGLWidget = new OGLWidget(splitter);
        openGLWidget->setObjectName(QString::fromUtf8("openGLWidget"));
        QSizePolicy sizePolicy5(QSizePolicy::Expanding, QSizePolicy::Maximum);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(openGLWidget->sizePolicy().hasHeightForWidth());
        openGLWidget->setSizePolicy(sizePolicy5);
        openGLWidget->setMinimumSize(QSize(0, 0));
        openGLWidget->setMaximumSize(QSize(16000000, 16000000));
        openGLWidget->setBaseSize(QSize(1200, 534));
        openGLWidget->setCursor(QCursor(Qt::ArrowCursor));
        openGLWidget->setMouseTracking(true);
        openGLWidget->setFocusPolicy(Qt::StrongFocus);
        openGLWidget->setAutoFillBackground(true);
        splitter->addWidget(openGLWidget);
        tabWidget_2 = new QTabWidget(splitter);
        tabWidget_2->setObjectName(QString::fromUtf8("tabWidget_2"));
        QSizePolicy sizePolicy6(QSizePolicy::Maximum, QSizePolicy::Minimum);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(tabWidget_2->sizePolicy().hasHeightForWidth());
        tabWidget_2->setSizePolicy(sizePolicy6);
        tabWidget_2->setMinimumSize(QSize(0, 1));
        tabWidget_2->setMaximumSize(QSize(16777215, 200));
        tabWidget_2->setBaseSize(QSize(0, 0));
        tab_7 = new QWidget();
        tab_7->setObjectName(QString::fromUtf8("tab_7"));
        QSizePolicy sizePolicy7(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(tab_7->sizePolicy().hasHeightForWidth());
        tab_7->setSizePolicy(sizePolicy7);
        tabWidget_2->addTab(tab_7, QString());
        tab_8 = new QWidget();
        tab_8->setObjectName(QString::fromUtf8("tab_8"));
        sizePolicy7.setHeightForWidth(tab_8->sizePolicy().hasHeightForWidth());
        tab_8->setSizePolicy(sizePolicy7);
        gridLayout_5 = new QGridLayout(tab_8);
        gridLayout_5->setSpacing(0);
        gridLayout_5->setContentsMargins(11, 11, 11, 11);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        gridLayout_5->setContentsMargins(0, 0, 0, 0);
        textEdit = new QTextEdit(tab_8);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        QSizePolicy sizePolicy8(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy8.setHorizontalStretch(0);
        sizePolicy8.setVerticalStretch(0);
        sizePolicy8.setHeightForWidth(textEdit->sizePolicy().hasHeightForWidth());
        textEdit->setSizePolicy(sizePolicy8);
        textEdit->setReadOnly(true);
        textEdit->setTextInteractionFlags(Qt::NoTextInteraction);

        gridLayout_5->addWidget(textEdit, 0, 0, 1, 1);

        tabWidget_2->addTab(tab_8, QString());
        splitter->addWidget(tabWidget_2);

        gridLayout_3->addWidget(splitter, 1, 0, 1, 1);

        pushButton_2 = new QPushButton(widget);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        sizePolicy8.setHeightForWidth(pushButton_2->sizePolicy().hasHeightForWidth());
        pushButton_2->setSizePolicy(sizePolicy8);
        pushButton_2->setMaximumSize(QSize(30, 30));

        gridLayout_3->addWidget(pushButton_2, 0, 0, 1, 1);

        splitter_2->addWidget(widget);
        gridWidget = new QWidget(splitter_2);
        gridWidget->setObjectName(QString::fromUtf8("gridWidget"));
        gridWidget->setMaximumSize(QSize(280, 16777215));
        gridLayout_6 = new QGridLayout(gridWidget);
        gridLayout_6->setSpacing(0);
        gridLayout_6->setContentsMargins(11, 11, 11, 11);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        gridLayout_6->setSizeConstraint(QLayout::SetDefaultConstraint);
        gridLayout_6->setContentsMargins(0, 0, 0, 0);
        scrollArea = new QScrollArea(gridWidget);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        sizePolicy.setHeightForWidth(scrollArea->sizePolicy().hasHeightForWidth());
        scrollArea->setSizePolicy(sizePolicy);
        scrollArea->setMinimumSize(QSize(151, 0));
        scrollArea->setMaximumSize(QSize(270, 16777215));
        scrollArea->setBaseSize(QSize(200, 0));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 268, 728));
        QSizePolicy sizePolicy9(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy9.setHorizontalStretch(0);
        sizePolicy9.setVerticalStretch(0);
        sizePolicy9.setHeightForWidth(scrollAreaWidgetContents->sizePolicy().hasHeightForWidth());
        scrollAreaWidgetContents->setSizePolicy(sizePolicy9);
        scrollAreaWidgetContents->setBaseSize(QSize(200, 0));
        gridLayout_11 = new QGridLayout(scrollAreaWidgetContents);
        gridLayout_11->setSpacing(6);
        gridLayout_11->setContentsMargins(11, 11, 11, 11);
        gridLayout_11->setObjectName(QString::fromUtf8("gridLayout_11"));
        groupBox_8 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_8->setObjectName(QString::fromUtf8("groupBox_8"));
        sizePolicy9.setHeightForWidth(groupBox_8->sizePolicy().hasHeightForWidth());
        groupBox_8->setSizePolicy(sizePolicy9);
        groupBox_8->setMinimumSize(QSize(0, 200));
        groupBox_8->setMaximumSize(QSize(245, 16777215));
        groupBox_8->setBaseSize(QSize(200, 0));
        gridLayout_4 = new QGridLayout(groupBox_8);
        gridLayout_4->setSpacing(0);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        gridLayout_4->setContentsMargins(0, 0, 0, 0);
        EntityTree = new QTreeWidget(groupBox_8);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        EntityTree->setHeaderItem(__qtreewidgetitem);
        EntityTree->setObjectName(QString::fromUtf8("EntityTree"));

        gridLayout_4->addWidget(EntityTree, 0, 0, 1, 1);

        property_tree = new QTreeWidget(groupBox_8);
        QTreeWidgetItem *__qtreewidgetitem1 = new QTreeWidgetItem();
        __qtreewidgetitem1->setText(0, QString::fromUtf8("1"));
        property_tree->setHeaderItem(__qtreewidgetitem1);
        property_tree->setObjectName(QString::fromUtf8("property_tree"));

        gridLayout_4->addWidget(property_tree, 1, 0, 1, 1);


        gridLayout_11->addWidget(groupBox_8, 0, 0, 1, 1);

        scrollArea->setWidget(scrollAreaWidgetContents);

        gridLayout_6->addWidget(scrollArea, 0, 0, 1, 1);

        splitter_2->addWidget(gridWidget);

        gridLayout_12->addWidget(splitter_2, 0, 0, 1, 1);

        tabWidget_3->addTab(tab_5, QString());
        tab_6 = new QWidget();
        tab_6->setObjectName(QString::fromUtf8("tab_6"));
        tabWidget_3->addTab(tab_6, QString());

        gridLayout->addWidget(tabWidget_3, 0, 0, 1, 2);

        LevelEditor->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(LevelEditor);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1886, 22));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuEdit = new QMenu(menuBar);
        menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        menuRendering_Mode = new QMenu(menuView);
        menuRendering_Mode->setObjectName(QString::fromUtf8("menuRendering_Mode"));
        menuProjection = new QMenu(menuView);
        menuProjection->setObjectName(QString::fromUtf8("menuProjection"));
        menuCamera = new QMenu(menuView);
        menuCamera->setObjectName(QString::fromUtf8("menuCamera"));
        menuComplements = new QMenu(menuBar);
        menuComplements->setObjectName(QString::fromUtf8("menuComplements"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuCompile = new QMenu(menuBar);
        menuCompile->setObjectName(QString::fromUtf8("menuCompile"));
        LevelEditor->setMenuBar(menuBar);
        mainToolBar = new QToolBar(LevelEditor);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        LevelEditor->addToolBar(Qt::BottomToolBarArea, mainToolBar);
        statusBar = new QStatusBar(LevelEditor);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        LevelEditor->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuEdit->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuCompile->menuAction());
        menuBar->addAction(menuComplements->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionNew_Scene);
        menuFile->addAction(actionOpen_Scene);
        menuFile->addAction(actionSave_Scene);
        menuFile->addAction(actionSave_Acene_As);
        menuFile->addAction(actionExit);
        menuView->addAction(menuRendering_Mode->menuAction());
        menuView->addAction(menuProjection->menuAction());
        menuView->addAction(menuCamera->menuAction());
        menuRendering_Mode->addAction(actionLighting);
        menuRendering_Mode->addAction(actionTexture);
        menuRendering_Mode->addAction(actionTexture_Lighting);
        menuRendering_Mode->addAction(actionNormals);
        menuProjection->addAction(actionOrthogonal);
        menuProjection->addAction(actionPerspective);
        menuCamera->addAction(actionTop);
        menuCamera->addAction(actionBottom);
        menuCamera->addAction(actionRight);
        menuCamera->addAction(actionLeft);
        menuCamera->addAction(actionFront);
        menuCamera->addAction(actionBack);
        menuCompile->addAction(actionCompile_2);
        menuCompile->addAction(actionCompile_and_Run_2);
        menuCompile->addAction(actionSettings);

        retranslateUi(LevelEditor);
        QObject::connect(treeView, SIGNAL(doubleClicked(QModelIndex)), LevelEditor, SLOT(raise()));

        tabWidget_3->setCurrentIndex(0);
        tabWidget->setCurrentIndex(0);
        tabWidget_2->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(LevelEditor);
    } // setupUi

    void retranslateUi(QMainWindow *LevelEditor)
    {
        LevelEditor->setWindowTitle(QCoreApplication::translate("LevelEditor", "LevelEditor", nullptr));
        actionOpen_Scene->setText(QCoreApplication::translate("LevelEditor", "Open Scene", nullptr));
        actionSave_Scene->setText(QCoreApplication::translate("LevelEditor", "Save Scene", nullptr));
        actionSave_Acene_As->setText(QCoreApplication::translate("LevelEditor", "Save Scene As...", nullptr));
        actionExit->setText(QCoreApplication::translate("LevelEditor", "Exit", nullptr));
        actionCompile->setText(QCoreApplication::translate("LevelEditor", "Compile", nullptr));
        actionCompile_and_Run->setText(QCoreApplication::translate("LevelEditor", "Compile and Run", nullptr));
        actionCompile_2->setText(QCoreApplication::translate("LevelEditor", "Compile", nullptr));
        actionCompile_and_Run_2->setText(QCoreApplication::translate("LevelEditor", "Compile and Run", nullptr));
        actionSettings->setText(QCoreApplication::translate("LevelEditor", "Settings", nullptr));
        actionNew_Scene->setText(QCoreApplication::translate("LevelEditor", "New Scene", nullptr));
        actionLighting->setText(QCoreApplication::translate("LevelEditor", "Lighting", nullptr));
        actionTexture->setText(QCoreApplication::translate("LevelEditor", "Texture", nullptr));
        actionTexture_Lighting->setText(QCoreApplication::translate("LevelEditor", "Texture + Lighting", nullptr));
        actionNormals->setText(QCoreApplication::translate("LevelEditor", "Normals", nullptr));
        actionOrthogonal->setText(QCoreApplication::translate("LevelEditor", "Orthogonal", nullptr));
        actionPerspective->setText(QCoreApplication::translate("LevelEditor", "Perspective", nullptr));
        actionTop->setText(QCoreApplication::translate("LevelEditor", "Top", nullptr));
        actionBottom->setText(QCoreApplication::translate("LevelEditor", "Bottom", nullptr));
        actionRight->setText(QCoreApplication::translate("LevelEditor", "Right", nullptr));
        actionLeft->setText(QCoreApplication::translate("LevelEditor", "Left", nullptr));
        actionFront->setText(QCoreApplication::translate("LevelEditor", "Front", nullptr));
        actionBack->setText(QCoreApplication::translate("LevelEditor", "Back", nullptr));
        groupBox->setTitle(QCoreApplication::translate("LevelEditor", "Transformations", nullptr));
        label->setText(QCoreApplication::translate("LevelEditor", "Translation", nullptr));
        label_2->setText(QCoreApplication::translate("LevelEditor", "Rotation (Quaternion)", nullptr));
        label_3->setText(QCoreApplication::translate("LevelEditor", "Scale", nullptr));
        groupBox_11->setTitle(QCoreApplication::translate("LevelEditor", "GroupBox", nullptr));
        pushButton->setText(QCoreApplication::translate("LevelEditor", "Remove", nullptr));
        pushButton_4->setText(QCoreApplication::translate("LevelEditor", "Visualize", nullptr));
        pushButton_3->setText(QCoreApplication::translate("LevelEditor", "Add To scene", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("LevelEditor", "Models", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("LevelEditor", "Entities List", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("LevelEditor", "Entities", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("LevelEditor", "BSP", nullptr));
        pushButton_5->setText(QCoreApplication::translate("LevelEditor", "...", nullptr));
        label_5->setText(QCoreApplication::translate("LevelEditor", "Select BSP file", nullptr));
        pushButton_6->setText(QCoreApplication::translate("LevelEditor", "Load", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QCoreApplication::translate("LevelEditor", "Scene", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("LevelEditor", "Properties", nullptr));
        label_6->setText(QCoreApplication::translate("LevelEditor", "Translation", nullptr));
        label_7->setText(QCoreApplication::translate("LevelEditor", "Load From Hight Map", nullptr));
        pushButton_7->setText(QCoreApplication::translate("LevelEditor", "...", nullptr));
        pushButton_8->setText(QCoreApplication::translate("LevelEditor", "Load", nullptr));
        label_8->setText(QCoreApplication::translate("LevelEditor", "Generate Procedural", nullptr));
        label_9->setText(QCoreApplication::translate("LevelEditor", "Size", nullptr));
        label_10->setText(QCoreApplication::translate("LevelEditor", "Roughness", nullptr));
        label_11->setText(QCoreApplication::translate("LevelEditor", "Height", nullptr));
        generate_terrain_btn->setText(QCoreApplication::translate("LevelEditor", "Generate", nullptr));
        groupBox_6->setTitle(QCoreApplication::translate("LevelEditor", "Textures", nullptr));
        pushButton_9->setText(QCoreApplication::translate("LevelEditor", "...", nullptr));
        label_12->setText(QCoreApplication::translate("LevelEditor", "Base Color", nullptr));
        groupBox_7->setTitle(QCoreApplication::translate("LevelEditor", "Tweak", nullptr));
        label_13->setText(QCoreApplication::translate("LevelEditor", "Texture repeat", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QCoreApplication::translate("LevelEditor", "Terrain", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("LevelEditor", "Physics", nullptr));
        groupBox_10->setTitle(QCoreApplication::translate("LevelEditor", "Sky", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_9), QCoreApplication::translate("LevelEditor", "World", nullptr));
        groupBox_12->setTitle(QCoreApplication::translate("LevelEditor", "Sky Settings", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_10), QCoreApplication::translate("LevelEditor", "Sky", nullptr));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_7), QCoreApplication::translate("LevelEditor", "Resources", nullptr));
        textEdit->setHtml(QCoreApplication::translate("LevelEditor", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'MS Shell Dlg 2';\"><br /></p></body></html>", nullptr));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_8), QCoreApplication::translate("LevelEditor", "Output", nullptr));
        pushButton_2->setText(QCoreApplication::translate("LevelEditor", "Run", nullptr));
        groupBox_8->setTitle(QCoreApplication::translate("LevelEditor", "Scene", nullptr));
        tabWidget_3->setTabText(tabWidget_3->indexOf(tab_5), QCoreApplication::translate("LevelEditor", "Scene Editor", nullptr));
        tabWidget_3->setTabText(tabWidget_3->indexOf(tab_6), QCoreApplication::translate("LevelEditor", "Scripting", nullptr));
        menuFile->setTitle(QCoreApplication::translate("LevelEditor", "File", nullptr));
        menuEdit->setTitle(QCoreApplication::translate("LevelEditor", "Edit", nullptr));
        menuView->setTitle(QCoreApplication::translate("LevelEditor", "View", nullptr));
        menuRendering_Mode->setTitle(QCoreApplication::translate("LevelEditor", "Rendering Mode", nullptr));
        menuProjection->setTitle(QCoreApplication::translate("LevelEditor", "Projection", nullptr));
        menuCamera->setTitle(QCoreApplication::translate("LevelEditor", "Camera", nullptr));
        menuComplements->setTitle(QCoreApplication::translate("LevelEditor", "Complements", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("LevelEditor", "Help", nullptr));
        menuCompile->setTitle(QCoreApplication::translate("LevelEditor", "Compile", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LevelEditor: public Ui_LevelEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LEVELEDITOR_H
