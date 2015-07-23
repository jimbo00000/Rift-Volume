// TxWidgetPane.cpp

#include "TxWidgetPane.h"

#ifdef __APPLE__
#include "opengl/gl.h"
#endif

#ifdef _WIN32
#  define WINDOWS_LEAN_AND_MEAN
#  define NOMINMAX
#  include <windows.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#include "Logger.h"

TxWidgetPane::TxWidgetPane()
: Pane()
, m_holdingMouseButton(false)
{
}

TxWidgetPane::~TxWidgetPane()
{
}

void TxWidgetPane::initGL()
{
    Pane::initGL();

    m_txWidg.initGL();
    const int hm = 50;
    const glm::ivec2 fbsz = GetFBOSize();
    const glm::ivec4 r(hm, hm, fbsz.x - 2*hm, 300);
    m_txWidg.SetRect(r);
}

void TxWidgetPane::DrawToFBO() const
{
    if (MouseCursorActive() || !m_tx.m_lockedAtClickPos)
        glClearColor(0.25f, 0.25f, 0.25f, 0.0f);
    else
        glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ///@todo Remove this fixed-function stuff
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    const glm::ivec2 fbsz = GetFBOSize();
    glOrtho(0, fbsz.x, 0, fbsz.y, -2.0f, 2.0f);

    GLint prog;
    glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
    glUseProgram(0);
    m_txWidg.Display();
    glUseProgram(prog);

    glDisable(GL_DEPTH_TEST);
    DrawCursor();
    glEnable(GL_DEPTH_TEST);
}

void TxWidgetPane::DrawPaneWithShader(
    const glm::mat4&, // modelview
    const glm::mat4&, // projection
    const ShaderWithVariables& sh) const
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_paneRenderBuffer.tex);
    glUniform1i(sh.GetUniLoc("fboTex"), 0);

    glUniform1f(sh.GetUniLoc("u_brightness"), m_cursorInPane ? 1.0f : 0.5f);

    sh.bindVAO();
    {
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    glBindVertexArray(0);
}

void TxWidgetPane::timestep(double absTime, double dt)
{
    Pane::timestep(absTime, dt);
}

void TxWidgetPane::OnMouseClick(int state, int x, int y)
{
    ///@todo This seems redundant...
    const glm::ivec2 fbsz = GetFBOSize();
    const int mx = static_cast<int>( m_pointerCoords.x * static_cast<float>(fbsz.x) );
    const int my = static_cast<int>( (1.0f-m_pointerCoords.y) * static_cast<float>(fbsz.y) );

    m_txWidg.OnMouseClick(mx, my);
    m_holdingMouseButton = (state == 1);
}

void TxWidgetPane::OnMouseMove(int x, int y)
{
    if (m_holdingMouseButton)
    {
        const glm::ivec2 fbsz = GetFBOSize();
        m_txWidg.OnMouseMove(x,fbsz.y-y);
    }
}
