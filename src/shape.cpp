#include "shape.h"
#include <iostream>
#include <QDebug>

#include "dynamics/dynamicUtils.h"

Shape::Shape()
{
}

Shape::Shape(std::vector<Vertex> &_vertices,
             std::vector<unsigned int> &_indices)
        :  m_indices(_indices), m_vertices(_vertices)
{
    setupMesh();
}

Shape::Shape(std::vector<Vertex> &_vertices,
             std::vector<unsigned int> &_indices, std::vector<QVector3D> &_points, std::map<int, std::list<int> > &_pointsToVertsMap)
        :  m_indices(_indices), m_vertices(_vertices), m_points(_points) ,m_pointsToVerts(_pointsToVertsMap)
{
    setupMesh();
}

Shape::Shape(std::vector<Vertex> &_vertices,
             std::vector<unsigned int> &_indices,
             Scene *_scene,
             QOpenGLShaderProgram *_shaderProgram)
        :  m_indices(_indices), m_vertices(_vertices)
{

    setupMesh();
}


Shape& Shape::operator=(const Shape &_rhs)
{
    return *this;
}

Shape::Shape(const Shape &_rhs)
                    :m_Id(_rhs.m_Id),
                      m_vertices(_rhs.m_vertices),
                      m_indices(_rhs.m_indices)
{
    m_pVao = _rhs.m_pVao;
    m_ebo = _rhs.m_ebo;
    m_vvbo = _rhs.m_vvbo;
}

void Shape::allocate(const QVector3D* _data, int _size)
{
    m_pVao = new QOpenGLVertexArrayObject();
    m_pVao->create();
    m_pVao->bind();
    m_vvbo.create();
    m_vvbo.bind();
    m_vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vvbo.allocate(_data, _size);
}

void Shape::release()
{
    m_vao.release();
    m_vvbo.release();
}

void Shape::setupMesh()
{
    m_pVao = new QOpenGLVertexArrayObject();
    m_pVao->create();
    m_pVao->bind();

    m_vvbo.create();
    m_vvbo.bind();
    m_vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vvbo.allocate(m_vertices.data(), m_vertices.size() * sizeof(Vertex));

    m_ebo = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    m_ebo.create();
    m_ebo.bind();
    m_ebo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_ebo.allocate(m_indices.data(), m_indices.size()*4);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,                // vert attr index
                          3,                // num componets
                          GL_FLOAT,         // type
                          GL_FALSE,         // normalized
                          sizeof(Vertex),   // stride
                          nullptr);         // pointer to first components. (offset)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          (void*)offsetof(Vertex, Barycentric));
}

