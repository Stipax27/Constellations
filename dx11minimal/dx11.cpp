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
int Textures::texturesCount = 0;

void Textures::CreateTex(int i, bool uav)
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

	if (uav)
	{
		tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		tdesc.MiscFlags = 0;
	}

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

void Textures::Create(int i, tType type, tFormat format, XMFLOAT2 size, bool mipMaps, bool depth, bool uav)
{
	texturesCount = max(i, texturesCount + 1);

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
		Textures::CreateTex(i, uav);
		Textures::ShaderRes(i);

		if (!uav) {
			Textures::rtView(i);
		}
		else {
			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
			ZeroMemory(&uavDesc, sizeof(uavDesc));
			uavDesc.Format = dxTFormat[format];
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
			uavDesc.Texture2D.MipSlice = 0;

			HRESULT hr = device->CreateUnorderedAccessView(Texture[i].pTexture, &uavDesc, &Texture[i].UnorderedAccessView);
		}
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

void Textures::DepthTarget(int depthTarget, int depthMipLevel = 0)
{
	auto depthStencil = Texture[(int)depthTarget].depth ? Texture[(int)depthTarget].DepthStencilView[depthMipLevel] : 0;
	context->OMSetRenderTargets(1, &Texture[currentRT].RenderTargetView[depthMipLevel][0], depthStencil);

	SetViewport(currentRT, 0);
}

void Textures::LoadTexture(const char* filename)
{
	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;
	unsigned char* targaData;
	unsigned int rowPitch;

	// Open the targa file for reading in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		return;
	}

	// Read in the file header.
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return;
	}

	// Get the important information from the header.
	int m_height = (int)targaFileHeader.height;
	int m_width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	// Check that it is 32 bit and not 24 bit.
	if (bpp != 32)
	{
		return;
	}

	// Calculate the size of the 32 bit image data.
	imageSize = m_width * m_height * 4;

	// Allocate memory for the targa image data.
	targaImage = new unsigned char[imageSize];

	// Read in the targa image data.
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return;
	}

	// Allocate memory for the targa destination data.
	targaData = new unsigned char[imageSize];

	// Initialize the index into the targa destination data array.
	index = 0;

	// Initialize the index into the targa image data.
	k = (m_width * m_height * 4) - (m_width * 4);

	// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down and also is not in RGBA order.
	for (j = 0; j < m_height; j++)
	{
		for (i = 0; i < m_width; i++)
		{
			targaData[index + 0] = targaImage[k + 2];  // Red.
			targaData[index + 1] = targaImage[k + 1];  // Green.
			targaData[index + 2] = targaImage[k + 0];  // Blue
			targaData[index + 3] = targaImage[k + 3];  // Alpha

			// Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}

		// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
		k -= (m_width * 8);
	}

	int textureId = texturesCount;
	Create(textureId, tType::flat, tFormat::u8, XMFLOAT2(targaFileHeader.width, targaFileHeader.height), true, false);

	// Set the row pitch of the targa image data.
	rowPitch = (m_width * 4) * sizeof(unsigned char);

	// Copy the targa image data into the texture.
	context->UpdateSubresource(Texture[textureId].pTexture, 0, NULL, targaData, rowPitch, 0);

	// Generate mipmaps for this texture.
	context->GenerateMips(Texture[textureId].TextureResView);

	// Release the targa image data now that it was copied into the destination array.
	delete[] targaImage;
	targaImage = 0;

	// Release the targa image data now that the image data has been loaded into the texture.
	delete[] targaData;
	targaData = 0;
}

//////////////////////////////////////////////////////////////////////////////////

D3D11_BUFFER_DESC Models::vertexBufferDesc, Models::indexBufferDesc;
D3D11_SUBRESOURCE_DATA Models::vertexData, Models::indexData;

Models::modelDesc Models::Model[max_models];
int Models::modelsCount = 0;

int Models::vertexCount;
int Models::indexCount;

