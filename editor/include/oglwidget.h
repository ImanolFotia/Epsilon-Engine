#ifndef OGLWIDGET_H
#define OGLWIDGET_H


#include <QWidget>
#include <GL/glad.h>
#include <QOpenGLWidget>
#include <memory>
#include <windows.h>
#include <unordered_map>
#include "include/Shader.h"
#include "include/BSP.h"
#include <include/Model.h>
#include <include/terrain.h>
#include <include/camera.h>
#include <include/defaultgrid.h>
#include <include/picking.h>
#include <include/skybox.h>
#include <include/boundingbox.h>
#include <QDoubleSpinBox>
#include <include/Log.hpp>
#include <include/gizmos.hpp>

//class LevelEditor;

class OGLWidget : public QOpenGLWidget
{
public:
    OGLWidget(QWidget* parent = 0);
    ~OGLWidget(){
        _GizmoTranslate->Destroy();
    }

    void addModel(QString);
    void shouldLoad(bool, QString, float px, float py, float pz, float sx, float sy, float sz);
    void ShouldLoadBSP(QString);
    void ShouldLoadTerrain(int, double, double, glm::vec3);
    float t_s, t_r, t_h;
    float m_p, m_s, m_r;
    glm::vec3 t_p;
    int mode;
    bool tsl = false;
    std::shared_ptr<Model> mod;
    bool pause = false;

    void updatePositionX( double value);
    void updatePositionY( double value);
    void updatePositionZ( double value);

    void removeCurrentModel();

    std::vector<Model> modv;
    int selectID = -1;
    QDoubleSpinBox* PosXRef = nullptr;
    QDoubleSpinBox* PosYRef = nullptr;
    QDoubleSpinBox* PosZRef = nullptr;
    std::shared_ptr<Terrain> m_Terrain;

    CQuake3BSP* BSP = nullptr;
    std::shared_ptr<Camera> m_Camera;
    void resizeGL(int w, int h);
    void RenderQuad();
    GLuint SetupLut();
    /*void setUiRef(Ui_LevelEditor *ref) {
        UiReference = ref;
    }

    Ui_LevelEditor* getUiRef() {
        return UiReference;
    }*/
protected:
    void initializeGL();
    void paintGL();
private slots:
    void on_dial_actionTriggered(int action);

private:
    /*Ui_LevelEditor UiReference;*/
    std::shared_ptr<BoundingBox> m_BoundingBox;
    std::unordered_map<int, bool> KEYS;
    std::shared_ptr<OpenGL::Texture> LoadCubemap;
    GLuint quadVAO;
    GLuint brdfLUT;
    QTimer *timer;
    double time = 0.0;
    bool pick = false;
    GLuint VAO, VBO;
    Shader* m_Shader;
    Shader* m_TerrainShader;
    Shader* m_BoundingBoxShader;
    Shader* m_OutlineShader;
    Shader* GizmosShader = nullptr;
    Shader* brdfShader;
    Skybox* skybox;
    defaultGrid* grid;
    static float sc;
    bool LeftPressed = false;
    bool LeftRising = false;
    bool LeftFalling = false;
    bool RightPressed;
    bool MiddlePressed;
    bool should;
    bool BSPLoaded;
    bool loadBSP;
    bool somethingPicked = false;
    Ui::LevelEditor* UiReference;
    QString mname;
    QString BSPName;
    QPoint mousePos;
    glm::vec3 CurrentPos;
    glm::vec3 CurrentOrientation;
    bool CTRLPRessed = false;
    RayPick* ray;
    virtual void mousePressEvent(QMouseEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent * event);
    virtual void keyReleaseEvent(QKeyEvent * event);
    virtual void wheelEvent(QWheelEvent * event);
    std::shared_ptr<GizmoTranslate> _GizmoTranslate = nullptr;
    float zoomdelta;

    void testCollision();
    void LoadBSP();
    void LoadTerrain();
    void LoadModel();
    std::vector<ModelData> m_ModelData;
    /***/
    /***/
    float x = 1, y = 1;
    QPoint p;
    QPoint lastp;
    glm::vec3 currentMouse3DPosition;
    glm::vec3 lastMouse3DPosition;
    glm::vec3 selectedModelPosition;
    float mDistanceToObject = 0.0f;
    ACTION action;

    void PollEvents();
};

#endif // OGLWIDGET_H
