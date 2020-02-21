#include "include/oglwidget.h"
#include <QTimer>
#include <QEvent>
#include <QMouseEvent>
#include <iostream>
#include "include/cpu.hpp"
#include <functional>
#include <qopenglcontext.h>
#include <include/texture.hpp>
#include <include/leveleditor.h>

extern "C" {
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

OGLWidget::OGLWidget(QWidget* parent) : QOpenGLWidget(parent)
{
    //std::cout << "llega" << std::endl;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1);

    QSurfaceFormat format = this->format();
    format.setSamples(8);
    //format.setOption(QSurfaceFormat::FormatOption::):
    format.setAlphaBufferSize(0);
    this->setFormat(format);

    this->setMouseTracking(true);

    this->CurrentPos = glm::vec3(78.0, 4.0, -60.0);
    this->CurrentOrientation = glm::vec3(78.9,4.75,-64.0);
    this->LeftPressed = false;
    this->RightPressed = false;
    this->should = false;

    x = 0;
    y = 0;

    for(int i = 0;i < 255; i++)
    {
        KEYS[Qt::Key_0 + i] = false;
    }

    /*Log::setUiReference(getUiRef());*/

}

void OGLWidget::initializeGL()
{

    if (!gladLoadGL())
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return;
    }
    glClearColor(1,1,1,1);

    {
        glEnable(GL_DEPTH_TEST);

        glDepthFunc(GL_LESS);

        glEnable(GL_MULTISAMPLE);

        glEnable(GL_CULL_FACE);

        glEnable(GL_BLEND);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDepthFunc(GL_LESS);

        glEnable(GL_STENCIL_TEST);

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
     glLineWidth(2.0);
    loadBSP = false;
    BSPLoaded = false;

    PollEvents();

    m_Camera = (std::shared_ptr<Camera>) new Camera(glm::vec3(-6.43815, 5.03705, -6.40388), glm::vec3(0.599493, -0.49688, 0.62747), this->width(), this->height());
    action = (decltype(action))-1;
    m_Camera->Update(x, y, time, action);
    m_Camera->ZoomDelta = zoomdelta;
    zoomdelta = .0f;
    m_Camera->UpdateMatrices();

    // m_Camera->setDirection(glm::vec3(-0.551257,-0.648795,-0.524576));

    this->BSP = nullptr;
    _GizmoTranslate = std::make_shared<GizmoTranslate>();
    _GizmoTranslate->Setup();
    m_Shader = new Shader("../assets/shaders/vertex.glsl", "../assets/shaders/fragment.glsl");
    m_TerrainShader = new Shader("../assets/shaders/terrainvertex.glsl", "../assets/shaders/terrainfragment.glsl");
    m_BoundingBoxShader = new Shader("../assets/shaders/vertex.glsl", "../assets/shaders/fragment_BB.glsl");
    m_OutlineShader = new Shader("../assets/shaders/vertex.glsl", "../assets/shaders/OutLineFragment.glsl");
    brdfShader = new Shader("../assets/shaders/brdfvertex.glsl", "../assets/shaders/BRDF.glsl");
    GizmosShader = new Shader("../assets/shaders/Gizmos_v.glsl", "../assets/shaders/Gizmos.glsl");
    this->modv.push_back(Model("shape_mat.eml", glm::vec3(0.0,0.0,0.0), glm::vec3(1), glm::quat(1.0, 0.0, 0.0, 0.0)));
    //this->m_Terrain = (std::shared_ptr<Terrain>)  new Terrain(128, 0.05, 128, glm::vec3(0,0,0));

    this->grid = new defaultGrid();

    std::cout << "Llega" << std::endl;
    skybox = new Skybox();

    ray = new RayPick();

    m_BoundingBox = (std::shared_ptr<BoundingBox>) new BoundingBox();
    //m_Camera->setDirection(glm::vec3(-0.551257,-0.648795,-0.524576));
    pick = false;

    std::vector<std::string> texPaths;
    texPaths.push_back("../assets/textures/hdri/px.png");
    texPaths.push_back("../assets/textures/hdri/nx.png");
    texPaths.push_back("../assets/textures/hdri/py.png");
    texPaths.push_back("../assets/textures/hdri/ny.png");
    texPaths.push_back("../assets/textures/hdri/pz.png");
    texPaths.push_back("../assets/textures/hdri/nz.png");

    LoadCubemap = std::make_shared<OpenGL::Texture>();

    glDisable(GL_BLEND);
    brdfLUT = SetupLut();
    glEnable(GL_BLEND);

    LoadCubemap->LoadCubemap(texPaths);

    this->UiReference = Log::getInstance()->getUiReference();


}

void OGLWidget::resizeGL(int w, int h){
    std::cout << "Resized: " << w << "x" << h << std::endl;
    this->m_Camera->resize(w, h);
}

void OGLWidget::paintGL()
{
    glStencilMask(0x00);
    //std::cout << GetCPULoad() << std::endl;
    //setFocus();
    glClearColor(1,1,1,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    time += 0.16;
    PollEvents();

    m_Camera->Update(x, y, time, action);
    m_Camera->ZoomDelta = zoomdelta;
    //std::cout << m_Camera->getPosition().x << " " << m_Camera->getPosition().y << " " << m_Camera->getPosition().z << std::endl;
    zoomdelta = .0f;
    m_Camera->UpdateMatrices();

    //m_Camera->setDirection(glm::vec3(-0.551257,-0.648795,-0.524576));

    glDisable(GL_BLEND);
    skybox->Render(m_Camera->getViewMatrix(), m_Camera->getProjectionMatrix(), time);


    this->LoadModel();

    if(loadBSP)
        this->LoadBSP();
    if(tsl)
        this->LoadTerrain();

    glm::mat4 view = m_Camera->getViewMatrix();
    glm::mat4 proj = m_Camera->getProjectionMatrix();
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,0)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
    glm::mat4 MVP = proj * view * model;


    this->m_Shader->Use();
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,0)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
    MVP = proj * view * model;
    glUniformMatrix4fv(glGetUniformLocation(this->m_Shader->getProgramID(), "MVP"), 1, GL_FALSE, &MVP[0][0]);

    glUniform1i(glGetUniformLocation(this->m_OutlineShader->getProgramID(), "StencilPass"), 0);
    glUniform1i(glGetUniformLocation(this->m_Shader->getProgramID(), "Picked"), 0);
    glUniform1i(glGetUniformLocation(this->m_Shader->getProgramID(), "isLine"), 1);
    grid->Render();

    glEnable(GL_BLEND);
    this->m_Shader->Free();
    this->m_Shader->Use();
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,0)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
    MVP = proj * view * model;
    glUniform1i(glGetUniformLocation(this->m_Shader->getProgramID(), "isLine"), 0);
    glUniformMatrix4fv(glGetUniformLocation(this->m_Shader->getProgramID(), "MVP"), 1, GL_FALSE, &MVP[0][0]);
    glUniform1i(glGetUniformLocation(this->m_Shader->getProgramID(), "mode"), mode);

    glUniform1i(glGetUniformLocation(this->m_OutlineShader->getProgramID(), "StencilPass"), 0);
    glm::vec3 camPos = m_Camera->getPosition();
    glUniform3f(glGetUniformLocation(this->m_Shader->getProgramID(), "camPos"), camPos.x, camPos.y, camPos.z);

    glUniformMatrix4fv(glGetUniformLocation(this->m_Shader->getProgramID(), "model"), 1, GL_FALSE, &model[0][0]);
    glUniform1i(glGetUniformLocation(this->m_Shader->getProgramID(), "Picked"), 0);
    if(BSPLoaded)
    {

        BSP->Frustum.CalculateFrustum(glm::mat4(proj * view), model);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        BSP->RenderLevel(camPos, this->m_Shader->getProgramID(),false);

        glCullFace(GL_BACK);
        glDisable(GL_CULL_FACE);
    }

    this->m_Shader->Free();

    model = glm::mat4(1);
    MVP = proj * view * model;
    this->m_TerrainShader->Use();
    glUniform1f(glGetUniformLocation(this->m_TerrainShader->getProgramID(), "time"), time);
    glUniformMatrix4fv(glGetUniformLocation(this->m_TerrainShader->getProgramID(), "MVP"), 1, GL_FALSE, &MVP[0][0]);

    if(m_Terrain != nullptr)
        this->m_Terrain->Render(this->m_TerrainShader->getProgramID());

    this->m_TerrainShader->Free();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    QPoint mousePos = this->mapFromGlobal(QCursor::pos());
    /*
        std::cout << "\r-----" << "\nLeft pressed: " << LeftPressed << "\nLeft Rising: " << LeftRising << "\nLeft Falling: " << LeftFalling << std::endl;
*/
    float currentDistance = 10000000;
    bool reset = false;
    for(unsigned i = 0; i < modv.size(); i++){
        //if(somethingPicked && LeftPressed) continue;
        if(LeftRising) {
            if(!reset){
                somethingPicked = false;
                reset = true;
            }
            pick = ray->pickObject(mousePos.x(),
                                   mousePos.y(),
                                   this->width(), this->height(), 3000.0, m_Camera->getPosition(),
                                   view, proj, this->modv.at(i).MinMaxPoints,
                                   this->modv.at(i).Position,  this->modv.at(i).Scale);

            if(pick){
                if(ray->distance > currentDistance) continue;
                else {
                    somethingPicked = pick;
                    modv.at(i).picked = true;
                    selectID = static_cast<int>(i);
                    currentMouse3DPosition =  ((ray->getRayDir())*ray->distance + m_Camera->getPosition());
                    lastMouse3DPosition = currentMouse3DPosition;
                    currentDistance = ray->distance;
                    mDistanceToObject = ray->distance;
                    somethingPicked = true;

                    /* Update the properties UI*/
                    this->UiReference->property_tree->clear();
                    QTreeWidgetItem* item = new QTreeWidgetItem();
                    QPlainTextEdit* te = new QPlainTextEdit();
                    te->setFixedHeight(27);
                    item->setText(0, "Property Name");

                    this->UiReference->property_tree->addTopLevelItem(item);
                     this->UiReference->property_tree->setItemWidget(item, 1, te);
                }
            }
        }

    }

    if(somethingPicked) {
        ray->pickObject(mousePos.x(),
                        mousePos.y(),
                        this->width(), this->height(), 3000.0, m_Camera->getPosition(),
                        view, proj, this->modv.at(selectID).MinMaxPoints,
                        this->modv.at(selectID).Position,  this->modv.at(selectID).Scale);
        currentMouse3DPosition =  ((ray->getRayDir())*mDistanceToObject + m_Camera->getPosition());
    } else {

        selectID = -1;
        this->UiReference->property_tree->clear();
    }

    for(unsigned i = 0; i < modv.size(); i++){
        if(i != selectID)
            modv.at(i).picked = false;
    }


    //if(LeftPressed && selectID >= 0)


    glDisable(GL_CULL_FACE);
    glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    //glDepthMask(GL_FALSE);
    for(unsigned i = 0; i < modv.size(); ++i){

        if(KEYS[Qt::Key_Escape])
        {
            modv.at(i).picked = false;
            somethingPicked = false;
            selectID = -1;
        }
        if(selectID >= 0 && LeftPressed && i == selectID)
        {
            if(KEYS[Qt::Key_X]){
                this->modv.at(selectID).Position.x += (((ray->getRayDir())*ray->distance + m_Camera->getPosition()) - this->modv.at(selectID).Position).x;
                PosXRef->blockSignals(true);
                PosXRef->setValue(this->modv.at(selectID).Position.x);
                PosXRef->blockSignals(false);
            }
            else if(KEYS[Qt::Key_Y]){
                this->modv.at(selectID).Position.y += (((ray->getRayDir())*ray->distance + m_Camera->getPosition()) - this->modv.at(selectID).Position).y;
                PosYRef->blockSignals(true);
                PosYRef->setValue(this->modv.at(selectID).Position.y);
                PosYRef->blockSignals(false);
            }
            else if(KEYS[Qt::Key_Z]){
                this->modv.at(selectID).Position.z += (((ray->getRayDir())*ray->distance + m_Camera->getPosition()) - this->modv.at(selectID).Position).z;
                PosZRef->blockSignals(true);
                PosZRef->setValue(this->modv.at(selectID).Position.z);
                PosZRef->blockSignals(false);
            }
            else{
                //std::cout << "Moving" << std::endl;
                glm::vec3 movVector = currentMouse3DPosition - lastMouse3DPosition;
                //std::cout << "Distance: " << mDistanceToObject << std::endl;
                //std::cout << "x: " << movVector.x << " y: " << movVector.y << " z: " << movVector.z << std::endl;
                //if((lastp - p).manhattanLength() == 0)
                  //  movVector = glm::vec3(0.0f);
                this->modv.at(selectID).Position += movVector;
                PosXRef->blockSignals(true);
                PosYRef->blockSignals(true);
                PosZRef->blockSignals(true);
                PosXRef->setValue(this->modv.at(selectID).Position.x);
                PosYRef->setValue(this->modv.at(selectID).Position.y);
                PosZRef->setValue(this->modv.at(selectID).Position.z);
                PosXRef->blockSignals(false);
                PosYRef->blockSignals(false);
                PosZRef->blockSignals(false);
            }
        }

        if(modv.at(i).picked) {
            continue;
        } else {
            glStencilMask(0x00);
        }
        this->m_Shader->Use();
        model = glm::translate(glm::mat4(1.0f), this->modv.at(i).Position) * glm::scale(glm::mat4(1.0f),  this->modv.at(i).Scale);
        MVP = proj * view * model;

        glUniform1i(glGetUniformLocation(this->m_OutlineShader->getProgramID(), "StencilPass"), 0);
        glUniform1i(glGetUniformLocation(this->m_Shader->getProgramID(), "mode"), mode);
        glUniformMatrix4fv(glGetUniformLocation(this->m_Shader->getProgramID(), "MVP"), 1, GL_FALSE, &MVP[0][0]);
        camPos = m_Camera->getPosition();
        glUniform3f(glGetUniformLocation(this->m_Shader->getProgramID(), "camPos"), camPos.x, camPos.y, camPos.z);
        glUniform1i(glGetUniformLocation(this->m_Shader->getProgramID(), "Picked"), modv.at(i).picked);
        glUniformMatrix4fv(glGetUniformLocation(this->m_Shader->getProgramID(), "model"), 1, GL_FALSE, &model[0][0]);

        glActiveTexture(GL_TEXTURE4);
        glUniform1i(glGetUniformLocation(this->m_Shader->getProgramID(), "texture_cubemap"), 4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, LoadCubemap->getCubemapTextureId());

        glActiveTexture(GL_TEXTURE5);
        glUniform1i(glGetUniformLocation(this->m_Shader->getProgramID(), "brdfLUT"), 5);
        glBindTexture(GL_TEXTURE_2D, this->brdfLUT);

        this->modv.at(i).Draw(this->m_Shader->getProgramID());
        this->m_Shader->Free();
    }

    if(selectID != -1) {

/*
        this->m_BoundingBoxShader->Use();
        float dx = this->modv.at(selectID).MinMaxPoints.MAX_X - this->modv.at(selectID).MinMaxPoints.MIN_X;
        float dy = this->modv.at(selectID).MinMaxPoints.MAX_Y - this->modv.at(selectID).MinMaxPoints.MIN_Y;
        float dz = this->modv.at(selectID).MinMaxPoints.MAX_Z - this->modv.at(selectID).MinMaxPoints.MIN_Z;
        model = glm::translate(glm::mat4(1.0f), glm::vec3(this->modv.at(selectID).MinMaxPoints.MIN_X * this->modv.at(selectID).Scale.x, this->modv.at(selectID).MinMaxPoints.MIN_Y * this->modv.at(selectID).Scale.y, this->modv.at(selectID).MinMaxPoints.MIN_Z * this->modv.at(selectID).Scale.z) + modv.at(selectID).Position) * glm::scale(glm::mat4(1.0f),  this->modv.at(selectID).Scale * glm::vec3(dx, dy, dz));
        MVP = proj * view * model;
        glUniformMatrix4fv(glGetUniformLocation(this->m_BoundingBoxShader->getProgramID(), "MVP"), 1, GL_FALSE, &MVP[0][0]);
        this->m_BoundingBox->Render();
        this->m_BoundingBoxShader->Free();
*/


        glm::vec3 scaleVector = m_Camera->getPosition() - this->modv.at(selectID).Position;
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        this->m_Shader->Use();
        model = glm::translate(glm::mat4(1.0f), this->modv.at(selectID).Position) * glm::scale(glm::mat4(1.0f),  this->modv.at(selectID).Scale);
        MVP = proj * view * model;

        glUniformMatrix4fv(glGetUniformLocation(this->m_Shader->getProgramID(), "MVP"), 1, GL_FALSE, &MVP[0][0]);
        camPos = m_Camera->getPosition();
        glActiveTexture(GL_TEXTURE4);
        glUniform1i(glGetUniformLocation(this->m_Shader->getProgramID(), "texture_cubemap"), 4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, LoadCubemap->getCubemapTextureId());
        glUniform3f(glGetUniformLocation(this->m_Shader->getProgramID(), "camPos"), camPos.x, camPos.y, camPos.z);
        glUniform1i(glGetUniformLocation(this->m_Shader->getProgramID(), "Picked"), modv.at(selectID).picked);
        glUniformMatrix4fv(glGetUniformLocation(this->m_Shader->getProgramID(), "model"), 1, GL_FALSE, &model[0][0]);
        this->modv.at(selectID).Draw(this->m_Shader->getProgramID());
        this->m_Shader->Free();

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        //glDisable(GL_DEPTH_TEST);
        this->m_OutlineShader->Use();
        model = glm::translate(glm::mat4(1.0f), this->modv.at(selectID).Position) * glm::scale(glm::mat4(1.0f),  glm::vec3(this->modv.at(selectID).Scale));
        MVP = proj * view * model;

        glUniformMatrix4fv(glGetUniformLocation(this->m_OutlineShader->getProgramID(), "MVP"), 1, GL_FALSE, &MVP[0][0]);
        camPos = m_Camera->getPosition();
        glUniform1f(glGetUniformLocation(m_OutlineShader->getProgramID(),"time"), time);
        glUniform1f(glGetUniformLocation(m_OutlineShader->getProgramID(),"scaleFactor"), glm::length(scaleVector));
        glUniform3f(glGetUniformLocation(this->m_OutlineShader->getProgramID(), "camPos"), camPos.x, camPos.y, camPos.z);
        glUniform1i(glGetUniformLocation(this->m_OutlineShader->getProgramID(), "Picked"), modv.at(selectID).picked);
        glUniform1i(glGetUniformLocation(this->m_OutlineShader->getProgramID(), "StencilPass"), 1);
        glUniformMatrix4fv(glGetUniformLocation(this->m_OutlineShader->getProgramID(), "model"), 1, GL_FALSE, &model[0][0]);
        this->modv.at(selectID).Draw(this->m_OutlineShader->getProgramID());
        this->m_OutlineShader->Free();

        glBindVertexArray(0);

        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        glDisable(GL_DEPTH_TEST);
        this->GizmosShader->Use();
        model = glm::translate(glm::mat4(1.0f), this->modv.at(selectID).Position) * glm::scale(glm::mat4(1.0f),  glm::vec3( glm::length(scaleVector) * 0.16));
        MVP = proj * view * model;
        glUniformMatrix4fv(glGetUniformLocation(this->GizmosShader->getProgramID(), "MVP"), 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(this->GizmosShader->getProgramID(), "model"), 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(this->GizmosShader->getProgramID(), "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(this->GizmosShader->getProgramID(), "proj"), 1, GL_FALSE, &proj[0][0]);
        _GizmoTranslate->Render(GizmosShader);
        this->GizmosShader->Free();
        glEnable(GL_DEPTH_TEST);
    }


    //glDepthMask(GL_TRUE);
    glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    glEnable(GL_CULL_FACE);
    glStencilMask(0xff);

    lastp = p;


    if(LeftFalling){
        LeftRising = false;
        LeftFalling = false;
        LeftPressed = false;
    }

    if(LeftPressed && LeftRising) {
        LeftRising = false;
        LeftFalling = false;
        LeftPressed = true;
    }
    lastMouse3DPosition = currentMouse3DPosition;
    ThreadManager<8>::getInstance()->loadedLastFrame = 0;
}

static bool continues = false;
void OGLWidget::mousePressEvent(QMouseEvent * event)
{


    if(event->buttons() == Qt::LeftButton)
    {

        if(LeftPressed) {
            LeftRising = false;
            LeftFalling = false;
            LeftPressed = true;
        }
        else if(!LeftPressed) {
            std::cout << "Left rising" << std::endl;
            LeftRising = true;
            LeftPressed = true;
            LeftFalling = false;
        }
    }

    if(event->buttons() == Qt::MiddleButton && /*KEYS[0x01000021/*Ctrl key]*/ KEYS[Qt::Key::Key_Shift] == true)
    {
        RightPressed = true;
        action = MOVE;
        this->setCursor(Qt::ClosedHandCursor);
    }
    if(event->buttons() == Qt::MiddleButton && KEYS[Qt::Key::Key_Shift] /*KEYS[0x01000021/***Ctrl key]***/ != true)
    {
        MiddlePressed = true;

        action = ROTATE;
        this->setCursor(Qt::ClosedHandCursor);
    }
    QString bp = "";
    if(event->buttons() == Qt::LeftButton) bp = "Mouse 1";
    if(event->buttons() == Qt::RightButton) bp = "Mouse 3";
    if(event->buttons() == Qt::MiddleButton) bp = "Mouse 3";
    if(continues){
        this->UiReference->statusBar->showMessage(this->UiReference->statusBar->currentMessage() + bp);

        continues = false;
    } else {
        this->UiReference->statusBar->showMessage("Last key pressed: " + bp);
    }
}

void OGLWidget::keyPressEvent(QKeyEvent * event)
{
    // std::cout << event->key() << std::endl;
    // if(event->key() == Qt::Key_Control)
    // {
    //    CTRLPRessed = true;
    // }
    KEYS[event->key()] = true;

    if(event->key() == Qt::Key::Key_Control){
        this->UiReference->statusBar->showMessage("Last key pressed: " + QString("Control + "));
        continues = true;
        return;
    } else if(event->key() == Qt::Key::Key_Alt) {
        this->UiReference->statusBar->showMessage("Last key pressed: " + QString("Alt + "));
        continues = true;
        return;
    } else if(event->key() == Qt::Key::Key_Shift) {
        this->UiReference->statusBar->showMessage("Last key pressed: " + QString("Shift + "));
        continues = true;
        return;
    } else {
        if(continues){
            this->UiReference->statusBar->showMessage(this->UiReference->statusBar->currentMessage() + QString(event->key()));
        } else {
            this->UiReference->statusBar->showMessage("Last key pressed: " + QString(event->key()));
        }
        continues = false;
    }
}

void OGLWidget::keyReleaseEvent(QKeyEvent * event)
{
    KEYS[event->key()] = false;

    if(event->key() == Qt::Key_Delete) {
        removeCurrentModel();
    }

}

void OGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    //std::cout << event->button() << std::endl;
    if(event->button() == Qt::LeftButton){
        LeftRising = false;
        LeftFalling = true;
        LeftPressed = false;
        std::cout << "Left falling" << std::endl;

    }
    if(event->button() == Qt::RightButton)
    {
        RightPressed = false;
        action = NONE;
        this->unsetCursor();
    }

    if(event->button() == Qt::MiddleButton)
    {
        MiddlePressed = false;
        action = NONE;
        this->unsetCursor();
    }

    action = (decltype (action))-1;

}

