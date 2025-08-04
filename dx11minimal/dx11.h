#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "xaudio2.lib")

#include <d3d11.h>
#include <d3dcompiler.h>
#include "DirectXMath.h"
#include <DirectXPackedVector.h>
#include <debugapi.h>

using namespace DirectX;

#define FRAMES_PER_SECOND 60
#define FRAME_LEN (1000. / (float) FRAMES_PER_SECOND)

typedef unsigned long uint32;
typedef long int32;

static inline int32 _log2(float x)
{
	uint32 ix = (uint32&)x;
	uint32 exp = (ix >> 23) & 0xFF;
	int32 log2 = int32(exp) - 127;

	return log2;
}

/*namespace timer
{
	double PCFreq = 0.0;
	__int64 counterStart = 0;

	double startTime = 0;
	double frameBeginTime = 0;
	double frameEndTime = 0;
	double nextFrameTime = 0;
	double frameRenderingDuration = 0.0;
	int timeCursor = 0;

	void StartCounter()
	{
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);
		PCFreq = double(li.QuadPart) / 1000.0;

		QueryPerformanceCounter(&li);
		counterStart = li.QuadPart;
	}




	void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	// Normalized pixel coordinates (from 0 to 1)
	vec2 uv = fragCoord/iResolution.x-.5;

	// Time varying pixel color
	float x= length(uv);
	vec3 col = (abs(1./(abs(x*115.))*1.6*(1.-abs(x))))*(1.-abs(x))*vec3(1.,1.,1.);
	col=pow(col,vec3(.4));

	// Output to screen
	fragColor = vec4(col,1.0);
}




	double GetCounter()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return double(li.QuadPart - counterStart) / PCFreq;
	}

}*/

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

	void Cull(cullmode mode)
	{
		context->RSSetState(rasterState[(int)mode]);
	}

	void Scissors(rect r)
	{
		D3D11_RECT rect = { r.x,r.y,r.z,r.w };
		context->RSSetScissorRects(1, &rect);
	}

	void Init()
	{
		D3D11_RASTERIZER_DESC rasterizerState;
		rasterizerState.FillMode = D3D11_FILL_SOLID;
		rasterizerState.CullMode = D3D11_CULL_NONE;
		rasterizerState.FrontCounterClockwise = true;
		rasterizerState.DepthBias = false;
		rasterizerState.DepthBiasClamp = 0;
		rasterizerState.SlopeScaledDepthBias = 0;
		rasterizerState.DepthClipEnable = false;
		rasterizerState.ScissorEnable = true;
		rasterizerState.MultisampleEnable = false;
		rasterizerState.AntialiasedLineEnable = true;
		device->CreateRasterizerState(&rasterizerState, &rasterState[0]);

		rasterizerState.CullMode = D3D11_CULL_FRONT;
		device->CreateRasterizerState(&rasterizerState, &rasterState[1]);

		rasterizerState.CullMode = D3D11_CULL_BACK;
		device->CreateRasterizerState(&rasterizerState, &rasterState[2]);

		rasterizerState.CullMode = D3D11_CULL_NONE;
		rasterizerState.FillMode = D3D11_FILL_WIREFRAME;
		device->CreateRasterizerState(&rasterizerState, &rasterState[3]);

		context->RSSetState(rasterState[0]);
		Scissors(rect{ 0, 0, width, height });
	}

}

namespace Textures
{

#define max_tex 255
#define mainRTIndex 0

	enum tType { flat, cube };
	

	DXGI_FORMAT dxTFormat[4] = { DXGI_FORMAT_R8G8B8A8_UNORM ,DXGI_FORMAT_R8G8B8A8_SNORM ,DXGI_FORMAT_R16G16B16A16_FLOAT ,DXGI_FORMAT_R32G32B32A32_FLOAT };
	enum tFormat { u8, s8, s16, s32 };
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

	byte currentRT = 0;

