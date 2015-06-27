// VolumeScene.h

#pragma once

#ifdef _WIN32
#  define WINDOWS_LEAN_AND_MEAN
#  define NOMINMAX
#  include <windows.h>
#endif

#include "RaymarchShaderScene.h"

///@brief 
class VolumeScene : public RaymarchShaderScene
{
public:
    VolumeScene();
    virtual ~VolumeScene();

    virtual void initGL();
    virtual void LoadTextureFromBrickOfShorts(const char* pFilename);

    virtual void SetOpacityTexPointer(GLuint* pTex) { m_opacityTex = pTex; }
    virtual void SetColorTexPointer(GLuint* pTex) { m_colorTex = pTex; }

protected:
    virtual void _DrawScreenQuad() const;
    GLuint m_volumeTex;
    GLuint* m_opacityTex;
    GLuint* m_colorTex;

private: // Disallow copy ctor and assignment operator
    VolumeScene(const VolumeScene&);
    VolumeScene& operator=(const VolumeScene&);
};
