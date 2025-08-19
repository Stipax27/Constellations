#pragma once

#include <vector>
#include <algorithm> // ��� std::generate

namespace TextureGenerator {
    // ��������� ��������
    const UINT width = 4096;
    const UINT height = 4096;
    const UINT pixelSize = 4; // A

    // ��������� ���� (������� ���������������)
    float Noise(int x, int y, int seed = 0) {
        return std::fmod(std::sin(x * 12.9898 + y * 78.233 + seed) * 43758.5453, 1.0f);
    }

    // �������� ������ ������ ��������
    std::vector<unsigned char> GenerateNoiseData() {
        std::vector<unsigned char> data(width * height * pixelSize);

        for (UINT y = 0; y < height; ++y) {
            for (UINT x = 0; x < width; ++x) {
                UINT idx = (y * width + x) * pixelSize;

                // ������: ��� �������/�������� (����� - ���������������)
                float noiseValue = Noise(x, y, 42);

                // ������ �� ��� ������ (grayscale) + �����
                unsigned char value = static_cast<unsigned char>(noiseValue * 255);
                data[idx + 0] = value; // R
                data[idx + 1] = value; // G
                data[idx + 2] = value; // B
                data[idx + 3] = 255;   // A
            }
        }
        return data;
    }

    ID3D11Texture2D* CreateNoiseTexture() {
        // ���������� ������
        std::vector<unsigned char> noiseData = GenerateNoiseData();

        // �������� ��������
        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32-������ ������
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_IMMUTABLE; // ����������� ��� ��������� ��������
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        // ������ ��� �������������
        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = noiseData.data();
        initData.SysMemPitch = width * pixelSize; // ���� �� ������

        // �������� ��������
        ID3D11Texture2D* noiseTexture = nullptr;
        HRESULT hr = device->CreateTexture2D(&desc, &initData, &noiseTexture);
        if (FAILED(hr)) {

        }

        return noiseTexture;
    }

    ID3D11ShaderResourceView* CreateNoiseSRV(ID3D11Texture2D* texture) {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;

        ID3D11ShaderResourceView* srv = nullptr;
        device->CreateShaderResourceView(texture, &srvDesc, &srv);
        return srv;
    }

    void Init() {
        ID3D11Texture2D* texture = CreateNoiseTexture();
        ID3D11ShaderResourceView* noiseSRV = CreateNoiseSRV(texture);
        context->VSSetShaderResources(0, 1, &noiseSRV);
    }
}