#pragma once

#ifndef DX11_H
#define DX11_H


#include <d3d11.h>
#include <d2d1.h>
#include <d3dcompiler.h>
#include "DirectXMath.h"
#include <DirectXPackedVector.h>
#include <debugapi.h>

#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "xaudio2.lib")
#pragma comment(lib, "d2d1.lib")

#include "utils.h"
#include "timer.h"

using namespace DirectX;

#define FRAMES_PER_SECOND 60
#define FRAME_LEN (1000. / (float) FRAMES_PER_SECOND)

typedef unsigned long uint32;
typedef long int32;

static inline int32 _log2(float x);

extern ID3D11Device* device;
extern ID3D11DeviceContext* context;
extern IDXGISwapChain* swapChain;

extern int width;
extern int height;
extern float aspect;
extern float iaspect;

enum targetshader { vertex, pixel, both };

struct rect {
	int x; int y; int z; int w;
};

namespace Rasterizer
{
	enum class cullmode { off, front, back, wireframe };
	extern ID3D11RasterizerState* rasterState[4];

	void Cull(cullmode);
	void Scissors(rect);
	void Init();

}

namespace Textures
{

#define max_tex 255
#define mainRTIndex 0

	enum tType { flat, cube };

	extern DXGI_FORMAT dxTFormat[5];
	enum tFormat { u8, s8, s16, s32, r8 };

	extern D3D11_TEXTURE2D_DESC tdesc;
	extern D3D11_SHADER_RESOURCE_VIEW_DESC svDesc;
	extern D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	extern D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;

	extern ID3D11RenderTargetView* mrtView[8];

	struct textureDesc {

		ID3D11Texture2D* pTexture;
		ID3D11ShaderResourceView* TextureResView;
		ID3D11RenderTargetView* RenderTargetView[16][6];//16 for possible mips? // 6 for cubemap

		ID3D11Texture2D* pDepth;
		ID3D11ShaderResourceView* DepthResView;
		ID3D11DepthStencilView* DepthStencilView[16];

		tType type;
		tFormat format;
		XMFLOAT2 size;
		bool mipMaps;
		bool depth;

	};

	extern textureDesc Texture[max_tex];

	extern int currentRT;

	void CreateTex(int);
	void ShaderRes(int);
	void rtView(int);
	void Depth(int);
	void shaderResDepth(int);
	void Create(int, tType, tFormat, XMFLOAT2, bool, bool);
	void UnbindAll();
	void SetViewport(int, byte);
	void CopyColor(int, int);
	void CopyDepth(int, int);
	void TextureToShader(int, unsigned int, targetshader);
	void CreateMipMap();
	void RenderTarget(int, unsigned int);
}


namespace Shaders {

	struct VertexShader {
		ID3D11VertexShader* vShader;
		ID3DBlob* pBlob;
	};

	struct PixelShader {
		ID3D11PixelShader* pShader;
		ID3DBlob* pBlob;
	};

	struct GeometryShader {
		ID3D11GeometryShader* gShader;
		ID3DBlob* pBlob;
	};

	extern VertexShader VS[255];
	extern PixelShader PS[255];
	extern GeometryShader GS[255];

	extern ID3DBlob* pErrorBlob;

	extern wchar_t shaderPathW[MAX_PATH];

	LPCWSTR nameToPatchLPCWSTR(const char*);
	void Log(const char*);
	void CompilerLog(LPCWSTR, HRESULT, const char*);
	void CreateVS(int, LPCWSTR);
	void CreatePS(int, LPCWSTR);
	void CreateGS(int, LPCWSTR);
	void Init();
	void vShader(unsigned int);
	void pShader(unsigned int);
	void gShader(unsigned int);

}

namespace Sampler
{
	enum class filter { linear, point, minPoint_magLinear };
	enum class addr {clamp,wrap};

	extern ID3D11SamplerState* pSampler[3][2][2];//filter, addressU, addressV
	extern ID3D11SamplerState* pSamplerComp;//for shadowmapping

	void Init();
	void Sampler(targetshader, unsigned int, filter, addr, addr);
	void SamplerComp(unsigned int);
}

namespace ConstBuf
{
	extern ID3D11Buffer* buffer[7];

#define constCount 32

	//b0 - use "params" label in shader
	extern float drawerV[constCount];//update per draw call

	//b1 - use "params" label in shader
	extern float drawerP[constCount];//update per draw call

	//b2
	struct {
		XMMATRIX model;
		float hilight;
	} drawerMat;//update per draw call

	//b3 
	struct {
		XMMATRIX world[2];
		XMMATRIX view[2];
		XMMATRIX proj[2];
		float    camDist;
	} camera;//update per camera set

