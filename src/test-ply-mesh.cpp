//--------------------------------------------------
// Author: Aline Normoyle, Bryn Mawr College
// Date: Feb 2023
// Description: Simple test application for PLY files
//--------------------------------------------------

#include "plymesh.h"

using namespace glm;
using namespace agl;

int main() {
   PLYMesh mesh("../models/triangle.ply");

   // print vertices, normals, colors, etc
   std::cout << "This mesh has " << mesh.numVertices() << " vertices" << std::endl;
   std::cout << "Max bounds: " << mesh.maxBounds() << std::endl; 
   std::cout << "Min bounds: " << mesh.minBounds() << std::endl; 

   std::cout << std::endl << "Positions: " << std::endl;
   for (int i = 0; i < mesh.numVertices() * 3; i += 3) {
      float x = mesh.positions()[i + 0];
      float y = mesh.positions()[i + 1];
      float z = mesh.positions()[i + 2];
      std::cout << vec3(x,y,z) << std::endl;
   }

   std::cout << std::endl << "Normals: " << std::endl;
   for (int i = 0; i < mesh.numVertices() * 3; i += 3) {
      float x = mesh.normals()[i + 0];
      float y = mesh.normals()[i + 1];
      float z = mesh.normals()[i + 2];
      std::cout << vec3(x,y,z) << std::endl;
   }

   if (mesh.colors().size() != 0) {
      std::cout << std::endl << "Colors: " << std::endl;
      for (int i = 0; i < mesh.numTriangles() * 3; i += 3) {
         int r = mesh.colors()[i + 0];
         int g = mesh.colors()[i + 1];
         int b = mesh.colors()[i + 2];
         std::cout << r << " " << g << " " << b << std::endl;
      }
   }

   if (mesh.uvs().size() != 0) {
      std::cout << std::endl << "UVs: " << std::endl;
      for (int i = 0; i < mesh.numVertices() * 2; i += 2) {
         float u = mesh.uvs()[i + 0];
         float v = mesh.uvs()[i + 1];
         std::cout << u << " " << v << std::endl;
      }
   }

   std::cout << std::endl << "Indices: " << std::endl;
   for (int i = 0; i < mesh.numTriangles() * 3; i += 3) {
      int i1 = mesh.indices()[i + 0];
      int i2 = mesh.indices()[i + 1];
      int i3 = mesh.indices()[i + 2];
      std::cout << i1 << " " << i2 << " " << i3 << std::endl;
   }

   return 0;
}


