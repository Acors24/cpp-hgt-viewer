#version 330
#extension GL_ARB_explicit_uniform_location : require

in float ht;

out vec4 color;

void main(void) {
    if (ht <= 0)
        color = vec4(0., 0., 1, 1.0); //->blue
    else if (ht < 500)
        color = vec4(0., ht / 500, 0, 1.0); //->green
    else if (ht < 1000)
        color = vec4(ht / 500 - 1, 1., 0, 1.0); //->yellow
    else if (ht < 2000)
        color = vec4(1., 2. - ht / 1000, 0, 1.0); //->red
    else
        color = vec4(1., (ht - 2000) / 7000, (ht - 2000) / 7000, 1.0); //->white
}