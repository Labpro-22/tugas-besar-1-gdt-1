#pragma once
#include "View3DCamera.hpp"

class CameraManager {
    private :
        map<string, View3DCamera> cameraMap;
        View3DCamera* currentCamera;
    public :
        CameraManager();
        View3DCamera* getCurrentCamera();
        View3DCamera& getCamera(const string camKey);
        void addCamera(const string camKey, const View3DCamera& camera);
        void setCurrentCamera(const string camKey);
        void updateCamMap();
        void switchTo(const string camKey, const float duration);
        void switchToNextCam(const float duration);
        Camera3D& mount();
};