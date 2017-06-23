// Example low level rendering Unity plugin

#include "PlatformBase.h"
#include "RenderAPI.h"
#include "camera_viewer.h"

#include <assert.h>
#include <math.h>

#include <iostream>
#include <sstream>
//#include "VideoPlayer.h"

#include <concrt.h>
#include <shcore.h>
#include <ppltasks.h>

// --------------------------------------------------------------------------
// Include headers for the graphics APIs we support

#if SUPPORT_D3D11
#include <d3d11.h>
#include "Unity/IUnityGraphicsD3D11.h"
#endif

//#include <windows.h>
//#include <stdio.h>
//
//using namespace Platform;
//using namespace concurrency;
//using namespace Windows::Foundation;
//using namespace Windows::Storage;
////using namespace Windows::Storage::Pickers;
// --------------------------------------------------------------------------
// SetTimeFromUnity, an example function we export which is called by one of the scripts.

static float g_Time;

// Just allocate a system memory buffer here for simplicity
//static unsigned char* texdata;// = new unsigned char[1920 * 1080 * 4];



static void* g_RGBTextureHandle = NULL;
static int   g_RGBTextureWidth = 0;
static int   g_RGBTextureHeight = 0;
static void* g_DepthTextureHandle = NULL;
static int   g_DepthTextureWidth = 0;
static int   g_DepthTextureHeight = 0;

static bool g_bFileLoaded = false;
static IStream* g_pFileStreamData = nullptr;


int main() {
	printOne();
}
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API Play();

// --------------------------------------------------------------------------
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetTimeFromUnity(float t) {
	g_Time = t;

	//static bool firstRestart = true;
	//if (firstRestart && t > 7.0f) {
	//	firstRestart = false;
	//	g_pVideoPlayer->Stop();
	//	Play();
	//}

	//BOOL bRes = g_pVideoPlayer->Update();
	/*if (bRes)
	DebugInUnity("Update true");
	else
	DebugInUnity("Update false");*/
}

