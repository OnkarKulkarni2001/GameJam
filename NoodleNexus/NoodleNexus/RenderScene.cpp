#include "GLCommon.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/quaternion.hpp>       // for the LASER quaternion lookAt
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "sharedThings.h"
#include "cBasicTextureManager/cBasicTextureManager.h"
#include "cLightHelper/cLightHelper.h"

#include "cFBO/cFBO_deferred.h"
#include "cDepthCubeMap.h"
#include "Animation/Model.h"
//#include "cParticleEmitter.h"
#include "cParticleEmitter_2.h"

#include "cPhysics.h"
#include <PhysXWraper/cPhysXWraper.h>

extern cDepthCubeMap* g_pShadowCubeMapFBO;

extern std::vector<Model*> vecAnimatedModels;

extern cPhysXWraper* g_pPhysX; //= NULL;

//extern cParticleEmitter* g_pParticles;
extern cParticleEmitter_2* g_pParticles;

extern cBasicTextureManager* g_pTextures;
extern cVAOManager* g_pMeshManager;
extern std::vector<sMesh*> g_vecMeshesToDraw;

// Deferred rendering Geometry "G" buffer
extern cFBO_deferred* g_pFBO_G_Buffer;
extern cDepthCubeMap* g_pShadowCubeMapFBO;
// If SetTexturesFromMeshInfo == false, then we have to set them up manually
// Now we pass the original (parent) matrix.
// We can also pass this matrix instead of the position, orientation, etc.
void DrawMesh(sMesh* pCurMesh, glm::mat4 matModel, GLuint program, bool SetTexturesFromMeshInfo = true);
void DrawMeshDepth(sMesh* pCurMesh, glm::mat4 matModel, GLuint program, bool SetTexturesFromMeshInfo = true);
void DrawAnimationModelDepth(Model* animationModel, glm::mat4 matModel, GLuint program, bool SetTexturesFromMeshInfo = true);
//void DrawMesh(sMesh* pCurMesh, GLuint program, bool SetTexturesFromMeshInfo = true);



void window_size_callback(GLFWwindow* window, int width, int height)
{
    if (::g_pFBO_G_Buffer)
    {
        std::string error;
        if (!::g_pFBO_G_Buffer->reset(width, height, error))
        {
            std::cout << "Error: Can't reset the deferred FBO because: "
                << error << std::endl;
        }
    }
    return;
}

//glm::mat4 ParaboloidProjectionMatrix() {
//    float nearPlane = 0.1f;
//    float farPlane = 100.0f;
//    float aspect = 1.0f;  // Square shadow map
//    float fov = glm::radians(90.0f);  // 180°/2 for paraboloid mapping
//
//    return glm::perspective(fov, aspect, nearPlane, farPlane);
//}
//
//
//void RenderSceneDepth(GLuint program,
//    GLuint shadowFBO,
//    glm::mat4 lightViewFront,
//    glm::mat4 lightViewBack,
//    glm::mat4 lightProjection,
//    float width,
//    float height)
//{
//    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
//    glViewport(0, 0, width, height);
//    glClear(GL_DEPTH_BUFFER_BIT);
//
//    glUseProgram(program);
//
//    glm::mat4 lightSpaceMatrixFront = lightProjection * lightViewFront;
//    glm::mat4 lightSpaceMatrixBack = lightProjection * lightViewBack;
//
//    glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrixFront"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrixFront));
//    glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrixBack"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrixBack));
//
//    // Render scene normally (only writing depth)
//    for (unsigned int meshIndex = 0; meshIndex != ::g_vecMeshesToDraw.size(); meshIndex++)
//    {
//        sMesh* pCurMesh = ::g_vecMeshesToDraw[meshIndex];
//        glm::mat4 matModel = glm::mat4(1.0f);
//        DrawMesh(pCurMesh, matModel, program, true);
//    }
//
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//}
//
//
//void RenderSceneWithParaboloidFront(cShadowFBO shadowFBO, GLuint program, float width, float height, glm::vec3 lightPos) {
//    glm::mat4 lightViewFront = glm::lookAt(lightPos, lightPos + glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
//    glm::mat4 lightViewBack = glm::lookAt(lightPos, lightPos + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
//    glm::mat4 lightProjection = ParaboloidProjectionMatrix();
//
//    // Bind the front shadow map to Texture Unit 50
//    glActiveTexture(GL_TEXTURE0 + 50);
//    glBindTexture(GL_TEXTURE_2D, shadowFBO.depthMapFront_ID);
//    GLint shadowMapFrontSampler = glGetUniformLocation(program, "shadowMapFront");
//    glUniform1i(shadowMapFrontSampler, 50);  // Tell shader to sample from texture 50
//
//    RenderSceneDepth(program, shadowFBO.ID, lightViewFront, lightViewBack, lightProjection, width, height);
//}
//
//void RenderSceneWithParaboloidBack(cShadowFBO shadowFBO, GLuint program, float width, float height, glm::vec3 lightPos) {
//    glm::mat4 lightViewFront = glm::lookAt(lightPos, lightPos + glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
//    glm::mat4 lightViewBack = glm::lookAt(lightPos, lightPos + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
//    glm::mat4 lightProjection = ParaboloidProjectionMatrix();
//
//    // Bind the back shadow map to Texture Unit 51
//    glActiveTexture(GL_TEXTURE0 + 51);
//    glBindTexture(GL_TEXTURE_2D, shadowFBO.depthMapBack_ID);
//    GLint shadowMapBackSampler = glGetUniformLocation(program, "shadowMapBack");
//    glUniform1i(shadowMapBackSampler, 51);  // Tell shader to sample from texture 51
//
//    RenderSceneDepth(program, shadowFBO.ID, lightViewFront, lightViewBack, lightProjection, width, height);
//}