	void CreateTex(int i)
	{
		auto cTex = Texture[i];

		tdesc.Width = (UINT)cTex.size.x;
		tdesc.Height = (UINT)cTex.size.y;
		tdesc.MipLevels = cTex.mipMaps ? (UINT)(_log2(max(cTex.size.x, cTex.size.y))) : 0;
		tdesc.ArraySize = 1;
		tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		tdesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		tdesc.CPUAccessFlags = 0;
		tdesc.SampleDesc.Count = 1;
		tdesc.SampleDesc.Quality = 0;
		tdesc.Usage = D3D11_USAGE_DEFAULT;
		tdesc.Format = dxTFormat[cTex.format];

		if (cTex.type == cube)
		{
			tdesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | tdesc.MiscFlags;
			tdesc.ArraySize = 6;
		}

		HRESULT hr = device->CreateTexture2D(&tdesc, NULL, &Texture[i].pTexture);

	}

	void ShaderRes(int i)
	{
		svDesc.Format = tdesc.Format;
		svDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

		if (Texture[i].type == cube)
		{
			svDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			svDesc.TextureCube.MostDetailedMip = 0;
			svDesc.TextureCube.MipLevels = -1;

		}
		else
		{
			svDesc.Texture2D.MipLevels = -1;
			svDesc.Texture2D.MostDetailedMip = 0;
		}

		HRESULT hr = device->CreateShaderResourceView(Texture[i].pTexture, &svDesc, &Texture[i].TextureResView);
	}

	void rtView(int i)
	{
		renderTargetViewDesc.Format = tdesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		if (Texture[i].type == cube)
		{
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			renderTargetViewDesc.Texture2DArray.ArraySize = 1;
			renderTargetViewDesc.Texture2DArray.MipSlice = 0;

			for (int j = 0; j < 6; j++)
			{
				renderTargetViewDesc.Texture2DArray.FirstArraySlice = j;
				HRESULT hr = device->CreateRenderTargetView(Texture[i].pTexture, &renderTargetViewDesc, &Texture[i].RenderTargetView[0][j]);
			}
		}
		else
		{
			for (unsigned int m = 0; m < tdesc.MipLevels; m++)
			{
				renderTargetViewDesc.Texture2D.MipSlice = m;
				HRESULT hr = device->CreateRenderTargetView(Texture[i].pTexture, &renderTargetViewDesc, &Texture[i].RenderTargetView[m][0]);
			}
		}
	}

	void Depth(int i)
	{
		auto cTex = Texture[i];

		tdesc.Width = (UINT)cTex.size.x;
		tdesc.Height = (UINT)cTex.size.y;
		tdesc.MipLevels = cTex.mipMaps ? (UINT)(_log2(max(cTex.size.x, cTex.size.y))) : 0;
		tdesc.CPUAccessFlags = 0;
		tdesc.SampleDesc.Count = 1;
		tdesc.SampleDesc.Quality = 0;
		tdesc.Usage = D3D11_USAGE_DEFAULT;

		tdesc.ArraySize = 1;
		tdesc.Format = DXGI_FORMAT_R32_TYPELESS;
		tdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		tdesc.MiscFlags = 0;
		HRESULT hr = device->CreateTexture2D(&tdesc, NULL, &Texture[i].pDepth);

		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Flags = 0;

		for (unsigned int m = 0; m < max(1, tdesc.MipLevels); m++)
		{
			descDSV.Texture2D.MipSlice = m;
			HRESULT hr = device->CreateDepthStencilView(Texture[i].pDepth, &descDSV, &Texture[i].DepthStencilView[m]);
		}
	}

	void shaderResDepth(int i)
	{
		svDesc.Format = DXGI_FORMAT_R32_FLOAT;
		svDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		svDesc.Texture2D.MostDetailedMip = 0;
		svDesc.Texture2D.MipLevels = 1;

		HRESULT hr = device->CreateShaderResourceView(Texture[i].pDepth, &svDesc, &Texture[i].DepthResView);
	}

	void Create(int i, tType type, tFormat format, XMFLOAT2 size, bool mipMaps, bool depth)
	{
		ZeroMemory(&tdesc, sizeof(tdesc));
		ZeroMemory(&svDesc, sizeof(svDesc));
		ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
		ZeroMemory(&descDSV, sizeof(descDSV));

		Texture[i].type = type;
		Texture[i].format = format;
		Texture[i].size = size;
		Texture[i].mipMaps = mipMaps;
		Texture[i].depth = depth;

		if (i > 0)
		{
			CreateTex(i);
			ShaderRes(i);
			rtView(i);
		}

		if (depth)
		{
			Depth(i);
			shaderResDepth(i);
		}
	}