// SetTextureFromUnity, an example function we export which is called by one of the scripts.
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetRGBTextureFromUnity(void* textureHandle, int w, int h)
{
	// A script calls this at initialization time; just remember the texture pointer here.
	// Will update texture pixels each frame from the plugin rendering event (texture update
	// needs to happen on the rendering thread).
	initCameraView();
	processFrameInit();
	g_RGBTextureHandle = textureHandle;
	g_RGBTextureWidth = w;
	g_RGBTextureHeight = h;

	//texdata = new unsigned char[w * h * 4];
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetDepthTextureFromUnity(void* textureHandle, int w, int h)
{
	// A script calls this at initialization time; just remember the texture pointer here.
	// Will update texture pixels each frame from the plugin rendering event (texture update
	// needs to happen on the rendering thread).
	//initCameraView();
	//processFrameInit();
	g_DepthTextureHandle = textureHandle;
	g_DepthTextureWidth = w;
	g_DepthTextureHeight = h;

	//texdata = new unsigned char[w * h * 4];
}

//extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetVideoProperties(int* width, int* height, float* fps)
//{
//	if (!g_bFileLoaded)
//		return;
//	*width = g_pVideoPlayer->GetWidth();
//	*height= g_pVideoPlayer->GetHeight();
//	*fps = g_pVideoPlayer->GetFramerate();
//}

//extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetVideoWidth() {
//	return g_pVideoPlayer->GetWidth();
//}
//
//extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetVideoHeight() {
//	return g_pVideoPlayer->GetHeight();
//}
//
//extern "C" float UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetVideoFramerate() {
//	return g_pVideoPlayer->GetFramerate();
//}
//
//extern "C" float UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetVideoDuration() {
//	return g_pVideoPlayer->GetDuration();
//}

HRESULT typedef (__stdcall *CREATE_INSTANCE_FUNC)(void* ppv);


//extern "C" BOOL UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API InitializeDecoder() {
//	g_pVideoPlayer = new CVideoPlayer();
//	return g_pVideoPlayer->Initialize();
//}

char* MakeStringCopy(const char* string) {
	if (string == NULL) return NULL;
	char* res = (char*)malloc(strlen(string) + 1);
	strcpy_s(res, strlen(string) + 1, string);
	return res;
}

//extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API Play() {
//	if (g_pFileStreamData == nullptr)
//		return;
//	DebugInUnity("g_pFileStreamData NOT NULL");
//	g_pVideoPlayer->Play(g_pFileStreamData);
//	DebugInUnity("Stopping");
//}
//
//extern "C" BOOL UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API Seek(float time) {
//	DebugInUnity("Seeking");
//	return g_pVideoPlayer->Seek(time);
//}
//
//extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API LoadFile(char* filename) {
//	//fprintf(g_DebugFile, "Cpp LoadFile %s", filename);
//	g_bFileLoaded = false;
//	char* cstr = MakeStringCopy(filename);
//	std::string s_str(cstr);
//	std::wstring wid_str = std::wstring(s_str.begin(), s_str.end());
//	const wchar_t* w_char = wid_str.c_str();
//	Platform::String^ p_string = ref new Platform::String(w_char);
//	//DebugInUnity(std::string(s_str));
//
//	//Uri^ uri = ref new Uri("ms-appx:///Data/StreamingAssets/test_720p.mp4");
//	Uri^ uri = ref new Uri("ms-appx:///Data/", p_string);
//	
//	std::wstring wsstr(uri->ToString()->Data());
//	std::string res(wsstr.begin(), wsstr.end());
//	DebugInUnity(res);
//
//	auto taskfindfile = create_task(StorageFile::GetFileFromApplicationUriAsync(uri)).then([](Windows::Storage::StorageFile^ file)
//	{
//		DebugInUnity("createTask1 success");
//		//auto file = t.get();
//		if (file) {
//			DebugInUnity("file found");
//
//			auto taskopenfile = create_task(file->OpenAsync(Windows::Storage::FileAccessMode::Read)).then([](Windows::Storage::Streams::IRandomAccessStream^ stream)
//			{
//				DebugInUnity("file opened");
//
//				HRESULT hr = CreateStreamOverRandomAccessStream(reinterpret_cast<IUnknown*>(stream), IID_PPV_ARGS(&g_pFileStreamData));
//				if (S_OK != hr) {
//					DebugInUnity("CreateStreamOverRandomAccessStream error");
//					__debugbreak();
//				}
//
//				/*const int bytestoread = 1024000;
//				byte buffer[bytestoread];
//				ULONG bytesRead;*/
//				//hr = fileStreamData->Read(buffer, bytestoread, &bytesRead);
//				std::ostringstream s;
//				//s << "file opened" << (int)bytesRead;
//				s << "stream " << stream->CanRead << " " << stream->Size;
//				std::string i_as_string(s.str());
//				DebugInUnity(i_as_string);
//				//DebugInUnity(std::to_string((int)bytesRead));
//				//DebugInUnity("here");
//
//				g_bFileLoaded = true;
//
//				/*DebugInUnity("before play");
//				g_pVideoPlayer->Play(g_pFileStreamData);
//				DebugInUnity("after play");*/
//			});
//
//			taskopenfile.wait();
//		}
//		else {
//			DebugInUnity("file not found");
//		}
//
//	});
//
//	taskfindfile.wait();
//
//	Play();
//
//
//}


//extern "C" BOOL UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API IsFileLoaded() {
//	return g_bFileLoaded;
//}

// --------------------------------------------------------------------------
// UnitySetInterfaces

static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType);

static IUnityInterfaces* s_UnityInterfaces = NULL;
static IUnityGraphics* s_Graphics = NULL;

extern "C" void	UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
{
	//std::cout << "UnityPluginLoad" << std::endl;
	//return;
	s_UnityInterfaces = unityInterfaces;
	s_Graphics = s_UnityInterfaces->Get<IUnityGraphics>();
	s_Graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);

	// Run OnGraphicsDeviceEvent(initialize) manually on plugin load
	OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
{
	//return;
	s_Graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
}



