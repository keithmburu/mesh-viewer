#version 400

in vec2 uv;

uniform sampler2D textureImg;
out vec4 FragColor;

void main()
{
  FragColor = texture(textureImg, uv);
}
