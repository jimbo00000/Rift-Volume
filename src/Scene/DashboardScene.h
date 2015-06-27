// DashboardScene.h

#pragma once

#ifdef _WIN32
#  define WINDOWS_LEAN_AND_MEAN
#  define NOMINMAX
#  include <windows.h>
#endif
#include <stdlib.h>
#include <GL/glew.h>

#include <glm/glm.hpp>

#include "PaneScene.h"
#include "AntPane.h"
#include "TxWidgetPane.h"

///@brief 
class DashboardScene : public PaneScene
{
public:
    DashboardScene();
    virtual ~DashboardScene();

    // This special case is for the AntTweakBar
    virtual glm::ivec2 GetAntFBOSize() const { return m_antPane.GetFBOSize(); }
    virtual void ResizeTweakbar() { m_antPane.ResizeTweakbar(); }

    // Special cases for Txfr function widget pane
    virtual GLuint* GetOpacityTexPointer() { return m_widgPane.GetOpacityTexPointer(); }
    virtual GLuint* GetColorTexPointer() { return m_widgPane.GetColorTexPointer(); }

    AntPane m_antPane;
    TxWidgetPane m_widgPane;

private: // Disallow copy ctor and assignment operator
    DashboardScene(const DashboardScene&);
    DashboardScene& operator=(const DashboardScene&);
};
