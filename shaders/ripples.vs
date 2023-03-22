#version 400

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormals;
layout (location = 2) in vec2 vTextureCoords;

uniform mat3 NormalMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform bool HasUV;
uniform int numFaces;
uniform float time;

void main()
{
   vec3 pos = vPos + 0.1 * vNormals * sin(3.14 * time + vPos.x);
   gl_Position = MVP * vec4(pos, 1.0);
}
