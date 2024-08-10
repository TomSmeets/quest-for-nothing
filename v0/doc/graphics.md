<!-- Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl> -->
<!-- graphics.md - Learning about OpenGL, Vulkan, and designing a graphics API -->
# Graphics

- [gl_VertexIndex 1](https://www.youtube.com/watch?v=R-bjXOEQyX8)
- [gl_VertexIndex 2](https://www.youtube.com/watch?v=5zlfJW2VGLM)
- [VAO & Core Profiles](https://www.youtube.com/watch?v=JwAH_YGOcVo)
- [Debugging & API Trace](https://www.youtube.com/watch?v=Wk145_jUsBk)
- [Lighting](https://www.youtube.com/watch?v=x2FHHU50ktQ)
- [OpenGL 4.6 Core Profile Spec](https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf)
- [OpenGL 3.3 Core Profile Spec](https://registry.khronos.org/OpenGL/specs/gl/glspec33.core.pdf)


A good API does not limit itself to the implementation details. It should not leak implementation details.
To help with this, start with desiging the API first.
1. Consider the usecaes
2. Design the perfect api for these
3. Create an implementation for this api

There are two kinds of graphics in this game.
1. A 2D user interface.
2. A 3D World.

Do I want to combine these? Idk.

## UI