	void UnbindAll()
	{
		ID3D11ShaderResourceView* const null[128] = { NULL };
		context->VSSetShaderResources(0, 128, null);
		context->PSSetShaderResources(0, 128, null);
	}

	void SetViewport(int texId, byte level = 0)
	{
		XMFLOAT2 size = Textures::Texture[texId].size;
		float denom = powf(2, level);

		D3D11_VIEWPORT vp;

		vp.Width = (FLOAT)size.x / denom;
		vp.Height = (FLOAT)size.y / denom;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;

		context->RSSetViewports(1, &vp);

		rect r = rect{ 0,0,(int)vp.Width ,(int)vp.Height };
		Rasterizer::Scissors(r);
	}

	void CopyColor(int dst, int src)
	{
		context->CopyResource(Texture[(int)dst].pTexture, Texture[(int)src].pTexture);
	}

	void CopyDepth(int dst, int src)
	{
		context->CopyResource(Texture[(int)dst].pDepth, Texture[(int)src].pDepth);
	}

	void TextureToShader(int tex, unsigned int slot, targetshader tA = targetshader::both)
	{
		if (tA == targetshader::both || tA == targetshader::vertex)
		{
			context->VSSetShaderResources(slot, 1, &Texture[(int)tex].TextureResView);
		}

		if (tA == targetshader::both || tA == targetshader::pixel)
		{
			context->PSSetShaderResources(slot, 1, &Texture[(int)tex].TextureResView);
		}
	}

	void CreateMipMap()
	{
		context->GenerateMips(Texture[currentRT].TextureResView);
	}


	void RenderTarget(int target, unsigned int level = 0)
	{
		currentRT = (int)target;

		auto depthStencil = Texture[(int)target].depth ? Texture[(int)target].DepthStencilView[0] : 0;

		if (Texture[(int)target].type == tType::flat)
		{
			context->OMSetRenderTargets(1, &Texture[(int)target].RenderTargetView[0][0], depthStencil);
		}

		if (Texture[(int)target].type == tType::cube)
		{
			context->OMSetRenderTargets(6, &Texture[(int)target].RenderTargetView[0][0], 0);
		}

		SetViewport(target, level);
	}

}


namespace Shaders {

	typedef struct {
		ID3D11VertexShader* pShader;
		ID3DBlob* pBlob;
	} VertexShader;

	typedef struct {
		ID3D11PixelShader* pShader;
		ID3DBlob* pBlob;
	} PixelShader;

	VertexShader VS[255];
	PixelShader PS[255];

	ID3DBlob* pErrorBlob;

	wchar_t shaderPathW[MAX_PATH];

	LPCWSTR nameToPatchLPCWSTR(const char* path)
	{
		int len = MultiByteToWideChar(CP_ACP, 0, path, -1, NULL, 0);
		MultiByteToWideChar(CP_ACP, 0, path, -1, shaderPathW, len);

		return shaderPathW;
	}

	void Log(const char* message)
	{
		OutputDebugStringA(message);
	}

	void CompilerLog(LPCWSTR source, HRESULT hr, const char* message)
	{
		if (FAILED(hr))
		{
			Log((char*)pErrorBlob->GetBufferPointer());
		}
		else
		{
			char shaderName[1024];
			WideCharToMultiByte(CP_ACP, NULL, source, -1, shaderName, sizeof(shaderName), NULL, NULL);

			Log(message);
			Log((char*)shaderName);
			Log("\n");
		}
	}

