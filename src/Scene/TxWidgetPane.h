// TxWidgetPane.h

#pragma once

#ifdef _WIN32
#  define WINDOWS_LEAN_AND_MEAN
#  define NOMINMAX
#  include <windows.h>
#endif

#include "Pane.h"
#include "TransferWidget.h"

///@brief 
class TxWidgetPane : public Pane
{
public:
    TxWidgetPane();
    virtual ~TxWidgetPane();

    virtual void initGL();
    virtual void timestep(double absTime, double dt);

    virtual void DrawPaneWithShader(
        const glm::mat4& modelview,
        const glm::mat4& projection,
        const ShaderWithVariables& sh) const;

    virtual void OnMouseClick(int state, int x, int y);
    virtual void OnMouseMove(int x, int y);

    virtual GLuint* GetOpacityTexPointer() { return m_txWidg.GetOpacityTexPointer(); }
    virtual GLuint* GetColorTexPointer() { return m_txWidg.GetColorTexPointer(); }

protected:
    virtual void DrawToFBO() const;

    TransferWidget m_txWidg;
    bool m_holdingMouseButton;

private: // Disallow copy ctor and assignment operator
    TxWidgetPane(const TxWidgetPane&);
    TxWidgetPane& operator=(const TxWidgetPane&);
};
