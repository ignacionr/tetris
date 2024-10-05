cl.exe ../src/tetris.cpp ^
    /std:c++20 /EHsc ^
    /I..\vcpkg_installed\x64-windows\include\ ^
    /link /LIBPATH:..\vcpkg_installed\x64-windows\lib\ ^
    SDL2.lib ^
    /out:tetris.exe
