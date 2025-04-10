//#define GLAD_GL_IMPLEMENTATION
//#include <glad/glad.h>
//
//#define GLFW_INCLUDE_NONE
//#include <GLFW/glfw3.h>
#include "GLCommon.h"


//#include "linmath.h"
//#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
//#include <glm/gtc/matrix_transform.hpp> 
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include <iostream>     // "input output" stream
#include <fstream>      // "file" stream
#include <sstream>      // "string" stream ("string builder" in Java c#, etc.)
#include <string>
#include <vector>

//void ReadPlyModelFromFile(std::string plyFileName);
#include "PlyFileLoaders.h"
#include "Basic_Shader_Manager/cShaderManager.h"
#include "sMesh.h"
#include "cVAOManager/cVAOManager.h"
#include "sharedThings.h"       // Fly camera
#include "cPhysics.h"
#include "cLightManager.h"
#include <windows.h>    // Includes ALL of windows... MessageBox
#include "cLightHelper/cLightHelper.h"
//
#include "cBasicTextureManager/cBasicTextureManager.h"

#include "cLowPassFilter.h"

#include "cTerrainPathChooser.h"

// Frame Buffer Object (FBO)
//#include "cFBO/cFBO_RGB_depth.h"
// Deferred render FBO
#include "cFBO/cFBO_deferred.h"
#include "cDepthCubeMap.h"
#include "cViperFlagConnector.h"

#include "PhysXWraper/cPhysXWraper.h"

//#include "cParticleEmitter.h"
#include "cParticleEmitter_2.h"

#include "Animation/Model.h"
#include "cCharacter.h"
#include "Animation/Animator.h"
#include "Animation/Animation.h"


std::vector<Model*> vecAnimatedModels;
Animator* g_pAnimator = nullptr;
cCharacter* g_pCharacter = nullptr;

// Deferred rendering Geometry "G" buffer
cFBO_deferred* g_pFBO_G_Buffer = NULL;
cDepthCubeMap* g_pShadowCubeMapFBO = NULL;

GLuint program = NULL;
GLuint shadowShaderProgram = NULL;

//const unsigned int MAX_NUMBER_OF_MESHES = 1000;
//unsigned int g_NumberOfMeshesToDraw;
//sMesh* g_myMeshes[MAX_NUMBER_OF_MESHES] = { 0 };    // All zeros

std::vector<sMesh*> g_vecMeshesToDraw;

cPhysics* g_pPhysicEngine = NULL;
cPhysXWraper* g_pPhysX = NULL;

// This loads the 3D models for drawing, etc.
cVAOManager* g_pMeshManager = NULL;

cBasicTextureManager* g_pTextures = NULL;

cCommandGroup* g_pCommandDirector = NULL;
cCommandFactory* g_pCommandFactory = NULL;

cTerrainPathChooser* g_pTerrainPathChooser = NULL;

extern cViperFlagConnector* g_pViperFlagConnector;
extern cLightManager* g_pLightManager;

//cParticleEmitter* g_pParticles = NULL;
cParticleEmitter_2* g_pParticles = NULL;

//cLightManager* g_pLightManager = NULL;

void AddModelsToScene(cVAOManager* pMeshManager, GLuint shaderProgram, GLuint shadowShaderProgram);

//void DrawMesh(sMesh* pCurMesh, GLuint program, bool SetTexturesFromMeshInfo = true);
// Now we pass the original (parent) matrix.
// We can also pass this matrix instead of the position, orientation, etc.
void DrawMesh(sMesh* pCurMesh, glm::mat4 matModel, GLuint program, bool SetTexturesFromMeshInfo = true);

//glm::vec3 cameraEye = glm::vec3(0.0, 0.0, 4.0f);

void RenderSceneDepth(GLuint shadowShaderProgram);
void RenderScene(
    GLuint program,
    glm::mat4 matProjection,
    glm::mat4 matView,
    float ratio,
    glm::vec3 eyeLocation);

// This is the function that Lua will call when 
//void g_Lua_AddSerialCommand(std::string theCommandText)
int g_Lua_AddSerialCommand(lua_State* L)
{
//    std::cout << "**************************" << std::endl;
//    std::cout << "g_Lua_AddSerialCommand() called" << std::endl;
//    std::cout << "**************************" << std::endl;
    // AddSerialCommand() has been called
    // eg: AddSerialCommand('New_Viper_Player', -50.0, 15.0, 30.0, 5.0)

    std::string objectFriendlyName = lua_tostring(L, 1);      // 'New_Viper_Player'
    float x = (float)lua_tonumber(L, 2);                   // -50.0
    float y = (float)lua_tonumber(L, 3);                   // 15.0
    float z = (float)lua_tonumber(L, 4);                   // 30.0
    float timeSeconds = (float)lua_tonumber(L, 5);                   // 5.0

    std::vector<std::string> vecCommandDetails;
    vecCommandDetails.push_back(objectFriendlyName);    // Object command controls
    vecCommandDetails.push_back(::g_floatToString(x));
    vecCommandDetails.push_back(::g_floatToString(y));
    vecCommandDetails.push_back(::g_floatToString(z));
    vecCommandDetails.push_back(::g_floatToString(timeSeconds));

    iCommand* pMoveViper = ::g_pCommandFactory->pCreateCommandObject(
        "Move Relative ConstVelocity+Time", vecCommandDetails);

    ::g_pCommandDirector->addSerial(pMoveViper);

    // We'll return some value to indicate if the command worked or not
    // Here, we'll push "true" if it worked
    lua_pushboolean(L, true);
    // return 1 because we pushed 1 thing onto the stack
    return 1;
}




static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

bool isControlDown(GLFWwindow* window);
//{
//    if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) ||
//        (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS))
//    {
//        return true;
//    }
//    return false;
//}

// START OF: TANK GAME
//#include "iTank.h"
//#include "cTank.h"
//#include "cSuperTank.h"
//#include "cTankFactory.h"
#include "cTankBuilder.h"
#include "cArena.h"
void SetUpTankGame(void);
void TankStepFrame(double timeStep);
std::vector< iTank* > g_vecTheTanks;
cArena* g_pTankArena = NULL;
sMesh* g_pTankModel = NULL;

// END OF: TANK GAME





void ConsoleStuff(void);

// https://stackoverflow.com/questions/5289613/generate-random-float-between-two-floats
float g_getRandomFloat(float a, float b) {
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

glm::vec3 g_getRandom_vec3(glm::vec3 min, glm::vec3 max)
{
    return glm::vec3(
        ::g_getRandomFloat(min.x, max.x),
        ::g_getRandomFloat(min.y, max.y),
        ::g_getRandomFloat(min.z, max.z));
}

std::string g_getStringVec3(glm::vec3 theVec3)
{
    std::stringstream ssVec;
    ssVec << "(" << theVec3.x << ", " << theVec3.y << ", " << theVec3.z << ")";
    return ssVec.str();
}

// Returns NULL if NOT found
sMesh* g_pFindMeshByFriendlyName(std::string theNameToFind)
{
    for (unsigned int index = 0; index != ::g_vecMeshesToDraw.size(); index++)
    {
        if (::g_vecMeshesToDraw[index]->uniqueFriendlyName == theNameToFind)
        {
            return ::g_vecMeshesToDraw[index];
        }
    }
    // Didn't find it
    return NULL;
}

void AABBOctTree(void);

bool testDepthTexture(int width, int height, std::string& error)
{
    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
    glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_DEPTH_COMPONENT24, width, height);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        error = "Test failed: OpenGL error " + std::to_string(err);
        glDeleteTextures(1, &tex);
        return false;
    }
    glDeleteTextures(1, &tex);
    return true;
}

