//#version 330 core
//in vec3 vPos;
//
//uniform mat4 shadowMatrices[6]; // 6 matrices for the cubemap views
//uniform mat4 model;
//uniform sampler2D originalPositions;
//uniform sampler2D uvTexture;
//
//out vec4 FragPos;
//
//void main()
//{
//    vec3 originalPos = texture(originalPositions, uvTexture.xy).xyz;
//    FragPos = model * vec4(vPos, 1.0);
//    gl_Position = shadowMatrices[gl_InstanceID] * FragPos; // Choose the right matrix
//}


//#version 330 core
//in vec3 vCol;
//in vec3 vPos;
//in vec3 vNormal;
//in vec2 vUV;
//// Animation inputs (boneIds, weights, etc.) if needed...
//
//uniform mat4 matModel;
//
//out vec4 FragPos;
//
//void main()
//{
//    vec4 worldPos = matModel * vec4(vPos, 1.0); // Transform model-space to world-space
//    FragPos = worldPos;
//    gl_Position = worldPos; // Passed to geometry shader for cubemap projection
//}

//#version 330 core
//in vec3 vPos; // Vertex position input
//uniform mat4 matModel; // Model matrix
//out vec4 FragPosVS; // Fragment position in world space
//
//void main()
//{
//    // Compute the world position of the vertex
//    vec4 worldPos = matModel * vec4(vPos, 1.0);
//    FragPosVS = worldPos; // Pass world position to the next shader stage
//    gl_Position = worldPos; // Transform vertex to clip space
//}

#version 330 core
in vec3 vPos;

uniform mat4 matModel;

void main()
{
    gl_Position = matModel * vec4(vPos, 1.0);
} 