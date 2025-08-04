#include <vector>

int constellationsCounter = 0;


float hashf(float n)
{
    return fract(sin(n) * 43758.5453f);
}

float noise(point3d point)
{
    point3d p = point.floor3();
    point3d f = point.fract();

    f *= f;

    f = f * f * (3.0f - 2.0f * f.x);
    float n = p.x + p.y * 57.0f + 113.0f * p.z;

    float a = lerp(lerp(lerp(hashf(n + 0.0f), hashf(n + 1.0f), f.x),
        lerp(hashf(n + 57.0f), hashf(n + 58.0f), f.x), f.y),
        lerp(lerp(hashf(n + 113.0f), hashf(n + 114.0f), f.x),
            lerp(hashf(n + 170.0f), hashf(n + 171.0f), f.x), f.y), f.z);

    return a - 0.5f;
}


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
        if (!morphed)
        {

            morphed = true;
            morphProgress = 0;

            targetStarsCords = c.originStarsCords;
            targetConstellationEdges = c.originConstellationEdges;

            constellationEdges.clear();

            /*starsCords.clear();
            for (int i = 0; i < targetStarsCords.size(); i++) {
                if (i < originStarsCords.size())
                {
                    starsCords.push_back(originStarsCords[i]);
                }
                else
                {
                    starsCords.push_back(targetStarsCords[i]);
                }
            }

            constellationEdges.clear();
            for (int i = 0; i < targetConstellationEdges.size(); i++) {
                if (i < originConstellationEdges.size())
                {
                    constellationEdges.push_back(originConstellationEdges[i]);
                }
                else
                {
                    constellationEdges.push_back(targetConstellationEdges[i]);
                }
            }*/

        }
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

                float time = timer::GetCounter() / 500;
                for (int i = 0; i < starsCords.size(); i++) {

                    point3d origin;
                    if (i < originStarsCords.size())
                    {
                        origin = originStarsCords[i];
                    }
                    else
                    {
                        origin = targetStarsCords[i];
                    }

                    point3d p = starsCords[i];
                    starsCords[i] = p.lerp(
                        point3d(
                            noise(point3d(origin.x, origin.z, origin.y) + time),
                            noise(point3d(origin.y, origin.z, origin.x) + time),
                            noise(point3d(origin.z, origin.x, origin.y) + time)
                        ).normalized() * 0.1f,
                        morphProgress);
                }
            }
            else if (morphProgress <= 2.0f)
            {
                if (starsCords.size() != targetStarsCords.size())
                {
                    starsCords.clear();
                    float time = timer::GetCounter() / 500;
                    for (int i = 0; i < targetStarsCords.size(); i++) {
                        point3d origin = targetStarsCords[i];
                        starsCords.push_back(
                            point3d(
                                noise(point3d(origin.x, origin.z, origin.y) + time),
                                noise(point3d(origin.y, origin.z, origin.x) + time),
                                noise(point3d(origin.z, origin.x, origin.y) + time)
                            ).normalized() * 0.1f);
                    }
                }

                if (constellationEdges.size() != targetConstellationEdges.size())
                {
                    constellationEdges.clear();
                    for (int i = 0; i < targetConstellationEdges.size(); i++) {
                        constellationEdges.push_back(targetConstellationEdges[i]);
                    }
                }

                for (int i = 0; i < targetStarsCords.size(); i++) {
                    point3d p = starsCords[i];
                    starsCords[i] = p.lerp(targetStarsCords[i], morphProgress - 1.0f);
                }
            }
        }
    }

};