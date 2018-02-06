#version 330 core

//Input buffers.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

//Output Buffers.
out vec2 UV;

//Uniform values stay constant for the whole mesh.
uniform mat4 MVP;

void main() {
	gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
	
	//This is passed to the frag shader for interpolation.
        UV = vertexUV;
}
