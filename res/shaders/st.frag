#version 330 core



in vec2 UV;
in vec3 worldPos;
in vec3 Normal;

// Ouput data
out vec3 color;

uniform vec3 eyePosition;

void main()
{

	// Output color = red 

	vec3 diffuseColor = vec3(1,1,1);
	vec3 n = normalize(Normal);
	vec3 l = normalize( eyePosition -  worldPos );
	float cosTheta = clamp( dot( n,l ), 0,1 );


	color = diffuseColor*cosTheta;

}