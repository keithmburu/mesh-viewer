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
#include "image.h"
#include <algorithm>
#include "unistd.h"
#include <set>

using namespace std;
using namespace glm;
using namespace agl;

class MeshViewer : public Window {
public:
   MeshViewer() : Window() {
      _radius = _viewVolumeSide = 10;
      _azimuth = _elevation = 0;
      _lookPos = vec3(0, 0, 0);
      _up = vec3(0, 1, 0);
      _lightPos = vec3(_viewVolumeSide, _viewVolumeSide, _viewVolumeSide);
      _lightColor = vec3(1.0, 1.0, 1.0);
      _leftClick = false;
      _shiftKeysPressed = 0;
      _currentModelIdx = _currentShaderIdx = _currentTextureIdx = _currentCubemapIdx = 0;
      _animatedLight = _animatedEye = false;

      vector<string> _modelFileNames = GetFilenamesInDir("../models", "ply");
      cout << endl;
      // remove file extensions
      std::transform(_modelFileNames.begin(), _modelFileNames.end(), std::back_inserter(_modelNames), [](string name) {
         return name.substr(0, name.size() - 4);
      });

      vector<string> _shaderFileNames = GetFilenamesInDir("../shaders/", "vs");
      cout << endl;
      std::transform(_shaderFileNames.begin(), _shaderFileNames.end(), std::back_inserter(_shaderNames), [](string name) {
         return name.substr(0, name.size() - 3);
      });
      _shaderNames.erase(find(_shaderNames.begin(), _shaderNames.end(), "text"));
      
      _textureFileNames = GetFilenamesInDir("../textures/" + _modelNames[_currentModelIdx], ".");
      cout << endl;

      _cubemapNames = GetFilenamesInDir("../cubemaps/", "cubemap");
      cout << endl;

      cout << "Current model: " << _modelNames[_currentModelIdx] << endl;
      cout << "Current shader: " << _shaderNames[_currentShaderIdx] << endl;
      if (useTextures(true)) {
         setupTexture(true, "Current");
      } 
      if (useCubemaps(true)) {
         setupCubemap(true, "Current");
      } 

      _mesh = PLYMesh("../models/" + _modelNames[_currentModelIdx] + ".ply");
   }

   void setup() {
      // load shaders
      for (string shaderName : _shaderNames) {
         renderer.loadShader(shaderName, "../shaders/" + shaderName + ".vs", "../shaders/" + shaderName + ".fs");
      }
   }

   void mouseMotion(int x, int y, int dx, int dy) {
      // zoom in or out
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
      // scroll up or down to zoom in or out
      _radius -= dy;
      if (_radius < 1) {
         _radius = 1;
      }
      // scroll sideways to slowly pan sideways
      _azimuth += dx * (M_PI / 180);
      if (_azimuth > 2 * M_PI) {
         _azimuth = 0;
      } else if (_azimuth < 0) {
         _azimuth = 2 * M_PI;
      }
   }

