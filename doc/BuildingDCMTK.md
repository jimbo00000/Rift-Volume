# Building Offis DCMTK
Get the latest source from the git repo:  

     git clone git://git.dcmtk.org/dcmtk

#### Windows
Use CMake for an out-of-source build. Either run Visual Studio as Administrator or manually run the post-build step of the INSTALL target:  

     "C:\Program Files (x86)\CMake\bin\cmake.exe" -DBUILD_TYPE=$(Configuration) -P cmake_install.cmake
Be sure to substitute **Debug** or **Release** for **$(Configuration)**. You can make a separate out-of-source build for the debug libraries.

#### Linux
Use the standard build-from-source invocation with an extra install target:  

    cd dcmtk
    ./configure
    make
    make install
    make install-libs
Installs headers and libs to **/usr/local**.