void Shape::recomputeSmoothedNormals()
{
    std::pair<int, std::list<int>> pointToVerts;
    std::map<int , std::pair<std::list<int> , QVector3D>> pointToVertsToNormals;

    for(int i=0; i < m_indices.size(); i++)
    {
        if(!((i+1) % 3))
        {
            int vertIdxA = m_indices[i-2];
            int vertIdxB = m_indices[i-1];
            int vertIdxC = m_indices[i-0];


            QVector3D vertA = m_vertices[vertIdxA].Position;
            QVector3D vertB = m_vertices[vertIdxB].Position;
            QVector3D vertC = m_vertices[vertIdxC].Position;

            QVector3D AB, AC, CB, normal;
            AB  = vertB -vertA;
            AC  = vertC -vertA;
            normal = QVector3D::crossProduct(AB, AC).normalized();

            int poitIdxA = findKbyValueElement(getVertsMap(), vertIdxA);
            int poitIdxB = findKbyValueElement(getVertsMap(), vertIdxB);
            int poitIdxC = findKbyValueElement(getVertsMap(), vertIdxC);

            if(pointToVertsToNormals.find(poitIdxA) != pointToVertsToNormals.end())
            {
                QVector3D n = pointToVertsToNormals[poitIdxA].second;
                n += normal;
                pointToVertsToNormals[poitIdxA].first.push_back(vertIdxA);
                pointToVertsToNormals[poitIdxA].second = n;
            }
            else {
                pointToVertsToNormals[poitIdxA].first.push_back(vertIdxA);
                pointToVertsToNormals[poitIdxA].second = normal;
            }

            if(pointToVertsToNormals.find(poitIdxB) != pointToVertsToNormals.end())
            {
                QVector3D n = pointToVertsToNormals[poitIdxB].second;
                n += normal;
                pointToVertsToNormals[poitIdxB].first.push_back(vertIdxB);
                pointToVertsToNormals[poitIdxB].second = n;
            }
            else {
                pointToVertsToNormals[poitIdxB].first.push_back(vertIdxB);
                pointToVertsToNormals[poitIdxB].second = normal;
            }

            if(pointToVertsToNormals.find(poitIdxC) != pointToVertsToNormals.end())
            {
                QVector3D n = pointToVertsToNormals[poitIdxC].second;
                n += normal;
                pointToVertsToNormals[poitIdxC].first.push_back(vertIdxC);
                pointToVertsToNormals[poitIdxC].second = n;

            }
            else {
                pointToVertsToNormals[poitIdxC].first.push_back(vertIdxC);
                pointToVertsToNormals[poitIdxC].second = normal;
            }
        }
    }

    for(int i=0; i < m_indices.size(); i++)
    {
        {
            int vertIdx = m_indices[i];
            int pointIdx = findKbyValueElement(getVertsMap(), vertIdx);
            QVector3D smoothNormal = pointToVertsToNormals[pointIdx].second;
            m_vertices[vertIdx].Normal = smoothNormal.normalized();
        }
    }
}

void Shape::recomputeNormals()
{
    for(int i=0; i < m_indices.size(); i++)
    {
        if(!((i+1) % 3))
        {
            QVector3D vertA = m_vertices[m_indices[i-2]].Position;
            QVector3D vertB = m_vertices[m_indices[i-1]].Position;
            QVector3D vertC = m_vertices[m_indices[i  ]].Position;

            QVector3D AB, AC, CB, normal;
            AB  = vertB -vertA;
            AC  = vertC -vertA;
            normal = QVector3D::crossProduct(AB, AC).normalized();

            m_vertices[m_indices[i-2]].Normal = normal;
            m_vertices[m_indices[i-1]].Normal = normal;
            m_vertices[m_indices[i  ]].Normal = normal;
        }
    }
}

void Shape::updateVertexBuffer()
{
    recomputeNormals();

    m_pVao->bind();
    m_vvbo.create();
    m_vvbo.bind();
    m_vvbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_vvbo.allocate(m_vertices.data(), m_vertices.size() * sizeof(Vertex));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          (void*)offsetof(Vertex, Barycentric));

    m_pVao->release();
}

void Shape::draw()
{
    m_pVao->bind();
    unsigned int count = m_indices.size();
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
    m_pVao->release();
}

void Shape::drawPoints()
{
    m_pVao->bind();
    unsigned int count = m_indices.size();
    glDrawElements(GL_POINTS, count, GL_UNSIGNED_INT, nullptr);
    m_pVao->release();
}

void Shape::drawWireframe()
{

}

void Shape::drawOld()
{
    m_pVao->bind();
    unsigned int count= m_indices.size();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    m_pVao->release();
}

std::vector<Vertex>& Shape::getVertices()
{
    return m_vertices;
}

std::vector<QVector3D> &Shape::getPoints()
{
    return m_points;
}

std::map<int, std::list<int> > &Shape::getVertsMap()
{
    return m_pointsToVerts;
}

std::vector<unsigned int>& Shape::getIndices()
{
    return m_indices;
}

Vertex Shape::getVertexAtIndex(unsigned int idx)
{
    return m_vertices[idx];
}

void Shape::setVertexPositionAtIndex(unsigned int idx, const QVector3D _value)
{
    m_vertices[idx].Position = _value;
}



















