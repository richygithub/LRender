#version 330 core

// Ouput data
out vec3 color;
in  vec4 vcolor;

void main()
{

	// Output color = red 
	//color = vec3(0,0.2,0.8);

	color = vcolor.xyz;

}