void OGLWidget::wheelEvent(QWheelEvent * event)
{

    //if(KEYS[Qt::Key_Control])
    // if(event->)
    {
        action = ZOOM;
        zoomdelta = event->delta();
        //return;
    }


    event->accept();
}
void OGLWidget::testCollision()
{
    mapFromGlobal(mousePos);
}
/*
void OGLWidget::addModel(QString modelName)
{
    this->modv.push_back(Model(modelName.toStdString().c_str(), glm::vec3(78.9,4.75,-64.0), glm::vec3(5), glm::quat(-1.0, 0.0, -1.0, 0.0)));
}
*/
void OGLWidget::shouldLoad(bool should, QString modname, float px, float py, float pz, float sx, float sy, float sz)
{
    if(should){
        this->should = should;
        this->mname = modname;
        ModelData tmpData;
        tmpData.pos = glm::vec3(px, py, pz);
        tmpData.sc = glm::vec3(sx, sy, sz);
        this->m_ModelData.push_back(tmpData);
    }
}

void OGLWidget::LoadModel()
{
    if(should){
        std::cout << "Loading Model: " << this->mname.toStdString() << std::endl;
        should = false;
        this->modv.push_back(Model(this->mname.toStdString().c_str(), glm::vec3(0.0f, 0.0f, 0.0f), this->m_ModelData.back().sc, glm::quat(-1.0, 0.0, -1.0, 0.0)));
        this->UiReference->ItemsList->addItem(this->mname.toStdString().c_str());
    }
}

