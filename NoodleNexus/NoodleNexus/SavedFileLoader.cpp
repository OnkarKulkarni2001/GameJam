#include <fstream>
#include <sstream>
#include <iostream>

#include "sMesh.h"
#include "cVAOManager/cVAOManager.h"
#include "cLightManager.h"

extern std::vector<sMesh*> g_vecMeshesToDraw;
extern cVAOManager* g_pMeshManager;
extern cLightManager* g_pLightManager;
extern GLuint program;


void SaveDataToFile(const std::string& filename)
{
    std::ofstream file(filename, std::ios::out | std::ios::trunc);

    if (!file.is_open()) {
        std::cerr << "Failed to open file for saving.\n";
        return;
    }


    // Save Meshes
    file << "Meshes\n";
    for (sMesh* mesh : g_vecMeshesToDraw) {
        // Skip saving the mesh if its uniqueFriendlyName is "SkySphere"
        if (mesh->uniqueFriendlyName == "SkySphere") {
            continue;
        }

        file << "Mesh " << mesh->uniqueFriendlyName << "\n";
        file << mesh->modelFileName << "\n";
        file << mesh->positionXYZ.x << " " << mesh->positionXYZ.y << " " << mesh->positionXYZ.z << "\n";
        file << mesh->rotationEulerXYZ.x << " " << mesh->rotationEulerXYZ.y << " " << mesh->rotationEulerXYZ.z << "\n";
        file << mesh->textures[0] << "\n";
        file << mesh->blendRatio[0] << "\n";
        file << mesh->uniformScale << "\n";
        file << mesh->bDoNotLight << "\n";  // Boolean saved as 1 or 0
        //file << mesh->alphaTransparency << "\n";  
    }

    // Save Lights
    file << "Lights\n";
    for (int i = 0; i < cLightManager::NUMBEROFLIGHTS; ++i) {
        cLightManager::sLight& light = g_pLightManager->theLights[i];
        file << light.position.x << " " << light.position.y << " " << light.position.z << "\n";
        file << light.diffuse.x << " " << light.diffuse.y << " " << light.diffuse.z << "\n";
        file << light.atten.y << " " << light.atten.z << "\n";  // Attenuation values
        file << light.direction.x << " " << light.direction.y << " " << light.direction.z << "\n";
        file << light.param1.x << " " << light.param1.y << " " << light.param1.z << "\n"; // Light Type and Spotlight Params
        file << light.param2.x << "\n";  // Light On/Off
    }

    file.close();
    std::cout << "Data saved to " << filename << "\n";
}

void LoadDataFromFile(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open file for loading.\n";
        return;
    }

    std::string line;
    std::getline(file, line); // Read first line

    // Load Meshes
    if (line == "Meshes") {
        while (std::getline(file, line)) {
            if (line.find("Mesh") != std::string::npos) {

                //sMesh* newMesh = new sMesh();
                //std::getline(file, line); // Read mesh model filename
                std::istringstream iss(line);
                std::string discard, meshName;
                iss >> discard >> meshName; // Extract "Mesh" and the actual name

                sMesh* newMesh = new sMesh();
                newMesh->uniqueFriendlyName = meshName; // Store mesh name

                std::getline(file, line); // Read mesh model filename
                newMesh->modelFileName = line;

                std::getline(file, line); // Read position
                std::istringstream(line) >> newMesh->positionXYZ.x >> newMesh->positionXYZ.y >> newMesh->positionXYZ.z;

                std::getline(file, line); // Read rotation
                std::istringstream(line) >> newMesh->rotationEulerXYZ.x >> newMesh->rotationEulerXYZ.y >> newMesh->rotationEulerXYZ.z;

                std::getline(file, line); // Read texture file
                newMesh->textures[0] = line;

                std::getline(file, line); // Read blend ratio
                newMesh->blendRatio[0] = std::stof(line);

                std::getline(file, line); // Read scale
                newMesh->uniformScale = std::stof(line);

                std::getline(file, line); // Read "do not light" boolean
                newMesh->bDoNotLight = std::stoi(line) != 0;

                //std::getline(file, line); 
                //newMesh->alphaTransparency = std::stoi(line) != 0;

                sModelDrawInfo newMeshInfo;
                ::g_pMeshManager->LoadModelIntoVAO_Async(newMesh->modelFileName,
                    newMeshInfo, program);

                g_vecMeshesToDraw.push_back(newMesh);
            }
            if (line.find("Lights") != std::string::npos) {
                break;
            }
        }
    }

    // Load Lights
    if (line == "Lights") {
        int lightIndex = 0;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            cLightManager::sLight& light = g_pLightManager->theLights[lightIndex];
            std::istringstream(line) >> light.position.x >> light.position.y >> light.position.z;

            std::getline(file, line); // Read diffuse color
            std::istringstream(line) >> light.diffuse.x >> light.diffuse.y >> light.diffuse.z;

            std::getline(file, line); // Read attenuation values
            std::istringstream(line) >> light.atten.y >> light.atten.z;

            std::getline(file, line); // Read direction values
            std::istringstream(line) >> light.direction.x >> light.direction.y >> light.direction.z;

            std::getline(file, line); // Read light type and spotlight params
            std::istringstream(line) >> light.param1.x >> light.param1.y >> light.param1.z;

            std::getline(file, line); // Read light on/off
            std::istringstream(line) >> light.param2.x;

            ++lightIndex;
        }
    }

    file.close();
    std::cout << "Data loaded from " << filename << "\n";
}
