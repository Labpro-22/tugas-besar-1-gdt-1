#include "../include/views/animation/camera/CameraManager.hpp"

CameraManager::CameraManager() : currentCamera(nullptr) {}

void CameraManager::addCamera(const string camKey, const View3DCamera& camera) {
    cameraMap[camKey] = camera;
    if (currentCamera == nullptr) {
        currentCamera = &cameraMap.at(camKey);
    }
}

View3DCamera* CameraManager::getCurrentCamera() { return currentCamera; }

void CameraManager::setCurrentCamera(const string camKey) {
    currentCamera = &cameraMap.at(camKey);
}

Camera3D& CameraManager::mount() {
    return currentCamera->mount();
}