void RenderScene(
    GLuint program, 
    glm::mat4 matProjection,
    glm::mat4 matView,
    float ratio,
    glm::vec3 eyeLocation)
{

    GLint matProjection_UL = glGetUniformLocation(program, "matProjection");
    //glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, (const GLfloat*)&matProjection);

    GLint matView_UL = glGetUniformLocation(program, "matView");
    glUniformMatrix4fv(matView_UL, 1, GL_FALSE, (const GLfloat*)&matView);


    // **************************************************************
// Sky box
// Move the sky sphere with the camera
    sMesh* pSkySphere = ::g_pFindMeshByFriendlyName("SkySphere");
    pSkySphere->positionXYZ = eyeLocation;
//    pSkySphere->positionXYZ = glm::vec3(30.0f, -5.0f, 0.0f);

    // Disable backface culling (so BOTH sides are drawn)
    glDisable(GL_CULL_FACE);
    // Don't perform depth buffer testing
    glDisable(GL_DEPTH_TEST);
    // Don't write to the depth buffer when drawing to colour (back) buffer
//        glDepthMask(GL_FALSE);
//        glDepthFunc(GL_ALWAYS);// or GL_LESS (default)
        // GL_DEPTH_TEST : do or not do the test against what's already on the depth buffer


    // Move clipping plane close just for the skybox:
    glm::mat4 matProjSkyBox = glm::perspective(0.6f, ratio, 0.1f, 10.0f); 
    glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, (const GLfloat*)&matProjSkyBox);



    // Tell the shader this is the skybox, so use the cube map
    // uniform samplerCube skyBoxTexture;
    // uniform bool bIsSkyBoxObject;
    GLuint bIsSkyBoxObject_UL = glGetUniformLocation(program, "bIsSkyBoxObject");
    glUniform1f(bIsSkyBoxObject_UL, (GLfloat)GL_TRUE);

    // Set the cube map texture, just like we do with the 2D
    GLuint spaceCubeMapID = ::g_pTextures->getTextureIDFromName("Space");
    //        GLuint cubeSamplerID = ::g_pTextures->getTextureIDFromName("SunnyDay");
            // Make sure this is an unused texture unit
    glActiveTexture(GL_TEXTURE0 + 33);
    // *****************************************
    // NOTE: This is a CUBE_MAP, not a 2D
    //glBindTexture(GL_TEXTURE_CUBE_MAP, g_pShadowCubeMapFBO->depthCubemapTex_ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, spaceCubeMapID);
    //        glBindTexture(GL_TEXTURE_2D, cubeSamplerID);
            // *****************************************
    GLint skyBoxTextureSampler_UL = glGetUniformLocation(program, "skyBoxTextureSampler");
    glUniform1i(skyBoxTextureSampler_UL, 33);       // <-- Note we use the NUMBER, not the GL_TEXTURE3 here

    pSkySphere->bIsVisible = true;
    //    pSkySphere->bDoNotLight = true;

    pSkySphere->uniformScale = 1.0f;

    glm::mat4 matWorld = glm::mat4(1.0f);
    DrawMesh(pSkySphere, matWorld, program, true);

    pSkySphere->bIsVisible = false;

    glUniform1f(bIsSkyBoxObject_UL, (GLfloat)GL_FALSE);

    glEnable(GL_CULL_FACE);
    // Enable depth test and write to depth buffer (normal rendering)
    glEnable(GL_DEPTH_TEST);
    //        glDepthMask(GL_FALSE);
    //        glDepthFunc(GL_LESS);
            // **************************************************************


    // Set clipping planes back to normal

    glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, (const GLfloat*)&matProjection);


    ::g_pLightManager->updateShaderWithLightInfo();

