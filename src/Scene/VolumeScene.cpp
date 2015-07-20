// VolumeScene.cpp

#include "VolumeScene.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <algorithm>
#include <stdio.h>
#include <glm/glm.hpp>

#include "DicomLoader.h"

VolumeScene::VolumeScene()
: RaymarchShaderScene()
, m_volumeTex(0)
, m_opacityTex(NULL)
, m_colorTex(NULL)
, m_sampleScale(9.f)
, m_sampleBias(0.f)
{
    glm::mat4 ori = glm::rotate(glm::mat4(1.0f), 0.5f*static_cast<float>(M_PI), glm::vec3(1,0,0));
    ori = glm::rotate(ori, static_cast<float>(M_PI), glm::vec3(0,0,1));
    m_tx.SetDefaultOrientation(ori);
    m_tx.SetOrientation(ori);
}

VolumeScene::~VolumeScene()
{
    glDeleteTextures(1, &m_volumeTex);
}

void VolumeScene::initGL()
{
    m_raymarch.initProgram("volume");
    m_raymarch.bindVAO();
    _InitShaderRectAttributes();
    glBindVertexArray(0);

    //LoadTextureFromBrickOfShorts("ct002_256.bos");
#ifdef _LINUX
    LoadTextureFromDicom("/run/media/jim/300GHD/Datasets/nemamfct.images/DISCIMG/IMAGES/CT0002");
#else
    LoadTextureFromDicom("I:/Datasets/nemamfct.images/DISCIMG/IMAGES/CT0002");
#endif
}

void VolumeScene::LoadTextureFromBrickOfShorts(const char* pFilename)
{
    if (pFilename == NULL)
        return;

    FILE* pFile;
    size_t result;

    pFile = fopen(pFilename, "rb");
    if (pFile==NULL)
    {
        printf("File error : %s\n", pFilename);
        return;
    }

    unsigned int dimx, dimy, dimz;
    result = fread(&dimx, 1, sizeof(unsigned int), pFile);
    result = fread(&dimy, 1, sizeof(unsigned int), pFile);
    result = fread(&dimz, 1, sizeof(unsigned int), pFile);

    const unsigned int sz = dimx * dimy * dimz;
    short* pShortData = new short[sz];
    result = fread(pShortData, sizeof(short), sz, pFile);
    printf("Read '%s', %d unsigned shorts ( %d x %d x %d )\n",
        pFilename, result, dimx, dimy, dimz);

    // Find max value
    short minval = pShortData[0];
    short maxval = pShortData[0];
    for (int i=0; i<sz; ++i)
    {
        minval = std::min(minval, pShortData[i]);
        maxval = std::max(maxval, pShortData[i]);
    }
    const float scaledRange = static_cast<float>(USHRT_MAX) / static_cast<float>(maxval-minval);
    const float scaledOffset = static_cast<float>(-minval) / static_cast<float>(SHRT_MAX);

    glGenTextures(1, &m_volumeTex);
    glBindTexture(GL_TEXTURE_3D, m_volumeTex);
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
        glTexImage3D(GL_TEXTURE_3D,
            0,
            1,
            dimx, dimy, dimz,
            0,
            GL_LUMINANCE,
            GL_SHORT,
            NULL);
    }
    glBindTexture(GL_TEXTURE_3D, 0);

    //glPixelTransferf(GL_RED_SCALE, 10.0f);
    //glPixelTransferf(GL_GREEN_SCALE, 10.0f);
    //glPixelTransferf(GL_BLUE_SCALE, 10.0f);
    //glPixelTransferf(GL_ALPHA_SCALE, 10.0f);

    glBindTexture(GL_TEXTURE_3D, m_volumeTex);
    {
        glPixelTransferf(GL_RED_SCALE, scaledRange);
        glPixelTransferf(GL_RED_BIAS, scaledOffset);

        glTexImage3D(GL_TEXTURE_3D,
            0,
            1,
            dimx, dimy, dimz,
            0,
            GL_LUMINANCE,
            GL_SHORT,
            pShortData);
    }
    glBindTexture(GL_TEXTURE_3D, 0);

    glPixelTransferf(GL_RED_SCALE, 1.0f);
    glPixelTransferf(GL_RED_BIAS, 0.0f);

    delete [] pShortData;

    fclose(pFile);
}

void VolumeScene::LoadTextureFromDicom(const char* pFilename)
{
    DicomLoader* pDicomLoader = new DicomLoader(pFilename);
    if (pDicomLoader != NULL)
    {
        m_volumeTex = pDicomLoader->m_3dtex;
        //m_dim = pDicomLoader->m_dim;

        delete pDicomLoader;
    }
}

void VolumeScene::_DrawScreenQuad() const
{
    // glUseProgram has been set first
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, m_volumeTex);
    glUniform1i(m_raymarch.GetUniLoc("volume"), 0); // Value matches tex unit

    if (m_opacityTex != NULL)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, *m_opacityTex);
        glUniform1i(m_raymarch.GetUniLoc("opacitymap"), 1); // Value matches tex unit
    }

    if (m_colorTex != NULL)
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, *m_colorTex);
        glUniform1i(m_raymarch.GetUniLoc("colormap"), 2); // Value matches tex unit
    }

    if (m_pFm != NULL)
    {
        const float tl = m_pFm->GetTriggerValue(FlyingMouse::Left);
        const float tr = m_pFm->GetTriggerValue(FlyingMouse::Right);
        const float os = 0.05f + 0.2f * std::max(tl, tr);
        glUniform1f(m_raymarch.GetUniLoc("u_opacityScale"), os);
    }

    glUniform1f(m_raymarch.GetUniLoc("u_sampleScale"), m_sampleScale);
    glUniform1f(m_raymarch.GetUniLoc("u_sampleBias"), m_sampleBias);

    RaymarchShaderScene::_DrawScreenQuad();
}
