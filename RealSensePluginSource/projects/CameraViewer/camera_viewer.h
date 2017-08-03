#pragma once
int initCameraView();
int exitAndDestroy();
int processFrameInit();
int acquireOneFrame(void* textureDataPtrColor, void* textureDataPtrDepth);
int acquireOneFrame_capture(void* textureDataPtrColor, void* textureDataPtrDepth);