// ******************************************************************
// Draw the stuff that's in the PhysX world
    std::vector<cPhysicsObjectTypes> vecPhysActors;
    ::g_pPhysX->getSceneActors(vecPhysActors);

    sMesh* pPhysXCube = g_pFindMeshByFriendlyName("pPhysXCube");
    sMesh* pPhysXSphere = g_pFindMeshByFriendlyName("pPhysXSphere");


    for (cPhysicsObjectTypes& object : vecPhysActors)
    {
        switch (object.shapeType)
        {
        case cPhysicsObjectTypes::BOX:
            if (pPhysXCube)
            {
                pPhysXCube->bIsVisible = true;
                // Set to origin because we'll be using the model matrix
                //  from PhysX to set the values
                pPhysXCube->positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
                pPhysXCube->rotationEulerXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
                pPhysXCube->uniformScale = 1.0f;
                // 
                DrawMesh(pPhysXCube, object.matModel, program, true);
                pPhysXCube->bIsVisible = false;
            }

            break;

        case cPhysicsObjectTypes::SPHERE:
            if (pPhysXSphere)
            {
                pPhysXSphere->bIsVisible = true;
                // Set to origin because we'll be using the model matrix
                //  from PhysX to set the values
                pPhysXSphere->positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
                pPhysXSphere->rotationEulerXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
                pPhysXSphere->uniformScale = 1.0f;
                // 
                DrawMesh(pPhysXSphere, object.matModel, program, true);
                pPhysXSphere->bIsVisible = false;
            }
            break;
        }

    }//for (cPhysicsObjectTypes&
// ******************************************************************

    // Draw the particles
    DrawDebugSphere(
        ::g_pParticles->GetLocation(),
        glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),
        0.2f,
        program);

    std::vector<cParticle*> vecLiveParticles;
    ::g_pParticles->GetLiveParticles(vecLiveParticles);

    for (cParticle* pCurParticle : vecLiveParticles)
    {
        DrawDebugSphere(
            pCurParticle->position,
            glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
            0.1f,
            program);
    }
     

    // Draw everything again, but this time far away things
    for (unsigned int meshIndex = 0; meshIndex != ::g_vecMeshesToDraw.size(); meshIndex++)
    {
        //            sMesh* pCurMesh = ::g_myMeshes[meshIndex];
        sMesh* pCurMesh = ::g_vecMeshesToDraw[meshIndex];
        //            pCurMesh->bDoNotLight = true;

        glm::mat4 matModel = glm::mat4(1.0f);   // identity matrix

        DrawMesh(pCurMesh, matModel, program, true);

    }//for (unsigned int meshIndex..

    // *******************************************************************

    // HACK

 //   // HACK: Use sin() to "move" the vertices
 //   for (unsigned int index = 0; index != flagMesh.numberOfVertices; index++)
 //   {
 ////       flagMesh.pVertices[index].y += (sin(glfwGetTime()) / 100.0f);
 //       flagMesh.pVertices[index].x += getRandomFloat(-0.01f, 0.01f);
 //       flagMesh.pVertices[index].y += getRandomFloat(-0.01f, 0.01f);
 //       flagMesh.pVertices[index].z += getRandomFloat(-0.01f, 0.01f);
 //   }
