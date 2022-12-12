
#include "glwidget.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>
#include <QOpenGLFunctions>

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent) {

    QSurfaceFormat format;
    format.setSamples(4);
    setFormat(format);
}

GLWidget::~GLWidget() {
    cleanup();
}

QSize GLWidget::minimumSizeHint() const {
    return QSize(400, 400);
}

QSize GLWidget::sizeHint() const {
    return QSize(400, 400);
}

void GLWidget::setMesh(Mesh *mesh) {
    if (this->mesh != nullptr) {
        delete this->mesh;
        meshVBO.destroy();
    }

    this->mesh = mesh;
    meshIsReady = true;

    update();
}

static void qNormalizeAngle(int &angle) {
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle) {
    qNormalizeAngle(angle);
    if (angle != xrot) {
        xrot = angle;
//        emit xRotationChanged(angle);
        update();
    }
}

void GLWidget::setYRotation(int angle) {
    qNormalizeAngle(angle);
    if (angle != yrot) {
        yrot = angle;
//        emit yRotationChanged(angle);
        update();
    }
}

void GLWidget::setZRotation(int angle) {
    qNormalizeAngle(angle);
    if (angle != zrot) {
        zrot = angle;
//        emit zRotationChanged(angle);
        update();
    }
}

void GLWidget::cleanup() {
    if (shaderProgram == nullptr)
        return;
    makeCurrent();

    if (mesh != nullptr) {
        delete mesh;
        mesh = nullptr;

        meshVBO.destroy();
    }

    delete shaderProgram;
    shaderProgram = nullptr;

    if (texture != nullptr) {
        texture->destroy();

        delete texture;
        texture = nullptr;
    }

    doneCurrent();
}

void GLWidget::initializeGL() {
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    initializeOpenGLFunctions();
    glClearColor(0.25f, 0.25f, 0.25f, 1);

    glPointSize(6);
    glLineWidth(1);

    texture = new QOpenGLTexture(QImage(QString(":/res/flushed.png")));

    shaderProgram = new QOpenGLShaderProgram;
    shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/test.vert");
    shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/test.frag");
    shaderProgram->bindAttributeLocation("vertex", 0);
    shaderProgram->bindAttributeLocation("normal", 1);
    shaderProgram->link();

    shaderProgram->bind();
    projMatrixLoc = shaderProgram->uniformLocation("u_projMatrix");
    mvMatrixLoc = shaderProgram->uniformLocation("u_mvMatrix");
    normalMatrixLoc = shaderProgram->uniformLocation("u_normalMatrix");
    lightPosLoc = shaderProgram->uniformLocation("u_lightPos");
    colorLoc = shaderProgram->uniformLocation("u_color");

    // Our camera never changes in this example.
    camera.setToIdentity();
    camera.translate(0, 0, -8);

    // Light position is fixed.
    shaderProgram->setUniformValue(lightPosLoc, QVector3D(10, 30, 70));

    shaderProgram->release();
}

void GLWidget::setupVertexAttribs() {
    meshVBO.bind();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);
    f->glEnableVertexAttribArray(2);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Mesh::VERTEX_DATA_SIZE * sizeof(GLfloat),
                             nullptr);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, Mesh::VERTEX_DATA_SIZE * sizeof(GLfloat),
                             reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    f->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, Mesh::VERTEX_DATA_SIZE * sizeof(GLfloat),
                             reinterpret_cast<void *>(6 * sizeof(GLfloat)));
    meshVBO.release();
}

void GLWidget::paintGL() {
    glEnable(GL_DEPTH_WRITEMASK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    world.setToIdentity();
    world.rotate(180.0f - (xrot / 16.0f), 1, 0, 0);
    world.rotate(-yrot / 16.0f, 0, 1, 0);
    world.rotate(zrot / 16.0f, 0, 0, 1);

    shaderProgram->bind();
    shaderProgram->setUniformValue(projMatrixLoc, proj);
    shaderProgram->setUniformValue(mvMatrixLoc, camera * world);
    QMatrix3x3 normalMatrix = world.normalMatrix();
    shaderProgram->setUniformValue(normalMatrixLoc, normalMatrix);

    if (meshIsReady) {
        meshIsReady = false;

        // Setup our vertex buffer object.
        meshVBO.create();
        meshVBO.bind();
        meshVBO.allocate(mesh->constData(), mesh->vertexCount() * Mesh::VERTEX_DATA_SIZE * sizeof(GLfloat));
        meshVBO.release();

        setupVertexAttribs();
    }

    if (mesh != nullptr) {
        meshVBO.bind();
        texture->bind();

//        shaderProgram->setUniformValue(colorLoc, 0.75f, 0.75f, 0.75f);
        shaderProgram->setUniformValue(colorLoc, 1.f, 1.f, 1.f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.f, 1.f);
        glDrawArrays(GL_TRIANGLES, 0, mesh->vertexCount());

        texture->release();
        meshVBO.release();
    }

    shaderProgram->release();
}

void GLWidget::resizeGL(int w, int h) {
    proj.setToIdentity();
    proj.perspective(fovAngle, GLfloat(w) / h, 0.01f, 100.0f);
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
    lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xrot + 8 * dy);
        setYRotation(yrot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xrot + 8 * dy);
        setZRotation(zrot + 8 * dx);
    }
    lastPos = event->pos();
}

void GLWidget::wheelEvent(QWheelEvent *event) {
    float delta = -event->delta() / 50.f;
    fovAngle += delta;
    fovAngle = std::max(5.f, std::min(150.f, fovAngle));

    proj.setToIdentity();
    proj.perspective(fovAngle, GLfloat(width()) / height(), 0.01f, 100.0f);

    update();
}
