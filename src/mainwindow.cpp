#include "mainwindow.h"
#include <cmath>

/// Get texture coordinates for a point on a donut mesh
/// Texture coordinate outputs (u, v) should go from (0, 0) to (1, 1)
/// We are sampling an annulus of the texture image
/// (cx, cy) is the center of the annulus
/// r is the inner radius, R is the outer radius
void getDonutUV(float phi, float theta, float cx, float cy, float r, float R, float* u_out, float* v_out) {
    float dist = (r + (R - r) * theta);
    *u_out = cx - dist * cos(phi);
    *v_out = cy - dist * sin(phi);
}

/// Get texture coordinates for a point on a sphere mesh
/// Texture coordinate outputs (u, v) should go from (0, 0) to (1, 1)
/// We are sampling a circle of the texture image
/// (cx, cy) is the center of the circle
/// r is the radius
void getSphereUV(float phi, float theta, float cx, float cy, float r, float* u_out, float* v_out) {
    *u_out = cx + 2 * r * (phi / M_PI) * cos(theta);
    *v_out = cy + 2 * r * (phi / M_PI) * sin(theta);
}

Mesh* makeDonut(float a, float b, int nPhi, int nTheta, float cx, float cy, float r, float R) {
    Mesh* mesh = new Mesh;

    // donut parameterization
    // x = (a cos(theta) + b) cos(phi)
    // y = (a cos(theta) + b) sin(phi), z = a sin(theta)

    auto getTextureTheta = [](int i, int n) {
        int A = n * 0.5f;
        if(0 <= i && i < A) {
            return 1.f - 1.f * i / A;
        }else {
            return 1.f * (i - A) / (n - A);
        }
    };

    float u11, v11, u12, v12, u21, v21, u22, v22;
    for(int iPhi = 0; iPhi < nPhi; iPhi++) {
        int iPhi2 = (iPhi + 1) % nPhi;
        float phi = iPhi * 2 * M_PI / nPhi;
        float phi2 = iPhi2 * 2 * M_PI / nPhi;
        for(int iTheta = 0; iTheta < nTheta; iTheta++) {
            int iTheta2 = (iTheta + 1) % nTheta;
            float theta = iTheta * 2 * M_PI / nTheta;
            float theta2 = iTheta2 * 2 * M_PI / nTheta;

            float texTheta = getTextureTheta(iTheta, nTheta);
            float texTheta2 = getTextureTheta(iTheta2, nTheta);

            getDonutUV(phi, texTheta, cx, cy, r, R, &u11, &v11);
            getDonutUV(phi2, texTheta, cx, cy, r, R, &u12, &v12);
            getDonutUV(phi, texTheta2, cx, cy, r, R, &u21, &v21);
            getDonutUV(phi2, texTheta2, cx, cy, r, R, &u22, &v22);

            float c1 = a * cos(theta) + b;
            float c2 = a * cos(theta2) + b;

            mesh->addTriangle(
                QVector3D(c1 * cos(phi), c1 * sin(phi), a * sin(theta)),
                QVector2D(u11, v11),

                QVector3D(c1 * cos(phi2), c1 * sin(phi2), a * sin(theta)),
                QVector2D(u12, v12),

                QVector3D(c2 * cos(phi), c2 * sin(phi), a * sin(theta2)),
                QVector2D(u21, v21)
            );

            mesh->addTriangle(
                QVector3D(c1 * cos(phi2), c1 * sin(phi2), a * sin(theta)),
                QVector2D(u12, v12),

                QVector3D(c2 * cos(phi2), c2 * sin(phi2), a * sin(theta2)),
                QVector2D(u22, v22),

                QVector3D(c2 * cos(phi), c2 * sin(phi), a * sin(theta2)),
                QVector2D(u21, v21)
            );
        }
    }

    return mesh;
}

Mesh* makeSphere(int nPhi, int nTheta, float cx, float cy, float r) {
    Mesh* mesh = new Mesh;

    float u11, v11, u12, v12, u21, v21, u22, v22;
    for(int iPhi = 0; iPhi < nPhi; iPhi++) {
        int iPhi2 = (iPhi + 1);
        float phi = iPhi * M_PI / nPhi;
        float phi2 = iPhi2 * M_PI / nPhi;
        for(int iTheta = 0; iTheta < nTheta; iTheta++) {
            int iTheta2 = (iTheta + 1) % nTheta;
            float theta = iTheta * 2 * M_PI / nTheta;
            float theta2 = iTheta2 * 2 * M_PI / nTheta;

            if(iPhi < nPhi / 2) {
                // Front half of sphere: sample from texture
                getSphereUV(phi, theta, cx, cy, r, &u11, &v11);
                getSphereUV(phi2, theta, cx, cy, r, &u12, &v12);
                getSphereUV(phi, theta2, cx, cy, r, &u21, &v21);
                getSphereUV(phi2, theta2, cx, cy, r, &u22, &v22);
            }else {
                // Back half of sphere: constant color
                u11 = v11 = u12 = v12 = u21 = v21 = u22 = v22 = 0.f;
            }

            // make two triangles for this cell
            mesh->addTriangle(
                QVector3D(cos(theta) * sin(phi), sin(theta) * sin(phi), cos(phi)),
                QVector2D(u11, v11),

                QVector3D(cos(theta) * sin(phi2), sin(theta) * sin(phi2), cos(phi2)),
                QVector2D(u12, v12),

                QVector3D(cos(theta2) * sin(phi), sin(theta2) * sin(phi), cos(phi)),
                QVector2D(u21, v21)
            );

            mesh->addTriangle(
                QVector3D(cos(theta) * sin(phi2), sin(theta) * sin(phi2), cos(phi2)),
                QVector2D(u12, v12),

                QVector3D(cos(theta2) * sin(phi2), sin(theta2) * sin(phi2), cos(phi2)),
                QVector2D(u22, v22),

                QVector3D(cos(theta2) * sin(phi), sin(theta2) * sin(phi), cos(phi)),
                QVector2D(u21, v21)
            );
        }
    }
    return mesh;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    setWindowTitle("3D Viewer");

    glWidget = new GLWidget(this);

//    mesh = makeDonut(1.f, 2.f, 200, 400, 0.5f, 0.5f, 0.f, 0.5f);
    mesh = makeSphere(200, 400, 0.5f, 0.5f, 0.5f);

    glWidget->setMesh(mesh);

    setCentralWidget(glWidget);
}

MainWindow::~MainWindow() {

}

