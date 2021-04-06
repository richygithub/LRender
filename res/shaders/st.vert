#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;

out vec2 UV;
out vec3 worldPos;
out vec3 Normal;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 VP;

uniform vec3 eyePosition;

void main(){

    worldPos = (M*vec4(vertexPos,1.0)).xyz;
    gl_Position = VP*vec4(worldPos,1.0);
    UV = vertexUV;

    Normal = (M*vec4(vertexNormal,1.0)).xyz;
}

