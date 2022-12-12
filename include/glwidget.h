#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QOpenGLTexture>
#include "mesh.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    GLWidget(QWidget *parent = nullptr);
    ~GLWidget();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    void setMesh(Mesh *mesh);

public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void cleanup();

signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void setupVertexAttribs();

    int xrot = 0, yrot = 0, zrot = 0;
    QPoint lastPos;

    Mesh *mesh = nullptr;
    QOpenGLBuffer meshVBO;
    bool meshIsReady = false;
    QOpenGLShaderProgram *shaderProgram = nullptr;
    QOpenGLTexture *texture;

    int projMatrixLoc = 0;
    int mvMatrixLoc = 0;
    int normalMatrixLoc = 0;
    int lightPosLoc = 0;
    int colorLoc = 0;

    float fovAngle = 45.0f;
    QMatrix4x4 proj;
    QMatrix4x4 camera;
    QMatrix4x4 world;
};

#endif // GLWIDGET_H
