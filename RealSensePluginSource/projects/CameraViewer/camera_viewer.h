#pragma once
int initCameraView();
int exitAndDestroy();
void printOne();
int processFrameInit();
int acquireOneFrame(void* textureDataPtrColor, void* textureDataPtrDepth);
