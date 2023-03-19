#version 400

in vec2 uv;
in vec4 color;

uniform sampler2D billboardImage;

out vec4 FragColor;

void main()
{
  FragColor = color * texture(billboardImage, uv);
}
