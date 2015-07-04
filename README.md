Rift-Volume
============

Volume Rendering for the Oculus Rift,
plus some floating in-world UI panes.


## Portability 
 - Linux, MacOS, Windows  
 - NVIDIA, AMD

## Dependencies 
 - [Offis DCMTK](http://git.dcmtk.org/web/) - get the latest from git and see below for instructions
 - [CMake](http://www.cmake.org/) (for building)
 - [GLFW](http://www.glfw.org/download.html), [SDL2](https://www.libsdl.org/download-2.0.php) or [SFML](http://www.sfml-dev.org/download.php)
 - [GLEW](http://glew.sourceforge.net/)
 - [GLM](http://glm.g-truc.net/0.9.6/index.html)
 - [Oculus SDK](https://developer.oculus.com/downloads/) (optional)
 - [Sixense SDK](http://sixense.com/windowssdkdownload) (optional)
 - [AntTweakbar](http://anttweakbar.sourceforge.net/doc/tools:anttweakbar:download) (optional)

I set up my local build environment with libraries installed under a single directory(**C:/lib** on Windows, **~/lib** on Linux, **~/Development** on MacOS). This location can be changed in cmake-gui by modifying the **LIBS_HOME** variable or by editing it in CMakeLists.txt directly.

### Building Offis DCMTK
#### Windows
Use CMake for an out-of-source build. Either run Visual Studio as Administrator or manually run the post-build step of the INSTALL target:  

     "C:\Program Files (x86)\CMake\bin\cmake.exe" -DBUILD_TYPE=$(Configuration) -P cmake_install.cmake
Be sure to substitute **Debug** or **Release** for **$(Configuration)**. You can make a separate out-of-source build for the debug libraries.

#### Linux
Use the standard build-from-source invocation with an extra install   target:

    ./configure
    make
    make install
    make install-libs
Installs headers and libs to **/usr/local**.


## Features 
 - OVR SDK and Client rendering paths  
 - Adaptive render buffer resolution scaling to ensure fastest possible frame rate  
 - Camera frustum highlighting when headset approaches limits of tracking area  
 - Auxiliary window with AntTweakbar controls(toggle with backtick(`) press)  
 - Tap HMD to hide Health and Safety warning  
 - Mouse click to teleport in Scene, wheel tilt for "comfort mode" rotation  
 - Sixense SDK Hydra support  
 - Keyboard and gamepad world motion support  
 - Interchangeable GLFW, SDL2 and SFML backends  