uint64_t Models::GetFileModTime(const char* filename) {
	struct _stat64 fileStat;
	if (_stat64(filename, &fileStat) == 0) {
		return fileStat.st_mtime;
	}
	return 0;
}

void Models::CreateModel(int i, VertexType* vertices, unsigned long* indices)
{
	HRESULT result;

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &Model[i].vertexBuffer);
	if (FAILED(result))
	{
		Shaders::Log("Failed to create vertex buffer for the model\n");
		return;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &Model[i].indexBuffer);
	if (FAILED(result))
	{
		Shaders::Log("Failed to create index buffer for the model\n");
		return;
	}
}

void Models::Create(int i, VertexType* vertices, unsigned long* indices)
{
	modelsCount = max(i, modelsCount + 1);

	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	ZeroMemory(&vertexData, sizeof(vertexData));
	ZeroMemory(&indexData, sizeof(indexData));

	Model[i].indexes = indexCount;

	CreateModel(i, vertices, indices);
}

void Models::LoadTxtModel(const char* filename, bool vertexOnly)
{
	std::ifstream fin;
	char input;

	fin.open(filename);

	if (fin.fail())
	{
		Shaders::Log("Failed to read the txt model file\n");
		return;
	}

	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	fin >> vertexCount;
	indexCount = vertexCount;

	ModelType* model = new ModelType[vertexCount];

	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for (int i = 0; i < vertexCount; i++)
	{
		fin >> model[i].x >> model[i].y >> model[i].z;
		if (!vertexOnly) {
			fin >> model[i].tu >> model[i].tv;
			fin >> model[i].nx >> model[i].ny >> model[i].nz;
		}
	}

	fin.close();

	VertexType* vertices;
	unsigned long* indices;

	indexCount = vertexCount;

	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];

	// Load the vertex array and index array with data.
	for (int i = 0; i < vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(model[i].x, model[i].y, model[i].z);
		vertices[i].texture = XMFLOAT2(model[i].tu, model[i].tv);
		vertices[i].normal = XMFLOAT3(model[i].nx, model[i].ny, model[i].nz);

		indices[i] = i;
	}

	Create(modelsCount, vertices, indices);

	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	delete[] model;
	model = 0;

	Shaders::Log("Model txt file was read succesfully\n");
}

void Models::LoadGltfModel(const char* filename)
{
	std::ifstream fin;
	char input;

	int openedBrackets = 1;

	fin.open(filename);

	if (fin.fail())
	{
		Shaders::Log("Failed to read the gltf model file\n");
		return;
	}

	fin.get(input);
	while (openedBrackets > 0)
	{
		fin.get(input);

		if (input == '{') {
			openedBrackets++;
		}
		else if (input == '}') {
			openedBrackets--;
		}
		else {

			if (input == '"') {

			}

		}

		Shaders::Log(&input);
	}

	fin.close();

	Shaders::Log("Model glTF file was read succesfully\n");
}

