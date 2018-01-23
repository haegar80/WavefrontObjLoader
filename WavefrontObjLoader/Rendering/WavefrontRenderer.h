#ifndef MapRenderer_H
#define MapRenderer_H

#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QVector3D>
#include "../ObjectLoader/ObjLoader.h"

class ObjLoader;
class QOpenGLWidget;
class QOpenGLShaderProgram;
class QOpenGLTexture;

class WavefrontRenderer : protected QOpenGLFunctions
{
public:
    WavefrontRenderer(const ObjLoader& pc_objLoader, int p_width, int p_height);
    virtual ~WavefrontRenderer();

    void init(QOpenGLShaderProgram* p_shaderProgram);
    void render(QOpenGLShaderProgram* p_shaderProgram);

private:
    struct VertexData
    {
        QVector3D position[4];
        QVector3D color;
    };

    const ObjLoader& mc_objLoader;
    QOpenGLVertexArrayObject m_vertexArrayObjectWavefrontModel;
    QOpenGLBuffer m_wavefrontModelVertexBuffer;
    QOpenGLBuffer m_wavefrontModelNormalBuffer;
    QOpenGLBuffer m_wavefrontModelTextureBuffer;
    QOpenGLBuffer m_wavefrontModelIndexArrayBuffer;

    int m_width{ 0 };
    int m_height{ 0 };
    std::map<SubMesh*, int> m_subMeshIndicesCount;
    std::map<SubMesh*, QOpenGLTexture*> m_subMeshAmbientTextures;
    std::map<SubMesh*, QOpenGLTexture*> m_subMeshDiffuseTextures;
    std::map<SubMesh*, QOpenGLTexture*> m_subMeshSpecularTextures;
    float m_scaleFactor{ 0.0f };
    float m_moveToCenterX{ 0.0f };
    float m_moveToCenterY{ 0.0f };

    void initWavefrontModels(QOpenGLShaderProgram* p_shaderProgram);
    void renderWavefrontModels(QOpenGLShaderProgram* p_shaderProgram);

    std::vector<ObjVertexCoords> getScaledVerticesFromWavefrontModel();
    std::vector<ObjVertexCoords> getNormalsFromWaveFrontModel();
    std::vector<ObjTextureCoords> getTextureCoordsFromWaveFrontModel();

    void prepareTextures(SubMesh* p_submesh);
};

#endif // MapRenderer_H