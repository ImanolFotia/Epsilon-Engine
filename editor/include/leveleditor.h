#ifndef LEVELEDITOR_H
#define LEVELEDITOR_H

#include <QMainWindow>
#include <include/Types.h>
#include <QListWidgetItem>

namespace Ui {
class LevelEditor;
}

class LevelEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit LevelEditor(QWidget *parent = 0);
    ~LevelEditor();

private slots:
    void on_actionExit_triggered();

    void on_actionLighting_triggered();

    void on_actionTexture_triggered();

    void on_actionTexture_Lighting_triggered();

    void on_actionNormals_triggered();

    void on_openGLWidget_frameSwapped();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_listWidget_itemChanged(QListWidgetItem *item);

    void on_pushButton_9_clicked();


    void on_generate_terrain_btn_clicked();

    void on_doubleSpinBox_8_valueChanged(double arg1);

    void on_doubleSpinBox_7_valueChanged(double arg1);

    void on_doubleSpinBox_9_valueChanged(double arg1);

    void on_doubleSpinBox_5_valueChanged(double arg1);

    void on_doubleSpinBox_4_valueChanged(double arg1);

    void on_doubleSpinBox_6_valueChanged(double arg1);

    void on_doubleSpinBox_2_valueChanged(double arg1);

    void on_horizontalSlider_valueChanged(int value);

    void on_actionOrthogonal_triggered();

    void on_actionPerspective_triggered();

    void on_actionTop_triggered();

    void on_actionBottom_triggered();

    void on_actionRight_triggered();

    void on_actionLeft_triggered();

    void on_actionFront_triggered();

    void on_actionBack_triggered();

    Ui::LevelEditor* getUI() {
        return ui;
    }

    void on_pushButton_released();

private:
    Ui::LevelEditor *ui;
    double* ref;
};

#endif // LEVELEDITOR_H
