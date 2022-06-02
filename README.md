# RaytracerSDL
Again a port of my old raytracer, originally made in C# with [Lightning2X](https://github.com/Lightning2X) for a first years university project. This time ported directly to c++ with SDL. 
Note that this is a direct port and back then we were not good programmers.. The port was created quick and dirty, unoptimised and contains serious 
architectural issues. It would be better to recreate it from scratch in my opinion :). 
Which is especially the reason to make it public, to allow it to be optimised and see what optimisations work.

# NOTE
This repo only contains the logic itself. This will NOT compile without adding libraries and setting a few settings.
First of all you need to have SDL2.lib and SDL2main.lib. It also makes use of Agner Fog's library VCL and the VCL add on 
https://github.com/vectorclass and https://github.com/vectorclass/add-on/, which you will need to install by
yourself. It works on both x64 and x86, only tested on Windows.  
