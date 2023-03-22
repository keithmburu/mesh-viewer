#version 400

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormals;

uniform int numFaces;
uniform float time;
uniform mat4 MVP;

void main() {
    vec3 pos = vPos + 0.5 * vNormals * sin(3.14 * time / 2 * numFaces / 5000) * length(vPos - vec3(0));
    gl_Position = MVP * vec4(pos, 1.0);
}