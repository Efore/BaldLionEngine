#type vertex
#version 330 core

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in vec3 vertex_normal;	
layout (location = 3) in vec2 vertex_texcoord;
layout (location = 4) in vec3 vertex_tangent;
layout (location = 5) in vec3 vertex_bitangent;

uniform mat4 u_viewProjection;  
uniform mat4 u_transform;
		
out VS_OUT
{
	vec3 vs_position;
	vec3 vs_color;
	vec2 vs_texcoord;
	mat3 TBN;
} vs_out;			

void main()
{
	vs_out.vs_position = vec3(u_transform * vec4(vertex_position, 1.f)).xyz;
	vs_out.vs_color = vertex_color;
	vs_out.vs_texcoord = vec2(vertex_texcoord.x,-vertex_texcoord.y);

	vec3 T = normalize(mat3(u_transform) * vertex_tangent);
	vec3 N = normalize(mat3(u_transform) * vertex_normal);
	vec3 B = normalize(mat3(u_transform) * vertex_bitangent);

	vs_out.TBN = mat3(T,B,N);	

	gl_Position = u_viewProjection * u_transform * vec4(vertex_position, 1.f);
}

#type fragment
#version 330 core

in VS_OUT
{
	vec3 vs_position;
	vec3 vs_color;
	vec2 vs_texcoord;
	mat3 TBN;
} fs_in;	

struct DirectionalLight 
{
    vec3 direction;  

    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
};

struct PointLight
{
	vec3 position;

	float constant;
    float linear;
    float quadratic;  

    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
};
#define MAX_POINT_LIGHTS 20

struct Material
{
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 emissiveColor;
	vec3 specularColor;
	sampler2D ambientTex;
	sampler2D diffuseTex;
	sampler2D emissiveTex;
	sampler2D specularTex;
	sampler2D normalTex;
	float shininess;
};

out vec4 fs_color;

//Uniforms
uniform Material u_material;
uniform DirectionalLight u_directionalLight;
uniform PointLight u_pointLights[MAX_POINT_LIGHTS];
uniform int u_numPointLights;
uniform vec3 u_cameraPos;
uniform vec3 u_lightPos;

//Functions

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);	
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_material.shininess);

	// combine results
    vec3 ambient  = light.ambientColor * u_material.ambientColor * vec3(texture(u_material.ambientTex, fs_in.vs_texcoord));
    vec3 diffuse  = light.diffuseColor  * diff * u_material.diffuseColor * vec3(texture(u_material.diffuseTex, fs_in.vs_texcoord));
    vec3 specular = light.specularColor * spec * u_material.specularColor * vec3(texture(u_material.specularTex, fs_in.vs_texcoord));
    
	return (ambient + diffuse + specular);	
}  


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fs_in.vs_position);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_material.shininess);

    // attenuation
    float distance = length(light.position - fs_in.vs_position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    

    // combine results
    vec3 ambient  = light.ambientColor  * u_material.ambientColor * vec3(texture(u_material.ambientTex, fs_in.vs_texcoord));
    vec3 diffuse  = light.diffuseColor  * diff * u_material.diffuseColor * vec3(texture(u_material.diffuseTex, fs_in.vs_texcoord));
    vec3 specular = light.specularColor * spec * u_material.specularColor * vec3(texture(u_material.specularTex, fs_in.vs_texcoord));

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
} 

void main()
{	
	vec3 norm = (texture(u_material.normalTex, fs_in.vs_texcoord).rgb);
	norm = norm * 2.0 - 1.0;   
	norm = normalize(fs_in.TBN * norm); 
	//vec3 norm = normalize(fs_in.vs_normal);

	vec3 posToViewDir = normalize(u_cameraPos - fs_in.vs_position);

	//Directional Lighting
	vec3 result = CalcDirLight(u_directionalLight, norm, posToViewDir);

	for(int i = 0; i < u_numPointLights; i++)
        result += CalcPointLight(u_pointLights[i], norm, posToViewDir); 

	//Final light
	fs_color = vec4(result,1.0f);
}
