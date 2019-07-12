#include "include/Framebuffer.h"

#include "Scene.h"

#include <QDebug>
#include <iostream>


Framebuffer::Framebuffer()
{
    WindowWidth = 1392;
    WindowHeight = 1452;
    ms = true;
    m_msfbo = 33;
    init();
}

Framebuffer::Framebuffer(Scene *_scene) :
    m_scene(_scene)
{
    WindowWidth = _scene->width();
    WindowHeight = _scene->height();
    ms = true;
    init();
}

bool Framebuffer::init()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_msfbo);

    glGenTextures(1, &m_msColorBuffer);
    if(ms){ glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_msColorBuffer);}
    else{glBindTexture(GL_TEXTURE_2D, m_msColorBuffer);}

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    if(ms){ glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, WindowWidth, WindowHeight, GL_TRUE);}
    else{glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, WindowWidth, WindowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);}

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if(ms){ glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_msColorBuffer, 0);}
    else{glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_msColorBuffer, 0);}

    glGenRenderbuffers(1, &m_msDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_msDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WindowWidth, WindowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_msDepthBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      qCritical()<<"msfbo in Framebuffer class"<< (glCheckFramebufferStatus(GL_FRAMEBUFFER));

    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glGenTextures(1, &m_colorBuffer);
    glBindTexture(GL_TEXTURE_2D, m_colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBuffer, 0);

    glGenRenderbuffers(1, &m_depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WindowWidth, WindowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      qCritical()<<"fbo in Framebuffer class"<< (glCheckFramebufferStatus(GL_FRAMEBUFFER));
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Framebuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
//    glBindFramebuffer(GL_FRAMEBUFFER, m_msfbo);
}

void Framebuffer::readPixel(uint _x, uint _y)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    Pixel pixel;
    glReadPixels(_x, _y, 1, 1, GL_RGB, GL_FLOAT, &pixel);
}

void Framebuffer::resize(int _h, int _w)
{
    WindowWidth = _w;
    WindowHeight = _h;
}

void Framebuffer::debug()
{

}

uint Framebuffer::fbo()
{
    return m_fbo;
}


bool Framebuffer::isMultisample()
{
    return ms;
}

















