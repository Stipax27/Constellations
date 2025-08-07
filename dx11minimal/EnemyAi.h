
namespace Enemy
{
    class EnemyAI {
    private:
        EnemyData& data;

    public:
        EnemyAI(EnemyData& enemyData) : data(enemyData) {}

        void AiUpdate(float deltaTime,  point3d& heroPosition ,point3d& enemyPositions);
        point3d GetPosition() ;

    private:
        void Patrol(float deltaTime , point3d& enemyPositions);
        void Chase( point3d& heroPos , point3d& enemyPositions);
        //void UpdateRotation( point3d& direction);
    };
}


extern XMVECTOR enemyPositions;
void updateEnemyPosition(float deltaTime,  point3d& heroPosition);