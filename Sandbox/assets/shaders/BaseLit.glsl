#type vertex
#version 330 core

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in vec3 vertex_normal;	
layout (location = 3) in vec2 vertex_texcoord;

uniform mat4 u_viewProjection;  
uniform mat4 u_transform;
		
out vec3 vs_position;
out vec3 vs_color;
out vec2 vs_texcoord;
out vec3 vs_normal;						

void main()
{
	vs_position = vec3(u_transform * vec4(vertex_position, 1.f)).xyz;
	vs_color = vertex_color;
	vs_texcoord = vertex_texcoord;
	vs_normal = mat3(transpose(inverse(u_transform))) * vertex_normal;

	gl_Position = u_viewProjection * u_transform * vec4(vertex_position, 1.f);
}

#type fragment
#version 330 core

struct Light 
{
    vec3 position;  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material
{
	vec3 diffuseColor;
	vec3 emissiveColor;
	vec3 specularColor;
	sampler2D diffuseTex;
	sampler2D emissiveTex;
	sampler2D specularTex;
	sampler2D normalTex;
	float shininess;
};

in vec3 vs_position;
in vec3 vs_color;
in vec2 vs_texcoord;
in vec3 vs_normal;

out vec4 fs_color;

//Uniforms
uniform Material u_material;
uniform Light u_light;
uniform vec3 u_cameraPos;
uniform vec3 u_lightPos;

//Functions
vec3 calculateAmbient(Material material, Light light)
{
	return material.diffuseColor * light.ambient;
}

vec3 calculateEmission(Material material)
{
	return vec3(texture(material.emissiveTex, vs_texcoord)) * material.emissiveColor;
}

vec3 calculateDiffuse(Material material, Light light, vec3 vs_position, vec3 vs_normal, vec3 lightPos)
{
	vec3 posToLightDirVec = normalize(lightPos - vs_position);
	float diffuse = max(dot(posToLightDirVec, vs_normal), 0.0f);
	vec3 diffuseFinal = material.diffuseColor * diffuse * light.diffuse;

	return diffuseFinal;
}

vec3 calculateSpecular(Material material, Light light, vec3 vs_position, vec3 vs_normal, vec3 lightPos, vec3 cameraPos)
{
	vec3 lightToPosDirVec = normalize(vs_position - lightPos);
	vec3 reflectDirVec = normalize(reflect(lightToPosDirVec, vs_normal));
	vec3 posToViewDirVec = normalize(cameraPos - vs_position);
	float specularConstant = pow(max(dot(posToViewDirVec, reflectDirVec), 0.0f), material.shininess);
	vec3 specularFinal = vec3(texture(material.specularTex, vs_texcoord)) * material.specularColor * light.specular * specularConstant;

	return specularFinal;
}

void main()
{	
	vec3 norm = normalize(vs_normal);

	//Ambient light
	vec3 ambientFinal = calculateAmbient(u_material,u_light);

	//Diffuse light
	vec3 diffuseFinal = calculateDiffuse(u_material, u_light, vs_position, norm, u_light.position);

	//Specular light
	vec3 specularFinal = calculateSpecular(u_material, u_light, vs_position, norm, u_light.position, u_cameraPos);

	vec3 emissionFinal = calculateEmission(u_material);

	//Final light
	fs_color = texture(u_material.diffuseTex, vs_texcoord) * (vec4(ambientFinal, 1.f) + vec4(diffuseFinal, 1.f) + vec4(specularFinal, 1.f));

}
