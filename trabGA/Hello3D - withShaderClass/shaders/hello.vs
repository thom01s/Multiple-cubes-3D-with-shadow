#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex_coord;

out vec3 scaledNormal;
out vec3 fragPos;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    fragPos = vec3(model * vec4(position, 1.0f));
    scaledNormal = normal;  //mat3(transpose(inverse(model))) * normal; 
    texCoord = vec2(tex_coord.x, tex_coord.y);
}