# OpenGLRenderer
An OpenGL "Rendering Engine", as the kids are calling it

I feel like I really need to preface this by saying that yes, it runs and does some neat things, but I also wrote this while I was in university, as a universty graphics course project (albeit wildly extended beyond the assignment parameters), and so it is bad in almost every conceivable way beyond successfully rendering a few objects.

Notable features:
- Perspective camera (WASD to move, arrow keys to rotate, mouselook enabled)
- Object transformations over time (theoretically; there used to be a spinning cube. Apparently I made this render sponza at some point?)
- Shaders and models loaded from disk (notably, shaders are not precompiled)
- Texture mapping
- Extremely rudimentary point lighting
- Cubemapped shadows

Build instructions:
- This has only been tested to build with Visual Studio 2019 in x64 Debug/Release; I can't guarantee it will build correctly on other platforms.
