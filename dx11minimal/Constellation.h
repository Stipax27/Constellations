//#include "Windows.h"

int constellationsCounter = 0;


class Constellation {
public:

    XMMATRIX Transform;

    std::vector<point3d> starsCords;
    float hp;
    float maxHP;
    float defens;
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
    friend XMMATRIX CreateConstToWorldMatrix(const Constellation& c)
    {
        float zOffset = 1000.0f / c.scale;
        XMMATRIX translateZ = XMMatrixTranslation(0, 0, zOffset);
        XMMATRIX rotateX = XMMatrixRotationX(c.angle.x);
        XMMATRIX rotateY = XMMatrixRotationY(c.angle.y);
        XMMATRIX rotateZ = XMMatrixRotationZ(c.angle.z);
        XMMATRIX scale = XMMatrixScaling(c.scale, c.scale, c.scale);
       // XMMATRIX rotateX1 = XMMatrixRotationX( mouse.Angle.y * 0.1);
       // XMMATRIX rotateY1 = XMMatrixRotationY(mouse.Angle.x * 0.1);
        return translateZ * rotateX*  rotateY*  rotateZ * scale;
    }
    friend XMMATRIX CreatefightProjectMatrix(const Constellation& c)
    {
        int fadeInTime = currentTime - startTime;
        float startCamDist = 100;
        float finCamDist = 3000;

        float lerpCoef = smoothstep(0., 1, min(fadeInTime * 4, finCamDist) / (finCamDist - startCamDist));
        if (lerpCoef < .99) {
            camDist = lerpCoef * (finCamDist - startCamDist) + startCamDist;
        }
        camDist = clamp(camDist, 1000, 30000);


    }
    Constellation(std::vector<point3d> _starsCords, std::vector <float> _starsHealth, std::vector <std::vector <float>> _constellationEdges) : position{ 0,0,0 }
    {

        starsCords = _starsCords;
        starsHealth = _starsHealth;
        constellationEdges = _constellationEdges;
        ID = constellationsCounter;
        //name = zodiacSignToString((ZodiacSign)ID);

        maxHP = 0;
        for (int i = 0;i < starsHealth.size();i++)
        {
            maxHP += starsHealth[i];
        }

        constellationsCounter++;
    }



    void setPosition(const point3d& newPos) {

        point3d offset = {
            newPos - position
        };

        for (auto& star : starsCords) {
            star += offset;
        }

        position = newPos;
    }

    point3d getPosition() const {
        return position;
    }

    void Arrange(point3d& p) const {

        p -= position;

        p.move(0, 0, 3000. / scale);

        p.rotateX(p, angle.x);
        p.rotateY(p, angle.y);
        p.rotateZ(p, angle.z);

        p *= scale;

        p += position;

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