int main(void)
{
    
    AABBOctTree();




    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL Triangle", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Callback for keyboard, but for "typing"
    // Like it captures the press and release and repeat
    glfwSetKeyCallback(window, key_callback);

    // 
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetWindowFocusCallback(window, cursor_enter_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // Resets the FBO when we change window size
    // https://www.glfw.org/docs/3.3/window_guide.html#window_events
    glfwSetWindowSizeCallback(window, window_size_callback);



    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);


// **********************************************************
//    ____  _               _           
//   / ___|| |__   __ _  __| | ___ _ __ 
//   \___ \| '_ \ / _` |/ _` |/ _ \ '__|
//    ___) | | | | (_| | (_| |  __/ |   
//   |____/|_| |_|\__,_|\__,_|\___|_|   
//                                      

    cShaderManager* pShaderManager = new cShaderManager();

    cShaderManager::cShader vertexShader;
    vertexShader.fileName = "assets/shaders/vertex01.glsl";

    cShaderManager::cShader geometryShader;
//    geometryShader.fileName = "assets/shaders/geom_pass_through.glsl";
    geometryShader.fileName = "assets/shaders/geom_split_triangle.glsl";
//    geometryShader.fileName = "assets/shaders/geom_DrawNormal_lines.glsl";

    cShaderManager::cShader fragmentShader;
    fragmentShader.fileName = "assets/shaders/fragment01.glsl";

    //if ( ! pShaderManager->createProgramFromFile("shader01",
    //                                             vertexShader, fragmentShader))
    if ( ! pShaderManager->createProgramFromFile("shader01",
                                                 vertexShader, geometryShader, fragmentShader))
    {
        std::cout << "Error: " << pShaderManager->getLastError() << std::endl;
    }
    else
    {
        std::cout << "Shader built OK" << std::endl;
    }

    program = pShaderManager->getIDFromFriendlyName("shader01");

    glUseProgram(program);

    
    cShaderManager::cShaderProgram* pShaderProgram
        = pShaderManager->pGetShaderProgramFromFriendlyName("shader01");

    std::string strUniformSummary = pShaderProgram->getActiveUniformSummary();

    //-----------------------------------------------------------------------------ShadowShader----------------------------------------------------------------------------------------
    cShaderManager::cShader shadowVertexShader;
    shadowVertexShader.fileName = "assets/shaders/shadow_depth_vertex.glsl";

    cShaderManager::cShader shadowGeometryShader;
    shadowGeometryShader.fileName = "assets/shaders/shadow_depth_geometry.glsl";

    cShaderManager::cShader shadowFragmentShader;
    shadowFragmentShader.fileName = "assets/shaders/shadow_depth_fragment.glsl";

    if (!pShaderManager->createProgramFromFile("shadowShader",
        shadowVertexShader, shadowGeometryShader, shadowFragmentShader))
    {
        std::cout << "Error: " << pShaderManager->getLastError() << std::endl;
    }
    else
    {
        std::cout << "Shader built OK" << std::endl;
    }

    shadowShaderProgram = pShaderManager->getIDFromFriendlyName("shadowShader");
    
    glUseProgram(shadowShaderProgram);

    pShaderProgram = pShaderManager->pGetShaderProgramFromFriendlyName("shadowShader");
    //-----------------------------------------------------------------------------ShadowShader----------------------------------------------------------------------------------------

    strUniformSummary = pShaderProgram->getActiveUniformSummary();
// **********************************************************

//    std::cout << strUniformSummary << std::endl;

    ::g_pMyLuaMasterBrain = new cLuaBrain();


//    cVAOManager* pMeshManager = new cVAOManager();
    ::g_pMeshManager = new cVAOManager();


    // Traversing the path
    ::g_pTerrainPathChooser = new cTerrainPathChooser(::g_pMeshManager);
    // Set the terrain, etc. 
    // HACK:
    ::g_pTerrainPathChooser->setTerrainMesh(
        "assets/models/Simple_MeshLab_terrain_x5_xyz_N_uv.ply", 
        glm::vec3(0.0f, -175.0f, 0.0f));        // Offset of mesh
    //
    ::g_pTerrainPathChooser->startXYZ = glm::vec3(-500.0f, -75.0f, -500.0f);
    ::g_pTerrainPathChooser->destinationXYZ = glm::vec3(+500.0f, -75.0f, +500.0f);





    ::g_pPhysicEngine = new cPhysics();
    // For triangle meshes, let the physics object "know" about the VAO manager
    ::g_pPhysicEngine->setVAOManager(::g_pMeshManager);


    // Start up the PhysX middleware...
    ::g_pPhysX = new cPhysXWraper();
    ::g_pPhysX->initPhysics(true);



    ::g_pCommandDirector = new cCommandGroup();
    ::g_pCommandFactory = new cCommandFactory();
    // 
    // Tell the command factory about the phsyics and mesh stuff
    ::g_pCommandFactory->setPhysics(::g_pPhysicEngine);
    // (We are passing the address of this...)
    ::g_pCommandFactory->setVectorOfMeshes(&g_vecMeshesToDraw);

    // This also adds physics objects to the phsyics system
    AddModelsToScene(::g_pMeshManager, program, shadowShaderProgram);



    // --------------------------------------------------------------------------Animations-----------------------------------------------------------------------------------------------
    Model ourModel("res/Swat/Swat.dae");

    Animation idleAnimation("res/Swat/Breathing Idle.dae", &ourModel);
    Animation leftWalkAnimation("res/Swat/Left Strafe Walking.dae", &ourModel);
    Animation rightWalkAnimation("res/Swat/Right Strafe Walking.dae", &ourModel);
    Animation runAnimation("res/Swat/Standard Run.dae", &ourModel);
    Animation walkBackAnimation("res/Swat/Walking_Backwards.dae", &ourModel);
    Animation walkAnimation("res/Swat/Walking.dae", &ourModel);
    Animation jumpAnimation("res/Swat/Jumping.dae", &ourModel);

    jumpAnimation.SetLooping(&jumpAnimation, false);

    Animator animator(&runAnimation);

    g_pAnimator = &animator;

    cCharacter character(ourModel);

    g_pCharacter = &character;

    character.AddAnimation(&idleAnimation);			// 0
    character.AddAnimation(&leftWalkAnimation);		// 1
    character.AddAnimation(&rightWalkAnimation);	// 2
    character.AddAnimation(&runAnimation);			// 3
    character.AddAnimation(&walkBackAnimation);		// 4
    character.AddAnimation(&walkAnimation);			// 5
    character.AddAnimation(&jumpAnimation);			// 6

    g_pCharacter->currentAnimation = animator.GetCurrentAnimation();
    // --------------------------------------------------------------------------Animations-----------------------------------------------------------------------------------------------

    
     
    ::g_pFlyCamera = new cBasicFlyCamera();
    ::g_pFlyCamera->setEyeLocation(glm::vec3(0.0f, -25.0f, -75.0f));

// To see the terrain from high above
//    ::g_pFlyCamera->setEyeLocation(glm::vec3(72.2f, 1270.0f, -1123.0f));
//    ::g_pFlyCamera->pitchUpDown(-45.0f);

    // To see the Galactica:
//    ::g_pFlyCamera->setEyeLocation(glm::vec3(10'000.0f, 25'000.0f, 160'000.0f));
    // Rotate the camera 180 degrees
//    ::g_pFlyCamera->rotateLeftRight_Yaw_NoScaling(glm::radians(180.0f));



    glUseProgram(program);

    // Enable depth buffering (z buffering)
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glEnable.xhtml
    glEnable(GL_DEPTH_TEST);

    cLowPassFilter frameTimeFilter;
//    frameTimeFilter.setNumSamples(30000);

    double currentFrameTime = glfwGetTime();
    double lastFrameTime = glfwGetTime();





    // Set up the lights
    ::g_pLightManager = new cLightManager();
    // Called only once
    ::g_pLightManager->loadUniformLocations(program);

    // Set up one of the lights in the scene
    //::g_pLightManager->theLights[0].position = glm::vec4(25'000.0f, 50'000.0f, -100'000.0f, 1.0f);
    //::g_pLightManager->theLights[0].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    //::g_pLightManager->theLights[0].atten.y = 0.000006877f;
    //::g_pLightManager->theLights[0].atten.z = 0.0000000001184f;

    //::g_pLightManager->theLights[0].param1.x = 0.0f;    // Point light (see shader)
    //::g_pLightManager->theLights[0].param2.x = 1.0f;    // Turn on (see shader)


    // Set up one of the lights in the scene
    ::g_pLightManager->theLights[1].position = glm::vec4(50.0f, 20.0f, 0.0f, 1.0f);
    ::g_pLightManager->theLights[1].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    ::g_pLightManager->theLights[1].atten.y = 0.01f;
    ::g_pLightManager->theLights[1].atten.z = 0.001f;

    ::g_pLightManager->theLights[1].param1.x = 0.0f;    // Spot light (see shader)
    //::g_pLightManager->theLights[1].direction = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    //::g_pLightManager->theLights[1].param1.y = 5.0f;   //  y = inner angle
    //::g_pLightManager->theLights[1].param1.z = 10.0f;  //  z = outer angle

    ::g_pLightManager->theLights[1].param2.x = 1.0f;    // Turn on (see shader)

    int screen_width, screen_height;
    glfwGetFramebufferSize(window, &screen_width, &screen_height);
    std::string FBOinitError;

    g_pShadowCubeMapFBO = new cDepthCubeMap();
    if (!::g_pShadowCubeMapFBO->init(1024, 1024, FBOinitError))
    {
        std::cout << "ERROR: Can't init depth FBO buffer because: "
            << FBOinitError << std::endl;
    }
    else
    {
        std::cout << "Depth FBO init() OK." << std::endl;
    }
    int major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    std::cout << "OpenGL Version: " << major << "." << minor << std::endl;

    //if (!testDepthTexture(640, 480, FBOinitError)) {
    //    std::cout << "Failed: " << FBOinitError << std::endl;
    //}
    //else {
    //    std::cout << "Success!" << std::endl;
    //}
    //if (!::g_pShadowCubeMapFBO->initTest(screen_width, screen_height, FBOinitError))
    //{
    //    std::cout << "ERROR: Can't initTest depth FBO buffer because: "
    //        << FBOinitError << std::endl;
    //}
    //else
    //{
    //    std::cout << "Depth FBO init() OK." << std::endl;
    //}

    //g_pShadowCubeMapFBO->setupShadowMatrices(shadowShaderProgram, glm::vec3(::g_pLightManager->theLights[0].position.x, ::g_pLightManager->theLights[0].position.y, ::g_pLightManager->theLights[0].position.z), 1.0f, 25.0f);
    g_pShadowCubeMapFBO->setupShadowMatrices(shadowShaderProgram, glm::vec3(::g_pLightManager->theLights[1].position.x, ::g_pLightManager->theLights[1].position.y, ::g_pLightManager->theLights[1].position.z), 1.0f, 50.0f);




    ::g_pTextures = new cBasicTextureManager();

    ::g_pTextures->SetBasePath("assets/textures");

    std::cout << "Loading textures...";

    ::g_pTextures->Create2DTextureFromBMPFile("bad_bunny_1920x1080.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("dua-lipa-promo.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("Puzzle_parts.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("Non-uniform concrete wall 0512-3-1024x1024.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("UV_Test_750x750.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("shape-element-splattered-texture-stroke_1194-8223.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("Grey_Brick_Wall_Texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("dirty-metal-texture_1048-4784.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("bad_bunny_1920x1080_24bit_black_and_white.bmp");
    //
    ::g_pTextures->Create2DTextureFromBMPFile("SurprisedChildFace.bmp");
    // 
    ::g_pTextures->Create2DTextureFromBMPFile("Canadian_Flag_Texture.bmp");
    ::g_pTextures->Create2DTextureFromBMPFile("Chinese_Flag_Texture.bmp");
    //
    ::g_pTextures->Create2DTextureFromBMPFile("solid_black.bmp");
    //
    ::g_pTextures->Create2DTextureFromBMPFile("SpidermanUV_square.bmp");

    // Load the space skybox
    std::string errorString;
    ::g_pTextures->SetBasePath("assets/textures/CubeMaps");
    if (::g_pTextures->CreateCubeTextureFromBMPFiles("Space",
        "SpaceBox_right1_posX.bmp", 
        "SpaceBox_left2_negX.bmp",
        "SpaceBox_top3_posY.bmp", 
        "SpaceBox_bottom4_negY.bmp",
        "SpaceBox_front5_posZ.bmp", 
        "SpaceBox_back6_negZ.bmp", true, errorString))
    {
        std::cout << "Loaded space skybox" << std::endl;
    }
    else
    {
        std::cout << "ERROR: Didn't load space skybox because: " << errorString << std::endl;
    }
        
    //std::cout << "glTexStorage2D():" << glTexStorage2D << std::endl;
    //std::cout << "glCompileShader():" << glCompileShader << std::endl;

    //FARPROC glTexStorage2DProc = GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "glTexStorage2D");
    //std::cout << "glCompileShader():" << glTexStorage2DProc << std::endl;
    //FARPROC glCompileShaderProc = GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "glCompileShader");
    //std::cout << "glCompileShader():" << glCompileShaderProc << std::endl;

    // Load the sunny day cube map
    if (::g_pTextures->CreateCubeTextureFromBMPFiles("SunnyDay",
        "TropicalSunnyDayLeft2048.bmp",
        "TropicalSunnyDayRight2048.bmp",
        "TropicalSunnyDayUp2048.bmp",
        "TropicalSunnyDayDown2048.bmp",
        "TropicalSunnyDayFront2048.bmp",
        "TropicalSunnyDayBack2048.bmp",
        true, errorString))
    {
        std::cout << "Loaded space SunnyDay" << std::endl;
    }
    else
    {
        std::cout << "ERROR: Didn't load space SunnyDay because: " << errorString << std::endl;
    }
        
        
    std::cout << "done." << std::endl;
        
        


    //glGet with argument GL_ACTIVE_TEXTURE, or GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS.
    // 
    // void glGetIntegerv(GLenum pname, GLint* data);
    // 
    //GLint iActiveTextureUnits = 0;
    //glGetIntegerv(GL_ACTIVE_TEXTURE, &iActiveTextureUnits);
    //std::cout << "GL_ACTIVE_TEXTURE = " << (iActiveTextureUnits - GL_TEXTURE0) << std::endl;

    GLint iMaxCombinedTextureInmageUnits = 0;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &iMaxCombinedTextureInmageUnits);
    std::cout << "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS = " << iMaxCombinedTextureInmageUnits << std::endl;

    // data returns one value, the maximum number of components of the inputs read by the fragment shader, 
    // which must be at least 128.
    GLint iMaxFragmentInputComponents = 0;
    glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, &iMaxFragmentInputComponents);
    std::cout << "GL_MAX_FRAGMENT_INPUT_COMPONENTS = " << iMaxFragmentInputComponents << std::endl;
    

    // data returns one value, the maximum number of individual floating - point, integer, or boolean values 
    // that can be held in uniform variable storage for a fragment shader.The value must be at least 1024. 
    GLint iMaxFragmentUniformComponents = 0;
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &iMaxFragmentUniformComponents);
    std::cout << "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS = " << iMaxFragmentUniformComponents << std::endl;
        

    //  Turn on the blend operation
    glEnable(GL_BLEND);
    // Do alpha channel transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Is the default (cull back facing polygons)
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    // HACK:
    unsigned int numberOfNarrowPhaseTrianglesInAABB_BroadPhaseThing = 0;


//    // Camera view from withing the warehouse
//    cFBO_RGB_depth FBO_WarehouseView;
//    std::string FBOError;
//    if (!FBO_WarehouseView.init(1920, 1080, FBOError))
////    if (!FBO_WarehouseView.init(128, 64, FBOError))
////    if (!FBO_WarehouseView.init(3840 * 2, 2180 * 2, FBOError))
//    {
//        std::cout << "Error: FBO.init(): " << FBOError << std::endl;
//    }
//    else
//    {
//        std::cout << "FBO created OK" << std::endl;
//    }

    // Deferred rendering Geometry "G" buffer
    //int screen_width, screen_height;
    glfwGetFramebufferSize(window, &screen_width, &screen_height);

    ::g_pFBO_G_Buffer = new cFBO_deferred();
    //std::string FBOinitError;
    if ( ! ::g_pFBO_G_Buffer->init(screen_width, screen_height, FBOinitError) )
    {
        std::cout << "ERROR: Can't init deferred FBO buffer because: "
            << FBOinitError << std::endl;
    }
    else
    {
        std::cout << "Deferred FBO init() OK." << std::endl;
    }
 

//    ::g_pParticles = new cParticleEmitter();
    ::g_pParticles = new cParticleEmitter_2();
    ::g_pParticles->SetMaximumNumberOfParticles(15'000);

    ::g_pParticles->SetSourceLocation(glm::vec3(25.0f, -20.0f, 0.0f));
    ::g_pParticles->SetInitalVelocity(
        glm::vec3(-1.0f, 3.0f, -1.0f),        // Min
        glm::vec3( 1.0f, 10.0f, 1.0f));       // Max

    
    //glUseProgram(shadowShaderProgram);
    //g_pShadowCubeMapFBO = new cDepthCubeMap();
    //if (!::g_pShadowCubeMapFBO->init(screen_width, screen_height, FBOinitError))
    //{
    //    std::cout << "ERROR: Can't init depth FBO buffer because: "
    //        << FBOinitError << std::endl;
    //}
    //else
    //{
    //    std::cout << "Depth FBO init() OK." << std::endl;
    //}



    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        ratio = width / (float)height;

        // Calculate elapsed time
        // We'll enhance this
        currentFrameTime = glfwGetTime();
        double tempDeltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        // Set a limit on the maximum frame time
        const double MAX_FRAME_TIME = 1.0 / 60.0;   // 60Hz (16 ms)
        if (tempDeltaTime > MAX_FRAME_TIME)
        {
            tempDeltaTime = MAX_FRAME_TIME;
        }

        // Add this sample to the low pass filer ("averager")
        frameTimeFilter.addSample(tempDeltaTime);
        // 
        double deltaTime = frameTimeFilter.getAverage();


        // Physic update and test 
        // (The phsyics "engine" is NOT updating the Verlet (soft bodies) 
        //  in this method...)
        ::g_pPhysicEngine->StepTick(deltaTime);

        
        
// THREADED NOW
//         
//  Update this from THE LAST FRAME
// 
        // Update the meshes in the VAO to match any soft bodies
        // MUST keep this on the OpenGL thread that "has context"
        // (i.e. the one that we created the GL window on)
        // (i.e. we are NOT going to move this to another thread)
        ::g_pPhysicEngine->updateSoftBodyMeshes(program);


        // Update the mesh information from the LAST frame:
        // (i.e. from the thread we kicked off last frame)

        // Move the flag to where the viper is
        if (::g_pViperFlagConnector)
        {
            ::g_pViperFlagConnector->UpdateFlagLocation();
        }


        // This might be better inside the StepTick(),
        //  but I'm leaving it here for clarification
        //  or if you DON'T want any soft bodies
        ::g_pPhysicEngine->updateSoftBodies(deltaTime);


        // Update PhysX...
        ::g_pPhysX->update();

        //
        ::g_pParticles->Update(deltaTime);

        // Update the commands, too
        ::g_pCommandDirector->Update(deltaTime);


        // Handle any collisions
        if (::g_pPhysicEngine->vec_SphereAABB_Collisions.size() > 0)
        {
            // Yes, there were collisions

            for (unsigned int index = 0; index != ::g_pPhysicEngine->vec_SphereAABB_Collisions.size(); index++)
            {
                cPhysics::sCollision_SphereAABB thisCollisionEvent = ::g_pPhysicEngine->vec_SphereAABB_Collisions[index];

                if (thisCollisionEvent.pTheSphere->pPhysicInfo->velocity.y < 0.0f)
                {
                    // Yes, it's heading down
                    // So reverse the direction of velocity
                    thisCollisionEvent.pTheSphere->pPhysicInfo->velocity.y = fabs(thisCollisionEvent.pTheSphere->pPhysicInfo->velocity.y);
                }

            }//for (unsigned int index

        }//if (::g_pPhysicEngine->vec_SphereAABB_Collisions


        glm::mat4 matProjection = glm::mat4(1.0f);
        glm::mat4 matView = glm::mat4(1.0f);
        glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);



        //glUseProgram(shadowShaderProgram);
        //glBindFramebuffer(GL_FRAMEBUFFER, g_pShadowCubeMapFBO->depthMapFBO_ID);
        //glClear(GL_DEPTH_BUFFER_BIT);
        //glViewport(0, 0, g_pShadowCubeMapFBO->width, g_pShadowCubeMapFBO->height);
        //g_pShadowCubeMapFBO->setupShadowMatrices(shadowShaderProgram, glm::vec3(::g_pLightManager->theLights[0].position.x, ::g_pLightManager->theLights[0].position.y, ::g_pLightManager->theLights[0].position.z), 1.0f, 25.0f);
        //g_pShadowCubeMapFBO->setupShadowMatrices(shadowShaderProgram, glm::vec3(::g_pLightManager->theLights[1].position.x, ::g_pLightManager->theLights[1].position.y, ::g_pLightManager->theLights[1].position.z), 1.0f, 75.0f);
        //{
        //    glActiveTexture(GL_TEXTURE0 + 13);
        //    glBindTexture(GL_TEXTURE_2D, g_pFBO_G_Buffer->vertexWorldLocationXYZ);
        //    GLint vertexPos_texture_UL
        //        = glGetUniformLocation(shadowShaderProgram, "originalPositions");
        //    glUniform1i(vertexPos_texture_UL, 13);       // <-- Note we use the NUMBER, not the GL_TEXTURE3 here
        //}
        //{
        //    glActiveTexture(GL_TEXTURE0 + 17);
        //    glBindTexture(GL_TEXTURE_2D, g_pFBO_G_Buffer->vertexUV);
        //    GLint vertexUV_texture_UL
        //        = glGetUniformLocation(shadowShaderProgram, "uvTexture");
        //    glUniform1i(vertexUV_texture_UL, 17);       // <-- Note we use the NUMBER, not the GL_TEXTURE3 here
        //}

        g_pShadowCubeMapFBO->renderDepthFBO(shadowShaderProgram, glm::vec3(::g_pLightManager->theLights[1].position.x, ::g_pLightManager->theLights[1].position.y, ::g_pLightManager->theLights[1].position.z), 50.0f);
       // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // **************************************************
        // 
        // RENDER from the inside of the warehouse


//        // Point output to the off-screen FBO
//        glBindFramebuffer(GL_FRAMEBUFFER, FBO_WarehouseView.ID);
//
//        // These will ONLY work on the default framebuffer
////        glViewport(0, 0, width, height);
////       glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
////        FBO_WarehouseView.clearColourBuffer(0);
//        FBO_WarehouseView.clearBuffers(true, true);
//
//
//        matProjection = glm::perspective(0.6f,
//            ratio,
//            10.0f,
//            100'000.0f);
//
//        glm::vec3 eyeInsideWarehouse = glm::vec3(-197.0f, 14.0f, -72.0f);
//        float xOffset = 10.0f * glm::sin((float)glfwGetTime() / 2.0f);
//        glm::vec3 atInsideWareHouse =
//            eyeInsideWarehouse + glm::vec3(xOffset, 0.0f, 10.0f);
//        
//        matView = glm::lookAt(eyeInsideWarehouse, atInsideWareHouse, glm::vec3(0.0f, 1.0f, 0.0f));
//
//        RenderScene(program, matProjection, matView, ratio, eyeInsideWarehouse);
//        // 
//        // **************************************************

        // Pass #0 --> Render the G buffer
        //          output to the FBO deferred object
        // 
        // (Pass #1 --> optional effect pass, "2nd pass effect")
        //
        // Pass #2 --> Render the Deferred lighting pass 
        //          output to the screen onto a full screen quad
        //
        glUseProgram(program);

        // uniform int renderPassNumber;
        GLint renderPassNumber_UL = glGetUniformLocation(program, "renderPassNumber");
        // Pass "0" (regular forward rendering)
        // 


// **************************************************
//     ____           ____         __  __                                 
//    / ___|         | __ ) _   _ / _|/ _| ___ _ __   _ __   __ _ ___ ___ 
//   | |  _   _____  |  _ \| | | | |_| |_ / _ \ '__| | '_ \ / _` / __/ __|
//   | |_| | |_____| | |_) | |_| |  _|  _|  __/ |    | |_) | (_| \__ \__ \
//    \____|         |____/ \__,_|_| |_|  \___|_|    | .__/ \__,_|___/___/
//                                                   |_|                  
// 
        // Pass "1" for deferred G buffer pass
        glUniform1i(renderPassNumber_UL, 1);

        // Point the output to the G buffer...
        glBindFramebuffer(GL_FRAMEBUFFER, ::g_pFBO_G_Buffer->ID);

        

        // Clear the buffers on the FBO
        // (remember that glClear() only works on the regular screen buffer)
        ::g_pFBO_G_Buffer->clearBuffers(true, true);


        matView = glm::lookAt(
            ::g_pFlyCamera->getEyeLocation(),
            ::g_pFlyCamera->getTargetLocation(),
            upVector);

        GLint eyeLocation_UL = glGetUniformLocation(program, "eyeLocation");
        glUniform4f(eyeLocation_UL, 
            ::g_pFlyCamera->getEyeLocation().x,
            ::g_pFlyCamera->getEyeLocation().y, 
            ::g_pFlyCamera->getEyeLocation().z, 1.0f);

        matProjection = glm::perspective(0.6f,
            ratio,
            1.0f,
            50'000.0f);



//        // Render the offscreen FBO texture onto where Dua Lipa was...
//        sMesh* pFBOTextureMesh = ::g_pFindMeshByFriendlyName("WareHouseView");
//        // 
//        // Now apply that off-screen texture (from the FBO) to the canadian flag model
////        sMesh* pFBOTextureMesh = ::g_pFindMeshByFriendlyName("Canadian_Flag");
//
//        if (pFBOTextureMesh)
//        {
//            pFBOTextureMesh->bIsVisible = true;
//
//            GLint matProjection_UL = glGetUniformLocation(program, "matProjection");
//            glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, (const GLfloat*)&matProjection);
//
//            GLint matView_UL = glGetUniformLocation(program, "matView");
//            glUniformMatrix4fv(matView_UL, 1, GL_FALSE, (const GLfloat*)&matView);
//
//
//
//            // Connect texture unit #0 to the offscreen FBO
//            glActiveTexture(GL_TEXTURE0);
//
//            // The colour texture inside the FBO is just a regular colour texture.
//            // There's nothing special about it.
//            glBindTexture(GL_TEXTURE_2D, FBO_WarehouseView.colourTexture_0_ID);
////            glBindTexture(GL_TEXTURE_2D, ::g_pTextures->getTextureIDFromName("dua-lipa-promo.bmp"));
//
//            GLint texture01_UL = glGetUniformLocation(program, "texture00");
//            glUniform1i(texture01_UL, 0);       // <-- Note we use the NUMBER, not the GL_TEXTURE3 here
//
//            GLint texRatio_0_to_3_UL = glGetUniformLocation(program, "texRatio_0_to_3");
//            glUniform4f(texRatio_0_to_3_UL,
//                1.0f,
//                0.0f,
//                0.0f,
//                0.0f);
//
//            // This is for the blurring effect
//            GLint b_Is_FBO_Texture_UL = glGetUniformLocation(program, "b_Is_FBO_Texture");
//            GLint bUseTextureAsColour_UL = glGetUniformLocation(program, "bUseTextureAsColour");
//
//            glUniform1f(b_Is_FBO_Texture_UL, (float)GL_TRUE);
//            glUniform1f(bUseTextureAsColour_UL, (float)GL_FALSE);
//
//            DrawMesh(pFBOTextureMesh, program, false);
//
//            glUniform1f(b_Is_FBO_Texture_UL, (float)GL_FALSE);
//            glUniform1f(bUseTextureAsColour_UL, (float)GL_TRUE);
//
//            pFBOTextureMesh->bIsVisible = false;
//        }

        glUniform4f(eyeLocation_UL,
            ::g_pFlyCamera->getEyeLocation().x,
            ::g_pFlyCamera->getEyeLocation().y,
            ::g_pFlyCamera->getEyeLocation().z, 1.0f);

        
        //glUseProgram(shadowShaderProgram);
        //for (cLightManager::sLight curLight : g_pLightManager->theLights) {
        //    g_pShadowCubeMapFBO->renderDepthFromLightPers(shadowShaderProgram, curLight.position);
        //}
        //glUseProgram(program);

        //glActiveTexture(GL_TEXTURE0 + 21);
        //glBindTexture(GL_TEXTURE_CUBE_MAP, g_pShadowCubeMapFBO->depthCubemapTex_ID);

        //GLint shadowMap_UL = glGetUniformLocation(program, "shadowMap");
        //glUniform1i(shadowMap_UL, 21);

        //for (cLightManager::sLight curLight : g_pLightManager->theLights) {
        //    glUniform3f(glGetUniformLocation(shadowShaderProgram, "lightPos"), curLight.position.x, curLight.position.y, curLight.position.z);
        //    glUniform1f(glGetUniformLocation(shadowShaderProgram, "far_plane"), 1000.0f);
        //}
        

        ////--------------------------------------------------------------------------------Animations----------------------------------------------------------------------
        ourModel.positionXYZ.x += 5.5f * deltaTime;
        ourModel.uniformScale = 5.0f;
        ourModel.RenderAnimatedStuff(program, matProjection, matView, deltaTime);

        ////--------------------------------------------------------------------------------Animations----------------------------------------------------------------------

        //glActiveTexture(GL_TEXTURE0 + 51);
        //glBindTexture(GL_TEXTURE_CUBE_MAP, g_pShadowCubeMapFBO->depthCubemapTex_ID);
        //glUniform1i(glGetUniformLocation(program, "depthMap"), 51);

        //glUniform3fv(glGetUniformLocation(program, "lightPos"), 1,
        //    &glm::vec3(::g_pLightManager->theLights[1].position.x,
        //        ::g_pLightManager->theLights[1].position.y,
        //        ::g_pLightManager->theLights[1].position.z)[0]);
        //glUniform1f(glGetUniformLocation(program, "far_plane"), 75.0f);

        RenderScene(program, matProjection, matView, ratio, ::g_pFlyCamera->getEyeLocation());

        //RenderSceneDepth(shadowShaderProgram);
 

        

// **************************************************


// **************************************************
//    ____        __                        _   _ _       _     _   _                                   
//   |  _ \  ___ / _| ___ _ __ _ __ ___  __| | | (_) __ _| |__ | |_(_)_ __   __ _   _ __   __ _ ___ ___ 
//   | | | |/ _ \ |_ / _ \ '__| '__/ _ \/ _` | | | |/ _` | '_ \| __| | '_ \ / _` | | '_ \ / _` / __/ __|
//   | |_| |  __/  _|  __/ |  | | |  __/ (_| | | | | (_| | | | | |_| | | | | (_| | | |_) | (_| \__ \__ \
//   |____/ \___|_|  \___|_|  |_|  \___|\__,_| |_|_|\__, |_| |_|\__|_|_| |_|\__, | | .__/ \__,_|___/___/
//                                                  |___/                   |___/  |_|                  

// Point the output to the regular framebuffer (the screen)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glUseProgram(program);


        // These will ONLY work on the default framebuffer
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        // Pass "3" for deferred lighting pass
        glUniform1i(renderPassNumber_UL, 3);


        // We can render any object
//        sMesh* pFSQ = g_pFindMeshByFriendlyName("New_Viper_Player");
        sMesh* pFSQ = g_pFindMeshByFriendlyName("Full_Screen_Quad");


        // We are setting the camera (view) and projection matrix 
        //  specifically for this shot of the FSQ

        // In our case, the quad is 2x2 in size, centred at the origin
        //  facing along the +ve z axis.
        // It goes from -1.0 to 1.0 on the x and y axes

        pFSQ->positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
        pFSQ->bIsVisible = true;
       // pFSQ->rotationEulerXYZ.y += 0.1f;
        // 
        // 
        // Set the camera 
        //
        // ...We could make the quad bigger or move closer
        //
        // The key is we want to the full screen quad to be "too" big,
        //  like it's completely filling the creen and going off the edges
        // (that way whatever resolution or window size, we'll be OK)
        //
        matView = glm::lookAt(
            glm::vec3(0.0f, 0.0f, +1.0f),  // +1 units along the z
            glm::vec3(0.0f, 0.0f, 0.0f),    // Looking at the origin 
            glm::vec3(0.0f, 1.0f, 0.0f));   // "up" is +ve Y


        GLint matProjection_UL = glGetUniformLocation(program, "matProjection");
        GLint matView_UL = glGetUniformLocation(program, "matView");

        glUniformMatrix4fv(matView_UL, 1, GL_FALSE, (const GLfloat*)&matView);

        // glm::ortho(
        
        // Watch the near and far plane as we are REALLY close to the quad...
        matProjection = glm::perspective(0.6f,
            ratio,
            0.1f, 2.0f);       // FSQ is 10 units from the camera 
                                // (and it's a flat object)

        glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, (const GLfloat*)&matProjection);



        // Connect the FBO deferred textures to the textures in our final salighting pass
        //    uniform sampler2D vertexWorldLocationXYZ_texture;
        //    uniform sampler2D vertexNormalXYZ_texture;
        //    uniform sampler2D vertexDiffuseRGB_texture;
        //    uniform sampler2D vertexSpecularRGA_P_texture;
        //
        // Note here I'm picking texture unit numbers for not particular 
        //  reason. i.e. they don't have to be zero, or match the FBO, or 
        //  even in any order. They are independent of all that
        {
            glActiveTexture(GL_TEXTURE0 + 13);
            glBindTexture(GL_TEXTURE_2D, g_pFBO_G_Buffer->vertexWorldLocationXYZ);
            GLint vertexWorldLocationXYZ_texture_UL 
                = glGetUniformLocation(program, "vertexWorldLocationXYZ_texture");
            glUniform1i(vertexWorldLocationXYZ_texture_UL, 13);       // <-- Note we use the NUMBER, not the GL_TEXTURE3 here
        }
        {
            glActiveTexture(GL_TEXTURE0 + 14);
            glBindTexture(GL_TEXTURE_2D, g_pFBO_G_Buffer->vertexNormalXYZ);
            GLint vertexNormalXYZ_texture_UL
                = glGetUniformLocation(program, "vertexNormalXYZ_texture");
            glUniform1i(vertexNormalXYZ_texture_UL, 14);       // <-- Note we use the NUMBER, not the GL_TEXTURE3 here
        }
        {
            glActiveTexture(GL_TEXTURE0 + 15);
            glBindTexture(GL_TEXTURE_2D, g_pFBO_G_Buffer->vertexDiffuseRGB);
            GLint vertexDiffuseRGB_texture_UL
                = glGetUniformLocation(program, "vertexDiffuseRGB_texture");
            glUniform1i(vertexDiffuseRGB_texture_UL, 15);       // <-- Note we use the NUMBER, not the GL_TEXTURE3 here
        }
        {
            glActiveTexture(GL_TEXTURE0 + 16);
            glBindTexture(GL_TEXTURE_2D, g_pFBO_G_Buffer->vertexSpecularRGA_P);
            GLint vertexSpecularRGA_P_texture_UL
                = glGetUniformLocation(program, "vertexSpecularRGA_P_texture");
            glUniform1i(vertexSpecularRGA_P_texture_UL, 16);       // <-- Note we use the NUMBER, not the GL_TEXTURE3 here
        }
        {
            glActiveTexture(GL_TEXTURE0 + 17);
            glBindTexture(GL_TEXTURE_2D, g_pFBO_G_Buffer->vertexUV);
            GLint vertexUV_texture_UL
                = glGetUniformLocation(program, "vertexUV_texture");
            glUniform1i(vertexUV_texture_UL, 17);       // <-- Note we use the NUMBER, not the GL_TEXTURE3 here
        }
        // Also pass the current screen size
        // uniform vec2 screenSize_width_height
        int screen_width, height_height;
        glfwGetFramebufferSize(window, &screen_width, &height_height);

        GLint screenSize_width_height_UL
            = glGetUniformLocation(program, "screenSize_width_height");

        glUniform2f(screenSize_width_height_UL, 
                     (GLfloat)screen_width, 
                     (GLfloat)height_height);


        glActiveTexture(GL_TEXTURE0 + 51);
        glBindTexture(GL_TEXTURE_CUBE_MAP, g_pShadowCubeMapFBO->depthCubemapTex_ID);
        glUniform1i(glGetUniformLocation(program, "depthMap"), 51);

        glUniform3fv(glGetUniformLocation(program, "lightPos"), 1,
            &glm::vec3(::g_pLightManager->theLights[1].position.x,
                ::g_pLightManager->theLights[1].position.y,
                ::g_pLightManager->theLights[1].position.z)[0]);
        glUniform1f(glGetUniformLocation(program, "far_plane"), 50.0f);


        glm::mat4 matModel = glm::mat4(1.0f);   // Identity
        DrawMesh(pFSQ, matModel, program, false);

        // Hide the quad from rendering anywhere else
        pFSQ->bIsVisible = false;

        
// **************************************************

        
        // Load any outstanding models async...
        //::g_pMeshManager->LoadAsynModels(program, shadowShaderProgram);



        // Handle async IO stuff
        handleKeyboardAsync(window);
        handleMouseAsync(window);



        glfwSwapBuffers(window);
        glfwPollEvents();


        //std::cout << "Camera: "
        std::stringstream ssTitle;
        ssTitle << "Camera: "
            << ::g_pFlyCamera->getEyeLocation().x << ", "
            << ::g_pFlyCamera->getEyeLocation().y << ", "
            << ::g_pFlyCamera->getEyeLocation().z 
            << "   ";
        ssTitle << "light[" << g_selectedLightIndex << "] "
            << ::g_pLightManager->theLights[g_selectedLightIndex].position.x << ", "
            << ::g_pLightManager->theLights[g_selectedLightIndex].position.y << ", "
            << ::g_pLightManager->theLights[g_selectedLightIndex].position.z
            << "   "
            << "linear: " << ::g_pLightManager->theLights[0].atten.y
            << "   "
            << "quad: " << ::g_pLightManager->theLights[0].atten.z
            << " particles:" << ::g_pParticles->GetNumberOfLiveParticles();

        ssTitle << " BP tris: " << numberOfNarrowPhaseTrianglesInAABB_BroadPhaseThing;

        // Add the viper info, too
        cPhysics::sPhysInfo* pViperPhys = ::g_pPhysicEngine->pFindAssociateMeshByFriendlyName("New_Viper_Player");
        if (pViperPhys)
        {
            ssTitle
                << " Viper XYZ:" << ::g_getStringVec3(pViperPhys->position)
                << " vel:" << ::g_getStringVec3(pViperPhys->velocity)
                << " acc:" << ::g_getStringVec3(pViperPhys->acceleration);

        }//if (pViperPhys)

        // Show frame time
        ssTitle << " deltaTime = " << deltaTime
            << " FPS: " << 1.0 / deltaTime;

 //       std::cout << " deltaTime = " << deltaTime << " FPS: " << 1.0 / deltaTime << std::endl;


//        glfwSetWindowTitle(window, "Hey!");
        glfwSetWindowTitle(window, ssTitle.str().c_str());


    }// End of the draw loop


    // Delete everything
    delete ::g_pFlyCamera;
    delete ::g_pPhysicEngine;
    ::g_pFBO_G_Buffer->shutdown();
    delete ::g_pFBO_G_Buffer;

    ::g_pPhysX->cleanupPhysics(true);
    delete ::g_pPhysX;

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}










