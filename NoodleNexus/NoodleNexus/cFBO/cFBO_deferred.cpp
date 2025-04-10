#include "cFBO_deferred.h"


// Calls shutdown(), then init()
bool cFBO_deferred::reset(int width, int height, std::string& error)
{
	if (!this->shutdown())
	{
		error = "Could not shutdown";
		return false;
	}

	return this->init(width, height, error);
}

bool cFBO_deferred::shutdown(void)
{
//	glDeleteTextures(1, &(this->colourTexture_0_ID));
	glDeleteTextures(1, &(this->vertexWorldLocationXYZ));
	glDeleteTextures(1, &(this->vertexNormalXYZ));
	glDeleteTextures(1, &(this->vertexDiffuseRGB));
	glDeleteTextures(1, &(this->vertexSpecularRGA_P));
	glDeleteTextures(1, &(this->vertexUV));

	glDeleteTextures(1, &(this->depthTexture_ID));

	glDeleteFramebuffers(1, &(this->ID));

	return true;
}


bool cFBO_deferred::init(int width, int height, std::string& error)
{
	this->width = width;
	this->height = height;

	//	glCreateFramebuffers(1, &( this->ID ) );	// GL 4.5		//g_FBO
	glGenFramebuffers(1, &(this->ID));		// GL 3.0
	glBindFramebuffer(GL_FRAMEBUFFER, this->ID);

	//************************************************************
	glGenTextures(1, &(this->vertexWorldLocationXYZ));		
	glBindTexture(GL_TEXTURE_2D, this->vertexWorldLocationXYZ);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F,	// 32 bits (1 float) per colour
		//	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8,		
		this->width,				// g_FBO_SizeInPixes
		this->height);			// g_FBO_SizeInPixes

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//***************************************************************

	//************************************************************
	glGenTextures(1, &(this->vertexNormalXYZ));
	glBindTexture(GL_TEXTURE_2D, this->vertexNormalXYZ);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F,	// 32 bits (1 float) per colour		
		this->width,				// g_FBO_SizeInPixes
		this->height);				// g_FBO_SizeInPixes

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//***************************************************************

	//************************************************************
	glGenTextures(1, &(this->vertexDiffuseRGB));
	glBindTexture(GL_TEXTURE_2D, this->vertexDiffuseRGB);

	// likely 32 bit floats per pixel is too much (colours are 8 bit, so GL_RGBA32F not needed)
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8,	// 32 bits (1 float) per colour		
		this->width,				// g_FBO_SizeInPixes
		this->height);				// g_FBO_SizeInPixes

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//***************************************************************


	//************************************************************
	glGenTextures(1, &(this->vertexSpecularRGA_P));
	glBindTexture(GL_TEXTURE_2D, this->vertexSpecularRGA_P);

	// likely 32 bit floats per pixel is too much (colours are 8 bit, so GL_RGBA32F not needed)
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8,	// 32 bits (1 float) per colour		
		this->width,				// g_FBO_SizeInPixes
		this->height);				// g_FBO_SizeInPixes

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//***************************************************************


	//************************************************************
	glGenTextures(1, &(this->vertexUV));
	glBindTexture(GL_TEXTURE_2D, this->vertexUV);

	// likely 32 bit floats per pixel is too much (colours are 8 bit, so GL_RGBA32F not needed)
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8,	// 32 bits (1 float) per colour		
		this->width,				// g_FBO_SizeInPixes
		this->height);				// g_FBO_SizeInPixes

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//***************************************************************



	// Create the depth buffer (texture)
	glGenTextures(1, &(this->depthTexture_ID));			//g_FBO_depthTexture
	glBindTexture(GL_TEXTURE_2D, this->depthTexture_ID);

	//glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, ]

	// Note that, unless you specifically ask for it, the stencil buffer
	// is NOT present... i.e. GL_DEPTH_COMPONENT32F DOESN'T have stencil

	// These are:
	// - GL_DEPTH32F_STENCIL8, which is 32 bit float depth + 8 bit stencil
	// - GL_DEPTH24_STENCIL8,  which is 24 bit float depth + 8 bit stencil (more common?)
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8,	//GL_DEPTH32F_STENCIL8,
		this->width,		//g_FBO_SizeInPixes
		this->height);
	//	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_DEPTH_COMPONENT );
	//	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_STENCIL_COMPONENT );
	//	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, this->width, this->height, 0, GL_EXT_packe

	//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH24_STENCIL8, GL_TEXTURE_2D, this->depthTexture_ID, 0);

	// ***************************************************************



	//	glFramebufferTexture(GL_FRAMEBUFFER,
	//						 GL_DEPTH_ATTACHMENT,
	//						 this->depthTexture_ID, 0);
	glFramebufferTexture(GL_FRAMEBUFFER,
		GL_DEPTH_STENCIL_ATTACHMENT,
		this->depthTexture_ID, 0);



