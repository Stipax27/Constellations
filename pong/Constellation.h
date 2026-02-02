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
    point3d position;
    

    Constellation(std::vector<point3d> _starsCords, std::vector <float> _starsHealth, std::vector <std::vector <float>> _constellationEdges) : position{ 0,0,0 } 
    {
        
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

    

    void setPosition(const point3d& newPos) {
        
        point3d offset = {
            newPos.x - position.x,
            newPos.y - position.y,
            newPos.z - position.z
        };

        
        for (auto& star : starsCords) {
            star.x += offset.x;
            star.y += offset.y;
            star.z += offset.z;
        }

        position = newPos;
    }

    point3d getPosition() const {
        return position;
    }

    void Arrange(point3d& p) const {
        
        p.x -= position.x;
        p.y -= position.y;
        p.z -= position.z;

        move(p, 0, 0, 3000. / scale);
        rotateX(p, angle.x);
        rotateY(p, angle.y);
        rotateZ(p, angle.z);

        p.x *= scale;
        p.y *= scale;
        p.z *= scale;

       
        p.x += position.x;
        p.y += position.y;
        p.z += position.z;
    }

    void setStarsRenderedCords(float angleX, float angleY, float angleZ) {
        angle = { angleX, angleY, angleZ };
        distance = 3000. / scale;
        starsRenderedCords = starsCords;

        for (int i = 0; i < starsCords.size(); i++) {
            point3d p = starsRenderedCords[i];
            Arrange(p);
            starsRenderedCords[i] = p;
        }
    }
};