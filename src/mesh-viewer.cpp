//--------------------------------------------------
// Author: Keith Mburu
// Date: 2/25/2023
// Description: Visualizes triangle meshes interactively in 3D
//--------------------------------------------------

#include <cmath>
#include <string>
#include <vector>
#include "agl/window.h"
#include "plymesh.h"
#include "osutils.h"

using namespace std;
using namespace glm;
using namespace agl;

class MeshViewer : public Window {
public:
   MeshViewer() : Window() {
      _radius = _viewVolumeSide = 10;
      _azimuth = 0;
      _elevation = 0;
      _lookPos = vec3(0, 0, 0);
      _up = vec3(0, 1, 0);
      _lightPos = vec3(_viewVolumeSide, _viewVolumeSide, _viewVolumeSide);
      _lightColor = vec3(1.0, 1.0, 1.0);
      _leftClick = false;
      _shiftKeysPressed = 0;
      _fileNames = GetFilenamesInDir("../models", "ply");
      _shaderNames = {"test", "unlit", "normals", "phong-vertex", "phong-pixel", "billboard", "cubemap", "lines", "text", "toon"};
      _currentFileIdx = _currentShaderIdx = 0;
      cout << "Current model: " << _fileNames[_currentFileIdx] << endl;
      cout << "Current shader: " << _shaderNames[_currentShaderIdx] << endl;
      _mesh = PLYMesh("../models/" + _fileNames[_currentFileIdx]);
   }

   void setup() {
      for (string shaderName : _shaderNames) {
         renderer.loadShader(shaderName, "../shaders/" + shaderName + ".vs", "../shaders/" + shaderName + ".fs");
      }
   }

   void mouseMotion(int x, int y, int dx, int dy) {
      if (_leftClick) {
         if (_shiftKeysPressed) {
            scroll(dx, dy); // zoom in or out
         } else {
            float ONE_DEG = 0.017; // one degree in radians
            _elevation += dy * (M_PI / 180);
            if (_elevation > (M_PI / 2) - ONE_DEG) {
               _elevation = (M_PI / 2) - ONE_DEG;
            } else if (_elevation < -((M_PI / 2) - ONE_DEG)) {
               _elevation = -((M_PI / 2) - ONE_DEG);
            }
            _azimuth -= dx * (M_PI / 180);
            if (_azimuth > 2 * M_PI) {
               _azimuth = 0;
            } else if (_azimuth < 0) {
               _azimuth = 2 * M_PI;
            }
            // cout << "elevation " << _elevation << endl;
            // cout << "azimuth " << _azimuth << endl;
         }
      }
   }

   void mouseDown(int button, int mods) {
      if (button == GLFW_MOUSE_BUTTON_LEFT) {
         _leftClick = true;
      }
   }

   void mouseUp(int button, int mods) {
      if (button == GLFW_MOUSE_BUTTON_LEFT) {
         _leftClick = false;
      }
   }

   void scroll(float dx, float dy) {
      _radius -= dy;
      if (_radius < 1) {
         _radius = 1;
      }
      _azimuth += dx * (M_PI / 180);
      if (_azimuth > 2 * M_PI) {
         _azimuth = 0;
      } else if (_azimuth < 0) {
         _azimuth = 2 * M_PI;
      }
      // cout << "radius " << _radius << endl;
      // cout << "azimuth " << _azimuth << endl;
   }

   void keyDown(int key, int mods) {
      if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
         _shiftKeysPressed++;
      }
   }

   void keyUp(int key, int mods) {
      if (key == GLFW_KEY_N || key == GLFW_KEY_P) {
         if (key == GLFW_KEY_N) {
            _currentFileIdx = (_currentFileIdx + 1) % _fileNames.size();
            cout << "Next model: " << _fileNames[_currentFileIdx] << endl;
         } else if (key == GLFW_KEY_P) {
            _currentFileIdx--;
            if (_currentFileIdx == -1) {
               _currentFileIdx = _fileNames.size() - 1;
            }
            cout << "Previous model: " << _fileNames[_currentFileIdx] << endl;
         }
         _radius = _viewVolumeSide;
         _azimuth = 0;
         _elevation = 0;
         _mesh = PLYMesh("../models/" + _fileNames[_currentFileIdx]);
      } else if (key == GLFW_KEY_S) {
         _currentShaderIdx = (_currentShaderIdx + 1) % _shaderNames.size();
         _currentShader = _shaderNames[_currentShaderIdx];
         cout << "Next shader: " << _currentShader << endl;
      }
      if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
         _shiftKeysPressed--;
      }
   }

   void draw() {
      // activates shader with given name
      renderer.beginShader(_shaderNames[_currentShaderIdx]); 

      renderer.setUniform("ViewMatrix", renderer.viewMatrix());
      renderer.setUniform("ProjMatrix", renderer.projectionMatrix());

      renderer.setUniform("lightPos", _lightPos.x, _lightPos.y, _lightPos.z);
      renderer.setUniform("lightColor", _lightColor.x, _lightColor.y, _lightColor.z);
      renderer.setUniform("eyePos", _camPos);

      float ka = 0.1, kd = 0.7, ks = 0.6;
      float phongExp = 50.0;

      renderer.setUniform("ka", ka);
      renderer.setUniform("kd", kd);
      renderer.setUniform("ks", ks);
      renderer.setUniform("phongExp", phongExp);

      renderer.setUniform("time", elapsedTime());

      float aspect = ((float) width()) / height();
      renderer.perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);

      vec3 minBounds = _mesh.minBounds();
      vec3 maxBounds = _mesh.maxBounds();

      float windowX = abs(minBounds[0]) + (maxBounds[0]);
      float windowY = abs(minBounds[1]) + (maxBounds[1]);
      float windowZ = abs(minBounds[2]) + (maxBounds[2]);
      float maxDimension = std::max(std::max(windowX, windowY), windowZ);

      renderer.scale(vec3(_viewVolumeSide / maxDimension, _viewVolumeSide / maxDimension, _viewVolumeSide / maxDimension));

      float centroidX = (minBounds[0] + maxBounds[0]) / 2.0f;
      float centroidY = (minBounds[1] + maxBounds[1]) / 2.0f;
      float centroidZ = (minBounds[2] + maxBounds[2]) / 2.0f;
      renderer.translate(vec3(-centroidX, -centroidY, -centroidZ));

      float camPosX = _radius * sin(_azimuth) * cos(_elevation);
      float camPosY = _radius * sin(_elevation);
      float camPosZ = _radius * cos(_azimuth) * cos(_elevation);
      _camPos = vec3(camPosX, camPosY, camPosZ);
      renderer.lookAt(_camPos, _lookPos, _up);

      renderer.mesh(_mesh);
      // renderer.cube(); // for debugging!

      renderer.endShader();
   }

protected:
   PLYMesh _mesh;
   int _viewVolumeSide;
   float _radius;
   float _azimuth;
   float _elevation;
   vec3 _camPos;
   vec3 _lookPos;
   vec3 _up;
   vec3 _lightPos;
   vec3 _lightColor;
   bool _leftClick;
   int _shiftKeysPressed;
   vector<string> _fileNames;
   int _currentFileIdx;
   int _currentShaderIdx;
   string _currentShader;
   vector<string> _shaderNames;
};

int main(int argc, char** argv)
{
   MeshViewer viewer;
   viewer.run();
   return 0;
}

