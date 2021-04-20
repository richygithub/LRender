#version 330 core

// Ouput data
out vec3 fragColor;
uniform vec4 color = vec4(1,1,1,1);
void main()
{

	// Output color = red 
	fragColor = color.rgb;

}