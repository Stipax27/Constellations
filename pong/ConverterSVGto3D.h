#pragma once
#include <vector>
#include "point3d.h"
#include <cmath>
#include <string>
#include <fstream>
#include "tinyxml2-master/tinyxml2.cpp"

int main(){

	string polyline, path;

	std::ifstream svgFile("SVGfiles/example.svg");

	if (!svgFile.is_open()) {
		std::cerr << "Error opening file!" << std::endl;
		return 1;
	}

	// ��������� ������ ������ (��������, ����� �� ����� ��� ���������� ���������)
	std::string line;
	while (std::getline(svgFile, line)) { //getline, polyline todo
		std::cout << line << std::endl;
	}

	// ������: ���� ������������ �������� � SVG
	if (line.find("<rect") != std::string::npos) {
		std::cout << "������� ������ �������������!" << std::endl;
	}

	svgFile.close();
	return 0;
}