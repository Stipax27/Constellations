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
    std::vector<point3d> prevStarsCords;

    Constellation* targetConstellation;
    bool morphing = false;
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
        prevStarsCords = _starsCords;

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

    void Morph(Constellation* c)
    {
        if (!morphing && targetConstellation != c)
        {
            morphing = true;
            morphProgress = -0.1f;

            prevStarsCords = starsCords;
            targetConstellation = c;

            constellationEdges.clear();

        }
    }

    void RenderMorph(float deltaTime)
    {
        if (morphing)
        {
            morphProgress += deltaTime / 1000.0f;

            point3d up = point3d(
                XMVectorGetX(Camera::state.Up),
                XMVectorGetY(Camera::state.Up),
                XMVectorGetZ(Camera::state.Up)
            );
            point3d right = point3d(
                XMVectorGetX(Camera::state.Right),
                XMVectorGetY(Camera::state.Right),
                XMVectorGetZ(Camera::state.Right)
            );

            if (morphProgress <= 1.0f)
            {
                //starsCords = originStarsCords;
                //constellationEdges = originConstellationEdges;

                float time = timer::GetCounter() / 500;
                for (int i = 0; i < starsCords.size(); i++) {

                    point3d origin;
                    if (i < prevStarsCords.size())
                    {
                        origin = prevStarsCords[i];
                    }
                    else
                    {
                        origin = targetConstellation->originStarsCords[i];
                    }

                    point3d n = point3d(
                        noise(point3d(origin.y, origin.z, origin.x) + i * 0.1f),
                        noise(point3d(origin.z, origin.x, origin.y) + i * 0.1f),
                        noise(point3d(origin.x, origin.y, origin.z) + i * 0.1f)
                    ) * 2;
                    point3d localUp = (up * n).normalized();
                    point3d localRight = (right * n).normalized();

                    point3d p = starsCords[i];
                    starsCords[i] = p.lerp(
                        (localUp * sin(time * 10) + localRight * cos(time * 10)) * 0.15f,
                        morphProgress);
                }
            }
            else if (morphProgress <= 2.0f)
            {
                if (starsCords.size() != targetConstellation->originStarsCords.size())
                {
                    starsCords.clear();
                    float time = timer::GetCounter() / 500;
                    for (int i = 0; i < targetConstellation->originStarsCords.size(); i++) {
                        point3d origin = targetConstellation->originStarsCords[i];

                        point3d n = point3d(
                            noise(point3d(origin.y, origin.z, origin.x) + i * 0.1f),
                            noise(point3d(origin.z, origin.x, origin.y) + i * 0.1f),
                            noise(point3d(origin.x, origin.y, origin.z) + i * 0.1f)
                        ) * 2;
                        point3d localUp = (up * n).normalized();
                        point3d localRight = (right * n).normalized();

                        starsCords.push_back(
                            (localUp * sin(time * 10) + localRight * cos(time * 10)) * 0.15f);
                    }
                }

                if (constellationEdges.size() != targetConstellation->originConstellationEdges.size())
                {
                    constellationEdges.clear();
                    for (int i = 0; i < targetConstellation->originConstellationEdges.size(); i++) {
                        constellationEdges.push_back(targetConstellation->originConstellationEdges[i]);
                    }
                }

                for (int i = 0; i < targetConstellation->originStarsCords.size(); i++) {
                    point3d p = starsCords[i];
                    starsCords[i] = p.lerp(targetConstellation->originStarsCords[i], morphProgress - 1.0f);
                }
            }

            if (morphProgress >= 2.0f)
            {
                morphing = false;
            }
        }
    }

};