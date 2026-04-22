#include "../include/views/animation/camera/CameraManager.hpp"
#include "../include/views/animation/camera/CameraMovement.hpp"
#include <set>

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
    cameraMap["SWITCH_CAM"] = View3DCamera(currentCamera->getPos(), currentCamera->getTarget(), currentCamera->getFOV());
    View3DCamera* destCam = &cameraMap.at(camKey);
    currentCamera = &cameraMap.at("SWITCH_CAM");
    cameraMap.at("SWITCH_CAM").addMovement("SWITCH", new CameraMovement(cameraMap.at("SWITCH_CAM"), 120, false, [](){}, [this, camKey](){
        View3DCamera* doneCam = this->currentCamera;
        this->currentCamera = &cameraMap.at(camKey);
        doneCam->close();
    }));
    cameraMap.at("SWITCH_CAM").getMovement("SWITCH")->setMoveToCameraAnimation(*destCam, duration);
    cameraMap.at("SWITCH_CAM").getMovement("SWITCH")->start();
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

void CameraManager::updateCamMap() {
    set<string> closedCams;
    for (auto& pair : cameraMap) {
        if (cameraMap.at(pair.first).isClosed()) {
            closedCams.insert(pair.first);
        }
    }

    for (string key : closedCams) {
        cameraMap.erase(key);
    }
}


Camera3D& CameraManager::mount() {
    return currentCamera->mount();
}