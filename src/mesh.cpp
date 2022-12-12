#include "mesh.h"

Mesh::Mesh() {

}

void Mesh::addTriangle(const QVector3D &v1, const QVector2D &texUV1,
                       const QVector3D &v2, const QVector2D &texUV2,
                       const QVector3D &v3, const QVector2D &texUV3) {
    QVector3D normal = QVector3D::normal(v1, v2, v3);
    addVertex3(v1); addVertex3(normal); addVertex2(texUV1);
    addVertex3(v2); addVertex3(normal); addVertex2(texUV2);
    addVertex3(v3); addVertex3(normal); addVertex2(texUV3);
}

void Mesh::addVertex2(const QVector2D &v)
{
    data.push_back(v.x());
    data.push_back(v.y());
}

void Mesh::addVertex3(const QVector3D &v)
{
    data.push_back(v.x());
    data.push_back(v.y());
    data.push_back(v.z());
}