// The textures that are backing this FBO are created
// NOW we have to tell OpenGL where the shader needs to output its stuff

// Our shader output (pass #1) looks like this:
//	out vec4 vertexWorldLocationXYZ;	// w = TBD
//	out vec4 vertexNormalXYZ;			// w = TBD
//	out vec4 vertexDiffuseRGB;			// w = TBD
//	out vec4 vertexSpecularRGA_P;		// w = power


	glFramebufferTexture(GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0, this->vertexWorldLocationXYZ, 0);

	glFramebufferTexture(GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT1, this->vertexNormalXYZ, 0);

	glFramebufferTexture(GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT2, this->vertexDiffuseRGB, 0);

	glFramebufferTexture(GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT3, this->vertexSpecularRGA_P, 0);

	glFramebufferTexture(GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT4, this->vertexUV, 0);
	// this essentially "turns on and off" the various layers.
	// note the strange parameters
	static const GLenum draw_bufers[] =
	{
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT4
	};
	glDrawBuffers(5, draw_bufers);		// There are 5 outputs now

	// ***************************************************************




	// ADD ONE MORE THING...
	bool bFrameBufferIsGoodToGo = true;

	switch (glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
	case GL_FRAMEBUFFER_COMPLETE:
		bFrameBufferIsGoodToGo = true;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		error = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
		bFrameBufferIsGoodToGo = false;
		break;
		//	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
	case GL_FRAMEBUFFER_UNSUPPORTED:
	default:
		bFrameBufferIsGoodToGo = false;
		break;
	}//switch ( glCheckFramebufferStatus(GL_FRAMEBUFFER) )

	// Point back to default frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return bFrameBufferIsGoodToGo;
}

void cFBO_deferred::clearColourBuffer(int bufferindex)
{
	glViewport(0, 0, this->width, this->height);
	GLfloat	zero = 0.0f;
	glClearBufferfv(GL_COLOR, bufferindex, &zero);

	return;
}


void cFBO_deferred::clearBuffers(bool bClearColour, bool bClearDepth)
{
	glViewport(0, 0, this->width, this->height);
	GLfloat	zero = 0.0f;
	GLfloat one = 1.0f;
	if (bClearColour)
	{
		glClearBufferfv(GL_COLOR, 0, &zero);		// Colour "0" --> GL_COLOR_ATTACHMENT0
		glClearBufferfv(GL_COLOR, 1, &zero);		// Colour "1" --> GL_COLOR_ATTACHMENT1
		glClearBufferfv(GL_COLOR, 2, &zero);		// Colour "2" --> GL_COLOR_ATTACHMENT2
		glClearBufferfv(GL_COLOR, 3, &zero);		// Colour "3" --> GL_COLOR_ATTACHMENT3
		glClearBufferfv(GL_COLOR, 4, &zero);		// Colour "4" --> GL_COLOR_ATTACHMENT4
		// Colour "1" --> GL_COLOR_ATTACHMENT1, etc.
	}

	if (bClearDepth)
	{
		glClearBufferfv(GL_DEPTH, 0, &one);		// Depth is normalized 0.0 to 1.0f
	}
	// If buffer is GL_STENCIL, drawbuffer must be zero, and value points to a 
	//  single value to clear the stencil buffer to. Masking is performed in the 
	//  same fashion as for glClearStencil. Only the *iv forms of these commands 
	//  should be used to clear stencil buffers; be used to clear stencil buffers; 
	//  other forms do not accept a buffer of GL_STENCIL.

	// 
	glStencilMask(0xFF);

	{	// Clear stencil
		//GLint intZero = 0;
		//glClearBufferiv(GL_STENCIL, 0, &intZero );
		glClearBufferfi(GL_DEPTH_STENCIL,
			0,		// Must be zero
			1.0f,	// Clear value for depth
			0);	// Clear value for stencil
	}

	return;
}



int cFBO_deferred::getMaxColourAttachments(void)
{
	//  void glGetIntegerv(GLenum pname,
	// 				       GLint * data);

	int maxColourAttach = 0;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColourAttach);

	return maxColourAttach;
}

int cFBO_deferred::getMaxDrawBuffers(void)
{
	int maxDrawBuffers = 0;
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuffers);

	return maxDrawBuffers;
}
