#version 410

#extension GL_ARB_explicit_uniform_location : require

layout (quads, equal_spacing, ccw) in;

layout(location = 0) uniform mat4 view;
layout(location = 1) uniform mat4 projection;

uniform sampler2D heightMap;

in vec2 textureCoord[];

out float ht;

void main()
{
    // https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation
    
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec2 t00 = textureCoord[0];
    vec2 t01 = textureCoord[1];
    vec2 t10 = textureCoord[2];
    vec2 t11 = textureCoord[3];

    // bilinearly interpolate texture coordinate across patch
    vec2 t0 = (t01 - t00) * u + t00;
    vec2 t1 = (t11 - t10) * u + t10;
    vec2 texCoord = (t1 - t0) * v + t0;

    // lookup texel at patch coordinate for height
    ht = texture(heightMap, texCoord).r;

    // ----------------------------------------------------------------------
    // retrieve control point position coordinates
    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    // compute patch surface normal
    vec4 uVec = p01 - p00;
    vec4 vVec = p10 - p00;
    vec4 normal = normalize( vec4(cross(uVec.xyz, vVec.xyz), 0) );

    // bilinearly interpolate position coordinate across patch
    vec4 p0 = (p01 - p00) * u + p00;
    vec4 p1 = (p11 - p10) * u + p10;
    vec4 p = (p1 - p0) * v + p0;

    // displace point along normal
    p += normal * ht / 1000.0;

    // ----------------------------------------------------------------------
    // output patch point position in clip space
    gl_Position = projection * view * p;
}