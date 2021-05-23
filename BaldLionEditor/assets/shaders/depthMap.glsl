#type vertex
#version 330 core
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in vec3 vertex_normal;	
layout (location = 3) in vec3 vertex_tangent;
layout (location = 4) in vec2 vertex_texcoord;

uniform mat4 u_lightViewProjectionMatrix;  
uniform mat4 u_worldTransformMatrix; 

void main()
{
    gl_Position = u_lightViewProjectionMatrix * (u_worldTransformMatrix * vec4(vertex_position, 1.0));
}  

#type fragment
#version 330 core

// Ouput data
//layout(location = 0) out float fragmentdepth;

void main(){
    // Not really needed, OpenGL does it anyway
    //fragmentdepth = gl_FragCoord.z;
}