void Models::LoadObjModel(const char* filename, bool vertexOnly)
{
	// Checking if cash exists
	std::string cacheFilename = std::string(filename) + ".cache";
	uint64_t sourceTime = GetFileModTime(filename);

	// Trying to load from cash
	std::ifstream cacheFile(cacheFilename, std::ios::binary);
	if (cacheFile && sourceTime != 0) {
		CacheHeader header;
		cacheFile.read((char*)&header, sizeof(header));

		// Checking cash valide
		if (header.version == 1 &&
			header.sourceFileTime == sourceTime &&
			header.vertexStride == sizeof(VertexType)) {

			// Loading vericles
			vertexCount = header.vertexCount;
			indexCount = header.indexCount;

			VertexType* vertices = new VertexType[vertexCount];
			unsigned long* idxArray = new unsigned long[indexCount];

			cacheFile.read((char*)vertices, vertexCount * sizeof(VertexType));
			cacheFile.read((char*)idxArray, indexCount * sizeof(unsigned long));

			// Creating DirectX buffers
			Create(modelsCount, vertices, idxArray);

			delete[] vertices;
			delete[] idxArray;

			cacheFile.close();

			Shaders::Log("Model loaded from cache: ");
			Shaders::Log(filename);
			Shaders::Log("\n");

			return;
		}
		cacheFile.close();
	}

	// If cash doesn't exist or expired then reading .obj file
	ifstream fin(filename);
	if (!fin) {
		Shaders::Log("Failed to read the obj model file\n");
		return;
	}

	std::vector<XMFLOAT3> positions;   // v
	std::vector<XMFLOAT2> texcoords;   // vt
	std::vector<XMFLOAT3> normals;     // vn

	string prefix;
	while (fin >> prefix) {
		if (prefix == "v") {
			XMFLOAT3 p;
			fin >> p.x >> p.y >> p.z;
			positions.push_back(p);
		}
		else if (!vertexOnly && prefix == "vt") {
			XMFLOAT2 t;
			fin >> t.x >> t.y;
			texcoords.push_back(t);
		}
		else if (!vertexOnly && prefix == "vn") {
			XMFLOAT3 n;
			fin >> n.x >> n.y >> n.z;
			normals.push_back(n);
		}
		else if (prefix == "f") {
			break;
		}
	}

	std::vector<VertexType> uniqueVertices;   // final vertex buffer data
	std::vector<unsigned long> indices;       // final index buffer
	unordered_map<tuple<int, int, int>, unsigned int, VertexKeyHash> vertexMap;

	// Helper to convert OBJ index (1‑based, may be negative) to 0‑based vector index
	auto resolveIndex = [](int idx, size_t size) -> int {
		if (idx > 0) return idx - 1;
		if (idx < 0) return (int)size + idx;   // negative means relative to end
		return -1;                             // missing index (0 or empty)
		};

	string line;
	do {
		if (prefix != "f") {
			// Not a face line – read next line and continue
			getline(fin, line);
			fin >> prefix;
			continue;
		}

		// Read the whole face line
		getline(fin, line);
		istringstream iss(line);
		string vertexSpec;

		while (iss >> vertexSpec) {
			// Split vertex specification (e.g., "1/2/3" or "1//2")
			std::vector<string> parts = split(vertexSpec, "/");
			// parts[0] = v, parts[1] = vt, parts[2] = vn  (some may be empty)

			int vIdx = -1, vtIdx = -1, vnIdx = -1;

			// Position index (always present in valid OBJ)
			if (!parts.empty() && !parts[0].empty())
				vIdx = stoi(parts[0]);

			// Texture coordinate index
			if (parts.size() > 1 && !parts[1].empty())
				vtIdx = stoi(parts[1]);

			// Normal index
			if (parts.size() > 2 && !parts[2].empty())
				vnIdx = stoi(parts[2]);

			// Convert to 0‑based and clamp to valid range
			int posIndex = resolveIndex(vIdx, positions.size());
			if (posIndex < 0 || posIndex >= (int)positions.size())
				continue; // invalid position – skip this vertex

			int texIndex = -1;
			if (!vertexOnly && vtIdx != -1) {
				texIndex = resolveIndex(vtIdx, texcoords.size());
				if (texIndex < 0 || texIndex >= (int)texcoords.size())
					texIndex = -1; // invalid – treat as missing
			}

			int normIndex = -1;
			if (!vertexOnly && vnIdx != -1) {
				normIndex = resolveIndex(vnIdx, normals.size());
				if (normIndex < 0 || normIndex >= (int)normals.size())
					normIndex = -1;
			}

			// Create unique key: (position index, texcoord index, normal index)
			// Use -1 for missing attributes.
			auto key = make_tuple(posIndex, texIndex, normIndex);

			auto it = vertexMap.find(key);
			if (it == vertexMap.end()) {
				// New unique vertex – create it
				VertexType vert;

				// Position (always present)
				vert.position = positions[posIndex];

				// Texture coordinate (or default 0,0)
				if (!vertexOnly && texIndex >= 0)
					vert.texture = texcoords[texIndex];
				else
					vert.texture = XMFLOAT2(0.0f, 0.0f);

				// Normal (or default 0,0,0)
				if (!vertexOnly && normIndex >= 0)
					vert.normal = normals[normIndex];
				else
					vert.normal = XMFLOAT3(0.0f, 0.0f, 0.0f);

				unsigned int newIndex = (unsigned int)uniqueVertices.size();
				uniqueVertices.push_back(vert);
				vertexMap[key] = newIndex;
				indices.push_back(newIndex);
			}
			else {
				// Reuse existing vertex
				indices.push_back(it->second);
			}
		}

		fin >> prefix;
	} while (!fin.eof() && prefix == "f");

	fin.close();

	// Create DirectX buffers
	vertexCount = (int)uniqueVertices.size();
	indexCount = (int)indices.size();

	VertexType* vertices = new VertexType[vertexCount];
	unsigned long* idxArray = new unsigned long[indexCount];

	for (int i = 0; i < vertexCount; ++i)
		vertices[i] = uniqueVertices[i];

	for (int i = 0; i < indexCount; ++i)
		idxArray[i] = indices[i];

	Create(modelsCount, vertices, idxArray);

	// Saving cash
	std::ofstream outCache(cacheFilename, std::ios::binary);
	if (outCache) {
		CacheHeader header;
		header.sourceFileTime = sourceTime;
		header.vertexCount = vertexCount;
		header.indexCount = indexCount;
		header.vertexStride = sizeof(VertexType);
		header.version = 1;

		outCache.write((char*)&header, sizeof(header));
		outCache.write((char*)vertices, vertexCount * sizeof(VertexType));
		outCache.write((char*)idxArray, indexCount * sizeof(unsigned long));
		outCache.close();

		Shaders::Log("Model cached to: ");
		Shaders::Log(cacheFilename.c_str());
		Shaders::Log("\n");
	}

	delete[] vertices;
	delete[] idxArray;

	Shaders::Log("Model obj file was read successfully\n");
}

