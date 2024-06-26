#version 410
#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_shading_language_420pack : require

layout(location = 0) uniform mat4 view;
layout(location = 2) uniform float longitude;
layout(location = 3) uniform float latitude;
layout(location = 4) uniform int flat_;

out vec2 texCoord;

void main(void) {
    const float EARTH_RADIUS = 6378.0;
    const float PI = 3.14159265359;

    int row = gl_VertexID / 21;
    int col = gl_VertexID % 21;

    float rowNorm = row / 20.0;
    float colNorm = col / 20.0;

    texCoord = vec2(colNorm, rowNorm);

    float x = 0.0, y = 0.0, z = 0.0;

    mat4 invView = inverse(view);
    float longitude = longitude;
    float latitude = latitude;
    if (flat_ == 1) {
        if (invView[3][0] > 0 && longitude < -180.0 + invView[3][0] * 2 * PI) {
            longitude += 360.0;
        } else if (invView[3][0] < 0 && longitude > 180.0 + invView[3][0] * 2 * PI) {
            longitude -= 360.0;
        }
    }

    longitude = longitude + colNorm;
    latitude = latitude - rowNorm;

    longitude = radians(longitude);
    latitude = radians(latitude);
    
    if(flat_ == 1) {
        x = degrees(1 / (2 * PI) * longitude);
        y = degrees(1 / (2 * PI) * log(tan(PI / 4 + latitude / 2.0)));
        z = 0.0;
    } else {
        x = EARTH_RADIUS * cos(latitude) * sin(longitude);
        y = EARTH_RADIUS * sin(latitude);
        z = EARTH_RADIUS * cos(latitude) * cos(longitude);
    }

    gl_Position = vec4(x, y, z, 1.0);
}