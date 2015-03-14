# OBJ #

OBJ is open geometry definition file what was developed by Wavefront Technologies. The full specification can be found [here](http://www.martinreddy.net/gfx/3d/OBJ.spec), but WargameEngine supports only following data:
  * **Vertex Data (v)**
  * **Texture Vertices (vt)**
  * **Vertex Normals (vn)**
  * **Faces (f)**
  * **Load [MTL Library](Models#MTL.md) (mtllib)**
  * **Apply material (usemtl)**
  * **Group Name (g)**
OBJ specification allows a single vertex to have different normals and texture verticies, that is not supported by OpenGL. In this case this verticies is duplicated.

# MTL #

MTL is a companion file for OBJ format . It specifies the properties of the material. Full specification can be found [here](http://paulbourke.net/dataformats/mtl/), but WargameEngine supports only following data:
  * **Create new material (newmtl)**
  * **Set Ambient Color (Ka)**
  * **Set Diffuse Color (Kd)**
  * **Set Specular Color (Ks)**
  * **Set Specular Exponent (Ns)**
  * **Set texture (map\_Kd)**

# WBM #

WBM is a binary format developed especially for WargameEngine that specifies both geometry and materials. It contains the following information:
  * **Vertex Data**
  * **Texture Vertices**
  * **Vertex Normals**
  * **Indexes** (allows each vertex to be drawn multiple times)
  * **Meshes** (contains information about mesh name, material and indexes)
  * **Materials** (contains ambient color, diffuse color, specular color, shiness (specular exponent) and texture name)
The mesh can only have one material. If it doesn't it will be splitted into several meshes with equal names and different materials.
The WBM file can be obtained by converting [OBJ file](Models#OBJ.md) using OBJ2WBMConverter.exe (just specify the OBJ filename as the first command line parameter or simply drag an OBJ file to OBJ2WBMConverter.exe).

# TXT companion #

TXT companion file specifies model's bounding box and scale.
Bounding box is an Axis Aligned Bounding Box ([AABB](http://en.wikipedia.org/wiki/Axis-aligned_bounding_box#Axis-aligned_minimum_bounding_box)) that is specifies by 2 points: the point that has minimum x,y and z coordinates and the point that has maximum x, y and z. So you need to specify it like this:
`box _minx_ _miny_ _minz_ _maxx_ _maxy_ _maxz_`. For example, `box -0.808902 -0.386092 0 0.815431 0.672936 2.853` means that AABB is created by 2 points: (-0.808902, -0.386092, 0) and (0.815431 0.672936 2.853). The model can have complex bounding box that consist of several AABBs. In that case just specify all of them line by line. For example, this model has a bounding box that consist of 3 AABBs:
```
box -3 2 0 3.5 4 1
box -4 0 0 3.5 2 2
box -4 -6 0 3.5 0 4
```
Model can have no Bounding Box at all. In this case you cannot select it with mouse and it cannot collide with other models and cannot be a target for LoS detection.
Scale is specified by `scale` parameter, for example:
`scale 0.5` means that model will 2 times smaller than specified by its geometry. Note that bounding box is also affected by this parameter and scales too. If this parameter is not present the model has a scale of 1.
The model can have no txt companion. In this case it has no bounding box and scale of 1.