//////////////////////////////////////////////////////////////////////////////////

Shaders::VertexShader Shaders::VS[255];
Shaders::PixelShader Shaders::PS[255];
Shaders::GeometryShader Shaders::GS[255];
Shaders::ComputeShader Shaders::CS[255];

ID3DBlob* Shaders::pErrorBlob;
wchar_t Shaders::shaderPathW[MAX_PATH];
wchar_t Shaders::cachePathW[MAX_PATH];

int Shaders::currentVS = 0;
int Shaders::currentPS = 0;
int Shaders::currentGS = 0;
int Shaders::currentCS = 0;

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

const char* Shaders::GetBuildConfig() {
#ifdef _DEBUG
	return "_debug";
#else
	return "_release";
#endif
}

void Shaders::EnsureCacheDirectoryExists() {
	char cachePathA[MAX_PATH];
	WideCharToMultiByte(CP_ACP, 0, L"..\\dx11minimal\\Shaders\\Cash", -1, cachePathA, MAX_PATH, NULL, NULL);

	struct stat st = { 0 };
	if (stat(cachePathA, &st) == -1) {
		_mkdir(cachePathA);
		Log("Created cache directory\n");
	}
}

std::string Shaders::GetCacheFileName(const char* shaderName, const char* shaderType) {
	std::string fullPath(shaderName);
	size_t lastSlash = fullPath.find_last_of("\\/");
	std::string fileName = (lastSlash != std::string::npos) ? fullPath.substr(lastSlash + 1) : fullPath;

	size_t lastDot = fileName.find_last_of(".");
	std::string baseName = (lastDot != std::string::npos) ? fileName.substr(0, lastDot) : fileName;

	std::string cacheName = baseName + "_" + shaderType + GetBuildConfig() + ".cso";

	char cachePathA[MAX_PATH];
	WideCharToMultiByte(CP_ACP, 0, L"..\\dx11minimal\\Shaders\\Cash\\", -1, cachePathA, MAX_PATH, NULL, NULL);

	return std::string(cachePathA) + cacheName;
}

