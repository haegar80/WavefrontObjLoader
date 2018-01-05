#ifdef GL_ES

// It was expressed that some drivers required this next line to function properly
precision highp float;
#endif

struct LightInfo
{
    vec3 position;
	vec3 intensity;
};

struct MaterialInfo
{
	vec3 Ka;			// Ambient color
	vec3 Kd;			// Diffuse color
	vec3 Ks;			// Specular color
	float Shininess;	// Specular shininess factor
};

varying vec3 position_cameraspace;
varying vec3 normal_cameraspace;

uniform LightInfo light;
uniform MaterialInfo material;

void colorCalc( out vec3 ambient, out vec3 diffuse, out vec3 specular )
{
	vec3 lightPosition = vec3(0.0, 0.0, 0.0);
	vec3 n = normalize( normal_cameraspace );
	vec3 lightSource = normalize( light.position - position_cameraspace );
	vec3 eyeDirection = normalize( -position_cameraspace );
	vec3 specularReflection = reflect( -lightSource, normal_cameraspace );
 
	ambient = material.Ka;
 
	float dotProduct = max( dot( lightSource, normal_cameraspace ), 0.0 ); // dot product = scalar product
	diffuse = material.Kd * dotProduct;
 
	specular = material.Ks * pow( max( dot(specularReflection, eyeDirection) , 0.0 ), material.Shininess ); 
}

void main()
{
    vec3 ambient, diffuse, specular;
	
	//if ( gl_FrontFacing )
	//{
	    colorCalc( ambient, diffuse, specular );
	//}
	
    gl_FragColor = vec4((light.intensity * (ambient + diffuse + specular)), 1.0);
}