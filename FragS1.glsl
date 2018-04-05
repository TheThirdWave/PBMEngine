#version 330 core

//Input data.
//in vec3 fragmentColor;
in vec2 UV;

//Output.
out vec3 color;

//Values that stay constant for the whole mesh.
uniform sampler2D texSampler;

void main()
{
        color = vec3(1, 1, 1);//texture(texSampler, UV).rgb;
}