   void keyDown(int key, int mods) {
      if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
         _shiftKeysPressed++;
      }
   }

   void keyUp(int key, int mods) {
      if (key == GLFW_KEY_N || key == GLFW_KEY_P) {
         if (key == GLFW_KEY_N) {
            _currentModelIdx = (_currentModelIdx + 1) % _modelNames.size();
            cout << "\nNext model: " << _modelNames[_currentModelIdx] << endl;
         } else if (key == GLFW_KEY_P) {
            _currentModelIdx--;
            if (_currentModelIdx == -1) {
               _currentModelIdx = _modelNames.size() - 1;
            }
            cout << "\nPrevious model: " << _modelNames[_currentModelIdx] << endl;
         }
         _radius = _viewVolumeSide;
         _azimuth = _elevation = 0;
         _textureFileNames = GetFilenamesInDir("../textures/" + _modelNames[_currentModelIdx], ".");
         _mesh = PLYMesh("../models/" + _modelNames[_currentModelIdx] + ".ply");
         if (useTextures(true)) {
            setupTexture(true, "Current");
         } 
         if (useCubemaps(false)) {
            setupCubemap(false, "Current");
         }
      } else if (key == GLFW_KEY_S) {
         _currentShaderIdx = (_currentShaderIdx + 1) % _shaderNames.size();
         cout << "Next shader: " << _shaderNames[_currentShaderIdx] << endl;
         if (useTextures(true)) {
            setupTexture(false, "Current");
         } else if (useCubemaps(true)) {
            setupCubemap(false, "Current");
         } 
      } else if (key == GLFW_KEY_T) {
         if (useTextures(true)) {
           setupTexture(false, "Next");
         }
      } else if (key == GLFW_KEY_C) {
         if (useCubemaps(true)) {
            setupCubemap(false, "Next");
         }
      } else if (key == GLFW_KEY_L) {
         _animatedLight = !_animatedLight;
      } else if (key == GLFW_KEY_E) {
         _animatedEye = !_animatedEye;
      } else if (key == GLFW_KEY_X) {
         screenshot("../results/" + _modelNames[_currentModelIdx] + "-" + _shaderNames[_currentShaderIdx] + "-" + std::to_string(rand() % 10000) + ".png");
      } 
      if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
         _shiftKeysPressed--;
      }
   }

   void draw() {
      // activates shader with given name
      renderer.beginShader(_shaderNames[_currentShaderIdx]); 

      float aspect = ((float) width()) / height();
      renderer.perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);

      // center model and scale to fit view volume
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

      // set eye and light positions
      if (_animatedEye) {
         _camPos = vec3(_viewVolumeSide * cos(elapsedTime()), _viewVolumeSide * sin(elapsedTime()), _viewVolumeSide * sin(elapsedTime()));
      } else {
         float camPosX = _radius * sin(_azimuth) * cos(_elevation);
         float camPosY = _radius * sin(_elevation);
         float camPosZ = _radius * cos(_azimuth) * cos(_elevation);
         _camPos = vec3(camPosX, camPosY, camPosZ);
      }
      renderer.lookAt(_camPos, _lookPos, _up);

      if (_animatedLight) {
         _lightPos = vec3(_viewVolumeSide * cos(elapsedTime()), _viewVolumeSide * sin(elapsedTime()), _viewVolumeSide * (cos(elapsedTime()) + sin(elapsedTime())));
      } else {
         float lightPosX = _radius * sin(_azimuth + M_PI / 4) * cos(_elevation + M_PI / 4);
         float lightPosY = _radius * sin(_elevation + M_PI / 4);
         float lightPosZ = _radius * cos(_azimuth + M_PI / 4) * cos(_elevation + M_PI / 4);
         _lightPos = vec3(lightPosX, lightPosY, lightPosZ);
      }

      // phong shading uniforms
      std::set<string> reflectiveShaders = {"phong-pixel", "phong-vertex", "billboard", "toon-1", "toon-2", "glossy-black"};
      if (reflectiveShaders.find(_shaderNames[_currentShaderIdx]) != reflectiveShaders.end()) {
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
      }

      // billboard uniforms
      if (_shaderNames[_currentShaderIdx] == "billboard") {
         renderer.setUniform("CameraPos", _camPos);
         float size = _viewVolumeSide / 10;
         float rot = 0.0;
         vec3 offset = vec4(0.0);
         vec4 color = vec4(1);
         renderer.setUniform("Size", size);
         renderer.setUniform("Rot", rot);
         renderer.setUniform("Offset", offset);
         renderer.setUniform("Color", color);
      }

      // pulse shader uniforms
      if (_shaderNames[_currentShaderIdx] == "pulse") {
         renderer.setUniform("numFaces", (int) _mesh.indices().size());
      }
      
      // texture uniforms
      renderer.setUniform("UseTextures", useTextures(false));
      
      renderer.setUniform("time", elapsedTime());

      if (_shaderNames[_currentShaderIdx] == "glossy-black") {
         background(vec3(1, 1, 1));
      } else {
         background(vec3(0, 0, 0));
      }

      if (useTextures(false)) {
         renderer.texture("textureImg", _textureFileNames[_currentTextureIdx]);
      }
      if (useCubemaps(false)) {
         renderer.cubemap("cubemap", "cubemap");
      }
      if (useCubemaps(false) || _shaderNames[_currentShaderIdx] == "polka-dot") {
         renderer.skybox(_viewVolumeSide);
      }
      
      renderer.mesh(_mesh);

      renderer.endShader();
   }

   bool useTextures(bool printOut) {
      if (_shaderNames[_currentShaderIdx] == "texture") {
         if (_mesh.hasUV() && _textureFileNames.size() != 0) {
            return true;
         } else {
            if (printOut) {
               cout << "(No textures)" << endl;
            }
            return false;
         }
      } else {
         return false;
      }
   }

   void setupTexture(bool cleanUp, string adjective) {
      if (cleanUp) {
         // clean up GetFilenamesInDir() output
         _textureFileNames.erase(_textureFileNames.begin(), _textureFileNames.begin() + 2);
      }
      cout << adjective << " texture: " << _textureFileNames[_currentTextureIdx] << endl;
      if (adjective == "Next") {
         _currentTextureIdx = (_currentTextureIdx + 1) % _textureFileNames.size();
      }
      renderer.loadTexture(_textureFileNames[_currentTextureIdx], "../textures/" + _modelNames[_currentModelIdx] + "/" + _textureFileNames[_currentTextureIdx], 0);
   }

   bool useCubemaps(bool printOut) {
      if (_shaderNames[_currentShaderIdx] == "cubemap") {
         if (_cubemapNames.size() != 0) {
            return true;
         } else {
            if (printOut) {
               cout << "(No cubemaps)" << endl;
            }
            return false;
         }
      } else {
         return false;
      }
   }

   void setupCubemap(bool cleanUp, string adjective) {
      if (cleanUp) {
         _textureFileNames.erase(_textureFileNames.begin(), _textureFileNames.begin() + 2);
      }
      cout << adjective << " cubemap: " << _cubemapNames[_currentCubemapIdx] << endl;
      if (adjective == "Next") {
         _currentCubemapIdx = (_currentCubemapIdx + 1) % _cubemapNames.size();
      }
      renderer.loadCubemap("cubemap", "../cubemaps/" + _cubemapNames[_currentCubemapIdx], 1);
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
   vector<string> _modelNames;
   int _currentModelIdx;
   vector<string> _shaderNames;
   int _currentShaderIdx;
   vector<string> _textureFileNames;
   int _currentTextureIdx;
   vector<string> _cubemapNames;
   int _currentCubemapIdx;
   bool _animatedLight;
   bool _animatedEye;
};

int main(int argc, char** argv)
{
   MeshViewer viewer;
   viewer.run();
   return 0;
}

