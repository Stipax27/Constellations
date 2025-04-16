//#include "Windows.h"

int constellationsCounter = 0;

class Constellation {
public:

    std::vector<point3d> starsCords;
    float hp;
    std::vector <float> starsHealth;
    std::vector <std::vector <float>> constellationEdges;

    std::vector<point3d> starsRenderedCords = {};
    float scale = 1000;
    int ID;
    std::string name;

    point3d angle;
    float distance;

    char currentMoveDirection; 
    DWORD moveStartTime;
    bool isMoveActive;
    

    Constellation(std::vector<point3d> _starsCords, std::vector <float> _starsHealth, std::vector <std::vector <float>> _constellationEdges) {
        starsCords = _starsCords;
        starsHealth = _starsHealth;
        constellationEdges = _constellationEdges;
        ID = constellationsCounter;
        //name = zodiacSignToString((ZodiacSign)ID);
        constellationsCounter++;
    }

    void setStarsRenderedCords(float angleX, float angleY, float angleZ) // Подготавливаем данные[starArrayRendered] для отрисовки созвездия
    {
        angle = { angleX,angleY,angleZ };
        distance = 3000. / scale;

        starsRenderedCords = starsCords;


        for (int i = 0; i < starsCords.size(); i++)// РАзмещение Линий.
        {
            point3d p = { starsRenderedCords[i].x, starsRenderedCords[i].y, starsRenderedCords[i].z };

            move(p, 0, 0, 3000. / scale);
            rotateX(p, angleX);
            rotateY(p, angleY);
            rotateZ(p, angleZ);

            starsRenderedCords[i].x = p.x * scale;
            starsRenderedCords[i].y = p.y * scale;
            starsRenderedCords[i].z = p.z * scale;
        }
    }


};