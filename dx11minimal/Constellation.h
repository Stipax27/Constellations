#include <vector>

int constellationsCounter = 0;


class Constellation {
public:

    XMMATRIX Transform;
    
    point3d position;
    float scale = 200;
    point3d angle;
    float distance;
    point3d lookvector = point3d(0,0,1);

    float hp;
    float maxHP;
    float defens;

    std::vector<point3d> originStarsCords;
    std::vector <std::vector <float>> originConstellationEdges;
    std::vector<point3d> targetStarsCords;
    std::vector <std::vector <float>> targetConstellationEdges;
    bool morphed = false;
    float morphProgress = 0.0f;

    std::vector<point3d> starsCords;
    std::vector <float> starsHealth;
    std::vector <std::vector <float>> constellationEdges;

    std::vector<point3d> starsRenderedCords = {};
    int ID;
    std::string name;

    char currentMoveDirection;
    DWORD moveStartTime;
    bool isMoveActive;

    

    friend XMMATRIX CreateConstToWorldMatrix(const Constellation& c)
    {
        float zOffset = 1000.0f / c.scale;
        XMMATRIX translateZ = XMMatrixTranslation(0, 0, zOffset);
        XMMATRIX rotate = XMMatrixRotationRollPitchYaw(c.angle.x, c.angle.y,c.angle.z);
        XMMATRIX scale = XMMatrixScaling(c.scale, c.scale, c.scale);
        return translateZ * rotate * scale;
    }

    friend XMMATRIX CreateEnemyToWorldMatrix(Constellation& c)
    {
        updateEnemyPosition(deltaTime);
        c.scale = 20000;

        XMVECTOR currentPos = XMVectorSet(
            Enemy::enemyAi.enemyConstellationOffset.r[3].m128_f32[0],
            Enemy::enemyAi.enemyConstellationOffset.r[3].m128_f32[1],
            Enemy::enemyAi.enemyConstellationOffset.r[3].m128_f32[2],
            1.0f
        );

        XMMATRIX translation = XMMatrixTranslationFromVector(currentPos);
        XMMATRIX rotation = XMMatrixRotationQuaternion(Enemy::enemyAi.currentRotation);
        XMMATRIX scale = XMMatrixScaling(c.scale, c.scale, c.scale);

        return  scale * rotation * translation;
    }

    friend XMMATRIX HeroUITransform( const Constellation& c)
    {

       // fightMove();


        XMMATRIX scale = XMMatrixScaling(c.scale * .53, c.scale * .53, c.scale * .53);
    }

    friend XMMATRIX CreateHeroToWorldMatrix(const Constellation& c)
    {
        static float lastTime = currentTime;
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        if (deltaTime > 100.0f) deltaTime = 100.0f;
        updateFlyDirection();
        updateFlySpeed(deltaTime);
        updatePlayerPosition(deltaTime);

        XMVECTOR heroPosition = XMVectorSet(
            Hero::state.constellationOffset.r[3].m128_f32[0],
            Hero::state.constellationOffset.r[3].m128_f32[1],
            Hero::state.constellationOffset.r[3].m128_f32[2],
            0.0f
        );

        // 2. Матрица перемещения в начало координат
        XMMATRIX toOrigin = XMMatrixTranslationFromVector(-heroPosition);

        // 3. Матрица вращения
        XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(Hero::state.currentRotation);

        // 4. Матрица масштабирования
        XMMATRIX scaleMatrix = XMMatrixScaling(c.scale, c.scale, c.scale);

        // 5. Матрица перемещения (из constellationOffset)
        XMMATRIX translationMatrix = Camera::state.constellationOffset;

        // 6. Обратная матрица перемещения
        XMVECTOR det;
        XMMATRIX invTranslationMatrix = XMMatrixInverse(&det, translationMatrix);

        XMMATRIX Hero = translationMatrix * invTranslationMatrix * rotationMatrix * scaleMatrix * toOrigin;

        return -Hero;
        //rotationMatrix(rotation); 
        
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

        originStarsCords = _starsCords;
        originConstellationEdges = _constellationEdges;

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

        p *= 3000;

        p += position;
    }

    void setStarsRenderedCords(float angleX, float angleY, float angleZ) {
        angle = { angleX, angleY, angleZ };
        starsRenderedCords = starsCords;

        for (int i = 0; i < starsCords.size(); i++) {
            point3d p = starsRenderedCords[i];
            Arrange(p);
            starsRenderedCords[i] = p;
        }
    }

    //////////////////////////////////////////////////////////////////////////////////

    void Morph(const Constellation& c)
    {
        targetStarsCords = c.originStarsCords;
        targetConstellationEdges = c.originConstellationEdges;

        starsCords.clear();
        for (int i = 0; i < targetStarsCords.size(); i++) {
            if (i < originStarsCords.size())
            {
                starsCords.push_back(originStarsCords[i]);
            }
            else
            {
                starsCords.push_back(point3d());
            }
        }

        constellationEdges.clear();
        for (int i = 0; i < targetConstellationEdges.size(); i++) {
            if (i < originConstellationEdges.size())
            {
                constellationEdges.push_back(originConstellationEdges[i]);
            }
            /*else
            {
                constellationEdges.push_back(point3d());
            }*/
        }

        morphed = true;
        morphProgress = 0;
    }

    void RenderMorph(float deltaTime)
    {
        if (morphed)
        {
            morphProgress += deltaTime / 1000.0f;

            if (morphProgress <= 1.0f)
            {
                //starsCords = originStarsCords;
                //constellationEdges = originConstellationEdges;

                for (int i = 0; i < starsCords.size(); i++) {
                    point3d p = starsCords[i];
                    starsCords[i] = p.lerp(point3d(), morphProgress);
                }
            }
            else if (morphProgress <= 2.0f)
            {
                //starsCords = targetStarsCords;
                //constellationEdges = targetConstellationEdges;

                for (int i = 0; i < targetStarsCords.size(); i++) {
                    point3d p = starsCords[i];
                    starsCords[i] = p.lerp(targetStarsCords[i], morphProgress - 1.0f);
                }
            }
        }
    }

};