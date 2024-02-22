#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout(location = 0) in vec3 vertCoordinates_in;
layout(location = 1) in vec3 vertColor_in;

uniform mat4 transformationModel;
uniform mat4 transformationProjection;

out vec3 vertColor;

void main() {
    // Apply model transformation
    vec4 transformedLocation = transformationModel * vec4(vertCoordinates_in, 1.0);

    // Apply projection transformation
    gl_Position = transformationProjection * transformedLocation;

    // Pass color through
    vertColor = vertColor_in;
}
