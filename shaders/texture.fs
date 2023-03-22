#version 400

in vec2 uv;
flat in int useTextures;

uniform sampler2D textureImg;

out vec4 FragColor;

void main()
{
  if (useTextures == 1) {
    FragColor = texture(textureImg, uv);
  } else {
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
  }
}
