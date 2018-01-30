#version 330 core

//Input data.
in vec3 fragmentColor;

//Output.
out vec3 color;

void main()
{
	color = fragmentColor;
}