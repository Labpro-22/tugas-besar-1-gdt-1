#pragma once
#include "View3DCamera.hpp"

class CameraManager {
    private :
        map<string, View3DCamera> cameraMap;
        View3DCamera* currentCamera;
    public :
        CameraManager();
        View3DCamera* getCurrentCamera();
        void addCamera(const string camKey, const View3DCamera& camera);
        void setCurrentCamera(const string camKey);
        void switchTo(const string camKey);
        Camera3D& mount();
};