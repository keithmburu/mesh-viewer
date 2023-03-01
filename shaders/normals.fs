#version 400

in vec3 fNormal;
out vec4 FragColor;

void main()
{
   float r = ((fNormal[0] + 1) / 2) * 1.0;
   float g = ((fNormal[1] + 1) / 2) * 1.0;
   float b = ((fNormal[2] + 1) / 2) * 1.0;
   FragColor = vec4(r, g, b, 1.0);
}