	void CreateVS(int i, LPCWSTR name)
	{
		HRESULT hr;

		hr = D3DCompileFromFile(name, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_4_1", NULL, NULL, &VS[i].pBlob, &pErrorBlob);
		CompilerLog(name, hr, "vertex shader compiled: ");

		if (hr == S_OK)
		{
			hr = device->CreateVertexShader(VS[i].pBlob->GetBufferPointer(), VS[i].pBlob->GetBufferSize(), NULL, &VS[i].pShader);
		}

	}

	void CreatePS(int i, LPCWSTR name)
	{
		HRESULT hr;

		hr = D3DCompileFromFile(name, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_4_1", NULL, NULL, &PS[i].pBlob, &pErrorBlob);
		CompilerLog(name, hr, "pixel shader compiled: ");

		if (hr == S_OK)
		{
			hr = device->CreatePixelShader(PS[i].pBlob->GetBufferPointer(), PS[i].pBlob->GetBufferSize(), NULL, &PS[i].pShader);
		}

	}

	void Init()
	{
		CreateVS(0, nameToPatchLPCWSTR("VS.shader"));
		CreatePS(0, nameToPatchLPCWSTR("PS.shader"));

		CreateVS(1, nameToPatchLPCWSTR("VSS.shader"));
		CreatePS(1, nameToPatchLPCWSTR("PSS.shader"));

		CreateVS(2, nameToPatchLPCWSTR("VSFS.shader"));
		CreatePS(2, nameToPatchLPCWSTR("PSFS.shader"));

		CreateVS(3, nameToPatchLPCWSTR("BackgroundStars_VS.shader"));
		CreatePS(3, nameToPatchLPCWSTR("BackgroundStars_PS.shader"));

		CreateVS(4, nameToPatchLPCWSTR("StarLink_VS.shader"));
		CreatePS(4, nameToPatchLPCWSTR("StarLink_PS.shader"));

		CreateVS(5, nameToPatchLPCWSTR("GalaxyFog_VS.shader"));
		CreatePS(5, nameToPatchLPCWSTR("GalaxyFog_PS.shader"));

		CreateVS(6, nameToPatchLPCWSTR("Cursor_VS.shader"));
		CreatePS(6, nameToPatchLPCWSTR("Cursor_PS.shader"));

		CreateVS(7, nameToPatchLPCWSTR("AriesNebula_VS.shader"));
		CreatePS(7, nameToPatchLPCWSTR("AriesNebula_PS.shader"));

		CreateVS(8, nameToPatchLPCWSTR("AriesNebula2_VS.shader"));
		CreatePS(8, nameToPatchLPCWSTR("AriesNebula2_PS.shader"));

		CreateVS(9, nameToPatchLPCWSTR("DotText_VS.shader"));
		CreatePS(9, nameToPatchLPCWSTR("DotText_PS.shader"));

		CreateVS(10, nameToPatchLPCWSTR("PP_VS.shader"));
		CreatePS(10, nameToPatchLPCWSTR("PP_PS.shader"));

		CreateVS(11, nameToPatchLPCWSTR("Cross_VS.shader"));
		CreatePS(11, nameToPatchLPCWSTR("Cross_PS.shader"));

		CreateVS(12, nameToPatchLPCWSTR("UiParticle_VS.shader"));
		CreatePS(12, nameToPatchLPCWSTR("UiParticle_PS.shader"));
	}

	void vShader(unsigned int n)
	{
		context->VSSetShader(VS[n].pShader, NULL, 0);
	}

	void pShader(unsigned int n)
	{
		context->PSSetShader(PS[n].pShader, NULL, 0);
	}

}

namespace Sampler
{
	enum class filter { linear, point, minPoint_magLinear };
	enum class addr {clamp,wrap};

	ID3D11SamplerState* pSampler[3][2][2];//filter, addressU, addressV
	ID3D11SamplerState* pSamplerComp;//for shadowmapping

	void Init()
	{
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));

		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		D3D11_FILTER filter[] = { D3D11_FILTER_MIN_MAG_MIP_LINEAR,
								  D3D11_FILTER_MIN_MAG_MIP_POINT,
								  D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR };

		D3D11_TEXTURE_ADDRESS_MODE address[] = { D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_WRAP };

		constexpr byte fc = sizeof(filter) / sizeof(D3D11_FILTER);
		constexpr byte ac = sizeof(address) / sizeof(D3D11_TEXTURE_ADDRESS_MODE);

		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

		for (byte f = 0; f < fc; f++)
		{
			sampDesc.Filter = (D3D11_FILTER)filter[f];

			for (byte u = 0; u < ac; u++)
			{
				for (byte v = 0; v < ac; v++)
				{
					sampDesc.AddressU = address[u];
					sampDesc.AddressV = address[v];
					HRESULT hr = device->CreateSamplerState(&sampDesc, &pSampler[f][u][v]);
				}
			}
		}

		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		for (int x = 0; x < 4; x++) sampDesc.BorderColor[x] = 0;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
		sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		device->CreateSamplerState(&sampDesc, &pSamplerComp);

	}

