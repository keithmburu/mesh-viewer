#version 400

in vec3 normalVec;
in vec3 eyeVec;
in vec3 lightVec;
in vec2 uv;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float ka, kd, ks;
uniform float phongExp;
uniform sampler2D textureImg;

out vec4 FragColor;

void main()
{
   vec3 n = normalize(normalVec);
   vec3 l = normalize(lightVec);
   vec3 v = normalize(eyeVec);

   vec3 diffuseColor;
   float thresh = 0.3;
   if (abs(dot(v, n)) < thresh) {
      diffuseColor = vec3(1.0, 1.0, 1.0);
   } else if (abs(dot(l, n)) < thresh) {
      diffuseColor = vec3(0.7, 0.4, 0.0);
   } else {
      diffuseColor = vec3(1.0, 0.6, 0.0);
   }

   vec3 color = (1.0 - kd) * lightColor * texture(textureImg, uv).xyz;
   color += kd * diffuseColor;

   FragColor = vec4(color, 1.0);
}