void OGLWidget::ShouldLoadBSP(QString bspPath)
{
    loadBSP = true;
    this->BSPName = bspPath;
}

void OGLWidget::LoadBSP()
{
    if(this->BSP != nullptr){
        this->BSP->Destroy();
        delete this->BSP;
        this->BSP = nullptr;
    }
    this->BSP = new CQuake3BSP();
    this->BSP->LoadBSP(this->BSPName.toStdString().c_str());
    loadBSP  = false;
    BSPLoaded = true;

}

void OGLWidget::updatePositionX(double value)
{

    if(selectID != -1) {
        selectedModelPosition = glm::vec3(value, this->modv.at(selectID).Position.y, this->modv.at(selectID).Position.z);
        this->modv.at(selectID).Position = selectedModelPosition;
    }
}

void OGLWidget::updatePositionY(double value)
{
    if(selectID != -1) {
        selectedModelPosition = glm::vec3(this->modv.at(selectID).Position.x, value, this->modv.at(selectID).Position.z);
        this->modv.at(selectID).Position = selectedModelPosition;
    }
    //std::cout << value << std::endl;
}

void OGLWidget::updatePositionZ(double value)
{
    if(selectID != -1) {
        selectedModelPosition = glm::vec3(this->modv.at(selectID).Position.x, this->modv.at(selectID).Position.y, value);

        this->modv.at(selectID).Position = selectedModelPosition;
    }
}