	void Sampler(targetshader shader, unsigned int slot, filter filterType, addr addressU, addr addressV)
	{
		if (shader == targetshader::vertex) context->VSSetSamplers(slot, 1, &pSampler[(int)filterType][(int)addressU][(int)addressV]);
		if (shader == targetshader::pixel) context->PSSetSamplers(slot, 1, &pSampler[(int)filterType][(int)addressU][(int)addressV]);
	}

	void SamplerComp(unsigned int slot)
	{
		context->PSSetSamplers(slot, 1, &pSamplerComp);
	}



}

namespace ConstBuf
{
	ID3D11Buffer* buffer[8];

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

	//b7
	struct {
		int NebulaState;
		int NebulaPixelState;
	} demoDayBuffer;  // DELETE

	int roundUp(int n, int r)
	{
		return 	n - (n % r) + r;
	}

	void Create(ID3D11Buffer*& buf, int size)
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = roundUp(size, 16);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.StructureByteStride = 16;

		HRESULT hr = device->CreateBuffer(&bd, NULL, &buf);
	}

	void Init()
	{
		Create(buffer[0], sizeof(drawerV));
		Create(buffer[1], sizeof(drawerP));
		Create(buffer[2], sizeof(drawerMat));
		Create(buffer[3], sizeof(camera));
		Create(buffer[4], sizeof(frame));
		Create(buffer[5], sizeof(global));
		Create(buffer[6], sizeof(factors));
		Create(buffer[7], sizeof(demoDayBuffer));  // DELETE
	}

	template <typename T>
	void Update(int i, T* data)
	{
		context->UpdateSubresource(buffer[i], 0, NULL, data, 0, 0);
	}

	void UpdateFrame()
	{
		context->UpdateSubresource(buffer[4], 0, NULL, &frame, 0, 0);
	}

	void UpdateDrawerMat()
	{
		context->UpdateSubresource(ConstBuf::buffer[2], 0, NULL, &drawerMat, 0, 0);
	}

	void UpdateCamera()
	{
		context->UpdateSubresource(ConstBuf::buffer[3], 0, NULL, &camera, 0, 0);
	}

	void UpdateFactors()
	{
		context->UpdateSubresource(ConstBuf::buffer[6], 0, NULL, &factors, 0, 0);
	}

	void UpdateDemoDayBuffer()  // DELETE
	{
		context->UpdateSubresource(ConstBuf::buffer[7], 0, NULL, &demoDayBuffer, 0, 0);
	}

	void ConstToVertex(int i)
	{
		context->VSSetConstantBuffers(i, 1, &buffer[i]);
	}

	void ConstToPixel(int i)
	{
		context->PSSetConstantBuffers(i, 1, &buffer[i]);
	}


	namespace getbyname {
		enum { drawerV, drawerP, drawerMat, camera, frame, global, factors, demoDayBuffer };  // DELETE
	}
}



namespace Blend
{

	enum class blendmode { off, on, alpha };
	enum class blendop { add, sub, revsub, min, max };

	ID3D11BlendState* blendState[3][5];
	D3D11_BLEND_DESC bSDesc;

	void CreateMixStates(int j)
	{
		for (int i = 0; i < 5; i++)
		{
			bSDesc.RenderTarget[0].BlendOp = (D3D11_BLEND_OP)(i + 1);
			HRESULT hr = device->CreateBlendState(&bSDesc, &blendState[j][i]);
		}
	}

