# RaytracerSDL

Port of [@Lightning2X](https://github.com/Lightning2X)'s and [@Krien](https://github.com/Krien)'s old 2018 C# Raytracer in C++. 

Code ported by [@Krien](https://github.com/Krien), in a few days. So some functionality is missing such as camera movement or mirrors.

Code then optimized for the INFOMOV course by [@Lightning2X](https://github.com/Lightning2X) and [@PhoenixThrough](https://github.com/PhoenixThrough). Leading up to a 26x performance improvement compared to the original. Optimization was done primarily through SIMD and multithreading. Using a lot of early outs in SIMD dramatically speeds up the code in multiple places. This code is still not fully optimal (what code ever is?) so if you see further improvement feel free to send me a message. Switching between the old and new code is possible through setting the SIMD definition to 0 or 1 in precomp.h.

See the table below for detailed results:
| STAT     | AVG    | MIN   | MAX   |
|----------|--------|-------|-------|
| NEW      | 126.63 | 96.64 | 154.1 |
| OLD      | 5.03   | 4.91  | 5.09  |
| X FASTER | 25     | 20    | 30    |

# RUNNING THE CODE
This repo (currently) only contains the logic itself. This will NOT compile without adding libraries and setting a few settings. The instructions for this are described below

You first need to install SDL on your system from [Here](https://www.libsdl.org/release/SDL2-2.0.22-win32-x64.zip) (add it in your Program Files and add to PATH).

You then need to grab 3 libraries:
1. [SDL's Development Library](https://www.libsdl.org/release/SDL2-devel-2.0.22-VC.zip)
2. [Agner Fog's Vector Class](https://github.com/vectorclass)
3. [Agner Fog's Vec3Df from vector class add ons](https://github.com/vectorclass/add-on/tree/master/vector3d)

You then need to place SDL's include, vector class and vec3d in Raytracer/deps/include:
![image](https://user-images.githubusercontent.com/32514161/175519700-cdeb9178-7502-4bf6-ab24-7d524d755bb4.png)  

Finally, place SDL's lib/x64 from the development library in Raytracer/deps/lib

Then include these folders in VS2022 by going to Raytracer -> Properties -> VC++ Directories. Now add all the 4 folders from ./include and ./dep and the program should run.

