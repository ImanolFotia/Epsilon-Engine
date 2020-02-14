#include <QFileDialog>
#include <QFileSystemModel>
#include <QDir>
#include <ui_leveleditor.h>
#include <include/Log.hpp>
#include "include/leveleditor.h"



LevelEditor::LevelEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LevelEditor)
{
    ui->setupUi(this);
    ui->openGLWidget->mode = 2;
    Log::getInstance()->setUiReference(ui);
    QList<QString> entitiesList;
    entitiesList.push_back("INFO_SPAWNPOINT");
    entitiesList.push_back("INFO_SPOTLIGHT");
    entitiesList.push_back("INFO_POINTLIGHT");
    entitiesList.push_back("ENV_PCC");
    entitiesList.push_back("ENV_FOG");
    entitiesList.push_back("ENV_SUN");
    entitiesList.push_back("ENV_SMOKE");
    entitiesList.push_back("TRIGGER");
    entitiesList.push_back("ENV_AMBIENT_SOUND");
    entitiesList.push_back("ENV_OMNIDIRECTIONAL_SOUND");

    ui->listWidget_2->addItems(entitiesList);

    ui->property_tree->setHeaderLabels({tr("Property"), tr("Value")});

    ui->openGLWidget->PosXRef = ui->doubleSpinBox_5;
    ui->openGLWidget->PosYRef = ui->doubleSpinBox_4;
    ui->openGLWidget->PosZRef = ui->doubleSpinBox_6;

    ref = (double*) &ui->openGLWidget->selectID;

    QDir dir("C:/Users/Imanol/Documents/Code/C++/epsilon-engine/Epsilon Engine/bin/Release/models");
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setNameFilters(QStringList()<<"*.eml");
    dir.setSorting(QDir::Size | QDir::Reversed);
/*
    QFileInfoList list = dir.entryInfoList();
        for (int i = 0; i < list.size(); ++i) {
            QFileInfo fileInfo = list.at(i);
            ui->listView->addItem(fileInfo.fileName());
            std::cout << std::endl;
        }
        */
    QFileSystemModel* model = new QFileSystemModel(this);
    model->setRootPath("C:/Users/Imanol/Documents/Code/C++/epsilon-engine/Epsilon Engine/bin/Release/models");
    model->setReadOnly(false);
    model->setNameFilters(QStringList()<<"*.eml");
    model->setFilter(QDir::AllDirs | QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    model->sort(QDir::Name);
    ui->treeView->setModel(model);
    ui->treeView->sortByColumn(QDir::Name, Qt::SortOrder::AscendingOrder);
    ui->treeView->setRootIndex(model->index("C:/Users/Imanol/Documents/Code/C++/epsilon-engine/Epsilon Engine/bin/Release/models"));
    ui->treeView->setAnimated(false);
    ui->treeView->setIndentation(20);
    ui->treeView->setSortingEnabled(true);
    ui->treeView->show();

}

LevelEditor::~LevelEditor()
{
    if(ui->openGLWidget->BSP != nullptr){
        ui->openGLWidget->BSP->Destroy();
        delete ui->openGLWidget->BSP;
        ui->openGLWidget->BSP = nullptr;
    }
    delete ui;
}

void LevelEditor::on_actionExit_triggered()
{
    this->close();
}

void LevelEditor::on_actionLighting_triggered()
{
    ui->openGLWidget->mode = 1;
    std::cout << "mode 0" << std::endl;
}

void LevelEditor::on_actionTexture_triggered()
{
    ui->openGLWidget->mode = 0;
    std::cout << "mode 1" << std::endl;
}

void LevelEditor::on_actionTexture_Lighting_triggered()
{
    ui->openGLWidget->mode = 2;
    std::cout << "mode 2" << std::endl;
}

void LevelEditor::on_actionNormals_triggered()
{
    ui->openGLWidget->mode = 3;
    std::cout << "mode 3" << std::endl;
}

void LevelEditor::on_openGLWidget_frameSwapped()
{
}

void LevelEditor::on_pushButton_2_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Choose File"), "C:/Users/Imanol/Documents/Code/C++/epsilon-engine/Epsilon Engine/bin/Release/models/", "EML Files (*.eml)");
       // ui->listWidget->addItem(filename);
}


void LevelEditor::on_pushButton_3_clicked()
{
    QModelIndex index = ui->treeView->currentIndex();
    QFileSystemModel* model = static_cast<QFileSystemModel*>(ui->treeView->model());
    QVariant data = model->filePath(index);
    QString text = data.toString();
    ui->openGLWidget->shouldLoad(true, text, ui->doubleSpinBox_5->value(), ui->doubleSpinBox_4->value(), ui->doubleSpinBox_6->value(), ui->doubleSpinBox_8->value(), ui->doubleSpinBox_7->value(), ui->doubleSpinBox_9->value());
}


