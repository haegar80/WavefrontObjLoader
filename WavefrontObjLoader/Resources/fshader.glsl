#ifdef GL_ES

// It was expressed that some drivers required this next line to function properly
precision highp float;
#endif

struct LightInfo
{
    vec3 Position;
    vec3 AmbientColor;
    vec3 DiffuseColor;
    vec3 SpecularColor;
};

struct MaterialInfo
{
    vec3 Ka;			    // Ambient color
    vec3 Kd;			    // Diffuse color
    vec3 Ks;			    // Specular color
    float SpecularExponent;	// Specular shininess factor
};

varying vec3 position_cameraspace;
varying vec3 normal_cameraspace;
varying vec2 frag_textureCoord;

uniform LightInfo light;
uniform MaterialInfo material;
uniform sampler2D ambient_map;
uniform sampler2D diffuse_map;
uniform sampler2D specular_map;

void colorCalc( out vec3 ambient, out vec3 diffuse, out vec3 specular )
{
    vec3 normal = normalize( normal_cameraspace );
    vec3 lightSource = normalize( light.Position - position_cameraspace );
    vec3 eyeDirection = normalize( -position_cameraspace );
    vec3 specularReflection = reflect( -lightSource, normal );
 
    vec3 ambientMapColor = texture2D(ambient_map, frag_textureCoord.st).rgb;
    ambient = (ambientMapColor * material.Ka) * light.AmbientColor;

    vec3 diffuseMapColor = texture2D(diffuse_map, frag_textureCoord.st).rgb;
    float dotProduct = max( dot( lightSource, normal ), 0.0 ); // dot product = scalar product
    diffuse = (diffuseMapColor * material.Kd) * dotProduct * light.DiffuseColor;

    vec3 specularMapColor =  texture2D(specular_map, frag_textureCoord.st).rgb;
    specular = (specularMapColor * material.Ks) * pow( max( dot(specularReflection, eyeDirection) , 0.0 ), material.SpecularExponent ) * light.SpecularColor;
}

void main()
{
    vec3 ambient, diffuse, specular;
	
    colorCalc( ambient, diffuse, specular );

    gl_FragColor = vec4((ambient + diffuse + specular), 1.0);
}