	void Init()
	{
		ZeroMemory(&bSDesc, sizeof(D3D11_BLEND_DESC));

		//all additional rt's without alphablend!
		for (int x = 0; x < 8; x++)
		{
			bSDesc.RenderTarget[x].BlendEnable = false;
			bSDesc.RenderTarget[x].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}

		bSDesc.AlphaToCoverageEnable = false;
		bSDesc.IndependentBlendEnable = true;

		CreateMixStates(0);

		bSDesc.RenderTarget[0].BlendEnable = TRUE;
		bSDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		bSDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		bSDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		bSDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		bSDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		//NO ALPHA
		CreateMixStates(1);

		//ALPHA 
		bSDesc.RenderTarget[0].BlendEnable = TRUE;
		bSDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		bSDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		CreateMixStates(2);

		float blendFactor[4] = { .0f,.0f,.0f,.0f };
		context->OMSetBlendState(blendState[0][0], blendFactor, 0xffffffff);
	}

	void Blending(blendmode mode = blendmode::off, blendop operation = blendop::add)
	{
		float blendFactor[4] = { .0f,.0f,.0f,.0f };
		context->OMSetBlendState(blendState[(int)mode][(int)operation], blendFactor, 0xffffffff);
	}
}

namespace Depth
{
	enum class depthmode { off, on, readonly, writeonly };

	ID3D11DepthStencilState* pDSState[4];

	void Init()
	{
		D3D11_DEPTH_STENCIL_DESC dsDesc;
		// Depth test parameters
		dsDesc.DepthEnable = false;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

		// Stencil test parameters
		dsDesc.StencilEnable = true;
		dsDesc.StencilReadMask = 0xFF;
		dsDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing
		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing 
		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Create depth stencil state
		dsDesc.StencilEnable = false;

		dsDesc.DepthEnable = false;
		device->CreateDepthStencilState(&dsDesc, &pDSState[0]);//off

		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		device->CreateDepthStencilState(&dsDesc, &pDSState[1]);//read & write

		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		device->CreateDepthStencilState(&dsDesc, &pDSState[2]);//read

		dsDesc.DepthEnable = false;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		device->CreateDepthStencilState(&dsDesc, &pDSState[3]);//write
	}

	void Depth(depthmode mode)
	{
		context->OMSetDepthStencilState(pDSState[(int)mode], 1);
	}


}

ID2D1Factory* d2dFactory = nullptr;
ID2D1HwndRenderTarget* d2dRenderTarget = nullptr;

void InitD2D(HWND hwnd)
{
	// Шаг 1: Создание фабрики
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory);

	// Шаг 2: Получаем размеры окна
	RECT rc;
	GetClientRect(hwnd, &rc);

	// Шаг 3: Описание рендер-таргета
	D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
	D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps =
		D2D1::HwndRenderTargetProperties(hwnd,
			D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top));

	// Шаг 4: Создаём render target
	d2dFactory->CreateHwndRenderTarget(rtProps, hwndProps, &d2dRenderTarget);
}



namespace Device
{

#define DirectXDebugMode false

	D3D_DRIVER_TYPE	driverType = D3D_DRIVER_TYPE_NULL;

	void Init()
	{
		HRESULT hr;

		aspect = float(height) / float(width);
		iaspect = float(width) / float(height);

		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 2;
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = FRAMES_PER_SECOND;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = true;

		hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, DirectXDebugMode ? D3D11_CREATE_DEVICE_DEBUG : 0, 0, 0, D3D11_SDK_VERSION, &sd, &swapChain, &device, NULL, &context);

		Textures::Texture[0].pTexture = NULL;
		hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&Textures::Texture[0].pTexture);

		hr = device->CreateRenderTargetView(Textures::Texture[0].pTexture, NULL, &Textures::Texture[0].RenderTargetView[0][0]);

		Textures::Texture[0].pTexture->Release();
	}


}


namespace InputAssembler
{

	enum class topology { triList, lineList, lineStrip };

	void IA(topology topoType)
	{
		D3D11_PRIMITIVE_TOPOLOGY ttype = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		switch (topoType)
		{
		case topology::triList:
			ttype = D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			break;
		case topology::lineList:
			ttype = D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST;
			break;
		case topology::lineStrip:
			ttype = D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP;
		}

		context->IASetPrimitiveTopology(ttype);
		context->IASetInputLayout(NULL);
		context->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	}

}

