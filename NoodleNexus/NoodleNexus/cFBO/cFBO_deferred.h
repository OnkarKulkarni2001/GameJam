#pragma once

#ifndef _FBO_HG_
#define _FBO_HG_

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class cFBO_deferred
{
public:
	cFBO_deferred() :
		ID(0), 
		vertexWorldLocationXYZ(0),
		vertexNormalXYZ(0),
		vertexDiffuseRGB(0),
		vertexSpecularRGA_P(0),
		vertexUV(0),
		//
		depthTexture_ID(0), 
		width(-1), height(-1) {};

	GLuint ID;						
	GLuint vertexWorldLocationXYZ;	// w = TBD
	GLuint vertexNormalXYZ;			// w = TBD
	GLuint vertexDiffuseRGB;		// w = TBD
	GLuint vertexSpecularRGA_P;		// w = power 'P' for Power
	GLuint vertexUV;
	
	GLuint depthTexture_ID;		// = 0;
	GLint width;		// = 512 the WIDTH of the framebuffer, in pixels;
	GLint height;

	// Inits the FBP
	bool init(int width, int height, std::string &error);
	bool shutdown(void);
	// Calls shutdown(), then init()
	bool reset(int width, int height, std::string &error);
	
	void clearBuffers(bool bClearColour = true, bool bClearDepth = true);

	void clearColourBuffer(int bufferindex);
	void clearAllColourBuffers(void);
	void clearDepthBuffer(void);
	void clearStencilBuffer( int clearColour, int mask = 0xFF );

	int getMaxColourAttachments(void);
	int getMaxDrawBuffers(void);
};

#endif
