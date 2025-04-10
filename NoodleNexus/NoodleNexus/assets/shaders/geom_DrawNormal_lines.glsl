// Geometry draw normals to the surface
#version 420
layout(triangles) 			in;
layout(line_strip)			out;	// NOTE we are generating LINES, not triangles
layout(max_vertices = 2)	out;

// From the vertex shader
// Note the [] because you are accepting more than one vertex
in vec3 gColour[];
in vec4 gVertexWorldLocation[];
in vec4 gVertexNormal[];		
in vec2 gUV[];	

// 'f' for 'going to the Fragment shader'
out vec3 fColour;			
out vec4 fvertexWorldLocation;
out vec4 fvertexNormal;
out vec2 fUV;	

void main()
{
	// Centre of this triangle
	// https://www.khronos.org/opengl/wiki/Geometry_Shader
	// vec4 gl_Position;
	
	// Centre of this triangle
	vec3 triCentre = (gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz + gl_in[2].gl_Position.xyz) / 3.0f;
	vec3 normCentre = (gVertexNormal[0].xyz + gVertexNormal[1].xyz + gVertexNormal[2].xyz) / 3.0f;
	vec3 averageWorldLocation = (gVertexWorldLocation[0].xyz 
	                             + gVertexWorldLocation[1].xyz 
								 + gVertexWorldLocation[0].xyz) / 3.0f;
	vec2 centreUV = (gUV[0] + gUV[1] + gUV[2]) / 3.0f;
	
	

// Start of the normal line
	gl_Position = vec4(triCentre, 1.0f);
	fColour = vec3(1.0f, 1.0f, 1.0f);	// White	
	fvertexNormal = vec4(normCentre, 1.0f);				// They are lines, so?? 
	fvertexWorldLocation = vec4(averageWorldLocation, 1.0f);
	fUV = centreUV;
	EmitVertex();
	
// End of the normal line
	const float LENGTH_OF_LINE = 5.0f;
	vec3 endOfNormalLine = fvertexNormal.xyz * LENGTH_OF_LINE;
	gl_Position = vec4(triCentre.xyz + endOfNormalLine, 1.0f);
	fColour = vec3(1.0f, 1.0f, 1.0f);	// White	
	fvertexNormal = vec4(normCentre, 1.0f);				// They are lines, so?? 
	vec3 endOfNormalLine_WorldLocation = averageWorldLocation + endOfNormalLine;
	fvertexWorldLocation = vec4(endOfNormalLine_WorldLocation, 1.0f);
	fUV = centreUV;
	EmitVertex();
	
	// The line:
	EndPrimitive();
	
}
	

//// Triangle strip version
//// Triangle #1
//	gl_Position = gl_in[0].gl_Position;
//	fColour = gColour[0];	
//	fvertexNormal = gVertexNormal[0];
//	fvertexWorldLocation = gVertexWorldLocation[0];
//	fUV = gUV[0];
//	EmitVertex();
//
//	gl_Position = gl_in[1].gl_Position;
//	fColour = gColour[1];	
//	fvertexNormal = gVertexNormal[1];
//	fvertexWorldLocation = gVertexWorldLocation[1];
//	fUV = gUV[1];
//	EmitVertex();
//
//	gl_Position = gl_in[2].gl_Position;
//	fColour = gColour[2];	
//	fvertexNormal = gVertexNormal[2];
//	fvertexWorldLocation = gVertexWorldLocation[2];
//	fUV = gUV[2];
//	EmitVertex();
//	
//// Triangle #2 or Vertex #4 of strip	(+ centre vertex)
//	gl_Position = gl_in[2].gl_Position;
//	fColour = gColour[2];	
//	fvertexNormal = gVertexNormal[2];
//	fvertexWorldLocation = gVertexWorldLocation[2];
//	fUV = gUV[2];
//	EmitVertex();
//	
//// Triangle #3 or Vertex #5 of strip	(+ centre vertex)
//	gl_Position = gl_in[0].gl_Position;
//	fColour = gColour[0];	
//	fvertexNormal = gVertexNormal[0];
//	fvertexWorldLocation = gVertexWorldLocation[0];
//	fUV = gUV[0];
//	EmitVertex(); 	
//	
//	// Triangle #3
//	EndPrimitive();