#version 330 core

layout(location = 0) in vec3 aPos;   // ��ġ ������ attribute position 0�� �����ϴ�.

uniform mat4 Smatrix;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;

out vec3 inColor;

void main()
{
	gl_Position = projectionTransform * viewTransform * Smatrix * vec4(aPos, 1.0);

}