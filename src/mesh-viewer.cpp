//--------------------------------------------------
// Author: Keith Mburu
// Date: 2/25/2023
// Description: Loads PLY files in ASCII format
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
      _fileNames = GetFilenamesInDir("../models", "ply");
      _currentFileIdx = 0;
   }

   void setup() {
      _mesh = PLYMesh("../models/cube.ply");
   }

   void mouseMotion(int x, int y, int dx, int dy) {
      if (_leftClick) {
         _elevation += dy * (M_PI / 180);
         if (_elevation >= M_PI / 2) {
            _elevation = M_PI / 2;
         } else if (_elevation < -M_PI / 2) {
            _elevation = -M_PI / 2;
         }
         _azimuth -= dx * (M_PI / 180);
         if (_azimuth > 2 * M_PI) {
            _azimuth = 2 * M_PI;
         } else if (_azimuth < 0) {
            _azimuth = 0;
         }
         // cout << "elevation " << _elevation << endl;
         // cout << "azimuth " << _azimuth << endl;
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
      _radius += dy;
      if (_radius > viewVolumeSide) {
         _radius = viewVolumeSide;
      } else if (_radius < 1) {
         _radius = 1;
      }
      _azimuth += dx * (M_PI / 180);
      if (_azimuth > 2 * M_PI) {
         _azimuth = 2 * M_PI;
      } else if (_azimuth < 0) {
         _azimuth = 0;
      }
      // cout << "radius " << _radius << endl;
      // cout << "azimuth " << _azimuth << endl;
   }

   void keyUp(int key, int mods) {
      if (key == GLFW_KEY_N || key == GLFW_KEY_P) {
         if (key == GLFW_KEY_N) {
            _currentFileIdx = (_currentFileIdx + 1) % _fileNames.size();
            cout << "Next model " << _fileNames[_currentFileIdx] << endl;
         } else if (key == GLFW_KEY_P) {
            _currentFileIdx--;
            if (_currentFileIdx == -1) {
               _currentFileIdx = _fileNames.size() - 1;
            }
            cout << "Previous model " << _fileNames[_currentFileIdx] << endl;
         }
         _radius = 10.0f;
         _azimuth = M_PI;
         _elevation = 0;
         _mesh = PLYMesh("../models/" + _fileNames[_currentFileIdx]);
      }
   }

   void draw() {
      // renderer.beginShader("normals"); // activates shader with given name

      float aspect = ((float) width()) / height();
      renderer.perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);
      
      float camPosX = _radius * sin(_azimuth) * cos(_elevation);
      float camPosY = _radius * sin(_elevation);
      float camPosZ = _radius * cos(_azimuth) * cos(_elevation);
      _camPos = vec3(camPosX, camPosY, camPosZ);
      renderer.lookAt(_camPos, _lookPos, _up);

      // renderer.rotate(vec3(0,0,90));

      vec3 minBounds = _mesh.minBounds();
      vec3 maxBounds = _mesh.maxBounds();
      // cout << "x " << minBounds[0] << " " << maxBounds[0] << endl;
      // cout << "y " << minBounds[1] << " " << maxBounds[1] << endl;
      // cout << "z " << minBounds[2] << " " << maxBounds[2] << endl;

      float centroidX = (minBounds[0] + maxBounds[0]) / 2.0f;
      float centroidY = (minBounds[1] + maxBounds[1]) / 2.0f;
      float centroidZ = (minBounds[2] + maxBounds[2]) / 2.0f;
      renderer.translate(vec3(-centroidX, -centroidY, -centroidZ));
      
      float windowX = abs(minBounds[0] - centroidX) + (maxBounds[0] - centroidX);
      float windowY = abs(minBounds[1] - centroidY) + (maxBounds[1] - centroidY);
      float windowZ = abs(minBounds[2] - centroidZ) + (maxBounds[2] - centroidZ);
      float scaled = std::max(std::max(windowX, windowY), windowZ);
      // cout << "scaled " << scaled << endl;
      renderer.scale(vec3(viewVolumeSide / scaled, viewVolumeSide / scaled, viewVolumeSide / scaled));
      renderer.mesh(_mesh);
      // renderer.cube(); // for debugging!

      // renderer.endShader();
   }

protected:
   PLYMesh _mesh;
   float _radius = 10.0f;
   float _azimuth = M_PI;
   float _elevation = 0;
   vec3 _camPos;
   vec3 _lookPos = vec3(0, 0, 0);
   vec3 _up = vec3(0, 1, 0);
   bool _leftClick = false;
   int viewVolumeSide = 10;
   vector<string> _fileNames;
   int _currentFileIdx;
};

int main(int argc, char** argv)
{
   MeshViewer viewer;
   viewer.run();
   return 0;
}

