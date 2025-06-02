#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <filesystem>

namespace fs = std::filesystem;

// ������� ��� ���������� ����� �� ������
std::vector<float> extractNumbers(const std::string& s) {
    std::vector<float> numbers;
    std::regex num_regex(R"([-+]?\d*\.?\d+)");
    auto numbers_begin = std::sregex_iterator(s.begin(), s.end(), num_regex);
    auto numbers_end = std::sregex_iterator();

    for (auto i = numbers_begin; i != numbers_end; ++i) {
        numbers.push_back(std::stof(i->str()));
    }
    return numbers;
}

// ������� ��� ��������� SVG-������
void processSVGFile(const fs::path& input_path, const fs::path& output_dir_raw, const fs::path& output_dir_converted) {
    // 1. ������ SVG � ���������� "�����" �����
    std::ifstream svg_file(input_path);
    fs::path raw_output_path = output_dir_raw / (input_path.stem().string() + "Readed.txt");
    std::ofstream raw_file(raw_output_path);

    if (!svg_file || !raw_file) {
        std::cerr << "������ ��� ������ � ������: " << input_path << std::endl;
        return;
    }

    std::string line;
    std::vector<std::vector<float>> coords;

    while (getline(svg_file, line)) {
        raw_file << line << "\n";  // ��������� "�����" �����
        std::vector<float> numbers = extractNumbers(line);
        // ������������ ����� ������ (X, Y)
        for (size_t i = 0; i + 1 < numbers.size(); i += 2) {
            coords.push_back({ numbers[i], numbers[i + 1], 0.0f });  // Z = 0
        }
    }
    svg_file.close();
    raw_file.close();

    // 2. ���������� 3D-������
    fs::path converted_output_path = output_dir_converted / (input_path.stem().string() + "Converted.txt");
    std::ofstream converted_file(converted_output_path);

    if (!converted_file) {
        std::cerr << "������ �������� �����: " << converted_output_path << std::endl;
        return;
    }

    for (size_t i = 0; i < coords.size(); ++i) {
        converted_file << "ID:" << i + 1 << " P "
            << coords[i][0] << " "
            << coords[i][1] << " "
            << coords[i][2] << "\n";
    }
    converted_file.close();

    std::cout << "��������� ����: " << input_path.filename() << " -> "
        << converted_output_path.filename() << std::endl;
}

int main() {
    // ���� � ������
    fs::path input_dir = "SVGfiles";
    fs::path output_dir_raw = "ReadedSVG";
    fs::path output_dir_converted = "ConvertedData";

    // ������ �����, ���� �� ���
    fs::create_directories(output_dir_raw);
    fs::create_directories(output_dir_converted);

    // ������������ ��� SVG-����� � �����
    for (const auto& entry : fs::directory_iterator(input_dir)) {
        if (entry.path().extension() == ".svg") {
            processSVGFile(entry.path(), output_dir_raw, output_dir_converted);
        }
    }

    std::cout << "������! ��� ����� ����������." << std::endl;
    return 0;
}