// Add object to scene through Lua
// AddMeshToScene('plyname.ply', 'friendlyName', x, y, z);
int g_Lua_AddMeshToScene(lua_State* L)
{
//    std::cout << "g_Lua_AddMeshToScene" << std::endl;

    //{
    //    sModelDrawInfo galacticaModel;
    //    ::g_pMeshManager->LoadModelIntoVAO("assets/models/Battlestar_Galactica_Res_0_(444,087 faces)_xyz_n_uv (facing +z, up +y).ply",
    //        galacticaModel, program);
    //    std::cout << galacticaModel.meshName << ": " << galacticaModel.numberOfVertices << " vertices loaded" << std::endl;
    //}

    // AddMeshToScene('plyname.ply', 'friendlyName', x, y, z);

    sMesh* pNewMesh = new sMesh();
    pNewMesh->modelFileName = lua_tostring(L, 1);       // 'plyname.ply'
    pNewMesh->uniqueFriendlyName = lua_tostring(L, 2);  // Friendly name
    pNewMesh->positionXYZ.x = (float)lua_tonumber(L, 3);
    pNewMesh->positionXYZ.y = (float)lua_tonumber(L, 4);
    pNewMesh->positionXYZ.z = (float)lua_tonumber(L, 5);
    pNewMesh->textures[0] = lua_tostring(L, 6);
    pNewMesh->blendRatio[0] = (float)lua_tonumber(L, 7);
    //
    pNewMesh->bIsVisible = true;
    ::g_vecMeshesToDraw.push_back(pNewMesh);

    return 0;
}



























