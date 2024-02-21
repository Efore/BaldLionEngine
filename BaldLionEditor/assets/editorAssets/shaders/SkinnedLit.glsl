#type vertex
#version 330 core

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;	
layout (location = 2) in vec3 vertex_tangent;
layout (location = 3) in vec2 vertex_texcoord;
layout (location = 4) in vec3 vertex_joint_ids;
layout (location = 5) in vec3 vertex_joint_weights;

uniform mat4 u_viewProjectionMatrix;  
uniform mat4 u_worldTransformMatrix;
uniform mat4 u_lightViewProjectionMatrix;
uniform float u_shadowDistance;
uniform float u_shadowTransitionDistance;		

const int MAX_JOINTS = 100;
uniform mat4 u_joints[MAX_JOINTS];

out VS_OUT
{
	vec3 vs_position;	
	vec2 vs_texcoord;
	mat3 TBN;
	vec4 vs_posLightSpace;
} vs_out;			

void main()
{
	mat4 jointTransform = u_joints[int(vertex_joint_ids.x)] * vertex_joint_weights.x;
    jointTransform += u_joints[int(vertex_joint_ids.y)] * vertex_joint_weights.y;
    jointTransform += u_joints[int(vertex_joint_ids.z)] * vertex_joint_weights.z;   

	vec4 pos = u_worldTransformMatrix * (jointTransform * vec4(vertex_position, 1.f));
	vs_out.vs_position = vec3(pos);	
	vs_out.vs_texcoord = vec2(vertex_texcoord.x,-vertex_texcoord.y);
	vs_out.vs_posLightSpace = u_lightViewProjectionMatrix * vec4(vs_out.vs_position ,1.0);

	vec3 T = normalize(u_worldTransformMatrix * (jointTransform * vec4(vertex_tangent,0.f))).xyz;
	vec3 N = normalize(u_worldTransformMatrix * (jointTransform * vec4(vertex_normal,0.f))).xyz;
	
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	
	vs_out.TBN = mat3(T,B,N);	

	gl_Position = u_viewProjectionMatrix * pos;
}

#type fragment
#version 330 core

