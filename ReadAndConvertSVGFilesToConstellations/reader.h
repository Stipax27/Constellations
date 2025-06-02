#pragma once
#include <iostream>
#include <fstream>
#include <string>

int reader() {

    std::string inputFilePath = "SVGfiles/test.svg";

    // Извлекаем имя файла без пути и расширения
    size_t lastSlash = inputFilePath.find_last_of("/\\");
    std::string fileName = inputFilePath.substr(lastSlash + 1);
    size_t dotPos = fileName.rfind('.');
    if (dotPos != std::string::npos) {
        fileName = fileName.substr(0, dotPos);
    }

    // Формируем путь к выходному файлу в ReadedSVG
    std::string outputFilePath = "ReadedSVG/" + fileName + "Readed.txt";

    std::ifstream svgFile(inputFilePath);
    std::ofstream outFile(outputFilePath);

    if (!svgFile.is_open()) {
        std::cerr << "Error opening SVG file!" << std::endl;
        return 1;
    }
    if (!outFile.is_open()) {
        std::cerr << "Error creating output file!" << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(svgFile, line)) {
        std::cout << line << std::endl;
        outFile << line << std::endl;
    }

    svgFile.close();
    outFile.close();

    std::cout << "\nFile saved as: " << outputFilePath << std::endl;
    return 0;
}