bool Shaders::LoadShaderFromCache(const char* shaderName, const char* shaderType, void** shader, ID3DBlob** blob) {
	std::string cacheFile = GetCacheFileName(shaderName, shaderType);

	// Check if the file exists
	struct stat st;
	if (stat(cacheFile.c_str(), &st) != 0) {
		return false;
	}

	// Opening file
	std::ifstream file(cacheFile, std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		return false;
	}

	// Getting file size
	size_t size = static_cast<size_t>(file.tellg());
	file.seekg(0, std::ios::beg);

	// Creating data buffer
	std::vector<char> buffer(size);
	if (!file.read(buffer.data(), size)) {
		file.close();
		return false;
	}
	file.close();

	// Creating ID3DBlob out from data
	HRESULT hr = D3DCreateBlob(size, blob);
	if (FAILED(hr)) {
		return false;
	}

	memcpy((*blob)->GetBufferPointer(), buffer.data(), size);

	// Creating shader
	if (strcmp(shaderType, "VS") == 0) {
		hr = device->CreateVertexShader((*blob)->GetBufferPointer(), (*blob)->GetBufferSize(), NULL, (ID3D11VertexShader**)shader);
	}
	else if (strcmp(shaderType, "PS") == 0) {
		hr = device->CreatePixelShader((*blob)->GetBufferPointer(), (*blob)->GetBufferSize(), NULL, (ID3D11PixelShader**)shader);
	}
	else if (strcmp(shaderType, "GS") == 0) {
		hr = device->CreateGeometryShader((*blob)->GetBufferPointer(), (*blob)->GetBufferSize(), NULL, (ID3D11GeometryShader**)shader);
	}
	else if (strcmp(shaderType, "CS") == 0) {
		hr = device->CreateComputeShader((*blob)->GetBufferPointer(), (*blob)->GetBufferSize(), NULL, (ID3D11ComputeShader**)shader);
	}

	if (FAILED(hr)) {
		(*blob)->Release();
		*blob = nullptr;
		return false;
	}

	char logMsg[256];
	sprintf_s(logMsg, "Loaded %s from cache: %s\n", shaderType, cacheFile.c_str());
	Log(logMsg);

	return true;
}

bool Shaders::SaveShaderToCache(const char* shaderName, const char* shaderType, ID3DBlob* blob) {
	EnsureCacheDirectoryExists();

	std::string cacheFile = GetCacheFileName(shaderName, shaderType);

	// Open file for writing
	std::ofstream file(cacheFile, std::ios::binary);
	if (!file.is_open()) {
		return false;
	}

	// Writing shader data
	file.write(static_cast<char*>(blob->GetBufferPointer()), blob->GetBufferSize());
	file.close();

	char logMsg[256];
	sprintf_s(logMsg, "Saved %s to cache: %s\n", shaderType, cacheFile.c_str());
	Log(logMsg);

	return true;
}

void Shaders::CreateVS(int i, LPCWSTR name)
{
	HRESULT hr;
	char shaderNameA[1024];
	WideCharToMultiByte(CP_ACP, NULL, name, -1, shaderNameA, sizeof(shaderNameA), NULL, NULL);

	if (LoadShaderFromCache(shaderNameA, "VS", (void**)&VS[i].vShader, &VS[i].pBlob)) {
		return;
	}

	Log("Compiling vertex shader (not found in cache)...\n");

	hr = D3DCompileFromFile(name, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_5_0",
#ifdef _DEBUG
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
#else
		D3DCOMPILE_OPTIMIZATION_LEVEL3,
#endif
		NULL, &VS[i].pBlob, &pErrorBlob);

	Shaders::CompilerLog(name, hr, "vertex shader compiled: ");

	if (hr == S_OK)
	{
		hr = device->CreateVertexShader(VS[i].pBlob->GetBufferPointer(), VS[i].pBlob->GetBufferSize(), NULL, &VS[i].vShader);
		SaveShaderToCache(shaderNameA, "VS", VS[i].pBlob);
	}
}

