// Geometry splits one triangle into 3 triangles shader
#version 420
layout(triangles) 			in;
layout(triangle_strip)		out;	// Have to be 'strips'
layout(max_vertices = 15)	out;

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
	vec4 triCentre = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3.0f;
	vec3 normCentre = (gVertexNormal[0].xyz + gVertexNormal[1].xyz + gVertexNormal[2].xyz) / 3.0f;
	vec3 averageColour = (gColour[0] + gColour[1] + gColour[2]) / 3.0f;
	vec3 averageWorldLocation = (gVertexWorldLocation[0].xyz 
	                             + gVertexWorldLocation[1].xyz 
								 + gVertexWorldLocation[0].xyz) / 3.0f;
	vec2 centreUV = (gUV[0] + gUV[1] + gUV[2]) / 3.0f;
	
	
// Note: Even though this is emitting a traingle strip, 
//       it seems that you have to "re-emit" the same vertices 
//       AGAIN, not just add the next vertex in the strip.
// 
// From: https://www.khronos.org/opengl/wiki/Geometry_Shader
// "The GS defines what kind of primitive these vertex outputs represent. 
//  The GS can also end a primitive and start a new one, by calling the EndPrimitive() 
//  function. This does not emit a vertex."
// 
// "In order to write two independent triangles from a GS, you must write three separate 
//  vertices with EmitVertex() for the first three vertices, then call EndPrimitive() 
//  to end the strip and start a new one. Then you write three more vertices with EmitVertex()."
//	
	
// Triangle #1
	gl_Position = gl_in[0].gl_Position;
	fColour = gColour[0];	
	fvertexNormal = gVertexNormal[0];
	fvertexWorldLocation = gVertexWorldLocation[0];
	fUV = gUV[0];
	EmitVertex();

	gl_Position = gl_in[1].gl_Position;
	fColour = gColour[1];	
	fvertexNormal = gVertexNormal[1];
	fvertexWorldLocation = gVertexWorldLocation[1];
	fUV = gUV[1];
	EmitVertex();

	gl_Position = gl_in[2].gl_Position;
	fColour = gColour[2];	
	fvertexNormal = gVertexNormal[2];
	fvertexWorldLocation = gVertexWorldLocation[2];
	fUV = gUV[2];
	EmitVertex();
	
	// Triangle #1
	EndPrimitive();

// Triangle #2 or Vertex #4 of strip	(+ centre vertex)
	gl_Position = gl_in[1].gl_Position;
	fColour = gColour[1];	
	fvertexNormal = gVertexNormal[1];
	fvertexWorldLocation = gVertexWorldLocation[1];
	fUV = gUV[1];
	EmitVertex();

	gl_Position = triCentre;
	fColour = averageColour;	
	fvertexNormal = vec4(normCentre, 1.0f);
	fvertexWorldLocation = vec4(averageWorldLocation, 1.0f);
	fUV = centreUV;
	EmitVertex();
	
	gl_Position = gl_in[2].gl_Position;
	fColour = gColour[2];	
	fvertexNormal = gVertexNormal[2];
	fvertexWorldLocation = gVertexWorldLocation[2];
	fUV = gUV[2];
	EmitVertex();
	
	// Triangle #2
	EndPrimitive();
	
// Triangle #3 or Vertex #5 of strip	(+ centre vertex)
	gl_Position = triCentre;
	fColour = averageColour;	
	fvertexNormal = vec4(normCentre, 1.0f);
	fvertexWorldLocation = vec4(averageWorldLocation, 1.0f);
	fUV = centreUV;
	EmitVertex();

	gl_Position = gl_in[2].gl_Position;
	fColour = gColour[2];	
	fvertexNormal = gVertexNormal[2];
	fvertexWorldLocation = gVertexWorldLocation[2];
	fUV = gUV[2];
	EmitVertex();

	gl_Position = gl_in[0].gl_Position;
	fColour = gColour[0];	
	fvertexNormal = gVertexNormal[0];
	fvertexWorldLocation = gVertexWorldLocation[0];
	fUV = gUV[0];
	EmitVertex(); 	
	
	// Triangle #3
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