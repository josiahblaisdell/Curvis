#version 400 compatibility 

out vec4 fFragColor;
in vec2 vTexCoords;

uniform sampler2D uScreenTexture;

void main()
{ 
    fFragColor = texture(uScreenTexture, vTexCoords);
}