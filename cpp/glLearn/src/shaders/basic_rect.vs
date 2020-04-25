#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 fColor;
void main()
{
    fColor = aColor;
    // 注意gl_Position.w分量不是用作表达空间中的位置的（我们处理的是3D不是4D），而是用在所谓透视除法(Perspective Division)上
    gl_Position = vec4(aPos, 1.0);
}

