#version 330 core
layout (location = 0) in vec3 aPos;   // Attributo Posizione
layout (location = 1 ) in vec3 vertexNormal; // Attributo Normale
layout (location = 2 ) in vec2 coord_st; // Attributo texture

uniform mat4 Model;
uniform mat4 Projection;
uniform mat4 View;
uniform vec3 center;

uniform int sceltaS;
uniform float time;

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

out vec4 ourColor; // output a color to the fragment shader
out vec3 V;
out vec3 N;
out vec3 L;
out vec2 frag_coord_st;
out vec3 Normal;
out vec3 Position;
out float waveEdge;


void main()
{
    if (sceltaS==2)  // phong shading
    {
        gl_Position = Projection * View * Model * vec4(aPos, 1.0);

	    // Position in VCS
	    vec4 eyePosition = View * Model * vec4(aPos, 1.0);
	    // LightPos in VCS
	    vec4 eyeLightPos = View * vec4(light.position, 1.0);

	    // Compute vectors E,L,N in VCS
	    V = -eyePosition.xyz;
	    L = (eyeLightPos - eyePosition).xyz;
	    N = mat3(View) * transpose(inverse(mat3( Model))) * vertexNormal;
    }

    if (sceltaS==3)  // mare
    {
        vec4 v =  vec4(aPos, 1.0);
        
        float wavePeriod = 150;
        float offset = 2 * sin(aPos.x * wavePeriod + time) * sin(aPos.z * wavePeriod + time);
        
        v.y	= v.y + offset;

        gl_Position = Projection * View * Model * v;

        waveEdge = offset;
    }

    if (sceltaS==6 || sceltaS==7)  // Riflessione con Environment Mapping
    {
        Normal = mat3(transpose(inverse(Model))) * vertexNormal;  //Normali in coordinate del mondo
        Position = vec3(Model * vec4(aPos, 1.0)); //in WCS
        gl_Position = Projection * View * Model * vec4(aPos,1.0);
    }
}