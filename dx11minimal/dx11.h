#ifndef DX11_H
#define DX11_H

#define _XM_SSE_INTRINSICS_ 
#include <d3d11.h>
#include <d2d1.h>
#include <d3dcompiler.h>
#include "DirectXMath.h"
#include <DirectXPackedVector.h>
#include <debugapi.h>
#include <algorithm>
#include <deque>
#include <stdio.h>
#include <fstream>

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

#define FRAMES_PER_SECOND 144

typedef unsigned long uint32;
typedef long int32;

static inline int32 _log2(float x);

extern ID3D11Device* device;
extern ID3D11DeviceContext* context;
extern IDXGISwapChain* swapChain;

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
	void Init(int, int);

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
		
		ID3D11UnorderedAccessView* UnorderedAccessView;

		tType type;
		tFormat format;
		XMFLOAT2 size;
		bool mipMaps;
		bool depth;

	};

	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

	extern textureDesc Texture[max_tex];

	extern int currentRT;
	extern int texturesCount;

	void CreateTex(int, bool);
	void ShaderRes(int);
	void rtView(int);
	void Depth(int);
	void shaderResDepth(int);
	void Create(int, tType, tFormat, XMFLOAT2, bool, bool, bool = false);
	void UnbindAll();
	void SetViewport(int, byte);
	void CopyColor(int, int);
	void CopyDepth(int, int);
	void TextureToShader(int, unsigned int, targetshader = targetshader::both);
	void CreateMipMap();
	void RenderTarget(int, unsigned int);
	void DepthTarget(int, int);
	void LoadTexture(const char*);
}

namespace Models
{

#define max_models 255

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 texture;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	extern D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	extern D3D11_SUBRESOURCE_DATA vertexData, indexData;

	extern int vertexCount;
	extern int indexCount;

	struct modelDesc
	{
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;

		int indexes;
	};

	extern modelDesc Model[max_models];
	extern int modelsCount;

	void CreateModel(int, VertexType*, unsigned long*);
	void Create(int, VertexType*, unsigned long*);

	void LoadTxtModel(const char* filename, bool = false);
	void LoadGltfModel(const char* filename);
	void LoadObjModel(const char* filename);
}

namespace Shaders {

	struct VertexShader {
		ID3D11VertexShader* vShader;
		ID3DBlob* pBlob;
		ID3D11InputLayout* pLayout;
	};

	struct PixelShader {
		ID3D11PixelShader* pShader;
		ID3DBlob* pBlob;
	};

	struct GeometryShader {
		ID3D11GeometryShader* gShader;
		ID3DBlob* pBlob;
	};

	struct ComputeShader {
		ID3D11ComputeShader* cShader;
		ID3DBlob* pBlob;
	};

	extern VertexShader VS[255];
	extern PixelShader PS[255];
	extern GeometryShader GS[255];
	extern ComputeShader CS[255];

	extern ID3DBlob* pErrorBlob;

	extern wchar_t shaderPathW[MAX_PATH];

	extern int currentVS;
	extern int currentPS;
	extern int currentGS;
	extern int currentCS;

	LPCWSTR nameToPatchLPCWSTR(const char*);
	void Log(const char*);
	void CompilerLog(LPCWSTR, HRESULT, const char*);

	void CreateVS(int, LPCWSTR);
	void CreatePS(int, LPCWSTR);
	void CreateGS(int, LPCWSTR);
	void CreateCS(int, LPCWSTR);

	void Init();

	void vShader(unsigned int);
	void pShader(unsigned int);
	void gShader(unsigned int);
	void cShader(unsigned int);
}

namespace Compute
{
	void Dispatch(int csIndex, int texInput, int texOutput);
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
	// buffer structures
	
	struct DrawerMat {
		XMMATRIX model;
		float hilight;
	};

	struct Camera {
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;
		XMFLOAT4 cPos;
	};

	struct Frame {
		XMFLOAT4 time;
		XMFLOAT4 aspect;
	};

	struct Factors {
		float AriesNebulaLerpFactor;
	};

	//----------------------------------------------------------------

	extern ID3D11Buffer* buffer[9];

#define constCount 256

	//b0 - use "params" label in shader
	extern float drawerV[constCount];//update per draw call

	//b1 - use "params" label in shader
	extern float drawerP[constCount];//update per draw call

	//b2
	extern DrawerMat drawerMat;

	//b3 
	extern Camera camera;

	//b4
	extern Frame frame;

	//b5
	extern XMFLOAT4 global[constCount];//update once on start

	//b6
	extern Factors factors;

	//b7
	extern int drawerInt[constCount];

	//b8
	extern XMMATRIX drawerMatrix[constCount];

	int roundUp(int, int);
	void Create(ID3D11Buffer*&, int);
	void CreateVertexBuffer(int);
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
	void ConstToGeometry(int);
	void ConstToPixel(int);
	void ConstToCompute(int);

	namespace getbyname {
		enum { drawerV, drawerP, drawerMat, camera, frame, global, factors, drawerInt, drawerMatrix };
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

	void Init(HWND, int, int);
}


namespace InputAssembler
{
	enum class topology { triList, lineList, lineStrip, pointList };

	void IA(topology);
	void vBuffer(int);
}


void Dx11Init(HWND, int, int);


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

#endif // !DX11_H