#include <vector>
#include <cmath>
#include <regex>

struct vec3 {
    float x, y, z;
    vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
};

struct Constellation3D {
    std::vector<vec3> points;
    std::vector<std::pair<int, int>> edges;
    vec3 position;
    float scale = 1.0f;
};

namespace svg3d {
    vec3 svgTo3D(float x, float y, float width, float height) {
        // Нормализация координат SVG и преобразование в 3D пространство
        float normalizedX = (x / width - 0.5f) * 2.0f;
        float normalizedY = (0.5f - y / height) * 2.0f;

        // Добавляем случайное смещение по Z для 3D эффекта
        float z = static_cast<float>(rand() % 100) / 1000.0f;
        return vec3(normalizedX, normalizedY, z);
    }

    void parsePath(const std::string& path, Constellation3D& constellation, float svgWidth, float svgHeight) {
        // Упрощенный парсинг path commands (реализация для конкретного формата)
        std::regex pointRegex(R"((-?\d+\.?\d*)[, ](-?\d+\.?\d*))");
        std::sregex_iterator iter(path.begin(), path.end(), pointRegex);
        std::sregex_iterator end;

        std::vector<vec3> tempPoints;
        while (iter != end) {
            std::smatch match = *iter;
            float x = std::stof(match[1]);
            float y = std::stof(match[2]);
            tempPoints.push_back(svgTo3D(x, y, svgWidth, svgHeight));
            ++iter;
        }

        // Создание связей между последовательными точками
        for (size_t i = 1; i < tempPoints.size(); ++i) {
            constellation.edges.emplace_back(i - 1, i);
        }
        constellation.points = tempPoints;
    }
}

bool readSvg(Constellation3D& constellation, const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) return false;

    std::string line;
    float svgWidth = 1024, svgHeight = 768; // Должны извлекаться из SVG

    while (std::getline(in, line)) {
        if (line.find("<polygon") != std::string::npos) {
            size_t pointsStart = line.find("points=\"") + 8;
            size_t pointsEnd = line.find("\"", pointsStart);
            std::string pointsStr = line.substr(pointsStart, pointsEnd - pointsStart);

            std::stringstream ss(pointsStr);
            std::vector<vec3> points;
            float x, y;
            char comma;

            while (ss >> x >> comma >> y) {
                points.push_back(svg3d::svgTo3D(x, y, svgWidth, svgHeight));
            }

            // Создание связей для полигона
            for (size_t i = 1; i < points.size(); ++i) {
                constellation.edges.emplace_back(i - 1, i);
            }
            if (points.size() > 1) {
                constellation.edges.emplace_back(points.size() - 1, 0);
            }
            constellation.points = points;
        }
        else if (line.find("<path") != std::string::npos) {
            size_t dStart = line.find("d=\"") + 3;
            size_t dEnd = line.find("\"", dStart);
            std::string pathData = line.substr(dStart, dEnd - dStart);
            svg3d::parsePath(pathData, constellation, svgWidth, svgHeight);
        }
    }
    return true;
}


void renderConstellation(const Constellation3D& constellation) { // Функция рендеринга 3D созвездия
    for (const auto& point : constellation.points) {// Рендер точек
        draw3DSphere(
            point.x * constellation.scale + constellation.position.x,
            point.y * constellation.scale + constellation.position.y,
            point.z * constellation.scale + constellation.position.z,
            0.1f // радиус сферы
        );
    }

    for (const auto& edge : constellation.edges) { // Рендер линий между точками
        const vec3& p1 = constellation.points[edge.first];
        const vec3& p2 = constellation.points[edge.second];
        draw3DLine(
            p1.x * constellation.scale + constellation.position.x,
            p1.y * constellation.scale + constellation.position.y,
            p1.z * constellation.scale + constellation.position.z,
            p2.x * constellation.scale + constellation.position.x,
            p2.y * constellation.scale + constellation.position.y,
            p2.z * constellation.scale + constellation.position.z,
            RGB(255, 223, 0) // цвет золотой линии
        );
    }
}