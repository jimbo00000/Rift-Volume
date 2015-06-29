// DicomLoader.cpp

#include "DicomLoader.h"
#include "DicomConstants.h"

DicomLoader::DicomLoader(const char* filename)
: m_fileformat(0)
, m_dataset(0)
, m_patientsName()
, m_pixelData(0)
, m_pixelPadding(0)
, m_3dtex(0)
, m_3dtexGr(0)
{
    glm::ivec3 dim = {0,0,0};
    m_dim = dim;

    LoadDICOMFile(filename);
    GenerateGLTexture();
    PrintDICOMInfo();
}

DicomLoader::~DicomLoader()
{
    delete m_dataset;
    //delete m_fileformat;

    // TODO: glDeleteTextures
}

//
// Wrap up as much of the OFFIS library as we can in here.
// Remember to keep FileFormat and Dataset around.
//
void DicomLoader::LoadDICOMFile(const char* filename)
{
    m_fileformat = new DcmFileFormat();
    if (!m_fileformat)
        return;

    OFCondition status = m_fileformat->loadFile(filename);

    if (!status.good())
    {
        std::cerr << "Error: cannot read DICOM file (" << status.text() << ")" << std::endl;
        return;
    }

    m_dataset = m_fileformat->getDataset();
    if (!m_dataset)
        return;

    OFString patientName;


    // long int: image metadata
    long data;
    if (m_dataset->findAndGetLongInt(DCM_Columns, data).good())
    {
        m_dim.x = (int)data;
    }
    if (m_dataset->findAndGetLongInt(DCM_Rows, data).good())
    {
        m_dim.y = (int)data;
    }
    if (m_dataset->findAndGetLongInt(DCM_NumberOfFrames, data).good())
    {
        m_dim.z = (int)data;
    }


    
    // Pixel data
    const Uint16 *buf = NULL;
    unsigned long numPix;
    status = m_dataset->findAndGetUint16Array(DCM_PixelData, buf, &numPix, true);
    if (status.good())
    {
        std::cout << std::endl;
        std::cout << "numPix: " << numPix
            << std::endl << std::endl;
        int planesGuess = numPix/(m_dim.x*m_dim.y);

        if (m_dim.z != planesGuess)
        {
            std::cout << "DCM_NumberOfFrames and pixel count don't match." << std::endl;
            m_dim.z = planesGuess;
        }
    }
    else
    {
        std::cerr << "Error: cannot access DCM_PixelData!" << std::endl;
    }

    m_pixelData = (void*) buf;


    // Pixel padding
    Sint16 pixelPad;
    status = m_dataset->findAndGetSint16(DCM_PixelPaddingValue, pixelPad);
    if (status.good())
    {
        m_pixelPadding = pixelPad;
    }
    else
    {
        std::cerr << "Error: cannot access DCM_PixelPaddingValue!" << std::endl;
    }
    
#if 0
    // Grab from DiImg class instead of raw pixel array
    // Supposed to do something with windowWL...
    DicomImage diImag(m_dataset, EXS_Unknown);
    const DiPixel *dmp = diImag.getInterData();
    if (dmp)
    {
        EP_Representation er_rep = dmp->getRepresentation();
        m_pixelData = (void *)dmp->getData(); 
    }
#endif

}


// If we let m_fileformat or m_dataset fall out of scope and be deleted,
// we lose access to the m_pixelData array.
void DicomLoader::GenerateGLTexture()
{
    // TODO: preprocess loop to normalize values, perhaps with OpenCL

    glGenTextures(1, &m_3dtex);
    glBindTexture(GL_TEXTURE_3D, m_3dtex);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);

    glTexImage3D(GL_TEXTURE_3D,
                 0, 1,
                 m_dim.x, m_dim.y, m_dim.z,
                 0,
                 GL_LUMINANCE,
                 GL_SHORT,
                 m_pixelData);

    glBindTexture(GL_TEXTURE_3D, 0);

    // Again for the gradient
    glGenTextures(1, &m_3dtexGr);
    glBindTexture(GL_TEXTURE_3D, m_3dtexGr);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);

    glTexImage3D(GL_TEXTURE_3D,
                 0, 1,
                 m_dim.x, m_dim.y, m_dim.z,
                 0,
                 GL_LUMINANCE, 
                 GL_SHORT,
                 NULL);

    glBindTexture(GL_TEXTURE_3D, 0);
}

// Print DICOM info string values to stdout.
void DicomLoader::PrintDICOMInfo() const
{
    if (!m_dataset)
        return;

    OFString data;

    for (int i=0; constants[i].name != NULL; ++i)
    {
        if (m_dataset->findAndGetOFString(constants[i].tagKey, data).good())
        {
            std::cout << constants[i].name << ": " << data << std::endl;
        }
    }
}
