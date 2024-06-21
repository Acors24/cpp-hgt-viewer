#version 330
#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_shading_language_420pack : require

uniform sampler2D heightMap;

layout(location = 0) uniform mat4 view;
layout(location = 1) uniform mat4 projection;
layout(location = 2) uniform float longitude;
layout(location = 3) uniform float latitude;
layout(location = 4) uniform float flat_;

out float ht;

void main(void) {
    const float EARTH_RADIUS = 6378.0;
    const float PI = 3.14159265359;

    int row = gl_VertexID / 1201;
    int col = gl_VertexID % 1201;

    float rowNorm = row / 1200.0;
    float colNorm = col / 1200.0;

    float inHeight = texture(heightMap, vec2(colNorm, rowNorm)).r;

    float x = 0.0, y = 0.0, z = 0.0;
    float radiusAt = EARTH_RADIUS + inHeight / 1000.0;

    float longitude = longitude + colNorm;
    float latitude = latitude - rowNorm;
    longitude = radians(longitude);
    latitude = radians(latitude);
    
    if(flat_ > 0.5) {
        x = degrees(1 / (2 * PI) * longitude);
        y = degrees(1 / (2 * PI) * log(tan(PI / 4 + latitude / 2.0)));
        z = 0.0;
    } else {
        x = radiusAt * cos(latitude) * sin(longitude);
        y = radiusAt * sin(latitude);
        z = radiusAt * cos(latitude) * cos(longitude);
    }

    gl_Position = projection * view * vec4(x, y, z, 1.0);

    ht = inHeight;
}