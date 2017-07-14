README.txt
Author: Keegan van der Laag (jkvander@uvic.ca)

Description: Rendering engine which uses OpenGL 4.3 to draw arbitrary models, with textures and normals, using Phong shading from a single point light source.

- Renders a cube
- Implements basic camera controls using the mouse and keyboard
- Transforms the cube from model space to clip space using a ModelViewProjection matrix (actually the multiplication of Model, View, and Projection matrices)
- Shades the cube using a single point light source using Phong shading
- Renders a floor (and room) around the cube
- Renders a second cube with a different material
- Both cubes rotate in opposite directions around the Y-axis
- Loads and renders room and cube objects from Wavefront OBJ files using TinyObjLoader
- Renders point light shadows of all objects, projected from the point of view of the light source onto a cube map and sampled