//using namespace std;

void ConsoleStuff(void)
{
    // "o" for output
//    std::ofstream myFile("someData.txt");
    // Write something
    //myFile << "Hello" << std::endl;
    //myFile << "there";
    //myFile.close();

    // Now read this file
//    std::ifstream myFile2("someData.txt");
//    std::string someString;
//    myFile2 >> someString;
//    std::cout << someString << std::endl;
//
    //std::string aword;
    //while (aword != "END_OF_FILE")
    //{
    //    myFile2 >> aword;
    //    std::cout << aword << std::endl;
    //};

    //std::string aword;
    //while (myFile2 >> aword)
    //{
    //    std::cout << aword << std::endl;
    //};

    std::ifstream myFile2("assets/models/bun_zipper_res3.ply");
    if (myFile2.is_open())
    {

        std::string aword;
        while (myFile2 >> aword)
        {
            std::cout << aword << std::endl;
        };
    }
    else
    {
        std::cout << "Can't find file" << std::endl;
    }


    // iostream
    std::cout << "Type a number:" << std::endl;

    int x = 0;
    std::cin >> x;

    std::cout << "You typed: " << x << std::endl;

    std::cout << "Type your name:" << std::endl;
    std::string name;
    std::cin >> name;

    std::cout << "Hello " << name << std::endl;
    return;
}


