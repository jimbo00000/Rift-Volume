// TransferWidget.h

#pragma once

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>

class TransferWidget
{
public:
    TransferWidget();
    virtual ~TransferWidget();

    void initGL();
    void Display() const;

    bool OnMouseClick(int x, int y);
    bool OnMouseMove (int x, int y);

    void IncreaseSlices();
    void DecreaseSlices();
    void SetRect(const glm::ivec4& r) { m_rect = r; }

    glm::ivec4 GetRect() const { return m_rect; }
    unsigned int GetOpacitySlices() const { return m_opacitySlices; }
    GLuint* GetOpacityTexPointer() { return &m_opacityTex; }
    GLuint* GetColorTexPointer() { return &m_colorTex; }

protected:
    void _SetOpacitySlice(int x, int y);
    void _UploadOpacityTex();
    void _UploadColorTex();

    glm::ivec4 m_rect;
    unsigned int m_opacitySlices;
    std::vector<float> m_opacityValues;
    GLuint m_opacityTex;
    GLuint m_colorTex;

private: // Disallow copy ctor and assignment operator
    TransferWidget(const TransferWidget&);
    TransferWidget& operator=(const TransferWidget&);
};
