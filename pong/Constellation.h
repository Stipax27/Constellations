//#include "Windows.h"

int constellationsCounter = 0;


class Constellation {
public:
    float scale = 1000;
    float distance;

    std::vector<point3d> starsCords;
    std::vector <std::vector <float>> constellationEdges;
    std::vector<point3d> starsRenderedCords = {};
    point3d angle;
    point3d position;
    
    Constellation(std::vector<point3d> _starsCords, std::vector <std::vector <float>> _constellationEdges) : position{ 0,0,0 } 
    {
        starsCords = _starsCords;
        constellationEdges = _constellationEdges;
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

class Entity {
public:
    float starHP;
    float maxHP;
    float defens;
    char currentMoveDirection;
    int ID;
    Constellation* constellation; 

    std::vector<float> starsHealth;  
    std::string name;
    DWORD moveStartTime;
    bool isMoveActive;

    
    Entity(Constellation* _constellation) : constellation(_constellation) {
       
        int starCount = constellation->starsCords.size();

        starsHealth = std::vector<float>(starCount, 1.0f);

        maxHP = starCount;
        starHP = maxHP;

        ID = constellationsCounter++;
    }

    void updateStarsHP() {
        starHP = 0;
        for (float health : starsHealth) {
            starHP += health;
        }
    }

    void damageStar(int starIndex, float damage) {
        if (starIndex >= 0 && starIndex < starsHealth.size()) {
            starsHealth[starIndex] -= damage;
            if (starsHealth[starIndex] < 0) starsHealth[starIndex] = 0;
            updateStarsHP();
        }

    }
  float ariesHP = AriesEntity.starHP;
  float taurusHP = TaurusEntity.starHP;
  float geminiHP = GeminiEntity.starHP;
  float cencerHP = CancerEntity.starHP;
  float leoHP = LeoEntity.starHP;
  float virgoHP = VirgoEntity.starHP;
  float libraHP = LibraEntity.starHP;
  float scorpiusHP = ScorpiusEntity.starHP;
  float sagittariusHP = SagittariusEntity.starHP;
  float capricornusHP = CapricornusEntity.starHP;
  float aquariusHP = AquariusEntity.starHP;
  float piscesHP = PiscesEntity.starHP;
};

