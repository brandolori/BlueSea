#version 330 core

uniform vec3 cameraPos;
uniform mat4 Model;
uniform mat4 Projection;
uniform mat4 View;

uniform vec3 AmbientProduct, DiffuseProduct, SpecularProduct;
//uniform float Shininess;
//uniform vec3 LighPosition;
uniform int sceltaS;

struct PointLight{
	vec3 position;
	vec3 color;
	float power;
 };
uniform PointLight light;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material material;
uniform sampler2D tex0;
 

in vec3 L;
in vec3 N;
in vec3 V;
in vec4 ourColor;
in float waveEdge;
in vec2 frag_coord_st;


in vec3 Normal;
in vec3 Position;

uniform samplerCube cubemap;

out vec4 FragColor;
  
void main()
{
    if(sceltaS==1)
        FragColor = ourColor;

    if (sceltaS==2 || sceltaS==4)
    {
        vec3 ambient = material.ambient;    

        // diffuse 
        vec3 norm = normalize(N);
        vec3 lightDir = normalize(L);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.power * light.color * diff * material.diffuse;
    
        // specular
        vec3 viewDir = normalize(V);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular =  light.power * light.color * spec * material.specular;  
        
        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);

    }

    if (sceltaS==3)
    {
        //float colorOffset = (waveEdge) / 8;
        //FragColor = vec4(material.diffuse,1.0) + colorOffset;
        vec3 ambient = material.ambient;    

        // diffuse 
        vec3 norm = normalize(N);
        vec3 lightDir = normalize(L);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.power * light.color * diff * material.diffuse;
    
        // specular
        vec3 viewDir = normalize(V);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular =  light.power * light.color * spec * material.specular;  
        
        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);
    }

    if (sceltaS==5)
        FragColor=mix(ourColor,texture(tex0,vec2(frag_coord_st.x,1-frag_coord_st.y)),0.5);

    if (sceltaS==6)
    {
        vec3 E = normalize(Position - cameraPos);
        vec3 R = reflect(E, normalize(Normal));
        R =vec3(R.x,-R.yz);
        FragColor  =vec4(texture(cubemap, R).rgb,1.0);
    }

    if (sceltaS==7)
    {
        float ratio=1.00/1.52;
        vec3 E = normalize(Position - cameraPos);
        vec3 R = refract(E, normalize(Normal),ratio);
        R =vec3(R.x,-R.yz);
        FragColor  =vec4(texture(cubemap, R).rgb,1.0);
    }
}