void LevelEditor::on_doubleSpinBox_5_valueChanged(double arg1)
{
    ui->openGLWidget->updatePositionX(arg1);
}

void LevelEditor::on_doubleSpinBox_4_valueChanged(double arg1)
{
    ui->openGLWidget->updatePositionY(arg1);
}

void LevelEditor::on_doubleSpinBox_6_valueChanged(double arg1)
{
    ui->openGLWidget->updatePositionZ(arg1);
}

void LevelEditor::on_pushButton_5_clicked()
{
     QString filename = QFileDialog::getOpenFileName(this, tr("Choose File"), "C:/Users/Imanol/Documents/Code/C++/epsilon-engine/Epsilon Engine/bin/Release/maps/", "BSP Files (*.bsp)");
     ui->plainTextEdit->setPlainText(filename);
}

void LevelEditor::on_pushButton_6_clicked()
{
    ui->openGLWidget->ShouldLoadBSP(ui->plainTextEdit->toPlainText());
}

void LevelEditor::on_listWidget_itemChanged(QListWidgetItem *item)
{
    //QListWidgetItem selected, last;
    //ui->listWidget->currentItemChanged(&selected, &last);
}


void LevelEditor::on_generate_terrain_btn_clicked()
{
    ui->openGLWidget->ShouldLoadTerrain(ui->TerrainSizespbox->value() , ui->Roughness_spbox->value(), ui->TerrainHeightspbox->value(), glm::vec3(ui->spinBox_13->value(), ui->spinBox_12->value(),ui->spinBox_11->value()) );
}

void LevelEditor::on_doubleSpinBox_8_valueChanged(double arg1)
{
    if(ui->openGLWidget->selectID >= 0)
    ui->openGLWidget->modv.at(ui->openGLWidget->selectID).Scale.x = arg1;
}

void LevelEditor::on_doubleSpinBox_7_valueChanged(double arg1)
{
    if(ui->openGLWidget->selectID >= 0)
    ui->openGLWidget->modv.at(ui->openGLWidget->selectID).Scale.y = arg1;
}

void LevelEditor::on_doubleSpinBox_9_valueChanged(double arg1)
{
    if(ui->openGLWidget->selectID >= 0)
    ui->openGLWidget->modv.at(ui->openGLWidget->selectID).Scale.z = arg1;
}

void LevelEditor::on_doubleSpinBox_2_valueChanged(double arg1)
{

}

void LevelEditor::on_pushButton_9_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Choose Image"), "C:/Users/Imanol/Documents/Code/C++/epsilon-engine/Epsilon Engine/bin/Release/materials");
    ui->plainTextEdit_3->setPlainText(filename);
    ui->openGLWidget->m_Terrain->setTexture(filename.toStdString());
}


void LevelEditor::on_horizontalSlider_valueChanged(int value)
{
    std::cout << value << std::endl;
    ui->openGLWidget->m_Terrain->setTextureRepeat(value);
}


void LevelEditor::on_actionOrthogonal_triggered()
{
    ui->openGLWidget->m_Camera->setOrthogonalProjection();
}

void LevelEditor::on_actionPerspective_triggered()
{
    ui->openGLWidget->m_Camera->setPerspectiveProjection();
}

void LevelEditor::on_actionTop_triggered()
{
    ui->openGLWidget->m_Camera->setDirection(glm::vec3(0.0001f, -0.99, 0.0001f));
}

void LevelEditor::on_actionBottom_triggered()
{
    ui->openGLWidget->m_Camera->setDirection(glm::vec3(0.0001f, 0.99, 0.0001f));
}

void LevelEditor::on_actionRight_triggered()
{
    ui->openGLWidget->m_Camera->setDirection(glm::vec3(-1.0, 0.0, 0.0));
}

void LevelEditor::on_actionLeft_triggered()
{
    ui->openGLWidget->m_Camera->setDirection(glm::vec3(1.0, 0.0, 0.0));
}

void LevelEditor::on_actionFront_triggered()
{
    ui->openGLWidget->m_Camera->setDirection(glm::vec3(0.0, 0.0, -1.0));
}

void LevelEditor::on_actionBack_triggered()
{
    ui->openGLWidget->m_Camera->setDirection(glm::vec3(0.0, 0.0, 1.0));
}

void LevelEditor::on_pushButton_released()
{
    ui->openGLWidget->removeCurrentModel();
}