void Shaders::CreatePS(int i, LPCWSTR name)
{
	HRESULT hr;
	char shaderNameA[1024];
	WideCharToMultiByte(CP_ACP, NULL, name, -1, shaderNameA, sizeof(shaderNameA), NULL, NULL);

	if (LoadShaderFromCache(shaderNameA, "PS", (void**)&PS[i].pShader, &PS[i].pBlob)) {
		return;
	}

	Log("Compiling pixel shader (not found in cache)...\n");

	hr = D3DCompileFromFile(name, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_5_0",
#ifdef _DEBUG
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
#else
		D3DCOMPILE_OPTIMIZATION_LEVEL3,
#endif
		NULL, &PS[i].pBlob, &pErrorBlob);

	Shaders::CompilerLog(name, hr, "pixel shader compiled: ");

	if (hr == S_OK)
	{
		hr = device->CreatePixelShader(PS[i].pBlob->GetBufferPointer(), PS[i].pBlob->GetBufferSize(), NULL, &PS[i].pShader);
		SaveShaderToCache(shaderNameA, "PS", PS[i].pBlob);
	}
}

void Shaders::CreateGS(int i, LPCWSTR name)
{
	HRESULT hr;
	char shaderNameA[1024];
	WideCharToMultiByte(CP_ACP, NULL, name, -1, shaderNameA, sizeof(shaderNameA), NULL, NULL);

	if (LoadShaderFromCache(shaderNameA, "GS", (void**)&GS[i].gShader, &GS[i].pBlob)) {
		return;
	}

	Log("Compiling geometry shader (not found in cache)...\n");

	hr = D3DCompileFromFile(name, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "GS", "gs_5_0",
#ifdef _DEBUG
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
#else
		D3DCOMPILE_OPTIMIZATION_LEVEL3,
#endif
		NULL, &GS[i].pBlob, &pErrorBlob);

	Shaders::CompilerLog(name, hr, "geometry shader compiled: ");

	if (hr == S_OK)
	{
		hr = device->CreateGeometryShader(GS[i].pBlob->GetBufferPointer(), GS[i].pBlob->GetBufferSize(), NULL, &GS[i].gShader);
		SaveShaderToCache(shaderNameA, "GS", GS[i].pBlob);
	}
}

void Shaders::CreateCS(int i, LPCWSTR name)
{
	HRESULT hr;
	char shaderNameA[1024];
	WideCharToMultiByte(CP_ACP, NULL, name, -1, shaderNameA, sizeof(shaderNameA), NULL, NULL);

	if (LoadShaderFromCache(shaderNameA, "CS", (void**)&CS[i].cShader, &CS[i].pBlob)) {
		return;
	}

	Log("Compiling compute shader (not found in cache)...\n");

	hr = D3DCompileFromFile(name, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "CS", "cs_5_0",
#ifdef _DEBUG
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
#else
		D3DCOMPILE_OPTIMIZATION_LEVEL3,
#endif
		NULL, &CS[i].pBlob, &pErrorBlob);

	Shaders::CompilerLog(name, hr, "compute shader compiled: ");

	if (hr == S_OK)
	{
		hr = device->CreateComputeShader(CS[i].pBlob->GetBufferPointer(), CS[i].pBlob->GetBufferSize(), NULL, &CS[i].cShader);
		SaveShaderToCache(shaderNameA, "CS", CS[i].pBlob);
	}
}

