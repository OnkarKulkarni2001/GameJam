//#version 330 core
//layout(triangles) in;
//layout(triangle_strip, max_vertices=18) out;
//
//uniform mat4 shadowMatrices[6]; // Projection matrices for each face
//uniform vec3 lightPos;
//
//out vec4 FragPos; // Pass fragment position to fragment shader
//
//void main() {
//    for(int face = 0; face < 6; ++face) {
//        gl_Layer = face; // Specify which face to render to
//        for(int i = 0; i < 3; ++i) {
//            FragPos = gl_in[i].gl_Position;
//            gl_Position = shadowMatrices[face] * FragPos;
//            EmitVertex();
//        }
//        EndPrimitive();
//    }
//}


//#version 330 core
//layout(triangles) in;
//layout(triangle_strip, max_vertices=18) out;
//
//uniform mat4 shadowMatrices[6];
//uniform vec3 lightPos;
//
//out vec4 FragPos;
//
//void main() {
//    for(int face = 0; face < 6; ++face) {
//        gl_Layer = face;
//        for(int i = 0; i < 3; ++i) {
//            FragPos = gl_in[i].gl_Position; // Already in world space from vertex shader
//            gl_Position = shadowMatrices[face] * FragPos;
//            EmitVertex();
//        }
//        EndPrimitive();
//    }
//}


#version 330 core
layout(triangles) in; 
layout(triangle_strip, max_vertices = 18) out; // 6 faces * 3 vertices

uniform mat4 shadowMatrices[6]; // Array of 6 shadow matrices

//in vec4 FragPosVS[]; // World positions from vertex shader

out vec4 FragPos; // Output fragment position for shadow calculation

void main()
{
    for (int face = 0; face < 6; ++face)
    {
        gl_Layer = face; 
        for (int i = 0; i < 3; ++i)
        {
            //FragPos = FragPosVS[i]; 
            FragPos = gl_in[i].gl_Position;
            //gl_Position = shadowMatrices[face] * FragPosVS[i];
            gl_Position = shadowMatrices[face] * FragPos;
            //gl_Layer = face; // Write to the correct face of the cubemap
            EmitVertex();
        }
        EndPrimitive();
    }
}