// --------------------------------------------------------------------------
// GraphicsDeviceEvent


static RenderAPI* s_CurrentAPI = NULL;
static UnityGfxRenderer s_DeviceType = kUnityGfxRendererNull;


static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
{
	//return;
	// Create graphics API implementation upon initialization
	if (eventType == kUnityGfxDeviceEventInitialize)
	{
		assert(s_CurrentAPI == NULL);
		s_DeviceType = s_Graphics->GetRenderer();
		s_CurrentAPI = CreateRenderAPI(s_DeviceType);
	}

	// Let the implementation process the device related events
	if (s_CurrentAPI)
	{
		s_CurrentAPI->ProcessDeviceEvent(eventType, s_UnityInterfaces);
	}

	// Cleanup graphics API implementation upon shutdown
	if (eventType == kUnityGfxDeviceEventShutdown)
	{
		delete s_CurrentAPI;
		s_CurrentAPI = NULL;
		s_DeviceType = kUnityGfxRendererNull;
	}
}



// --------------------------------------------------------------------------
// OnRenderEvent
// This will be called for GL.IssuePluginEvent script calls; eventID will
// be the integer passed to IssuePluginEvent. In this example, we just ignore
// that value.


static void DrawColoredTriangle()
{
	// Draw a colored triangle. Note that colors will come out differently
	// in D3D and OpenGL, for example, since they expect color bytes
	// in different ordering.
	struct MyVertex
	{
		float x, y, z;
		unsigned int color;
	};
	MyVertex verts[3] =
	{
		{ -0.5f, -0.25f,  0, 0xFFff0000 },
		{ 0.5f, -0.25f,  0, 0xFF00ff00 },
		{ 0,     0.5f ,  0, 0xFF0000ff },
	};

	// Transformation matrix: rotate around Z axis based on time.
	float phi = g_Time; // time set externally from Unity script
	float cosPhi = cosf(phi);
	float sinPhi = sinf(phi);
	float worldMatrix[16] = {
		cosPhi,-sinPhi,0,0,
		sinPhi,cosPhi,0,0,
		0,0,1,0,
		0,0,0.7f,1,
	};

	s_CurrentAPI->DrawSimpleTriangles(worldMatrix, 1, verts);
}


//static void ModifyTexturePixels()
//{
//	void* textureHandle = g_TextureHandle;
//	int width = g_TextureWidth;
//	int height = g_TextureHeight;
//	if (!textureHandle)
//		return;
//
//	int textureRowPitch;
//	//void* textureDataPtr = (unsigned char*)s_CurrentAPI->BeginModifyTexture(textureHandle, width, height, &textureRowPitch);
//	void* textureDataPtr = (unsigned char*)texdata;
//	textureRowPitch = width * 4;
//
//	if (!textureDataPtr)
//		return;
//
//	const float t = g_Time * 4.0f;
//
//	unsigned char* dst = (unsigned char*)textureDataPtr;
//	for (int y = 0; y < height; ++y)
//	{
//		unsigned char* ptr = dst;
//		for (int x = 0; x < width; ++x)
//		{
//			// Simple "plasma effect": several combined sine waves
//			//int vv = 33;
//			int vv = int(
//				(127.0f + (127.0f * sinf(x / 7.0f + t))) /*+
//				(127.0f + (127.0f * sinf(y / 5.0f - t))) +
//				(127.0f + (127.0f * sinf((x + y) / 6.0f - t))) +
//				(127.0f + (127.0f * sinf(sqrtf(float(x*x + y*y)) / 4.0f - t)))*/
//				) / 4;
//
//			// Write the texture pixel
//			ptr[0] = vv;
//			ptr[1] = vv;
//			ptr[2] = vv;
//			ptr[3] = vv;
//
//			// To next pixel (our pixels are 4 bpp)
//			ptr += 4;
//		}
//
//		// To next image row
//		dst += textureRowPitch;
//	}
//
//	s_CurrentAPI->EndModifyTexture(textureHandle, width, height, textureRowPitch, textureDataPtr);
//}

