#pragma once

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

ID3D11Device* device = NULL;
ID3D11DeviceContext* context = NULL;
IDXGISwapChain* swapChain = NULL;

int width;
int height;
float aspect;
float iaspect;

enum targetshader { vertex, pixel, both };

struct rect {
	int x; int y; int z; int w;
};

namespace Rasterizer
{
	enum class cullmode { off, front, back, wireframe };
	ID3D11RasterizerState* rasterState[4];

	void Cull(cullmode mode);
	void Scissors(rect r);
	void Init();

}

namespace Textures
{

#define max_tex 255
#define mainRTIndex 0

	enum tType { flat, cube };
	

	DXGI_FORMAT dxTFormat[] = { DXGI_FORMAT_R8G8B8A8_UNORM ,DXGI_FORMAT_R8G8B8A8_SNORM ,DXGI_FORMAT_R16G16B16A16_FLOAT ,DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R8_SNORM };
	enum tFormat { u8, s8, s16, s32, r8 };
	D3D11_TEXTURE2D_DESC tdesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC svDesc;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;

	ID3D11RenderTargetView* mrtView[8];

	typedef struct {

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

	} textureDesc;

	textureDesc Texture[max_tex];

	int currentRT = 0;

	void CreateTex(int i);
	void ShaderRes(int i);
	void rtView(int i);
	void Depth(int i);
	void shaderResDepth(int i);
	void Create(int i, tType type, tFormat format, XMFLOAT2 size, bool mipMaps, bool depth);
	void UnbindAll();
	void SetViewport(int texId, byte level = 0);
	void CopyColor(int dst, int src);
	void CopyDepth(int dst, int src);
	void TextureToShader(int tex, unsigned int slot, targetshader tA = targetshader::both);
	void CreateMipMap();
	void RenderTarget(int target, unsigned int level = 0);

}


namespace Shaders {

	typedef struct {
		ID3D11VertexShader* vShader;
		ID3DBlob* pBlob;
	} VertexShader;

	typedef struct {
		ID3D11PixelShader* pShader;
		ID3DBlob* pBlob;
	} PixelShader;

	typedef struct {
		ID3D11GeometryShader* gShader;
		ID3DBlob* pBlob;
	} GeometryShader;

	VertexShader VS[255];
	PixelShader PS[255];
	GeometryShader GS[255];

	ID3DBlob* pErrorBlob;

	wchar_t shaderPathW[MAX_PATH];

	LPCWSTR nameToPatchLPCWSTR(const char* path);
	void Log(const char* message);
	void CompilerLog(LPCWSTR source, HRESULT hr, const char* message);
	void CreateVS(int i, LPCWSTR name);
	void CreatePS(int i, LPCWSTR name);
	void CreateGS(int i, LPCWSTR name);
	void Init();
	void vShader(unsigned int n);
	void pShader(unsigned int n);
	void gShader(unsigned int n);

}

namespace Sampler
{
	enum class filter { linear, point, minPoint_magLinear };
	enum class addr {clamp,wrap};

	ID3D11SamplerState* pSampler[3][2][2];//filter, addressU, addressV
	ID3D11SamplerState* pSamplerComp;//for shadowmapping

	void Init();
	void Sampler(targetshader shader, unsigned int slot, filter filterType, addr addressU, addr addressV);
	void SamplerComp(unsigned int slot);
}

namespace ConstBuf
{
	ID3D11Buffer* buffer[7];

#define constCount 32

	//b0 - use "params" label in shader
	float drawerV[constCount];//update per draw call

	//b1 - use "params" label in shader
	float drawerP[constCount];//update per draw call

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
	XMFLOAT4 global[constCount];//update once on start

	//b6
	struct {
		float AriesNebulaLerpFactor;
	} factors;

	int roundUp(int n, int r);

	void Create(ID3D11Buffer*& buf, int size);

	void Init();

	template <typename T>
	void Update(int i, T* data);

	void UpdateFrame();
	void UpdateDrawerMat();
	void UpdateCamera();
	void UpdateFactors();
	void ConstToVertex(int i);
	void ConstToPixel(int i);

	namespace getbyname {
		enum { drawerV, drawerP, drawerMat, camera, frame, global, factors };
	}
}



namespace Blend
{

	enum class blendmode { off, on, alpha };
	enum class blendop { add, sub, revsub, min, max };

	ID3D11BlendState* blendState[3][5];
	D3D11_BLEND_DESC bSDesc;

	void CreateMixStates(int j);
	void Init();
	void Blending(blendmode mode = blendmode::off, blendop operation = blendop::add);
}

namespace Depth
{
	enum class depthmode { off, on, readonly, writeonly };

	ID3D11DepthStencilState* pDSState[4];

	void Init();
	void Depth(depthmode mode);
}

ID2D1Factory* d2dFactory = nullptr;
ID2D1HwndRenderTarget* d2dRenderTarget = nullptr;

void InitD2D(HWND hwnd);


namespace Device
{
#define DirectXDebugMode true

	D3D_DRIVER_TYPE	driverType = D3D_DRIVER_TYPE_NULL;

	void Init(HWND hwnd);
}


namespace InputAssembler
{
	enum class topology { triList, lineList, lineStrip };

	void IA(topology topoType);
}

void Dx11Init(HWND hwnd);


struct color4 {
	float r;
	float g;
	float b;
	float a;
};

namespace Draw
{
	void Clear(color4 color);
	void ClearDepth();
	void NullDrawer(int quadCount, unsigned int instances = 1);
	void Drawer(int quadCount);
	void SwitchRenderTextures();
	void OutputRenderTextures();
	void elipse(int quadCount, unsigned int instances = 1);
	void Present();
}

void frameConst();
float DegreesToRadians(float degrees);

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


	void UpdateAttackRotation(float deltaTime);
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
}

namespace View {
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_worldMatrix;

	void GetWorldMatrix(XMMATRIX& worldMatrix);
}