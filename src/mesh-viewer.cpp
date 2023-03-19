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
      _modelFileNames = GetFilenamesInDir("../models", "ply");
      vector<string> _shaderFileNames = GetFilenamesInDir("../shaders/", "vs");
      // remove file extensions
      std::transform(_shaderFileNames.begin(), _shaderFileNames.end(), std::back_inserter(_shaderNames), [](string name) {
        return name.substr(0, name.size() - 3);
      });
      _currentModelIdx = _currentShaderIdx = _currentTextureIdx = _currentCubemapIdx = 0;
      cout << "Current model: " << _modelFileNames[_currentModelIdx] << endl;
      cout << "Current shader: " << _shaderNames[_currentShaderIdx] << endl;
      _textureNames = GetFilenamesInDir("../textures/" + _modelFileNames[_currentModelIdx].substr(0, _modelFileNames[_currentModelIdx].size() - 4), ".");
      _mesh = PLYMesh("../models/" + _modelFileNames[_currentModelIdx]);
      _animatedLight = true;
      if (_mesh.hasUV() && _textureNames.size() != 0) {
         // clean up GetFilenamesInDir() output
         _textureNames.erase(_textureNames.begin(), _textureNames.begin() + 2);
         cout << "Current texture: " << _textureNames[_currentTextureIdx] << endl;
         _textureSlot = ((_currentModelIdx + 1) * 10) + _currentTextureIdx;
         renderer.loadTexture("textureImg", "../textures/" + _modelFileNames[_currentModelIdx].substr(0, _modelFileNames[_currentModelIdx].size() - 4) + "/" + _textureNames[_currentTextureIdx], _textureSlot);
      } else {
         cout << "(No textures)" << endl;
      }
      _cubemapNames = GetFilenamesInDir("../cubemaps/", "cubemap");
      if (_cubemapNames.size() != 0) {
         cout << "Current cubemap: " << _cubemapNames[_currentCubemapIdx] << endl;
         renderer.loadCubemap("cubemap", "../cubemaps/" + _cubemapNames[_currentCubemapIdx], 0);
      }
      _animatedLight = _animatedEye = false;
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
            _currentModelIdx = (_currentModelIdx + 1) % _modelFileNames.size();
            cout << "Next model: " << _modelFileNames[_currentModelIdx] << endl;
         } else if (key == GLFW_KEY_P) {
            _currentModelIdx--;
            if (_currentModelIdx == -1) {
               _currentModelIdx = _modelFileNames.size() - 1;
            }
            cout << "Previous model: " << _modelFileNames[_currentModelIdx] << endl;
         }
         _radius = _viewVolumeSide;
         _azimuth = 0;
         _elevation = 0;
         _textureNames = GetFilenamesInDir("../textures/" + _modelFileNames[_currentModelIdx].substr(0, _modelFileNames[_currentModelIdx].size() - 4), ".");
         _mesh = PLYMesh("../models/" + _modelFileNames[_currentModelIdx]);
         if (_mesh.hasUV() && _textureNames.size() != 0) {
            _textureNames.erase(_textureNames.begin(), _textureNames.begin() + 2);
            cout << "Current texture: " << _textureNames[_currentTextureIdx] << endl;
            glDisable(GL_TEXTURE0 + _textureSlot);
            _textureSlot = ((_currentModelIdx + 1) * 10) + _currentTextureIdx;
            renderer.loadTexture("textureImg", "../textures/" + _modelFileNames[_currentModelIdx].substr(0, _modelFileNames[_currentModelIdx].size() - 4) + "/" + _textureNames[_currentTextureIdx], _textureSlot);
         } else {
            cout << "(No textures)" << endl;
         }
      } else if (key == GLFW_KEY_S) {
         _currentShaderIdx = (_currentShaderIdx + 1) % _shaderNames.size();
         cout << "Next shader: " << _shaderNames[_currentShaderIdx] << endl;
      } else if (key == GLFW_KEY_T) {
         if (_mesh.hasUV() && _textureNames.size() != 0) {
            _currentTextureIdx = (_currentTextureIdx + 1) % _textureNames.size();
            cout << "Next texture: " << _textureNames[_currentTextureIdx] << endl;
            glDisable(GL_TEXTURE0 + _textureSlot);
            _textureSlot = ((_currentModelIdx + 1) * 10) + _currentTextureIdx;
            renderer.loadTexture("textureImg", "../textures/" + _modelFileNames[_currentModelIdx].substr(0, _modelFileNames[_currentModelIdx].size() - 4) + "/" + _textureNames[_currentTextureIdx], _textureSlot);
         }
      } else if (key == GLFW_KEY_C) {
         if (_cubemapNames.size() != 0) {
            _currentCubemapIdx = (_currentCubemapIdx + 1) % _cubemapNames.size();
            cout << "Next cubemap: " << _cubemapNames[_currentCubemapIdx] << endl;
            renderer.loadCubemap("cubemap", "../cubemaps/" + _cubemapNames[_currentCubemapIdx], 0);
         }
      } else if (key == GLFW_KEY_L) {
         _animatedLight = !_animatedLight;
      } else if (key == GLFW_KEY_E) {
         _animatedEye = !_animatedEye;
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

      renderer.setUniform("UseTextures", _mesh.hasUV() && _textureNames.size() != 0);
      if (_mesh.hasUV() && _textureNames.size() != 0) {
         renderer.texture("textureImg", "textureImg");
      }

      if (_shaderNames[_currentShaderIdx] == "cubemap" || _shaderNames[_currentShaderIdx] == "billboard") {
         renderer.cubemap("cubemap", "cubemap");
      }

      // billboard uniforms
      renderer.setUniform("CameraPos", _camPos);
      float size = _viewVolumeSide / 10;
      float rot = 0.0;
      vec3 offset = vec4(0.0);
      vec4 color = vec4(1);
      renderer.setUniform("Size", size);
      renderer.setUniform("Rot", rot);
      renderer.setUniform("Offset", offset);
      renderer.setUniform("Color", color);

      renderer.mesh(_mesh);

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
   vector<string> _modelFileNames;
   int _currentModelIdx;
   vector<string> _shaderNames;
   int _currentShaderIdx;
   vector<string> _textureNames;
   int _currentTextureIdx;
   int _textureSlot;
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