void OGLWidget::PollEvents()
{
    p = this->mapFromGlobal(QCursor::pos());

    if(MiddlePressed || RightPressed){
        x += lastp.x() - p.x();
        y += lastp.y() - p.y();
    }

}

void OGLWidget::ShouldLoadTerrain(int size, double roughness, double height, glm::vec3 pos)
{
    t_s = size;
    t_r = roughness;
    t_h = height;
    tsl = true;
    t_p = pos;
}

void OGLWidget::removeCurrentModel() {
    if(selectID == -1) return;

    this->modv.at(selectID).Destroy();
    this->modv.erase(modv.begin() + selectID);

    //update vector's capacity
    decltype (this->modv) newv;
    this->modv.swap(newv);
    this->modv = newv;

    selectID = -1;
    somethingPicked = false;
}

void OGLWidget::LoadTerrain()
{
    if(this->m_Terrain != nullptr){
        this->m_Terrain->Destroy();
        this->m_Terrain.reset();
        this->m_Terrain = nullptr;
    }
    std::cout << "Loaded Terrain" << std::endl;
    this->m_Terrain = (std::shared_ptr<Terrain>) new Terrain(t_s , t_r, t_h, t_p);
    tsl = false;
}

GLuint OGLWidget::SetupLut() {

    unsigned int captureFBO;
    unsigned int captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    unsigned int brdfLUTTexture;
    glGenTextures(1, &brdfLUTTexture);

    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glViewport(0, 0, 512, 512);
    brdfShader->Use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderQuad();
    brdfShader->Free();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return brdfLUTTexture;
}

void OGLWidget::RenderQuad() {
    if (quadVAO == 0)
    {
        GLuint quadVBO;
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

