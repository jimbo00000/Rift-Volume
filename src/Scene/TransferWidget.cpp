// TransferWidget.cpp

#include "TransferWidget.h"
#include <algorithm>

#ifdef _WIN32
#  define WINDOWS_LEAN_AND_MEAN
#  define NOMINMAX
#  include <windows.h>
#endif

#ifdef __APPLE__
#include "opengl/gl.h"
#else
#include <GL/glew.h>
#endif

TransferWidget::TransferWidget()
: m_rect(0,0,0,0)
, m_opacitySlices(8)
, m_opacityValues(m_opacitySlices, 0.5f)
, m_opacityTex(0)
, m_colorTex(0)
{
}

TransferWidget::~TransferWidget()
{
    glDeleteTextures(1, &m_opacityTex);
    glDeleteTextures(1, &m_colorTex);
}

void TransferWidget::initGL()
{
    _UploadOpacityTex();
    _UploadColorTex();
}

/// Keep the structure of the opacity curve intact as we resize.
void TransferWidget::IncreaseSlices()
{
    if (m_opacitySlices >= 256)
        return;

    std::vector<float> resizedVector;
    for (unsigned int i=0; i<m_opacitySlices; ++i)
    {
        resizedVector.push_back(m_opacityValues[i]);
        resizedVector.push_back(m_opacityValues[i]);
    }

    m_opacitySlices *= 2;

    m_opacityValues = resizedVector;
}

/// Keep the structure of the opacity curve intact as we resize.
void TransferWidget::DecreaseSlices()
{
    if (m_opacitySlices <= 2)
        return;

    std::vector<float> resizedVector;
    for (unsigned int i=0; i<m_opacitySlices; i+=2)
    {
        resizedVector.push_back(m_opacityValues[i]);
    }

    m_opacitySlices /= 2;
    m_opacityValues = resizedVector;
}

bool pointInsideRect(int x, int y, const glm::ivec4& r)
{
    if (
        (x > r.x    ) &&
        (x < r.x+r.z) &&
        (y > r.y    ) &&
        (y < r.y+r.w)
        )
    {
        return true;
    }
    return false;
}


float clamp(float val, float bottom, float top)
{
    return std::min(
            std::max(val, bottom),
            top);
}

void TransferWidget::_SetOpacitySlice(int x, int y)
{
    float opacityVal = (float)(y-m_rect.y) / (float)(m_rect.w);
    opacityVal = clamp(opacityVal, 0.0f, 1.0f);

    const int idx = (int) (
                (float)m_opacitySlices
            * ( (float)(x-m_rect.x) / (float)m_rect.z )
        );
    if ((idx >= 0) && (idx < (int)m_opacityValues.size()))
    {
        m_opacityValues[idx] = opacityVal;
        _UploadOpacityTex();
    }
}

void TransferWidget::_UploadOpacityTex()
{
    ///@todo Only re-create this when the size changes

    glDeleteTextures(1, &m_opacityTex);
    glGenTextures(1, &m_opacityTex);
    glBindTexture(GL_TEXTURE_2D, m_opacityTex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_LUMINANCE,
        m_opacityValues.size(),
        1, // Simulate a 1D texture
        0,
        GL_LUMINANCE,
        GL_FLOAT,
        &m_opacityValues[0]);
}

void TransferWidget::_UploadColorTex()
{
    glDeleteTextures(1, &m_colorTex);
    glGenTextures(1, &m_colorTex);
    glBindTexture(GL_TEXTURE_2D, m_colorTex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);

    const GLfloat cols[] = {
        1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
    };
    const int sz = sizeof(cols)/(3*sizeof(GLfloat));
    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_RGB,
        sz,
        1, // Simulate a 1D texture
        0,
        GL_RGB,
        GL_FLOAT,
        &cols);
}

bool TransferWidget::OnMouseClick(int x, int y)
{
    if (pointInsideRect(x, y, m_rect))
    {
        _SetOpacitySlice(x,y);
        return true;
    }
    return false;
}

bool TransferWidget::OnMouseMove (int x, int y)
{
    /// Function should only be called when the first click was within the widget.
    {
        _SetOpacitySlice(x,y);
        return true;
    }
    return false;
}

///@todo No more immediate mode
void TransferWidget::Display() const
{
    const glm::ivec4& r = GetRect();
    const unsigned int numSlices = GetOpacitySlices();
    const float sliceWidth = (float)r.z / (float)numSlices;

    // Draw the color texture behind the sliding bars
    glColor4f(1,1,1,1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_colorTex);
    glBegin(GL_QUADS);
    {
        glTexCoord2f(0.0f, 0.0f);
        glVertex3i(r.x, r.y, -1);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3i(r.x+r.z, r.y, -1);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3i(r.x+r.z, r.y+r.w, -1);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3i(r.x, r.y+r.w, -1);
    }
    glEnd();

    // Draw a series of alpha blended window shades over the slices of histogram.
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (unsigned int i=0; i<numSlices; ++i)
    {
        const int xmin = r.x + (int)((float) i   *sliceWidth);
        const int xmax = r.x + (int)((float)(i+1)*sliceWidth);
        const float opacity = m_opacityValues[i];
        const int yPx = (int)( (float)r.w * opacity );
        glColor4f(0.5f, 0.5f, 0.5f, 1.0f - opacity);
        glBegin(GL_QUADS);
        glVertex2i(xmin, r.y+r.w);
        glVertex2i(xmax, r.y+r.w);
        glVertex2i(xmax, r.y+yPx);
        glVertex2i(xmin, r.y+yPx);
        glEnd();
    }
    glDisable(GL_BLEND);


    // Draw opacity sampler strip above histo rect
    glEnable(GL_TEXTURE_2D);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, m_opacityTex);
    glBegin(GL_QUADS);
    {
        const int osHeight = 30;

        glTexCoord2f(0.0f, 0.0f);
        glVertex2i(r.x    , r.y+r.w);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2i(r.x+r.z, r.y+r.w);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2i(r.x+r.z, r.y+r.w + osHeight);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2i(r.x    , r.y+r.w + osHeight);
    }
    glEnd();
    
    glBindTexture(GL_TEXTURE_2D, m_colorTex);
    glBegin(GL_QUADS);
    {
        const int osHeight = 30;

        glTexCoord2f(0.0f, 0.0f);
        glVertex2i(r.x    , r.y+r.w + osHeight);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2i(r.x+r.z, r.y+r.w + osHeight);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2i(r.x+r.z, r.y+r.w + 2*osHeight);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2i(r.x    , r.y+r.w + 2*osHeight);
    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}
