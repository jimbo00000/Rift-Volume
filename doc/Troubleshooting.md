# Troubleshooting


## Common build issues:

### Cannot find g_shaders.h  

```
Rift-Volume\src\Util\GL\ShaderFunctions.cpp(21): fatal error C1083: Cannot open include file: 'g_shaders.h': No such file or directory
```

#### Solution
`python` needs to be installed and in your path. When CMakeLists.txt configures the build, `tools/hardcode_shaders.py` runs and generates the file g_shaders.h


### Cannot find DCMTK

```
Rift-Volume\src\Dicom\DicomLoader.h(12): fatal error C1083: Cannot open include file: 'dcmtk/config/osconfig.h': No such file or directory
```

#### Solution

See [BuildingDCMTK.md](BuildingDCMTK.md): obtain and build [OFFIS's DCMTK DICOM Toolkit](http://git.dcmtk.org/web/), and configure **DCMTK\_DEBUG** or **DCMTK\_RELEASE** in CMakeLists.txt(or `cmake-gui`) to point to the root of its path. Also, make sure **USE\_DCMTK** is turned on (as it is by default).  

