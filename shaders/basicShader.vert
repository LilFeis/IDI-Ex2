#version 330 core

in vec3 vertex;
in vec3 color;

uniform mat4 TG;
uniform mat4 Proj;
uniform mat4 View;
uniform int Porta;

out vec3 fcolor;

void main() {
    if (Porta == 1) fcolor = vec3(1.0f, 0.0f, 0.0f);
    else fcolor = color;
    gl_Position = Proj * View * TG * vec4 (vertex, 1.0);
}
