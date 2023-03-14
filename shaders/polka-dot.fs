#version 400

in vec4 localPosition;
out vec4 FragColor;

void main()
{
   vec2 d = mod(localPosition.xy, 1.0) - 0.5;
   float t = smoothstep(length(d), 0, 0.1);

   FragColor = vec4(mix(vec3(1), vec3(1, 0, 0), t), 1.0);
}
