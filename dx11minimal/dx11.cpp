#include "dx11.h"

static inline int32 _log2(float x)
{
	uint32 ix = (uint32&)x;
	uint32 exp = (ix >> 23) & 0xFF;
	int32 log2 = int32(exp) - 127;

	return log2;
}

//////////////////////////////////////////////////////////////////////////////////

ID3D11Device* device = NULL;
ID3D11DeviceContext* context = NULL;
IDXGISwapChain* swapChain = NULL;

//////////////////////////////////////////////////////////////////////////////////

ID3D11RasterizerState* Rasterizer::rasterState[4];

void Rasterizer::Cull(cullmode mode)
{
	context->RSSetState(rasterState[(int)mode]);
}

void Rasterizer::Scissors(rect r)
{
	D3D11_RECT rect = { r.x,r.y,r.z,r.w };
	context->RSSetScissorRects(1, &rect);
}

void Rasterizer::Init(int width, int height)
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
	Rasterizer::Scissors(rect{ 0, 0, width, height });
}

//////////////////////////////////////////////////////////////////////////////////

DXGI_FORMAT Textures::dxTFormat[5] = { DXGI_FORMAT_R8G8B8A8_UNORM ,DXGI_FORMAT_R8G8B8A8_SNORM ,DXGI_FORMAT_R16G16B16A16_FLOAT ,DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R8_SNORM };

D3D11_TEXTURE2D_DESC Textures::tdesc;
D3D11_SHADER_RESOURCE_VIEW_DESC Textures::svDesc;
D3D11_RENDER_TARGET_VIEW_DESC Textures::renderTargetViewDesc;
D3D11_DEPTH_STENCIL_VIEW_DESC Textures::descDSV;

ID3D11RenderTargetView* Textures::mrtView[8];

Textures::textureDesc Textures::Texture[max_tex];

int Textures::currentRT = 0;

void Textures::CreateTex(int i)
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

void Textures::ShaderRes(int i)
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

void Textures::rtView(int i)
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

void Textures::Depth(int i)
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

void Textures::shaderResDepth(int i)
{
	svDesc.Format = DXGI_FORMAT_R32_FLOAT;
	svDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	svDesc.Texture2D.MostDetailedMip = 0;
	svDesc.Texture2D.MipLevels = 1;

	HRESULT hr = device->CreateShaderResourceView(Texture[i].pDepth, &svDesc, &Texture[i].DepthResView);
}

void Textures::Create(int i, tType type, tFormat format, XMFLOAT2 size, bool mipMaps, bool depth)
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
		Textures::CreateTex(i);
		Textures::ShaderRes(i);
		Textures::rtView(i);
	}

	if (depth)
	{
		Textures::Depth(i);
		Textures::shaderResDepth(i);
	}
}

void Textures::UnbindAll()
{
	ID3D11ShaderResourceView* const null[128] = { NULL };
	context->VSSetShaderResources(0, 128, null);
	context->PSSetShaderResources(0, 128, null);
}

void Textures::SetViewport(int texId, byte level = 0)
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

void Textures::CopyColor(int dst, int src)
{
	context->CopyResource(Texture[(int)dst].pTexture, Texture[(int)src].pTexture);
}

void Textures::CopyDepth(int dst, int src)
{
	context->CopyResource(Texture[(int)dst].pDepth, Texture[(int)src].pDepth);
}

void Textures::TextureToShader(int tex, unsigned int slot, targetshader tA)
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

void Textures::CreateMipMap()
{
	context->GenerateMips(Texture[currentRT].TextureResView);
}