void Dx11Init()
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
	aspect = float(height) / float(width);
	iaspect = float(width) / float(height);

	Device::Init();
	Rasterizer::Init();
	Depth::Init();
	Blend::Init();
	ConstBuf::Init();
	Sampler::Init();
	Shaders::Init();
	
	//main RT
	Textures::Create(0, Textures::tType::flat, Textures::tFormat::u8, XMFLOAT2(width, height), false, true);
	//rt1
	Textures::Create(1, Textures::tType::flat, Textures::tFormat::u8, XMFLOAT2(width, height), false, true);
	//rt2
	Textures::Create(2, Textures::tType::flat, Textures::tFormat::u8, XMFLOAT2(width, height), false, true);
}


struct color4 {
	float r;
	float g;
	float b;
	float a;
};

namespace Draw
{

	void Clear(color4 color)
	{
		context->ClearRenderTargetView(Textures::Texture[Textures::currentRT].RenderTargetView[0][0], XMVECTORF32{ color.r,color.g,color.b,color.a });
	}

	void ClearDepth()
	{
		context->ClearDepthStencilView(Textures::Texture[Textures::currentRT].DepthStencilView[0], D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void NullDrawer(int quadCount, unsigned int instances = 1)
	{
		ConstBuf::Update(0, ConstBuf::drawerV);
		ConstBuf::ConstToVertex(0);
		ConstBuf::Update(1, ConstBuf::drawerP);
		ConstBuf::ConstToPixel(1);

		context->DrawInstanced(quadCount*50 , instances, 0, 0);
	}

	void Drawer(int quadCount)
	{
		ConstBuf::Update(0, ConstBuf::drawerV);
		ConstBuf::ConstToVertex(0);
		ConstBuf::Update(1, ConstBuf::drawerP);
		ConstBuf::ConstToPixel(1);

		context->Draw(quadCount * 6, 0);
	}

	void SwitchRenderTextures() {
		int index = 3 - Textures::currentRT;
		Textures::RenderTarget(index, 0);
		context->PSSetShaderResources(0, 1, &Textures::Texture[3 - index].TextureResView);
	}

	void OutputRenderTextures(){
		int index = Textures::currentRT;
		Textures::RenderTarget(0, 0);
		context->PSSetShaderResources(0, 1, &Textures::Texture[index].TextureResView);
	}

	void elipse(int quadCount, unsigned int instances = 1)
	{
		ConstBuf::Update(0, ConstBuf::drawerV);
		ConstBuf::ConstToVertex(0);
		ConstBuf::Update(1, ConstBuf::drawerP);
		ConstBuf::ConstToPixel(1);

		context->DrawInstanced(quadCount * 6 , instances, 0, 0);
	}

	void Starfield(int quadCount, unsigned int instances = 1)
	{
		ConstBuf::Update(0, ConstBuf::drawerV);
		ConstBuf::ConstToVertex(0);
		ConstBuf::Update(1, ConstBuf::drawerP);
		ConstBuf::ConstToPixel(1);

		context->DrawInstanced(quadCount * 60000, instances, 0, 0);
	}

	void GalaxyFog(int quadCount)
	{
		ConstBuf::Update(0, ConstBuf::drawerV);
		ConstBuf::ConstToVertex(0);
		ConstBuf::Update(1, ConstBuf::drawerP);
		ConstBuf::ConstToPixel(1);

		context->Draw(quadCount * 6, 0);
	}

	void Cursor()
	{
		ConstBuf::Update(5, ConstBuf::global);
		ConstBuf::ConstToVertex(5);
		ConstBuf::Update(1, ConstBuf::drawerP);
		ConstBuf::ConstToPixel(1);

		context->Draw(6, 0);
	}

	void Present()
	{
		Textures::UnbindAll();
		swapChain->Present(1, 0);
	}

}

void frameConst()
{
	ConstBuf::frame.time = XMFLOAT4{ (float)(timer::frameBeginTime * .01) ,0,0,0 };
	ConstBuf::frame.aspect = XMFLOAT4{ aspect,iaspect, float(width), float(height)};
	ConstBuf::UpdateFrame();
}

#define PI 3.1415926535897932384626433832795f
float DegreesToRadians(float degrees)
{
	return degrees * PI / 180.f;
}
namespace Hero
{

	struct State
	{
		
		XMVECTOR relativeMovement = XMVectorSet(-1, 0, 0, 0);
		XMVECTOR currentRotation = XMQuaternionIdentity();
		XMVECTOR Forward = XMVectorSet(0, 0, 1, 0);
		XMVECTOR Forwardbuf = XMVectorSet(0, 0, 1, 0);
		XMVECTOR defaultUp = XMVectorSet(0, -1, 0, 0);
		XMVECTOR Right = XMVectorSet(-1, 0, 0, 0);
		XMVECTOR at = XMVectorSet(0, 0, 0, 0);
		XMVECTOR Up = XMVector3Rotate(defaultUp, currentRotation);
		XMVECTOR heroPosition;
		XMMATRIX constellationOffset = XMMatrixTranslation(0, 0, 0);

		
		
	} static state;

	
	

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
		float verticalOffset = 500.0f;  
		float horizontalOffset = 0.0f; 
		float distanceOffset = 500.0f;
		XMVECTOR relativeMovement = XMVectorSet(-1, 0, 0, 0);
		XMVECTOR currentRotation = XMQuaternionIdentity();
		XMVECTOR Forward = XMVectorSet(0, 0, 1, 0);
		//XMVECTOR Forwardbuf = XMVectorSet(0, 0, 1, 0);//аналог форварда локальный тут
		XMVECTOR defaultUp = XMVectorSet(0, -1, 0, 0);
		XMVECTOR Right = XMVectorSet(-1, 0, 0, 0);
		XMVECTOR at = XMVectorSet(0, 0, 0, 0);
		XMVECTOR Up = XMVector3Rotate(defaultUp, currentRotation);
		XMVECTOR Eye;
		XMMATRIX constellationOffset = XMMatrixTranslation(0, 0, 0);
		XMVECTOR EyeBack = XMVectorSet(0, 0, -1, 0);
	} static state;

	
	void UpdateCameraPosition()
	{
		// Получаем текущую позицию героя
		Hero::state.heroPosition = XMVectorSet(
			Camera::state.constellationOffset.r[3].m128_f32[0],
			Camera::state.constellationOffset.r[3].m128_f32[1],
			Camera::state.constellationOffset.r[3].m128_f32[2],
			0.0f
		);

		
		XMVECTOR heroForward = Hero::state.Forwardbuf;
		XMVECTOR heroUp = Hero::state.Up;
		XMVECTOR heroRight = Hero::state.Right;

		
		XMVECTOR cameraPosition = Hero::state.heroPosition
			- (heroForward * state.distanceOffset)
			+ (heroUp * state.verticalOffset)
			+ (heroRight * state.horizontalOffset);

		
		XMVECTOR cameraTarget = Hero::state.heroPosition + (heroUp * state.verticalOffset * 0.5f);

		
		state.Eye = cameraPosition;
		state.at = cameraTarget;
		state.Up = heroUp; 

		
		state.Forward = XMVector3Normalize(cameraTarget - cameraPosition);
		state.Right = XMVector3Normalize(XMVector3Cross(heroUp, state.Forward));
	}

	void Camera()//обновление позиции камеры после обновления позиции созвездия. В Navigation.h, Loop.h, Mouse.h(navigationByMouse 2 раза) и тут тоже есть вызовы.
	{
		UpdateCameraPosition();

		
		ConstBuf::camera.view[0] = XMMatrixTranspose(XMMatrixLookAtLH(
			state.Eye,
			state.at,
			state.Up
		));

		
		ConstBuf::camera.proj[0] = XMMatrixTranspose(XMMatrixPerspectiveFovLH(
			DegreesToRadians(state.fovAngle),
			iaspect,
			0.01f,
			10000.0f
		));

		ConstBuf::UpdateCamera();
		ConstBuf::ConstToVertex(3);
		ConstBuf::ConstToPixel(3);
		
	}
	
	void HandleMouseWheel(int delta)
	{
		state.distanceOffset -= delta * 5.0f;
		state.distanceOffset = clamp(state.distanceOffset, state.minDist, state.maxDist);
	}
}
