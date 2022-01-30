#type vertex
#version 330 core
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in vec3 vertex_normal;	
layout (location = 3) in vec3 vertex_tangent;
layout (location = 4) in vec2 vertex_texcoord;
layout (location = 5) in vec3 vertex_joint_ids;
layout (location = 6) in vec3 vertex_joint_weights;

uniform mat4 u_lightViewProjectionMatrix;  
uniform mat4 u_worldTransformMatrix; 

const int MAX_JOINTS = 100;
uniform mat4 u_joints[MAX_JOINTS];

void main()
{	
	mat4 jointTransform = u_joints[int(vertex_joint_ids.x)] * vertex_joint_weights.x;
    jointTransform += u_joints[int(vertex_joint_ids.y)] * vertex_joint_weights.y;
    jointTransform += u_joints[int(vertex_joint_ids.z)] * vertex_joint_weights.z;   

	vec4 pos = u_worldTransformMatrix * (jointTransform * vec4(vertex_position, 1.f));	

	gl_Position = u_lightViewProjectionMatrix * pos;   
}  

#type fragment
#version 330 core

void main(){
    // Not really needed, OpenGL does it anyway
    //fragmentdepth = gl_FragCoord.z;
}