static void ModifyVideoTexturePixels()
{
	//void* textureHandle = g_TextureHandle;
	int width_rgb = g_RGBTextureWidth;
	int height_rgb = g_RGBTextureHeight;

	int width_depth = g_DepthTextureWidth;
	int height_depth = g_DepthTextureHeight;
	
	int RGBtextureRowPitch = width_rgb * 4;
	int DepthtextureRowPitch = width_depth * 2;

	if (!g_RGBTextureHandle || !g_DepthTextureHandle)
		return;

	//void* textureDataPtr = (unsigned char*)s_CurrentAPI->BeginModifyTexture(textureHandle, width, height, &textureRowPitch);
	/*void* textureDataPtr = (unsigned char*)texdata;*/
	void* textureDataPtrColor = new unsigned char[width_rgb * height_rgb * 4];
	void* textureDataPtrDepth = new unsigned char[width_depth * height_depth * 2];
	
	acquireOneFrame(textureDataPtrColor, textureDataPtrDepth);


	if (!textureDataPtrDepth || !textureDataPtrColor)
		return;
	
	/*g_pVideoPlayer->Process(textureDataPtr, g_Time);*/
	
	s_CurrentAPI->EndModifyTexture(g_RGBTextureHandle, width_rgb, height_rgb, RGBtextureRowPitch, textureDataPtrColor);
	s_CurrentAPI->EndModifyTexture(g_DepthTextureHandle, width_depth, height_depth, DepthtextureRowPitch, textureDataPtrDepth);
	delete textureDataPtrColor;
	delete textureDataPtrDepth;
}

static void UNITY_INTERFACE_API OnRenderEvent(int eventID)
{
//	DebugInUnity("OnRenderEvent");
	//std::cout << "OnRenderEvent " << eventID << std::endl;
	//return;
	// Unknown / unsupported graphics device type? Do nothing
	if (s_CurrentAPI == NULL)
		return;
	if (g_RGBTextureHandle == NULL || g_DepthTextureHandle == NULL)
		return;

	//DrawColoredTriangle();
	//ModifyTexturePixels();
	ModifyVideoTexturePixels();
}


// --------------------------------------------------------------------------
// GetRenderEventFunc, an example function we export which is used to get a rendering event callback function.

