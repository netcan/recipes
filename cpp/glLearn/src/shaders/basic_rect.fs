#version 330 core
out vec4 FragColor;
in vec3 fColor;
uniform float alpha;

void main()
{
    FragColor = vec4(fColor.r, fColor.g, alpha, 1.0);
}