//    sModelDrawInfo softBodyBunnyMesh;
//    ::g_pMeshManager->FindDrawInfoByModelName("SoftBodyBunnyMesh", softBodyBunnyMesh);
//
//
//    cPhysics::cSoftBody_Verlet* pSoftBodyBunny
//        = ::g_pPhysicEngine->pFindSoftBodyByFriendlyName("Soft_Body_Bunny");
//
//    // Found it?
//    if (pSoftBodyBunny)
//    {
//        // Copy the current soft body vertex information back to the mesh
//        //  we are rendering...
//        for (unsigned int index = 0; index != softBodyBunnyMesh.numberOfVertices; index++)
//        {
//            softBodyBunnyMesh.pVertices[index].x = pSoftBodyBunny->vecPoints[index].position.x;
//            softBodyBunnyMesh.pVertices[index].y = pSoftBodyBunny->vecPoints[index].position.y;
//            softBodyBunnyMesh.pVertices[index].z = pSoftBodyBunny->vecPoints[index].position.z;
//        }
//
//        ::g_pMeshManager->UpdateDynamicMesh("SoftBodyBunnyMesh", softBodyBunnyMesh, program);
//    }
//
//    sModelDrawInfo flagMesh;
//    ::g_pMeshManager->FindDrawInfoByModelName("Canadian_Flag_Mesh", flagMesh);
 //   cPhysics::cSoftBody_Verlet* pCanFlagSB 
 //       = ::g_pPhysicEngine->pFindSoftBodyByFriendlyName("Flag_Canada_SoftBody");
 //
 //   // Found it?
 //   if (pCanFlagSB)
 //   {
 //       // Copy the current soft body vertex information back to the mesh
 //       //  we are rendering...
 //       for (unsigned int index = 0; index != flagMesh.numberOfVertices; index++)
 //       {
 //           flagMesh.pVertices[index].x = pCanFlagSB->vecPoints[index].position.x;
 //           flagMesh.pVertices[index].y = pCanFlagSB->vecPoints[index].position.y;
 //           flagMesh.pVertices[index].z = pCanFlagSB->vecPoints[index].position.z;
 //       }
 //
 //       ::g_pMeshManager->UpdateDynamicMesh("Canadian_Flag_Mesh", flagMesh, program);
 //   }




    // **********************************************************************************
    if (::g_bShowDebugSpheres)
    {
        cLightHelper TheLightHelper;

        DrawDebugSphere(::g_pLightManager->theLights[::g_selectedLightIndex].position,
            glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.1f, program);

        const float DEBUG_LIGHT_BRIGHTNESS = 0.3f;

        const float ACCURACY = 0.1f;       // How many units distance
        float distance_75_percent =
            TheLightHelper.calcApproxDistFromAtten(0.75f, ACCURACY, FLT_MAX,
                ::g_pLightManager->theLights[::g_selectedLightIndex].atten.x,   // Const attent
                ::g_pLightManager->theLights[::g_selectedLightIndex].atten.y,   // Linear attenuation
                ::g_pLightManager->theLights[::g_selectedLightIndex].atten.z);  // Quadratic attenuation

        DrawDebugSphere(::g_pLightManager->theLights[::g_selectedLightIndex].position,
            glm::vec4(DEBUG_LIGHT_BRIGHTNESS, 0.0f, 0.0f, 1.0f),
            distance_75_percent,
            program);


        float distance_50_percent =
            TheLightHelper.calcApproxDistFromAtten(0.5f, ACCURACY, FLT_MAX,
                ::g_pLightManager->theLights[::g_selectedLightIndex].atten.x,   // Const attent
                ::g_pLightManager->theLights[::g_selectedLightIndex].atten.y,   // Linear attenuation
                ::g_pLightManager->theLights[::g_selectedLightIndex].atten.z);  // Quadratic attenuation

        DrawDebugSphere(::g_pLightManager->theLights[::g_selectedLightIndex].position,
            glm::vec4(0.0f, DEBUG_LIGHT_BRIGHTNESS, 0.0f, 1.0f),
            distance_50_percent,
            program);

        float distance_25_percent =
            TheLightHelper.calcApproxDistFromAtten(0.25f, ACCURACY, FLT_MAX,
                ::g_pLightManager->theLights[::g_selectedLightIndex].atten.x,   // Const attent
                ::g_pLightManager->theLights[::g_selectedLightIndex].atten.y,   // Linear attenuation
                ::g_pLightManager->theLights[::g_selectedLightIndex].atten.z);  // Quadratic attenuation

        DrawDebugSphere(::g_pLightManager->theLights[::g_selectedLightIndex].position,
            glm::vec4(0.0f, 0.0f, DEBUG_LIGHT_BRIGHTNESS, 1.0f),
            distance_25_percent,
            program);

        float distance_05_percent =
            TheLightHelper.calcApproxDistFromAtten(0.05f, ACCURACY, FLT_MAX,
                ::g_pLightManager->theLights[::g_selectedLightIndex].atten.x,   // Const attent
                ::g_pLightManager->theLights[::g_selectedLightIndex].atten.y,   // Linear attenuation
                ::g_pLightManager->theLights[::g_selectedLightIndex].atten.z);  // Quadratic attenuation

        DrawDebugSphere(::g_pLightManager->theLights[::g_selectedLightIndex].position,
            glm::vec4(DEBUG_LIGHT_BRIGHTNESS, DEBUG_LIGHT_BRIGHTNESS, 0.0f, 1.0f),
            distance_05_percent,
            program);

    }
    // **********************************************************************************









    //sMesh* pBall = pFindMeshByFriendlyName("Ball");
    //if (pBall)
    //{
    //    pBall->positionXYZ.y -= 1.0f * deltaTime;
    //}

    // HACK: Update "shadow" of ball to be where the ball hits the large block ground
    sMesh* pBallShadow = ::g_pFindMeshByFriendlyName("Ball_Shadow");
    sMesh* pBall = ::g_pFindMeshByFriendlyName("Ball");
    pBallShadow->positionXYZ.x = pBall->positionXYZ.x;
    pBallShadow->positionXYZ.z = pBall->positionXYZ.z;
    // Don't update the y - keep the shadow near the plane


    // Point the spot light to the ball
    sMesh* pBouncy_5_Ball = ::g_pFindMeshByFriendlyName("Bouncy_5");
    if (pBouncy_5_Ball)
    {
        glm::vec3 directionToBal
            = pBouncy_5_Ball->positionXYZ - glm::vec3(::g_pLightManager->theLights[1].position);

        // Normalize to get the direction only
        directionToBal = glm::normalize(directionToBal);

        // Point the spot light at the bouncy ball
        ::g_pLightManager->theLights[1].direction = glm::vec4(directionToBal, 1.0f);
    }


	return;
}

