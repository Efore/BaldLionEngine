#type vertex
#version 330 core
layout (location = 0) in vec3 vertex_position;

out vec3 TexCoords;

uniform mat4 u_viewProjection;  
uniform mat4 u_transform;
		
void main()
{
    TexCoords = vertex_position;
    vec4 pos = u_viewProjection * u_transform * vec4(vertex_position, 1.0);
    gl_Position = pos.xyww;
}  

#type fragment
#version 330 core

in vec3 TexCoords;

out vec4 fs_color;

uniform samplerCube skybox;

void main()
{    
    fs_color = texture(skybox, TexCoords);
}