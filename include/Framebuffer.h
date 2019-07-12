#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLTexture>
#include <QVector3D>
//#include <QOpenGLFunctions>

class Scene;

class Framebuffer
{
public:
    Framebuffer();
    Framebuffer(Scene *_scene);
    bool init();
    void bind();
    void readPixel(uint _x, uint _y);
    void resize(int _h, int _w);
    void debug();
    uint fbo();

    bool isMultisample();
//private:
    bool ms;
    uint m_msfbo, m_msColorBuffer, m_msDepthBuffer;
    uint m_fbo, m_colorBuffer, m_depthBuffer;

    uint m_pickingTexture, m_depthTexture;
    int WindowWidth;
    int WindowHeight;
    Scene* m_scene;

//    QOpenGLFramebufferObject* qfbo;
//    QOpenGLTexture* color;
//    QOpenGLTexture* depth;

};

#endif // FRAMEBUFFER_H
