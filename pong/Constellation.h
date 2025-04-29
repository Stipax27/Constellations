//#include "Windows.h"

int constellationsCounter = 0;

class Constellation {
public:

    std::vector<point3d> starsCords;
    float hp;
    float maxHP;
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

        maxHP = 0;
        for (int i=0;i< starsHealth.size();i++)
        { 
            maxHP += starsHealth[i];
        }

        constellationsCounter++;
    }

    void Arrange(point3d& p) 
    {
        move(p, 0, 0, 3000. / scale);
        rotateX(p, angle.x);
        rotateY(p, angle.y);
        rotateZ(p, angle.z);

        p.x *= scale;
        p.y *= scale;
        p.z *= scale;
    }

    point3d getPosition()
    {
        point3d p = { 0,0,0 };
        Arrange(p);
        return p;
    }

    void setStarsRenderedCords(float angleX, float angleY, float angleZ) // Подготавливаем данные[starArrayRendered] для отрисовки созвездия
    {
        angle = { angleX,angleY,angleZ };
        distance = 3000. / scale;

        starsRenderedCords = starsCords;


        for (int i = 0; i < starsCords.size(); i++)// РАзмещение Линий.
        {
            point3d p = { starsRenderedCords[i].x, starsRenderedCords[i].y, starsRenderedCords[i].z };

            Arrange(p);

            starsRenderedCords[i].x = p.x;
            starsRenderedCords[i].y = p.y;
            starsRenderedCords[i].z = p.z;
        }
    }


};