extern "C" UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc()
{
	return OnRenderEvent;
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API ExitAndDestroy()
{
	exitAndDestroy();
}

//
//// -------------------------------------------------------------------
////  Direct3D 11 setup/teardown code
//
//
//#if SUPPORT_D3D11
//
//static ID3D11Buffer* g_D3D11VB = NULL; // vertex buffer
//static ID3D11Buffer* g_D3D11CB = NULL; // constant buffer
//static ID3D11VertexShader* g_D3D11VertexShader = NULL;
//static ID3D11PixelShader* g_D3D11PixelShader = NULL;
//static ID3D11InputLayout* g_D3D11InputLayout = NULL;
//static ID3D11RasterizerState* g_D3D11RasterState = NULL;
//static ID3D11BlendState* g_D3D11BlendState = NULL;
//static ID3D11DepthStencilState* g_D3D11DepthState = NULL;
//
//static D3D11_INPUT_ELEMENT_DESC s_DX11InputElementDesc[] = {
//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//};
//
//static bool EnsureD3D11ResourcesAreCreated()
//{
//	if (g_D3D11VertexShader)
//		return true;
//
//	// D3D11 has to load resources. Wait for Unity to provide the streaming assets path first.
//	if (s_UnityStreamingAssetsPath.empty())
//		return false;
//
//	D3D11_BUFFER_DESC desc;
//	memset(&desc, 0, sizeof(desc));
//
//	// vertex buffer
//	desc.Usage = D3D11_USAGE_DEFAULT;
//	desc.ByteWidth = 1024;
//	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	g_D3D11Device->CreateBuffer(&desc, NULL, &g_D3D11VB);
//
//	// constant buffer
//	desc.Usage = D3D11_USAGE_DEFAULT;
//	desc.ByteWidth = 64; // hold 1 matrix
//	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	desc.CPUAccessFlags = 0;
//	g_D3D11Device->CreateBuffer(&desc, NULL, &g_D3D11CB);
//
//
//	HRESULT hr = -1;
//	Buffer vertexShader;
//	Buffer pixelShader;
//	std::string vertexShaderPath(s_UnityStreamingAssetsPath);
//	vertexShaderPath += "/Shaders/DX11_9_1/SimpleVertexShader.cso";
//	std::string fragmentShaderPath(s_UnityStreamingAssetsPath);
//	fragmentShaderPath += "/Shaders/DX11_9_1/SimplePixelShader.cso";
//	LoadFileIntoBuffer(vertexShaderPath, vertexShader);
//	LoadFileIntoBuffer(fragmentShaderPath, pixelShader);
//
//	if (vertexShader.size() > 0 && pixelShader.size() > 0)
//	{
//		hr = g_D3D11Device->CreateVertexShader(&vertexShader[0], vertexShader.size(), nullptr, &g_D3D11VertexShader);
//		if (FAILED(hr)) DebugLog("Failed to create vertex shader.\n");
//		hr = g_D3D11Device->CreatePixelShader(&pixelShader[0], pixelShader.size(), nullptr, &g_D3D11PixelShader);
//		if (FAILED(hr)) DebugLog("Failed to create pixel shader.\n");
//	}
//	else
//	{
//		DebugLog("Failed to load vertex or pixel shader.\n");
//	}
//	// input layout
//	if (g_D3D11VertexShader && vertexShader.size() > 0)
//	{
//		g_D3D11Device->CreateInputLayout(s_DX11InputElementDesc, 2, &vertexShader[0], vertexShader.size(), &g_D3D11InputLayout);
//	}
//
//	// render states
//	D3D11_RASTERIZER_DESC rsdesc;
//	memset(&rsdesc, 0, sizeof(rsdesc));
//	rsdesc.FillMode = D3D11_FILL_SOLID;
//	rsdesc.CullMode = D3D11_CULL_NONE;
//	rsdesc.DepthClipEnable = TRUE;
//	g_D3D11Device->CreateRasterizerState(&rsdesc, &g_D3D11RasterState);
//
//	D3D11_DEPTH_STENCIL_DESC dsdesc;
//	memset(&dsdesc, 0, sizeof(dsdesc));
//	dsdesc.DepthEnable = TRUE;
//	dsdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
//	dsdesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
//	g_D3D11Device->CreateDepthStencilState(&dsdesc, &g_D3D11DepthState);
//
//	D3D11_BLEND_DESC bdesc;
//	memset(&bdesc, 0, sizeof(bdesc));
//	bdesc.RenderTarget[0].BlendEnable = FALSE;
//	bdesc.RenderTarget[0].RenderTargetWriteMask = 0xF;
//	g_D3D11Device->CreateBlendState(&bdesc, &g_D3D11BlendState);
//
//	return true;
//}
//
//static void ReleaseD3D11Resources()
//{
//	SAFE_RELEASE(g_D3D11VB);
//	SAFE_RELEASE(g_D3D11CB);
//	SAFE_RELEASE(g_D3D11VertexShader);
//	SAFE_RELEASE(g_D3D11PixelShader);
//	SAFE_RELEASE(g_D3D11InputLayout);
//	SAFE_RELEASE(g_D3D11RasterState);
//	SAFE_RELEASE(g_D3D11BlendState);
//	SAFE_RELEASE(g_D3D11DepthState);
//}
//
//static void DoEventGraphicsDeviceD3D11(UnityGfxDeviceEventType eventType)
//{
//	if (eventType == kUnityGfxDeviceEventInitialize)
//	{
//		IUnityGraphicsD3D11* d3d11 = s_UnityInterfaces->Get<IUnityGraphicsD3D11>();
//		g_D3D11Device = d3d11->GetDevice();
//
//		EnsureD3D11ResourcesAreCreated();
//	}
//	else if (eventType == kUnityGfxDeviceEventShutdown)
//	{
//		ReleaseD3D11Resources();
//	}
//}
//
//#endif // #if SUPPORT_D3D11