in VS_OUT
{
	vec3 vs_position;	
	vec2 vs_texcoord;
	mat3 TBN;
	vec4 vs_posLightSpace;
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
	int useAmbientTex;
	sampler2D ambientTex;
	int useDiffuseTex;
	sampler2D diffuseTex;
	int useEmissiveTex;
	sampler2D emissiveTex;
	int useSpecularTex;
	sampler2D specularTex;
	int useNormalTex;
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

uniform int u_useShadowMap;
uniform sampler2D u_shadowMapTex;

//Functions
const int pcfCount = 2;
const float totalTexels = (pcfCount * 2.0 + 1.0) * (pcfCount * 2.0 + 1.0);

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = (projCoords * 0.5) + 0.5; 

	float closestDepth = texture(u_shadowMapTex, projCoords.xy).r;  	

	float currentDepth = projCoords.z; 
	
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

	float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_shadowMapTex, 0);

    for(int x = -pcfCount; x <= pcfCount; ++x)
    {
        for(int y = -pcfCount; y <= pcfCount; ++y)
        {
            float pcfDepth = texture(u_shadowMapTex, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 0.8 : 0.0;        
        }    
    }

    shadow /= totalTexels;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;

	return shadow;
}

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec4 posLightSpace)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);	
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_material.shininess);

	vec3 ambientTexResult = vec3(texture(u_material.ambientTex, fs_in.vs_texcoord));
	ambientTexResult.x = clamp(ambientTexResult.x + (1 - u_material.useAmbientTex), 0.0, 1.0);
	ambientTexResult.y = clamp(ambientTexResult.y + (1 - u_material.useAmbientTex), 0.0, 1.0);
	ambientTexResult.z = clamp(ambientTexResult.z + (1 - u_material.useAmbientTex), 0.0, 1.0);

	vec3 diffuseTexResult = vec3(texture(u_material.diffuseTex, fs_in.vs_texcoord));
	diffuseTexResult.x = clamp(diffuseTexResult.x + (1 - u_material.useDiffuseTex), 0.0, 1.0);
	diffuseTexResult.y = clamp(diffuseTexResult.y + (1 - u_material.useDiffuseTex), 0.0, 1.0);
	diffuseTexResult.z = clamp(diffuseTexResult.z + (1 - u_material.useDiffuseTex), 0.0, 1.0);

	vec3 specularTexResult = vec3(texture(u_material.specularTex, fs_in.vs_texcoord));
	specularTexResult.x = clamp(specularTexResult.x + (1 - u_material.useSpecularTex), 0.0, 1.0);
	specularTexResult.y = clamp(specularTexResult.y + (1 - u_material.useSpecularTex), 0.0, 1.0);
	specularTexResult.z = clamp(specularTexResult.z + (1 - u_material.useSpecularTex), 0.0, 1.0);

	float shadow = ShadowCalculation(posLightSpace, normal, lightDir) * float(u_useShadowMap);
    
	// combine results
    vec3 ambient  = light.ambientColor * u_material.ambientColor * ambientTexResult;
    vec3 diffuse  = light.diffuseColor  * diff * u_material.diffuseColor * diffuseTexResult;
    vec3 specular = light.specularColor * spec * u_material.specularColor * specularTexResult;

	return (ambient + ((1.0 - shadow) * (diffuse + specular)));	
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

	vec3 ambientTexResult = vec3(texture(u_material.ambientTex, fs_in.vs_texcoord));
	ambientTexResult.x = clamp(ambientTexResult.x + (1 - u_material.useAmbientTex), 0.0, 1.0);
	ambientTexResult.y = clamp(ambientTexResult.y + (1 - u_material.useAmbientTex), 0.0, 1.0);
	ambientTexResult.z = clamp(ambientTexResult.z + (1 - u_material.useAmbientTex), 0.0, 1.0);

	vec3 diffuseTexResult = vec3(texture(u_material.diffuseTex, fs_in.vs_texcoord));
	diffuseTexResult.x = clamp(diffuseTexResult.x + (1 - u_material.useDiffuseTex), 0.0, 1.0);
	diffuseTexResult.y = clamp(diffuseTexResult.y + (1 - u_material.useDiffuseTex), 0.0, 1.0);
	diffuseTexResult.z = clamp(diffuseTexResult.z + (1 - u_material.useDiffuseTex), 0.0, 1.0);

	vec3 specularTexResult = vec3(texture(u_material.specularTex, fs_in.vs_texcoord));
	specularTexResult.x = clamp(specularTexResult.x + (1 - u_material.useSpecularTex), 0.0, 1.0);
	specularTexResult.y = clamp(specularTexResult.y + (1 - u_material.useSpecularTex), 0.0, 1.0);
	specularTexResult.z = clamp(specularTexResult.z + (1 - u_material.useSpecularTex), 0.0, 1.0);

	// combine results
    vec3 ambient  = light.ambientColor * u_material.ambientColor * ambientTexResult;
    vec3 diffuse  = light.diffuseColor  * diff * u_material.diffuseColor * diffuseTexResult;
    vec3 specular = light.specularColor * spec * u_material.specularColor * specularTexResult;
    
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
} 

void main()
{	
	vec3 norm = vec3(texture(u_material.normalTex, fs_in.vs_texcoord));
	norm.x = clamp(norm.x + (1 - u_material.useNormalTex), 0.0, 1.0);
	norm.y = clamp(norm.y + (1 - u_material.useNormalTex), 0.0, 1.0);
	norm.z = clamp(norm.z + (1 - u_material.useNormalTex), 0.0, 1.0);

	norm = norm * 2.0 - 1.0;   
	norm = normalize(fs_in.TBN * norm); 

	vec3 posToViewDir = normalize(u_cameraPos - fs_in.vs_position);

	//Directional Lighting
	vec3 result = CalcDirLight(u_directionalLight, norm, posToViewDir, fs_in.vs_posLightSpace);

	for(int i = 0; i < u_numPointLights; i++)
        result += CalcPointLight(u_pointLights[i], norm, posToViewDir); 

	//Final light
	fs_color = vec4(result,1.0f);
}
