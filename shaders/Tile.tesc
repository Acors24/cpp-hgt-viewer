#version 410
#extension GL_ARB_explicit_uniform_location : require

layout(vertices = 4) out;

layout(location = 0) uniform mat4 view;
layout(location = 4) uniform int flat_;
layout(location = 5) uniform float sideLength;

in vec2 texCoord[];
out vec2 textureCoord[];

void main()
{
    // https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation
    
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    textureCoord[gl_InvocationID] = texCoord[gl_InvocationID];

    if (sideLength > 0 && gl_InvocationID == 0) {
        gl_TessLevelOuter[0] = sideLength;
        gl_TessLevelOuter[1] = sideLength;
        gl_TessLevelOuter[2] = sideLength;
        gl_TessLevelOuter[3] = sideLength;
        gl_TessLevelInner[0] = sideLength;
        gl_TessLevelInner[1] = sideLength;
        return;
    }

    if (gl_InvocationID == 0)
    {
        // ----------------------------------------------------------------------
        // Step 1: define constants to control tessellation parameters
        const int MIN_TESS_LEVEL = 8;
        const int MAX_TESS_LEVEL = 60;

        float MIN_DISTANCE;
        float MAX_DISTANCE;

        if (flat_ == 1) {
            MIN_DISTANCE = 0;
            MAX_DISTANCE = 1;
        } else {
            MIN_DISTANCE = 1;
            MAX_DISTANCE = 500;
        }

        // ----------------------------------------------------------------------
        // Step 2: transform each vertex into eye space
        vec4 eyeSpacePos00 = view * gl_in[0].gl_Position;
        vec4 eyeSpacePos01 = view * gl_in[1].gl_Position;
        vec4 eyeSpacePos10 = view * gl_in[2].gl_Position;
        vec4 eyeSpacePos11 = view * gl_in[3].gl_Position;

        float factor00;
        float factor01;
        float factor10;
        float factor11;

        if (flat_ == 1) {
            factor00 = length(eyeSpacePos00);
            factor01 = length(eyeSpacePos01);
            factor10 = length(eyeSpacePos10);
            factor11 = length(eyeSpacePos11);
        } else {
            factor00 = abs(eyeSpacePos00.z);
            factor01 = abs(eyeSpacePos01.z);
            factor10 = abs(eyeSpacePos10.z);
            factor11 = abs(eyeSpacePos11.z);
        }

        // ----------------------------------------------------------------------
        // Step 3: "distance" from camera scaled between 0 and 1
        float distance00 = clamp((factor00-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
        float distance01 = clamp((factor01-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
        float distance10 = clamp((factor10-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
        float distance11 = clamp((factor11-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);

        // ----------------------------------------------------------------------
        // Step 4: interpolate edge tessellation level based on closer vertex
        float tessLevel0 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance10, distance00) );
        float tessLevel1 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance00, distance01) );
        float tessLevel2 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance01, distance11) );
        float tessLevel3 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance11, distance10) );

        // ----------------------------------------------------------------------
        // Step 5: set the corresponding outer edge tessellation levels
        gl_TessLevelOuter[0] = tessLevel0;
        gl_TessLevelOuter[1] = tessLevel1;
        gl_TessLevelOuter[2] = tessLevel2;
        gl_TessLevelOuter[3] = tessLevel3;

        // ----------------------------------------------------------------------
        // Step 6: set the inner tessellation levels to the max of the two parallel edges
        gl_TessLevelInner[0] = max(tessLevel1, tessLevel3);
        gl_TessLevelInner[1] = max(tessLevel0, tessLevel2);
    }
}

