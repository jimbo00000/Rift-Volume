Rift-Volume
============

Volume Rendering for the Oculus Rift,
plus some floating in-world UI panes.


## Portability 
 - Linux, MacOS, Windows  
 - NVIDIA, AMD

## Dependencies 
 - [CMake](http://www.cmake.org/) (for building)
 - [GLFW](http://www.glfw.org/download.html), [SDL2](https://www.libsdl.org/download-2.0.php) or [SFML](http://www.sfml-dev.org/download.php)
 - [GLEW](http://glew.sourceforge.net/)
 - [GLM](http://glm.g-truc.net/0.9.6/index.html)
 - [Oculus SDK](https://developer.oculus.com/downloads/) (optional)
 - [Sixense SDK](http://sixense.com/windowssdkdownload) (optional)
 - [AntTweakbar](http://anttweakbar.sourceforge.net/doc/tools:anttweakbar:download) (optional)

I set up my local build environment with libraries installed under a single directory(**C:/lib** on Windows, **~/lib** on Linux, **~/Development** on MacOS). This location can be changed in cmake-gui by modifying the **LIBS_HOME** variable or by editing it in CMakeLists.txt directly.

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