void Shaders::Init()
{
	EnsureCacheDirectoryExists();

	Shaders::CreateVS(0, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\VS.shader"));
	Shaders::CreatePS(0, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\PS.shader"));
	
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

	Shaders::CreateVS(15, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\Mesh_VS.shader"));
	Shaders::CreatePS(15, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\Mesh_Stretch_PS.shader"));

	Shaders::CreatePS(16, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\Mesh_Fit_PS.shader"));

	Shaders::CreateVS(17, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\PointCloud_VS.shader"));
	Shaders::CreatePS(17, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\PointCloud_PS.shader"));
	Shaders::CreateGS(17, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\PointCloud_GS.shader"));

	Shaders::CreateVS(18, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\Sphere_VS.shader"));
	Shaders::CreatePS(18, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\Sphere_PS.shader"));

	Shaders::CreateVS(19, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\StarSphere_VS.shader"));
	Shaders::CreatePS(19, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\StarSphere_PS.shader"));

	Shaders::CreateVS(20, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\StarCrown_VS.shader"));
	Shaders::CreatePS(20, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\StarCrown_PS.shader"));

	Shaders::CreateVS(21, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\Particle_VS.shader"));
	Shaders::CreatePS(21, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\Particle_Basic_PS.shader"));

	Shaders::CreatePS(22, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\SwordBeam_PS.shader"));
	
	//-----------------------------------------------
	
	Shaders::CreatePS(100, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\ColorCorrection_PS.shader"));
	
	//-----------------------------------------------
	
	Shaders::CreatePS(200, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\PerlinNoise.shader"));
	Shaders::CreatePS(201, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\VoronoiNoise.shader"));

	//-----------------------------------------------

	Shaders::CreateCS(0, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\DepthDown.shader"));
	//Shaders::CreateCS(1, nameToPatchLPCWSTR("..\\dx11minimal\\Shaders\\Lib\\utils.shader"));

	//-----------------------------------------------

	ConstBuf::CreateVertexBuffer(15);
	ConstBuf::CreateVertexBuffer(17);
}

void Shaders::CleanupCache()
{
	std::string searchPath = "..\\dx11minimal\\Shaders\\Cash\\*" + std::string(GetBuildConfig()) + ".cso";

	WIN32_FIND_DATAA findData;
	HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			std::string filePath = "..\\dx11minimal\\Shaders\\Cash\\" + std::string(findData.cFileName);
			DeleteFileA(filePath.c_str());
			Log("Deleted cache file: ");
			Log(findData.cFileName);
			Log("\n");
		} while (FindNextFileA(hFind, &findData));
		FindClose(hFind);
	}
}

void Shaders::vShader(unsigned int n)
{
	currentVS = n;
	context->VSSetShader(VS[n].vShader, NULL, 0);
}

void Shaders::pShader(unsigned int n)
{
	currentPS = n;
	context->PSSetShader(PS[n].pShader, NULL, 0);
}

void Shaders::gShader(unsigned int n)
{
	currentGS = n;
	context->GSSetShader(GS[n].gShader, NULL, 0);
}

void Shaders::cShader(unsigned int n)
{
	currentCS = n;
	context->CSSetShader(CS[n].cShader, NULL, 0);
}

//////////////////////////////////////////////////////////////////////////////////

void Compute::Dispatch(int csIndex, int texInput, int texOutput)
{
	Shaders::cShader(csIndex);

	// Input texture
	context->CSSetShaderResources(0, 1, &Textures::Texture[texInput].DepthResView);

	// Out UAV
	Textures::textureDesc outTexture = Textures::Texture[texOutput];
	context->CSSetUnorderedAccessViews(0, 1, &outTexture.UnorderedAccessView, nullptr);

	// Num of thread groups
	uint32_t groupCountX = ((int)outTexture.size.x + 7) / 8;
	uint32_t groupCountY = ((int)outTexture.size.y + 7) / 8;

	context->Dispatch(groupCountX, groupCountY, 1);

	// Disable resources
	ID3D11ShaderResourceView* nullSRV = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	context->CSSetShaderResources(0, 1, &nullSRV);
	context->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
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

ID3D11Buffer* ConstBuf::buffer[11];

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

//b8
XMMATRIX ConstBuf::drawerMatrix[constCount];

//b9
ConstBuf::ParticlesDesc ConstBuf::particlesInfo;

//b10
XMFLOAT4X4 ConstBuf::drawerFloat4x4[constCount];


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

void ConstBuf::CreateVertexBuffer(int vertexShader)
{
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "NORMAL";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "TEXCOORD";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	device->CreateInputLayout(polygonLayout, numElements, Shaders::VS[vertexShader].pBlob->GetBufferPointer(),
		Shaders::VS[vertexShader].pBlob->GetBufferSize(), &Shaders::VS[vertexShader].pLayout);
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
	ConstBuf::Create(ConstBuf::buffer[8], sizeof(drawerMatrix));
	ConstBuf::Create(ConstBuf::buffer[9], sizeof(particlesInfo));
	ConstBuf::Create(ConstBuf::buffer[10], sizeof(drawerFloat4x4));
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

void ConstBuf::UpdateParticlesInfo()
{
	context->UpdateSubresource(ConstBuf::buffer[9], 0, NULL, &particlesInfo, 0, 0);
}

void ConstBuf::ConstToVertex(int i)
{
	context->VSSetConstantBuffers(i, 1, &ConstBuf::buffer[i]);
}

void ConstBuf::ConstToGeometry(int i)
{
	context->GSSetConstantBuffers(i, 1, &ConstBuf::buffer[i]);
}

void ConstBuf::ConstToPixel(int i)
{
	context->PSSetConstantBuffers(i, 1, &ConstBuf::buffer[i]);
}

void ConstBuf::ConstToCompute(int i)
{
	context->CSSetConstantBuffers(i, 1, &ConstBuf::buffer[i]);
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
	// ??? 1: ???????? ???????
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory);

	// ??? 2: ???????? ??????? ????
	RECT rc;
	GetClientRect(hwnd, &rc);

	// ??? 3: ???????? ??????-???????
	D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
	D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps =
		D2D1::HwndRenderTargetProperties(hwnd,
			D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top));

	// ??? 4: ??????? render target
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
		break;
	case topology::pointList:
		ttype = D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;
		break;
	}

	context->IASetPrimitiveTopology(ttype);
	//context->IASetInputLayout(NULL);
	//context->IASetVertexBuffers(0, 0, NULL, NULL, NULL);

	context->IASetInputLayout(Shaders::VS[Shaders::currentVS].pLayout);
}

void InputAssembler::vBuffer(int i)
{
	unsigned int stride = sizeof(Models::VertexType);
	unsigned int offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	context->IASetVertexBuffers(0, 1, &Models::Model[i].vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(Models::Model[i].indexBuffer, DXGI_FORMAT_R32_UINT, 0);
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

	// 1/2 sized uav
	Textures::Create(3, Textures::tType::flat, Textures::tFormat::s16, XMFLOAT2(width / 2, height / 2), false, false, true);
	// 1/2 sized rt
	Textures::Create(4, Textures::tType::flat, Textures::tFormat::s16, XMFLOAT2(width / 2, height / 2), true, true);

	// 1/4 sized uav
	Textures::Create(5, Textures::tType::flat, Textures::tFormat::s16, XMFLOAT2(width / 4, height / 4), false, false, true);
	// 1/4 sized rt
	Textures::Create(6, Textures::tType::flat, Textures::tFormat::s16, XMFLOAT2(width / 4, height / 4), true, true);

	// 1/8 sized uav
	Textures::Create(7, Textures::tType::flat, Textures::tFormat::s16, XMFLOAT2(width / 8, height / 8), false, false, true);
	// 1/8 sized rt
	Textures::Create(8, Textures::tType::flat, Textures::tFormat::s16, XMFLOAT2(width / 8, height / 8), true, true);

	// 1/16 sized uav
	Textures::Create(9, Textures::tType::flat, Textures::tFormat::s16, XMFLOAT2(width / 16, height / 16), false, false, true);
	// 1/16 sized rt
	Textures::Create(10, Textures::tType::flat, Textures::tFormat::s16, XMFLOAT2(width / 16, height / 16), true, true);

	// perlin noise rt
	Textures::Create(11, Textures::tType::flat, Textures::tFormat::r8, XMFLOAT2(256, 256), true, false);
	// voronoi noise rt
	Textures::Create(12, Textures::tType::flat, Textures::tFormat::s16, XMFLOAT2(1024, 1024), true, false);
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

	context->DrawInstanced(quadCount * 6, instances, 0, 0);
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