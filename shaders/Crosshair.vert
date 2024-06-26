#version 330 
#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_shading_language_420pack : require
layout(location = 0) in vec3 vpos;

void main(void) {
    gl_Position = vec4(vpos, 1.0);
}