void Textures::RenderTarget(int target, unsigned int level = 0)
{
	currentRT = target;

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

//////////////////////////////////////////////////////////////////////////////////

Shaders::VertexShader Shaders::VS[255];
Shaders::PixelShader Shaders::PS[255];
Shaders::GeometryShader Shaders::GS[255];

ID3DBlob* Shaders::pErrorBlob;
wchar_t Shaders::shaderPathW[MAX_PATH];

LPCWSTR Shaders::nameToPatchLPCWSTR(const char* path)
{
	int len = MultiByteToWideChar(CP_ACP, 0, path, -1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, path, -1, shaderPathW, len);

	return shaderPathW;
}

void Shaders::Log(const char* message)
{
	OutputDebugStringA(message);
}

void Shaders::CompilerLog(LPCWSTR source, HRESULT hr, const char* message)
{
	if (FAILED(hr))
	{
		Shaders::Log((char*)pErrorBlob->GetBufferPointer());
	}
	else
	{
		char shaderName[1024];
		WideCharToMultiByte(CP_ACP, NULL, source, -1, shaderName, sizeof(shaderName), NULL, NULL);

		Shaders::Log(message);
		Shaders::Log((char*)shaderName);
		Shaders::Log("\n");
	}
}

void Shaders::CreateVS(int i, LPCWSTR name)
{
	HRESULT hr;

	hr = D3DCompileFromFile(name, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_4_1", NULL, NULL, &VS[i].pBlob, &pErrorBlob);
	Shaders::CompilerLog(name, hr, "vertex shader compiled: ");

	if (hr == S_OK)
	{
		hr = device->CreateVertexShader(VS[i].pBlob->GetBufferPointer(), VS[i].pBlob->GetBufferSize(), NULL, &VS[i].vShader);
	}
}

void Shaders::CreatePS(int i, LPCWSTR name)
{
	HRESULT hr;

	hr = D3DCompileFromFile(name, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_4_1", NULL, NULL, &PS[i].pBlob, &pErrorBlob);
	Shaders::CompilerLog(name, hr, "pixel shader compiled: ");

	if (hr == S_OK)
	{
		hr = device->CreatePixelShader(PS[i].pBlob->GetBufferPointer(), PS[i].pBlob->GetBufferSize(), NULL, &PS[i].pShader);
	}
}

void Shaders::CreateGS(int i, LPCWSTR name)
{
	HRESULT hr;

	hr = D3DCompileFromFile(name, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "GS", "gs_4_1", NULL, NULL, &GS[i].pBlob, &pErrorBlob);
	Shaders::CompilerLog(name, hr, "geometry shader compiled: ");

	if (hr == S_OK)
	{
		hr = device->CreateGeometryShader(GS[i].pBlob->GetBufferPointer(), GS[i].pBlob->GetBufferSize(), NULL, &GS[i].gShader);
	}
}

void Shaders::Init()
{
	Shaders::CreateVS(0, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\VS.shader"));
	Shaders::CreatePS(0, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\PS.shader"));
	//Shaders::CreateGS(0, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\Static_GS.shader"));
	
	Shaders::CreateVS(1, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\VSS.shader"));
	Shaders::CreatePS(1, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\PSS.shader"));
	
	Shaders::CreateVS(2, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\VSFS.shader"));
	Shaders::CreatePS(2, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\PSFS.shader"));
	
	Shaders::CreateVS(3, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\SpaceStars_VS.shader"));
	Shaders::CreatePS(3, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\SpaceStars_PS.shader"));
	
	Shaders::CreateVS(4, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\StarLink_VS.shader"));
	Shaders::CreatePS(4, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\StarLink_PS.shader"));
	
	Shaders::CreateVS(5, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\GalaxyFog_VS.shader"));
	Shaders::CreatePS(5, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\GalaxyFog_PS.shader"));
	
	Shaders::CreateVS(6, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\Cursor_VS.shader"));
	Shaders::CreatePS(6, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\Cursor_PS.shader"));
	
	Shaders::CreateVS(7, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\AriesNebula_VS.shader"));
	Shaders::CreatePS(7, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\AriesNebula_PS.shader"));
	Shaders::CreateGS(7, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\AriesNebula_GS.shader"));
	
	Shaders::CreateVS(8, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\SpeedParticles_VS.shader"));
	Shaders::CreatePS(8, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\SpeedParticles_PS.shader"));
	
	Shaders::CreateVS(9, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\DotText_VS.shader"));
	Shaders::CreatePS(9, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\DotText_PS.shader"));
	
	Shaders::CreateVS(10, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\PP_VS.shader"));
	Shaders::CreatePS(10, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\PP_PS.shader"));
	
	Shaders::CreateVS(11, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\Cross_VS.shader"));
	Shaders::CreatePS(11, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\Cross_PS.shader"));
	
	Shaders::CreateVS(12, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\UiParticle_VS.shader"));
	Shaders::CreatePS(12, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\UiParticle_PS.shader"));
	
	Shaders::CreateVS(13, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\Rect_VS.shader"));
	Shaders::CreatePS(13, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\Rect_Strict_PS.shader"));

	Shaders::CreatePS(14, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\Rect_Smooth_PS.shader"));
	//Shaders::CreatePS(14, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\BlackHole_Body_PS.shader"));
	
	//-----------------------------------------------
	
	Shaders::CreatePS(100, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\ColorCorrection_PS.shader"));
	Shaders::CreatePS(101, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\Lensing_PS.shader"));
	
	//-----------------------------------------------
	
	Shaders::CreatePS(200, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\PerlinNoise.shader"));
	Shaders::CreatePS(201, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\VoronoiNoise.shader"));
}

void Shaders::vShader(unsigned int n)
{
	context->VSSetShader(VS[n].vShader, NULL, 0);
}

void Shaders::pShader(unsigned int n)
{
	context->PSSetShader(PS[n].pShader, NULL, 0);
}

void Shaders::gShader(unsigned int n)
{
	context->GSSetShader(GS[n].gShader, NULL, 0);
}

//////////////////////////////////////////////////////////////////////////////////

ID3D11SamplerState* Sampler::pSampler[3][2][2];
ID3D11SamplerState* Sampler::pSamplerComp;

void Sampler::Init()
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

void Sampler::Sampler(targetshader shader, unsigned int slot, filter filterType, addr addressU, addr addressV)
{
	if (shader == targetshader::vertex) context->VSSetSamplers(slot, 1, &pSampler[(int)filterType][(int)addressU][(int)addressV]);
	if (shader == targetshader::pixel) context->PSSetSamplers(slot, 1, &pSampler[(int)filterType][(int)addressU][(int)addressV]);
}

void Sampler::SamplerComp(unsigned int slot)
{
	context->PSSetSamplers(slot, 1, &pSamplerComp);
}

//////////////////////////////////////////////////////////////////////////////////

ID3D11Buffer* ConstBuf::buffer[8];

//b0
float ConstBuf::drawerV[constCount];

//b1
float ConstBuf::drawerP[constCount];

//b2
ConstBuf::DrawerMat ConstBuf::drawerMat;

//b3 
ConstBuf::Camera ConstBuf::camera;

//b4
ConstBuf::Frame ConstBuf::frame;

//b5
XMFLOAT4 ConstBuf::global[constCount];

//b6
ConstBuf::Factors ConstBuf::factors;

//b7
int ConstBuf::drawerInt[constCount];


int ConstBuf::roundUp(int n, int r)
{
	return 	n - (n % r) + r;
}

void ConstBuf::Create(ID3D11Buffer*& buf, int size)
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

void ConstBuf::Init()
{
	ConstBuf::Create(ConstBuf::buffer[0], sizeof(drawerV));
	ConstBuf::Create(ConstBuf::buffer[1], sizeof(drawerP));
	ConstBuf::Create(ConstBuf::buffer[2], sizeof(drawerMat));
	ConstBuf::Create(ConstBuf::buffer[3], sizeof(camera));
	ConstBuf::Create(ConstBuf::buffer[4], sizeof(frame));
	ConstBuf::Create(ConstBuf::buffer[5], sizeof(global));
	ConstBuf::Create(ConstBuf::buffer[6], sizeof(factors));
	ConstBuf::Create(ConstBuf::buffer[7], sizeof(drawerInt));
}

void ConstBuf::UpdateFrame()
{
	context->UpdateSubresource(ConstBuf::buffer[4], 0, NULL, &frame, 0, 0);
}

void ConstBuf::UpdateDrawerMat()
{
	context->UpdateSubresource(ConstBuf::buffer[2], 0, NULL, &drawerMat, 0, 0);
}

void ConstBuf::UpdateCamera()
{
	context->UpdateSubresource(ConstBuf::buffer[3], 0, NULL, &camera, 0, 0);
}

void ConstBuf::UpdateFactors()
{
	context->UpdateSubresource(ConstBuf::buffer[6], 0, NULL, &factors, 0, 0);
}

void ConstBuf::ConstToVertex(int i)
{
	context->VSSetConstantBuffers(i, 1, &ConstBuf::buffer[i]);
}

void ConstBuf::ConstToPixel(int i)
{
	context->PSSetConstantBuffers(i, 1, &ConstBuf::buffer[i]);
}

//////////////////////////////////////////////////////////////////////////////////

ID3D11BlendState* Blend::blendState[3][5];
D3D11_BLEND_DESC Blend::bSDesc;

void Blend::CreateMixStates(int j)
{
	for (int i = 0; i < 5; i++)
	{
		bSDesc.RenderTarget[0].BlendOp = (D3D11_BLEND_OP)(i + 1);
		HRESULT hr = device->CreateBlendState(&bSDesc, &blendState[j][i]);
	}
}

void Blend::Init()
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

	Blend::CreateMixStates(0);

	bSDesc.RenderTarget[0].BlendEnable = TRUE;
	bSDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	bSDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	bSDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bSDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bSDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	//NO ALPHA
	Blend::CreateMixStates(1);

	//ALPHA 
	bSDesc.RenderTarget[0].BlendEnable = TRUE;
	bSDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bSDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	Blend::CreateMixStates(2);

	float blendFactor[4] = { .0f,.0f,.0f,.0f };
	context->OMSetBlendState(blendState[0][0], blendFactor, 0xffffffff);
}

void Blend::Blending(blendmode mode = blendmode::off, blendop operation = blendop::add)
{
	float blendFactor[4] = { .0f,.0f,.0f,.0f };
	context->OMSetBlendState(blendState[(int)mode][(int)operation], blendFactor, 0xffffffff);
}

//////////////////////////////////////////////////////////////////////////////////

ID3D11DepthStencilState* Depth::pDSState[4];

void Depth::Init()
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

void Depth::Depth(depthmode mode)
{
	context->OMSetDepthStencilState(pDSState[(int)mode], 1);
}

//////////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////////

D3D_DRIVER_TYPE	Device::driverType = D3D_DRIVER_TYPE_NULL;

void Device::Init(HWND hwnd, int width, int height)
{
	HRESULT hr;

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
	sd.OutputWindow = hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;

	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, DirectXDebugMode ? D3D11_CREATE_DEVICE_DEBUG : 0, 0, 0, D3D11_SDK_VERSION, &sd, &swapChain, &device, NULL, &context);

	Textures::Texture[0].pTexture = NULL;
	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&Textures::Texture[0].pTexture);

	hr = device->CreateRenderTargetView(Textures::Texture[0].pTexture, NULL, &Textures::Texture[0].RenderTargetView[0][0]);

	Textures::Texture[0].pTexture->Release();
}

//////////////////////////////////////////////////////////////////////////////////

void InputAssembler::IA(topology topoType)
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

//////////////////////////////////////////////////////////////////////////////////

void Dx11Init(HWND hwnd, int width, int height)
{
	Device::Init(hwnd, width, height);
	Rasterizer::Init(width, height);
	Depth::Init();
	Blend::Init();
	ConstBuf::Init();
	Sampler::Init();
	Shaders::Init();

	// main RT
	Textures::Create(0, Textures::tType::flat, Textures::tFormat::u8, XMFLOAT2(width, height), false, true);
	// rt1
	Textures::Create(1, Textures::tType::flat, Textures::tFormat::s16, XMFLOAT2(width, height), true, true);
	// rt2
	Textures::Create(2, Textures::tType::flat, Textures::tFormat::s16, XMFLOAT2(width, height), true, true);

	// perlin noise rt
	Textures::Create(3, Textures::tType::flat, Textures::tFormat::r8, XMFLOAT2(256, 256), true, false);
	// voronoi noise rt
	Textures::Create(4, Textures::tType::flat, Textures::tFormat::s16, XMFLOAT2(1024, 1024), true, false);
}

//////////////////////////////////////////////////////////////////////////////////

void Draw::Clear(color4 color)
{
	context->ClearRenderTargetView(Textures::Texture[Textures::currentRT].RenderTargetView[0][0], XMVECTORF32{ color.r,color.g,color.b,color.a });
}

void Draw::ClearDepth()
{
	context->ClearDepthStencilView(Textures::Texture[Textures::currentRT].DepthStencilView[0], D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Draw::NullDrawer(int quadCount, unsigned int instances = 1)
{
	ConstBuf::Update(0, ConstBuf::drawerV);
	ConstBuf::ConstToVertex(0);
	ConstBuf::Update(1, ConstBuf::drawerP);
	ConstBuf::ConstToPixel(1);

	context->DrawInstanced(quadCount * 50, instances, 0, 0);
}

void Draw::Drawer(int quadCount)
{
	ConstBuf::Update(0, ConstBuf::drawerV);
	ConstBuf::ConstToVertex(0);
	ConstBuf::Update(1, ConstBuf::drawerP);
	ConstBuf::ConstToPixel(1);

	context->Draw(quadCount * 6, 0);
}

void Draw::SwitchRenderTextures() {
	int index = 3 - Textures::currentRT;
	Textures::RenderTarget(index, 0);
	context->PSSetShaderResources(0, 1, &Textures::Texture[3 - index].TextureResView);
}

void Draw::OutputRenderTextures() {
	int index = Textures::currentRT;
	Textures::RenderTarget(0, 0);
	context->PSSetShaderResources(0, 1, &Textures::Texture[index].TextureResView);
}

void Draw::elipse(int quadCount, unsigned int instances = 1)//
{
	ConstBuf::Update(0, ConstBuf::drawerV);
	ConstBuf::ConstToVertex(0);
	ConstBuf::Update(1, ConstBuf::drawerP);
	ConstBuf::ConstToPixel(1);

	context->DrawInstanced(quadCount * 6, instances, 0, 0);
}

void Draw::Present()
{
	Textures::UnbindAll();
	swapChain->Present(1, 0);
}