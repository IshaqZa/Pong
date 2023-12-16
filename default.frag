#version 330 core
in vec3 vertexColor;		// Input color from vertex shader
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D myTexture;

void main()
{
   FragColor = texture(myTexture, TexCoord) * vec4(vertexColor, 1.0);  // Set the fragment color using the input color
}