// PaneScene.h

#pragma once

#ifdef _WIN32
#  define WINDOWS_LEAN_AND_MEAN
#  define NOMINMAX
#  include <windows.h>
#endif
#include <stdlib.h>
#include <vector>
#include <GL/glew.h>

#include <glm/glm.hpp>

#include "IScene.h"
#include "ShaderWithVariables.h"
#include "FBO.h"

#include "Pane.h"
#include "VirtualTrackball.h"
#include "Timer.h"

///@brief 
class PaneScene : public IScene
{
public:
    PaneScene(bool chassisLocal=false);
    virtual ~PaneScene();

    virtual void initGL();
    virtual void timestep(double absTime, double dt);
    virtual void RenderPrePass() const;
    virtual void RenderForOneEye(const float* pMview, const float* pPersp) const;

    virtual std::vector<Transformation*> GetTransformationPointers();
    virtual void ResetTransformation();

    virtual bool SendMouseMotion(int x, int y);
    virtual void SendMouseClick(int state);
    virtual void SendHmdTap();
    virtual void SetHoldingFlag(int state);

    virtual void SetFlyingMousePointer(FlyingMouse* pFM) { m_pFm = pFM; }
    virtual void SetHmdPositionPointer(glm::vec3* pRo) { m_pHmdRo = pRo; }
    virtual void SetHmdDirectionPointer(glm::vec3* pRd) { m_pHmdRd = pRd; }
    virtual void SetChassisTransformation(glm::mat4 tx) { m_chassisTransformCopy = tx; }

protected:
    virtual void _InitPlaneAttributes();
    virtual void DrawScene(
        const glm::mat4& modelview,
        const glm::mat4& projection) const;

    virtual bool _GetFlyingMouseRightHandPaneRayIntersectionCoordinates(Pane* pPane, glm::vec2& planePt, float& tParam);
    virtual bool _GetHmdViewRayIntersectionCoordinates(Pane* pPane, glm::vec2& planePt, float& tParam);
    virtual void _SetHeldPanePositionAndOrientation(Pane* pP);

    FlyingMouse* m_pFm;
    glm::vec3* m_pHmdRo;
    glm::vec3* m_pHmdRd;
    glm::mat4 m_chassisTransformCopy; // updated per-frame
    ShaderWithVariables m_paneShader;

public:
    std::vector<Pane*> m_panes;
    std::vector<glm::vec3> m_panePts;
    Timer m_mouseMotionCooldown;

private: // Disallow copy ctor and assignment operator
    PaneScene(const PaneScene&);
    PaneScene& operator=(const PaneScene&);
};
