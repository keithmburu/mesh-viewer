#version 400

in vec3 normalVec;
in vec3 eyeVec;
in vec3 lightVec;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float ka, kd, ks;
uniform float phongExp;

out vec4 FragColor;

void main()
{
   vec3 n = normalize(normalVec);
   vec3 l = normalize(lightVec);
   vec3 v = normalize(eyeVec);
   vec3 r = normalize((2 * dot(n, l) * n) - l);

   vec3 color = vec3(0, 0, 0);
   color += ka * lightColor;
   color += kd * lightColor * min(0.0, dot(n, l));
   color += ks * lightColor * pow(min(0.0, dot(v, r)), phongExp);

   FragColor = vec4(color, 1.0);
}
