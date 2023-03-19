#version 400

layout (location = 0) in vec3 vPosition;
out vec3 ReflectDir;

uniform mat4 ModelMatrix;
uniform mat4 MVP;

void main()
{
   // ReflectDir = (ModelMatrix * vec4(vPosition, 1.0)).xyz;
   ReflectDir = vPosition;
   gl_Position = MVP * vec4(vPosition, 1.0);
}
