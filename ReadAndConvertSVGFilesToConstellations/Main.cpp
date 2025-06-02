#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <filesystem>

namespace fs = std::filesystem;

// Функция для извлечения чисел из строки
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

// Функция для обработки SVG-данных
void processSVGFile(const fs::path& input_path, const fs::path& output_dir_raw, const fs::path& output_dir_converted) {
    // 1. Чтение SVG и сохранение "сырой" копии
    std::ifstream svg_file(input_path);
    fs::path raw_output_path = output_dir_raw / (input_path.stem().string() + "Readed.txt");
    std::ofstream raw_file(raw_output_path);

    if (!svg_file || !raw_file) {
        std::cerr << "Ошибка при работе с файлом: " << input_path << std::endl;
        return;
    }

    std::string line;
    std::vector<std::vector<float>> coords;

    while (getline(svg_file, line)) {
        raw_file << line << "\n";  // Сохраняем "сырую" копию
        std::vector<float> numbers = extractNumbers(line);
        // Обрабатываем числа парами (X, Y)
        for (size_t i = 0; i + 1 < numbers.size(); i += 2) {
            coords.push_back({ numbers[i], numbers[i + 1], 0.0f });  // Z = 0
        }
    }
    svg_file.close();
    raw_file.close();

    // 2. Сохранение 3D-данных
    fs::path converted_output_path = output_dir_converted / (input_path.stem().string() + "Converted.txt");
    std::ofstream converted_file(converted_output_path);

    if (!converted_file) {
        std::cerr << "Ошибка создания файла: " << converted_output_path << std::endl;
        return;
    }

    for (size_t i = 0; i < coords.size(); ++i) {
        converted_file << "ID:" << i + 1 << " P "
            << coords[i][0] << " "
            << coords[i][1] << " "
            << coords[i][2] << "\n";
    }
    converted_file.close();

    std::cout << "Обработан файл: " << input_path.filename() << " -> "
        << converted_output_path.filename() << std::endl;
}

int main() {
    // Пути к папкам
    fs::path input_dir = "SVGfiles";
    fs::path output_dir_raw = "ReadedSVG";
    fs::path output_dir_converted = "ConvertedData";

    // Создаём папки, если их нет
    fs::create_directories(output_dir_raw);
    fs::create_directories(output_dir_converted);

    // Обрабатываем все SVG-файлы в папке
    for (const auto& entry : fs::directory_iterator(input_dir)) {
        if (entry.path().extension() == ".svg") {
            processSVGFile(entry.path(), output_dir_raw, output_dir_converted);
        }
    }

    std::cout << "Готово! Все файлы обработаны." << std::endl;
    return 0;
}