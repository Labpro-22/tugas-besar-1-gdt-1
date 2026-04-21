#include "../include/views/animation/camera/CameraManager.hpp"
#include "../include/views/animation/camera/CameraMovement.hpp"

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

View3DCamera& CameraManager::getCamera(const string camKey) {
    return cameraMap.at(camKey);
}


void CameraManager::switchTo(string camKey, const float duration) {
    View3DCamera* transitionCam = new View3DCamera(*currentCamera);
    View3DCamera* destCam = &cameraMap.at(camKey);
    currentCamera = transitionCam;
    transitionCam->addMovement("SWITCH", new CameraMovement(*transitionCam, 120, false, [](){}, [this, camKey](){
        View3DCamera* doneCam = this->currentCamera;
        this->currentCamera = &cameraMap.at(camKey);
    }));
    transitionCam->getMovement("SWITCH")->setMoveToCameraAnimation(*destCam, duration);
    transitionCam->getMovement("SWITCH")->start();
}

void CameraManager::switchToNextCam(const float duration) {
    auto it = std::find_if(cameraMap.begin(), cameraMap.end(),
    [this](const auto& pair) { return &pair.second == this->currentCamera; });
    string nextCam;
    if (next(it, 1) == cameraMap.end()) {
        nextCam = cameraMap.begin()->first;
    } else {
        nextCam = next(it, 1)->first;
    }
    switchTo(nextCam, duration);
}

Camera3D& CameraManager::mount() {
    return currentCamera->mount();
}