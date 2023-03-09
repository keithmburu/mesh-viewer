#version 400

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTextureCoords;

uniform mat3 NormalMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform bool HasUV;

uniform mat4 ViewMatrix;
uniform mat4 ProjMatrix;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 eyePos;
uniform float ka, kd, ks;
uniform float phongExp;

out vec3 color;

void main()
{
   vec4 pos = ModelViewMatrix * vec4(vPos, 1.0);
   vec4 eyePos = ModelViewMatrix * vec4(eyePos, 1.0);
   vec4 lightPos = ViewMatrix * vec4(lightPos, 1.0);
   vec3 normalVec = NormalMatrix * vNormal;

   vec3 eyeVec = normalize(eyePos.xyz - pos.xyz);
   vec3 lightVec = normalize(lightPos.xyz - pos.xyz);

   gl_Position = ProjMatrix * pos;
   
   vec3 n = normalize(normalVec);
   vec3 l = normalize(lightVec);
   vec3 v = normalize(eyeVec);
   vec3 r = normalize((2 * max(0.0, dot(n, l)) * n) - l);

   color = ka * lightColor;
   color += kd * lightColor * max(0.0, dot(n, l));
   color += ks * lightColor * pow(max(0.0, dot(v, r)), phongExp);
}