	//b4
	struct {
		XMFLOAT4 time;
		XMFLOAT4 aspect;
	} frame;//update per frame

	//b5
	extern XMFLOAT4 global[constCount];//update once on start

	//b6
	struct {
		float AriesNebulaLerpFactor;
	} factors;

	int roundUp(int, int);
	void Create(ID3D11Buffer*&, int);
	void Init();

	template <typename T>
	void Update(int i, T* data)
	{
		context->UpdateSubresource(buffer[i], 0, NULL, data, 0, 0);
	}

	void UpdateFrame();
	void UpdateDrawerMat();
	void UpdateCamera();
	void UpdateFactors();
	void ConstToVertex(int);
	void ConstToPixel(int);

	namespace getbyname {
		enum { drawerV, drawerP, drawerMat, camera, frame, global, factors };
	}
}



namespace Blend
{

	enum class blendmode { off, on, alpha };
	enum class blendop { add, sub, revsub, min, max };

	extern ID3D11BlendState* blendState[3][5];
	extern D3D11_BLEND_DESC bSDesc;

	void CreateMixStates(int);
	void Init();
	void Blending(blendmode, blendop);
}

namespace Depth
{
	enum class depthmode { off, on, readonly, writeonly };

	extern ID3D11DepthStencilState* pDSState[4];

	void Init();
	void Depth(depthmode);
}


extern ID2D1Factory* d2dFactory;
extern ID2D1HwndRenderTarget* d2dRenderTarget;

void InitD2D(HWND);


namespace Device
{
#define DirectXDebugMode true

	extern D3D_DRIVER_TYPE	driverType;

	void Init(HWND);
}


namespace InputAssembler
{
	enum class topology { triList, lineList, lineStrip };

	void IA(topology);
}


void Dx11Init(HWND);


struct color4 {
	float r;
	float g;
	float b;
	float a;
};

namespace Draw
{
	void Clear(color4);
	void ClearDepth();
	void NullDrawer(int, unsigned int);
	void Drawer(int);
	void SwitchRenderTextures();
	void OutputRenderTextures();
	void elipse(int, unsigned int);
	void Present();
}

void frameConst();
float DegreesToRadians(float);

namespace Hero
{

	struct State
	{
		float timeShiftPressed;
		XMVECTOR relativeMovement = XMVectorSet(-1, 0, 0, 0);
		XMVECTOR currentRotation = XMQuaternionIdentity();
		XMVECTOR Forwardbuf = XMVectorSet(0, 0, 1, 0);
		XMVECTOR defaultUp = XMVectorSet(0, -1, 0, 0);
		XMVECTOR Right = XMVectorSet(-1, 0, 0, 0);
		XMVECTOR at = XMVectorSet(0, 0, 0, 0);
		XMVECTOR Up = XMVector3Rotate(defaultUp, currentRotation);
		XMMATRIX constellationOffset = XMMatrixTranslation(0, 0, 0);
		XMVECTOR position = XMVectorSet(0, 0, 0, 0);
		XMMATRIX worldMatrix = XMMatrixIdentity();

		
		float attackRotationProgress = 0.0f;
		bool isAttackRotating = false;
		DWORD attackStartTime = 0;

	} static state;


	void UpdateAttackRotation(float);
}

namespace Camera
{
	struct State
	{
		int n = 0; //угол поворота
		bool mouse = false;
		float camDist = 500.0f;
		float minDist = 500.0f;
		float maxDist = 1000.0f;
		float fovAngle = 60.0f;
		float verticalOffset = 180.0f;  
		float horizontalOffset = 0.0f; 
		float distanceOffset = 600.0f;
		XMVECTOR relativeMovement = XMVectorSet(-1, 0, 0, 0);
		XMVECTOR currentRotation = XMQuaternionIdentity();
		XMVECTOR Forward = XMVectorSet(0, 0, 1, 0);
		XMVECTOR defaultUp = XMVectorSet(0, -1, 0, 0);
		XMVECTOR Right = XMVectorSet(-1, 0, 0, 0);
		XMVECTOR at = XMVectorSet(0, 0, 0, 0);
		XMVECTOR Up = XMVector3Rotate(defaultUp, currentRotation);
		XMVECTOR Eye;
		XMMATRIX constellationOffset = XMMatrixTranslation(0, 0, 0);
		XMVECTOR EyeBack = XMVectorSet(0, 0, -1, 0);
	} static state;

	
	void UpdateCameraPosition();
	void Camera();
	void HandleMouseWheel(int);
}

namespace View {
	extern XMMATRIX m_projectionMatrix;
	extern XMMATRIX m_worldMatrix;

	void GetWorldMatrix(XMMATRIX&);
}

#endif // !DX11_H