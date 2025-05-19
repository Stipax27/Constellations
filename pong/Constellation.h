int constellationsCounter = 0;

class HealthSystem {
public:
    float starHP;
    float maxHP;
    float defens;
    std::vector<float> starsHealth;

    HealthSystem(int starCount) {
        starsHealth = std::vector<float>(starCount, 1.0f);
        maxHP = starCount;
        starHP = maxHP;
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

    void resetHealth() {
        std::fill(starsHealth.begin(), starsHealth.end(), 1.0f);
        starHP = maxHP;
    }

    bool operator==(const HealthSystem& other) const {
        
        if (starsHealth.size() != other.starsHealth.size()) return false;

       
        const float epsilon = 0.0001f;
        if (fabs(starHP - other.starHP) > epsilon) return false;
        if (fabs(maxHP - other.maxHP) > epsilon) return false;
        if (fabs(defens - other.defens) > epsilon) return false;

       
        for (size_t i = 0; i < starsHealth.size(); ++i) {
            if (fabs(starsHealth[i] - other.starsHealth[i]) > epsilon) {
                return false;
            }
        }
        return true;
    }

};

class Constellation {
public:

    float scale = 1000;
    float distance;

    std::vector<point3d> starsCords;
    std::vector<std::vector<float>> constellationEdges;
    std::vector<point3d> starsRenderedCords = {};
    point3d angle;
    point3d position;
    std::vector<point3d> posEnemy;
    

    Constellation(std::vector<point3d> _starsCords, std::vector<std::vector<float>> _constellationEdges): position{ 0,0,0 } 
    {
        starsCords = _starsCords;
        constellationEdges = _constellationEdges;
    }

    

    void setPosition(const point3d& newPos) {
        point3d offset = { newPos - position };
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

    bool operator==(const Constellation& other) const {
        const float epsilon = 0.0001f;
        if (fabs(scale - other.scale) > epsilon) return false;
        if (fabs(distance - other.distance) > epsilon) return false;

        if (starsCords.size() != other.starsCords.size()) return false;
        if (constellationEdges.size() != other.constellationEdges.size()) return false;

        if (angle != other.angle) return false;
        if (position != other.position) return false;

         
    }



   /* void enemyPosConstallations() {
        posEnemy = starSet[currentEnemyID]->starsCords;
    }*/
};

class Entity {
public:
    char currentMoveDirection;
    int ID;
    Constellation* constellation;
    
    HealthSystem* healthSystem;
    std::string name;
    DWORD moveStartTime;
    bool isMoveActive;

    Entity(Constellation& src_constellation) {
        constellation = &src_constellation;
        ID = constellationsCounter++;
        healthSystem = new HealthSystem(constellation->starsCords.size());
    }
    ~Entity() {
        delete healthSystem;
    }

    
    float getHP() const {
        return healthSystem->starHP;
    }

    float getMaxHP() const {
        return healthSystem->maxHP;
    }

    void damageStar(int starIndex, float damage) {
        healthSystem->damageStar(starIndex, damage);
    }

    void resetHealth() {
        healthSystem->resetHealth();
    }

    bool operator==(const Entity& other) const {
        if (ID != other.ID) return false;

       
        if (!constellation || !other.constellation) {
            return constellation == other.constellation;
        }
        return *constellation == *other.constellation;

        if (healthSystem == nullptr || other.healthSystem == nullptr) {
            return healthSystem == other.healthSystem;
        }
        return (*healthSystem) == (*other.healthSystem);
    }
};