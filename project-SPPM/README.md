# Photon Mapping

Final project of cs171 computer graphics.

## How to use

You can modify parameters in main.cpp main() function.

1. The first parameter of scene->render() is the round number to execute, and the second is the number of photons. The default is (600,200000), more rounds would lead to better performance.
2. The size of the output picture is 800\*600 by default.
3. Coding is done on Windows10, using compiler VS2022 amd64.
4. We use eigen3, stb_image, json. All added in libs directory.
5. You may want to change the file path in main.cpp.