namespace Enemy {
  
    enum class AIState {
        PATROL,
        CHASE,
        ORBIT,
        ATTACK
    };

    struct EnemyData {
        AIState currentState = AIState::PATROL;
        float playerDistance = 0.0f;
        bool playerVisible = false;
        std::vector<point3d> waypoints;
        int currentWaypoint = 0;
        float patrolSpeed = 10.0f;
        float chaseSpeed = 10.0f;
        float rotationSpeed = 0.1f;
        float patrolProgress = 0.f;
        float patrolRadius = 1000.0f;
        float orbitRadius = 15000.0f;  // Радиус орбиты вокруг игрока
        float orbitSpeed = .0001f;     // Скорость движения по орбите
        float attackCooldown = 1000.0f;  // Таймер между атаками
        float attackInterval = 1000.0f;  // Интервал между атаками (секунды)
        float attackDuration = 2.0f;  // Длительность атаки
        float attackTimer = 0.0f;  
        point3d lastOrbitPosition;// Таймер атаки
        bool isAttacking = false;

        XMVECTOR currentRotation;
        XMVECTOR ForwardEn;
        XMVECTOR defaultUp;
        XMVECTOR RightEn;
        XMVECTOR UpEn;
        XMMATRIX enemyConstellationOffset;

        
        EnemyData() :
            currentRotation(XMQuaternionIdentity()),
            ForwardEn(XMVectorSet(0, 0, 1, 0)),
            defaultUp(XMVectorSet(0, 1, 0, 0)),
            RightEn(XMVectorSet(1, 0, 0, 0)),
            enemyConstellationOffset(XMMatrixIdentity())
        {
            waypoints = {
                point3d(-50000.0f, 0.0f, 0.0f),
                point3d(0.0f, 0.0f, 25000.0f),
                point3d(25000.0f, 0.0f, 0.0f),
                point3d(0.0f, 0.0f, -50000.0f)
            };
            UpEn = XMVector3Rotate(defaultUp, currentRotation);
        }
    };
    extern EnemyData enemyData;
 
    class SplinePath {
    public:
        struct SplineSegment {
            point3d p0, p1, p2, p3;
            float length;
        };

        std::vector<SplineSegment> segments;
        float totalLength = 0.0f;

        void BuildSpline(const std::vector<point3d>& waypoints);
        point3d Evaluate(float t) const;

    private:
        point3d CubicBezier(point3d p0, point3d p1, point3d p2, point3d p3, float t) const;
    };


    class EnemyAI {
    public:
        EnemyData data;
        SplinePath splinePath;
        bool splineInitialized = false;
        float splineProgress = 0.0f;

        void AiUpdate(float deltaTime, point3d& heroPosition, point3d& enemyPositions);
        void Patrol(float deltaTime, point3d& enemyPositions);
        void Chase(point3d& heroPos, point3d& enemyPositions , float deltaTime);
        void UpdateRotation(point3d direction);
        void OrbitPlayer(float deltaTime, point3d& heroPos, point3d& enemyPos);
        void AttackPlayer(float deltaTime, point3d& heroPos, point3d& enemyPos);
    };

    static EnemyData enemyData;
}