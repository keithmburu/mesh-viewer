#version 400

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vUV;

uniform mat3 NormalMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform bool HasUV;

uniform mat4 ViewMatrix;
uniform vec3 lightPos;
uniform vec3 eyePos;

out vec3 color;

void main()
{
   gl_Position = MVP * vec4(vPos, 1.0);

   vec4 pos = ModelViewMatrix * vec4(vPos, 1.0);
   vec4 eyePos = ModelViewMatrix * vec4(eyePos, 1.0);
   vec4 lightPos = ViewMatrix * vec4(lightPos, 1.0);
   vec3 normalVec = NormalMatrix * vNormal;
   vec3 eyeVec = normalize(eyePos.xyz - pos.xyz);
   vec3 lightVec = normalize(lightPos.xyz - pos.xyz);

   vec3 n = normalize(normalVec);
   vec3 l = normalize(lightVec);

   float thresh = 0.1;
   if (abs(dot(eyeVec, normalVec)) < 0.25) {
      color = vec3(1.0, 1.0, 1.0);
   } else if (dot(n, l) > 0.9) {
    color = vec3(1.0, 0.4, 0.0);
   } else if (dot(n, l) > 0.4) {
    color = vec3(1.0, 0.7, 0.0);
   } else {
    color = vec3(1.0, 1.0, 0.0);
   }
}
