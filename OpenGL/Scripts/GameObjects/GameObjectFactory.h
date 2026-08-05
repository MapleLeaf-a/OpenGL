#pragma once

#include "GameObject.h"
#include "CameraComponent.h"
#include "MeshRenderer.h"

class GameObjectFactory
{
public:
    static GameObject* CreateCamera(const std::string& name = "Camera")
    {
        std::unique_ptr<GameObject> camera = std::make_unique<GameObject>(name);
        CameraComponent* cameraComp = camera->AddComponent<CameraComponent>();
        cameraComp->SetPerspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);
        return camera.get();
    }

    static GameObject* CreateCube(const std::string& name = "Cube", const glm::vec3& color = glm::vec3(0.5f, 0.5f, 0.5f))
    { 
        std::unique_ptr<GameObject> cube = std::make_unique<GameObject>(name);
        std::shared_ptr<Mesh> cubeMesh = Mesh::CreateCube();
        std::shared_ptr<Material> cubeMaterial = std::make_shared<Material>(color, 0.5f, 0.1f);
        cube->AddComponent<MeshRenderer>(cubeMesh, cubeMaterial);
        return cube.get();
    }

    static GameObject* CreatePlane(const std::string& name = "Plane", float size = 5.0f, const glm::vec3& color = glm::vec3(0.5f, 0.5f, 0.5f))
    {
        std::unique_ptr<GameObject> plane = std::make_unique<GameObject>(name);
        std::shared_ptr<Mesh> planeMesh = Mesh::CreatePlane(size);
        std::shared_ptr<Material> planeMaterial = std::make_shared<Material>(color, 0.5f, 0.1f);
        plane->AddComponent<MeshRenderer>(planeMesh, planeMaterial);
        return plane.get();
    }

    static GameObject* CreateSphere(const std::string& name = "Sphere", int segments = 24, const glm::vec3& color = glm::vec3(0.5f, 0.5f, 0.5f))
    {
        std::unique_ptr<GameObject> sphere = std::make_unique<GameObject>(name);
        std::shared_ptr<Mesh> sphereMesh = Mesh::CreateSphere(segments);
        std::shared_ptr<Material> sphereMaterial = std::make_shared<Material>(color, 0.5f, 0.1f);
        sphere->AddComponent<MeshRenderer>(sphereMesh, sphereMaterial);
        return sphere.get();
    }
};