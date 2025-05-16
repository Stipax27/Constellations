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
        return starHP == other.starHP &&
            maxHP == other.maxHP &&
            defens == other.defens &&
            starsHealth == other.starsHealth;
    }

    bool operator!=(const HealthSystem& other) const {
        return !(*this == other);
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
    HealthSystem* healthSystem;

    Constellation(std::vector<point3d> _starsCords, std::vector<std::vector<float>> _constellationEdges): position{ 0,0,0 } 
    {
        starsCords = _starsCords;
        constellationEdges = _constellationEdges;
        healthSystem = new HealthSystem(starsCords.size());
    }

    ~Constellation() {
        delete healthSystem;
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
        return scale == other.scale &&
            distance == other.distance &&
            starsCords == other.starsCords &&
            constellationEdges == other.constellationEdges &&
            angle == other.angle &&
            position == other.position &&
            (*healthSystem) == (*other.healthSystem);
    }

    bool operator!=(const Constellation& other) const {
        return !(*this == other);
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

    std::string name;
    DWORD moveStartTime;
    bool isMoveActive;

    Entity(Constellation& src_constellation) {
        constellation = &src_constellation;
        ID = constellationsCounter++;
    }

    
    float getHP() const {
        return constellation->healthSystem->starHP;
    }

    float getMaxHP() const {
        return constellation->healthSystem->maxHP;
    }

    void damageStar(int starIndex, float damage) {
        constellation->healthSystem->damageStar(starIndex, damage);
    }

    void resetHealth() {
        constellation->healthSystem->resetHealth();
    }

    bool operator==(const Entity& other) const {
        return ID == other.ID &&
            constellation == other.constellation;
    }

    bool operator!=(const Entity& other) const {
        return !(*this == other);
    }
};