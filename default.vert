#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 vertexColor;                  // Output color to fragment shader
out vec2 TexCoord;
uniform mat4 transform;				   // Transformation uniform

void main()
{
   gl_Position =  transform * vec4(aPos, 1.0);
   vertexColor = aColor;               //Pass color to fragment shader
   TexCoord = aTexCoord;
}