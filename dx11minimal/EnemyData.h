namespace Enemy
{
    enum class AIState {
        PATROL,
        CHASE,
        ATTACK,
        SEARCH
    };

    class EnemyData {
    public:
       
        AIState currentState;
        float playerDistance;
        bool playerVisible;
        int currentWaypoint;

      
        float patrolSpeed;
        float chaseSpeed;
        float rotationSpeed;

       
        point3d currentDirection;
        XMVECTOR currentRotation;
        XMVECTOR ForwardEn;
        XMVECTOR defaultUp;
        XMVECTOR RightEn;
        XMVECTOR UpEn;

       
        std::vector<point3d> waypoints;
        XMMATRIX enemyConstellationOffset;

        EnemyData();
    };
}
