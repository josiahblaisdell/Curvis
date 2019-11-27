#version 400 compatibility 
layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inTexCoords;

out vec2 vTexCoords;

uniform sampler2D uScreenTexture;
void main()
{
    gl_Position = vec4(inPos.x, inPos.y, 0.0, 1.0); 
    vTexCoords = inTexCoords;
}  