//int& getNumber(void)
//{
//    int p = 0;
//    return p;
//}

//cTankFactory* pTankFactory = NULL;
cTankBuilder* pTheTankBuilder = NULL;

// This is here for speed 
void SetUpTankGame(void)
{
 
    ::g_pTankArena = new cArena();

    if (!pTheTankBuilder)
    {
        pTheTankBuilder = new cTankBuilder();
    }



    

    std::vector<std::string> vecTankTpyes;
//    pTankFactory->GetTankTypes(vecTankTpyes);
//    cTankFactory::get_pTankFactory()->GetTankTypes(vecTankTpyes);
    pTheTankBuilder->GetTankTypes(vecTankTpyes);
    std::cout << "The tank factory can create "
        << vecTankTpyes.size() << " types of tanks:" << std::endl;
    for (std::string tankTypeString : vecTankTpyes)
    {
        std::cout << tankTypeString << std::endl;
    }
    std::cout << std::endl;

    // Create 1 super tank
//    iTank* pTheTank = cTankFactory::get_pTankFactory()->CreateATank("Super Tank");
    iTank* pTheTank = pTheTankBuilder->CreateATank("Super Tank!");
    if (pTheTank)
    {
        ::g_vecTheTanks.push_back(pTheTank);
    }

    // Create 10 tanks
    for (unsigned int count = 0; count != 50; count++)
    {
//        iTank* pTheTank = cTankFactory::get_pTankFactory()->CreateATank("Regular Tank");
        iTank* pTheTank = pTheTankBuilder->CreateATank("Regular Tank with Shield");
        if (pTheTank)
        {
            ::g_vecTheTanks.push_back(pTheTank);
        }
    }
    
    // Also a hover tank
//    iTank* pHoverTank = cTankFactory::get_pTankFactory()->CreateATank("Hover Tank");
    iTank* pHoverTank = pTheTankBuilder->CreateATank("Hover Tank");
    if (pHoverTank)
    {
        ::g_vecTheTanks.push_back(pHoverTank);
    }



    const float WORLD_SIZE(100.0f);

    for (iTank* pCurrentTank : ::g_vecTheTanks)
    {
        glm::vec3 tankLocXYZ;
        tankLocXYZ.x = ::g_getRandomFloat(-WORLD_SIZE, WORLD_SIZE);
        tankLocXYZ.y = -5.0f;
        tankLocXYZ.z = ::g_getRandomFloat(-WORLD_SIZE, WORLD_SIZE);

        pCurrentTank->setLocation(tankLocXYZ);
    }

    // Tell the tanks about the mediator
    for (iTank* pCurrentTank : ::g_vecTheTanks)
    {
        pCurrentTank->setMediator(::g_pTankArena);
    }


    for (iTank* pCurrentTank : ::g_vecTheTanks)
    {
        ::g_pTankArena->AddTank(pCurrentTank);
    }

    return;
}


