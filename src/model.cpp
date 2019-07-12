#include <iostream>

#include "model.h"

Model::Model()
{

}

Model::Model(std::vector<ShapePtr> _meshes)
{
    mlog<<"Model meshes: "<<&_meshes[0]<<" &: ";
    meshes = _meshes;
}

Model::Model(Scene *_scene, QOpenGLShaderProgram *_shaderProgram)
            : pScene(_scene), pShader(_shaderProgram)
{
    qDebug()<<"construct Model with Scene and Shader";
}


void Model::loadModel(std::string _path)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(_path,
                                             aiProcess_Triangulate |
//                                             aiProcess_JoinIdenticalVertices |
//                                             aiProcess_FlipUVs |
//                                             aiProcess_CalcTangentSpace |
//                                             aiProcess_GenNormals
                                             aiProcess_GenSmoothNormals
//                                             aiProcess_FixInfacingNormals
                                             );

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    std::cout<<_path<<std::endl;
    processNode(scene->mRootNode, scene, _path);
}

void Model::processNode(aiNode *node, const aiScene *scene, std::string _path)
{

    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        ShapePtr ptrShape = processMesh(mesh, scene, _path);

        if(ptrShape->data() != nullptr)
        {
            // (note, pushing_back for objects on the stack calls copy ctor)
            meshes.push_back(ptrShape);
        }
    }

    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, _path);
    }
}

ShapePtr Model::processMesh(aiMesh *mesh, const aiScene *scene,  std::string _path)
{
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<QVector3D> points;

    std::map<int, std::list<int>> pointsToVerts;

    // cache vertex attributes
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {

        Vertex vertex;
        QVector3D vector;
        vector.setX(mesh->mVertices[i].x);
        vector.setY(mesh->mVertices[i].y);
        vector.setZ(mesh->mVertices[i].z);
        vertex.Position = vector;
        vector.setX(mesh->mNormals[i].x);
        vector.setY(mesh->mNormals[i].y);
        vector.setZ(mesh->mNormals[i].z);
        vertex.Normal = vector;
        // Barycentric depends ebo.
        vertex.Barycentric = QVector3D(0,0,0);
        vertices.push_back(vertex);

        // cache position
        auto itr = std::find(points.begin(), points.end(), vertex.Position);
        if(itr != points.end())
        {
            int idx = itr - points.begin();
            pointsToVerts[idx].push_back(i);
        }
        else
        {
            points.push_back(vertex.Position);
            pointsToVerts[points.size() -1].push_back(i);
        }
    }


    // cache vert indices
    int maxIndex = -1;
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for(unsigned int j = 0; j < face.mNumIndices; j++)
        {
          indices.push_back(face.mIndices[j]);
          QVector3D _barycentric = QVector3D(0, 0, 0);
          int _mod = j % 3;
          _barycentric[_mod] = 1;
          vertices[face.mIndices[j]].Barycentric = _barycentric;
        }
    }
    ShapePtr ptrNewShape = std::make_shared<Shape>(vertices, indices, points, pointsToVerts);
    return ptrNewShape;
}

void Model::draw()
{
    if(hidden)
        return;

    for(unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i]->draw();
    }
}

void Model::drawPoints()
{
    for(unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i]->drawPoints();
    }
}

void Model::bind()
{
    for(unsigned int i = 0; i < meshes.size(); i++)
        meshes[i]->bind();
}

void Model::clone(const ModelPtr &_model)
{
    for(auto mesh : _model->getMeshes())
    {
        ShapePtr shape = std::make_shared<Shape>(mesh->getVertices(),
                                                 mesh->getIndices(),
                                                 mesh->getPoints(),
                                                 mesh->getVertsMap());
        meshes.push_back(shape);
    }
}

void Model::setHidden(bool _hidden)
{
    hidden = _hidden;
}

std::vector<ShapePtr> Model::getMeshes()
{
    return meshes;
}


ShapePtr Model::getShape(unsigned int _index)
{
    if(!meshes[_index])
    {
        qDebug ()<<"WARNING NO SHAPE RETURN FROM GETSHAPE()";
        return nullptr;
    }
    return meshes[_index];
}
