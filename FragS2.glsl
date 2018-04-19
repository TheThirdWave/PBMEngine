#version 330 core

//Input data.
in vec2 UV;
//in vec3 COL;

//Output.
out vec3 color;

//Values that stay constant for the whole mesh.
uniform sampler2D texSampler;

void main()
{
        color = texture(texSampler, UV).rgb;
}
