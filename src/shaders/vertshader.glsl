#version 330 core

// Define constants

layout(location = 0) in vec3 vertCoordinates_in;
layout(location = 1) in vec3 vertColor_in;

uniform mat4 transformationModel;
uniform mat4 transformationProjection;

out vec3 vertColor;

void main() {
    vec4 transformedLocation = transformationModel * vec4(vertCoordinates_in, 1.0);
    gl_Position = transformationProjection * transformedLocation;
    vertColor = vertColor_in;
}
