#version 400

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormals;
layout (location = 2) in vec2 vTextureCoords;

uniform mat3 NormalMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform bool HasUV;
uniform float time;

out vec4 localPosition;

void main()
{
   vec3 pos = vPos + 0.05 * vNormals * sin(10 * time + vPos.x * 100.0);
   gl_Position = localPosition = MVP * vec4(pos, 1.0);
}
