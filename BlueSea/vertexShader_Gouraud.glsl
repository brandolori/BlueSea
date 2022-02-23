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

float wavePeriod = 50;

float waveOffset(float x, float y) {

    return sin(x * wavePeriod + time) + sin(y * wavePeriod + time);
}

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
        vec4 worldPos = Model* v;
        
        float offset = waveOffset(worldPos.x, worldPos.z);
        
        v.y	= v.y + offset;

        gl_Position = Projection * View * Model * v;

        waveEdge = offset;
    }

    if (sceltaS==5)  //Texture + Gourad Shading
    {
	    //Trasforma le coordinate del modello nelle coordinate di clipping

	    gl_Position = Projection * View * Model * vec4(aPos, 1.0);
    

        // Transforma la posizione del vertice nelle coordinate di vista (VCS)
        vec4 eyePosition = View * Model * vec4(aPos, 1.0);
	    // Transforma la posizione della luce nelle coordinate di vista (VCS) 
	    vec4 eyeLightPos = View * vec4(light.position, 1.0);
        // Transforma le normali nei vertici nel sistema delle coordinate di vista (VCS)
        vec3 N = normalize(mat3(View) *transpose(inverse(mat3( Model))) * vertexNormal);

	    // Calcola la direzizone della luce, di Riflessione e di vista V nel sistema di coordunae 
	    vec3 V = normalize(-eyePosition.xyz);
	    vec3 L = normalize((eyeLightPos - eyePosition).xyz);
        vec3 R = reflect(-L, N);  

        // ambient
        vec3 ambient = light.power * material.ambient;
  	
        // diffuse 
        float diff = max(dot(L,N), 0.0);
        vec3 diffuse = light.power * light.color * diff * material.diffuse;

        // specular
        float spec = pow(max(dot(V, R), 0.0), material.shininess);
        vec3 specular =  light.power * light.color * spec * material.specular;  

        ourColor = vec4( ambient + diffuse + specular,1.0);

        frag_coord_st=coord_st;
    }

    if (sceltaS==6 || sceltaS==7)  // Riflessione con Environment Mapping
    {
        Normal = mat3(transpose(inverse(Model))) * vertexNormal;  //Normali in coordinate del mondo
        Position = vec3(Model * vec4(aPos, 1.0)); //in WCS
        gl_Position = Projection * View * Model * vec4(aPos,1.0);
    }
}