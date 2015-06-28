// DicomLoader.h

#pragma once

#if defined(WIN32)
#include <windows.h>
#endif

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h" 
#include "dcmtk/dcmimgle/dcmimage.h"

class DicomLoader
{
private:
    DicomLoader(); // Do not call
public:
    DicomLoader(const char* filename);
    ~DicomLoader();

    void LoadDICOMFile(const char* filename);
    void GenerateGLTexture();

    void PrintDICOMInfo() const;

//protected:
    DcmFileFormat* m_fileformat;
    DcmDataset*    m_dataset;

    std::string    m_patientsName;

    void*    m_pixelData;
    glm::ivec3 m_dim;
    int      m_pixelPadding;
    GLuint   m_3dtex;
    GLuint   m_3dtexGr;
};
