#pragma once
#include <vector>
#include "point3d.h"
#include "ConverterSVGto3D.h"
#include <cmath>

class ConverterSVGto3D {
public:
    static bool Convert(const char* svgPath,
        std::vector<point3d>& vertices,
        std::vector<std::pair<int, int>>& edges);

private:
    static void ParsePathData(const char* d, std::vector<point3d>& vertices);
    static void NormalizeVertices(std::vector<point3d>& vertices);
};

// ��������� ���������� ��� ������������
bool ConverterSVGto3D::Convert(const char* svgPath,
    std::vector<point3d>& vertices,
    std::vector<std::pair<int, int>>& edges)
{
    // �������� - ������ ������ ��� �����
    vertices = { //�������
        {100.0f, 100.0f, 0.0f},
        {200.0f, 150.0f, 0.0f},
        {150.0f, 250.0f, 0.0f}
    };

    edges = { //�����
        {0, 1},
        {1, 2},
        {2, 0}
    };

    return true;
}

void ConverterSVGto3D::ParsePathData(const char* d, std::vector<point3d>& vertices) { 
    // ������ SVG ���� �����(��, ���������� ���������� ��) (��������� ��������)
}

void ConverterSVGto3D::NormalizeVertices(std::vector<point3d>& vertices) { // ������ ������������ ��������� ������ �����
    
    float maxX = 0, maxY = 0;
    for (auto& v : vertices) {
        if (v.x > maxX) maxX = v.x;
        if (v.y > maxY) maxY = v.y;
    }

    for (auto& v : vertices) {
        v.x /= maxX;
        v.y /= maxY;
    }
}