void RenderSceneDepth(GLuint shadowShaderProgram, float ratio)
{
    //glUseProgram(shadowShaderProgram);
    //glViewport(0, 0, g_pShadowCubeMapFBO->width, g_pShadowCubeMapFBO->height);
    //glBindFramebuffer(GL_FRAMEBUFFER, g_pShadowCubeMapFBO->depthMapFBO_ID);
    //glClear(GL_DEPTH_BUFFER_BIT);

    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // Draw the stuff that's in the PhysX world

    //glm::mat4 matProjection = glm::mat4(1.0f);
    //glm::mat4 matView = glm::mat4(1.0f);
    //glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);


    /*matView = glm::lookAt(
        ::g_pFlyCamera->getEyeLocation(),
        ::g_pFlyCamera->getTargetLocation(),
        upVector);

    matProjection = glm::perspective(0.6f,
        ratio,
        1.0f,
        50'000.0f);

    GLint matProjection_UL = glGetUniformLocation(shadowShaderProgram, "matProjection");
    glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, (const GLfloat*)&matProjection);

    GLint matView_UL = glGetUniformLocation(shadowShaderProgram, "matView");
    glUniformMatrix4fv(matView_UL, 1, GL_FALSE, (const GLfloat*)&matView);*/

    ::g_pLightManager->updateShaderWithLightInfo();

    std::vector<cPhysicsObjectTypes> vecPhysActors;
    ::g_pPhysX->getSceneActors(vecPhysActors);

    sMesh* pPhysXCube = g_pFindMeshByFriendlyName("pPhysXCube");
    sMesh* pPhysXSphere = g_pFindMeshByFriendlyName("pPhysXSphere");


    for (cPhysicsObjectTypes& object : vecPhysActors)
    {
        switch (object.shapeType)
        {
        case cPhysicsObjectTypes::BOX:
            if (pPhysXCube)
            {
                pPhysXCube->bIsVisible = true;
                // Set to origin because we'll be using the model matrix
                //  from PhysX to set the values
                pPhysXCube->positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
                pPhysXCube->rotationEulerXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
                pPhysXCube->uniformScale = 1.0f;
                // 
                DrawMeshDepth(pPhysXCube, object.matModel, shadowShaderProgram, true);
                pPhysXCube->bIsVisible = false;
            }

            break;

        case cPhysicsObjectTypes::SPHERE:
            if (pPhysXSphere)
            {
                pPhysXSphere->bIsVisible = true;
                // Set to origin because we'll be using the model matrix
                //  from PhysX to set the values
                pPhysXSphere->positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
                pPhysXSphere->rotationEulerXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
                pPhysXSphere->uniformScale = 1.0f;
                // 
                DrawMeshDepth(pPhysXSphere, object.matModel, shadowShaderProgram, true);
                pPhysXSphere->bIsVisible = false;
            }
            break;
        }

    }//for (cPhysicsObjectTypes&

    // Draw animated models
    for (Model* animatedModel : vecAnimatedModels) {
        glm::mat4 matModel = glm::mat4(1.0f);
        DrawAnimationModelDepth(animatedModel, matModel, shadowShaderProgram, true);
    }

    // Draw everything again, but this time far away things
    for (unsigned int meshIndex = 0; meshIndex != ::g_vecMeshesToDraw.size(); meshIndex++)
    {
        //            sMesh* pCurMesh = ::g_myMeshes[meshIndex];
        sMesh* pCurMesh = ::g_vecMeshesToDraw[meshIndex];
        //            pCurMesh->bDoNotLight = true;

        glm::mat4 matModel = glm::mat4(1.0f);   // identity matrix

        DrawMeshDepth(pCurMesh, matModel, shadowShaderProgram, true);

    }//for (unsigned int meshIndex..

    //// Render the scene for each face of the cube map
    //for (int face = 0; face < 6; ++face)
    //{
    //    // Bind the framebuffer and set the layer to render to this face
    //    glBindFramebuffer(GL_FRAMEBUFFER, g_pShadowCubeMapFBO->depthMapFBO_ID);
    //    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, g_pShadowCubeMapFBO->depthCubemapTex_ID, 0, face);

    //    // Clear the depth buffer for this face
    //    glClear(GL_DEPTH_BUFFER_BIT);

    //    // Set the current face
    //    GLint faceLoc = glGetUniformLocation(shadowShaderProgram, "currentFace");
    //    if (faceLoc != -1)
    //    {
    //        glUniform1i(faceLoc, face);
    //    }

    //    // Draw the scene
    //    std::vector<cPhysicsObjectTypes> vecPhysActors;
    //    ::g_pPhysX->getSceneActors(vecPhysActors);

    //    sMesh* pPhysXCube = g_pFindMeshByFriendlyName("pPhysXCube");
    //    sMesh* pPhysXSphere = g_pFindMeshByFriendlyName("pPhysXSphere");

    //    for (cPhysicsObjectTypes& object : vecPhysActors)
    //    {
    //        switch (object.shapeType)
    //        {
    //        case cPhysicsObjectTypes::BOX:
    //            if (pPhysXCube)
    //            {
    //                pPhysXCube->bIsVisible = true;
    //                pPhysXCube->positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
    //                pPhysXCube->rotationEulerXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
    //                pPhysXCube->uniformScale = 1.0f;
    //                DrawMeshDepth(pPhysXCube, object.matModel, shadowShaderProgram, true);
    //                pPhysXCube->bIsVisible = false;
    //            }
    //            break;

    //        case cPhysicsObjectTypes::SPHERE:
    //            if (pPhysXSphere)
    //            {
    //                pPhysXSphere->bIsVisible = true;
    //                pPhysXSphere->positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
    //                pPhysXSphere->rotationEulerXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
    //                pPhysXSphere->uniformScale = 1.0f;
    //                DrawMeshDepth(pPhysXSphere, object.matModel, shadowShaderProgram, true);
    //                pPhysXSphere->bIsVisible = false;
    //            }
    //            break;
    //        }
    //    }

    //    // Draw animated models
    //    for (Model* animatedModel : vecAnimatedModels)
    //    {
    //        glm::mat4 matModel = glm::mat4(1.0f);
    //        DrawAnimationModelDepth(animatedModel, matModel, shadowShaderProgram, true);
    //    }

    //    // Draw everything else
    //    for (unsigned int meshIndex = 0; meshIndex != ::g_vecMeshesToDraw.size(); meshIndex++)
    //    {
    //        sMesh* pCurMesh = ::g_vecMeshesToDraw[meshIndex];
    //        glm::mat4 matModel = glm::mat4(1.0f);
    //        DrawMeshDepth(pCurMesh, matModel, shadowShaderProgram, true);
    //    }
    //}

    glEnable(GL_CULL_FACE);
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return;
}