void TankStepFrame(double timeStep)
{



    return;
}

// x = 5, y = 15    --> 0, 1
// x = 40.0, y = 80.0   --> [4][8]
// (40.0, 80.0) --> box size = 100
//   [0][0]   
void calcBoxXYFromCoord(float x, float y, int &xIndex, int &yIndex, float boxSize)
{
    xIndex = (int)(x / boxSize);
    yIndex = (int)(y / boxSize);
    return;
}


void AABBOctTree(void)
{
    struct sSquare
    {
        //       vector< cTriangles* > vecTriangleInThisSquare
        glm::vec2 minXY;
        glm::vec2 maxXY;
        float width;
        unsigned int indexColRow;
    };

    sSquare grid[10][10];
    float sqaureWidth = 10;

    for (unsigned int x = 0; x < 10; x++)
    {
        for (unsigned int y = 0; y < 10; y++)
        {
            grid[x][y].width = sqaureWidth;
            grid[x][y].minXY.x = sqaureWidth * x;
            grid[x][y].minXY.y = sqaureWidth * y;

            grid[x][y].maxXY.x = sqaureWidth * x + sqaureWidth;
            grid[x][y].maxXY.y = sqaureWidth * y + sqaureWidth;
        }
    }

    int xIndex, yIndex;
    calcBoxXYFromCoord(5.0f, 15.0f, xIndex, yIndex, sqaureWidth);
    std::cout << xIndex << ", " << yIndex << std::endl;


    calcBoxXYFromCoord(40.0f, 80.0f, xIndex, yIndex, sqaureWidth);
    std::cout << xIndex << ", " << yIndex << std::endl;




    return;
}