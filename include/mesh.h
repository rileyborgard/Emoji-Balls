#ifndef MESH_H
#define MESH_H

#include <qopengl.h>
#include <vector>
#include <QVector2D>
#include <QVector3D>

class Mesh {
public:
    Mesh();

    const static int VERTEX_DATA_SIZE = 8;

    const GLfloat *constData() const { return data.data(); }
    int vertexCount() const { return data.size() / Mesh::VERTEX_DATA_SIZE; }

    void addTriangle(const QVector3D &v1, const QVector2D &texUV1,
                     const QVector3D &v2, const QVector2D &texUV2,
                     const QVector3D &v3, const QVector2D &texUV3);

private:
    void addVertex2(const QVector2D &v);
    void addVertex3(const QVector3D &v);

    std::vector<GLfloat> data;
};

#endif // MESH_H
