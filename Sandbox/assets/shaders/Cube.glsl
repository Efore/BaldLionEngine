#type vertex
#version 330 core

layout(location = 0) in vec3 vertex_position;		
layout(location = 1) in vec3 vertex_normal;		
layout(location = 2) in vec2 vertex_texCoord;	

uniform mat4 u_viewProjection;  
uniform mat4 u_transform;
		
out vec2 v_texCoord;			
			
void main()
{
	v_texCoord = vertex_texCoord;						
	gl_Position = u_viewProjection * u_transform * vec4(vertex_position, 1.0);
}

#type fragment
#version 330 core

out vec4 color;					
			
in vec2 v_texCoord;

uniform vec4 u_baseColor;
uniform sampler2D u_texture;
	
void main()
{
	color = vec4(1.0,0.0,0.0,1.0);
}