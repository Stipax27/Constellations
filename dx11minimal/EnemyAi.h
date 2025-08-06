
namespace Enemy
{
    class EnemyAI {
    private:
        EnemyData& data;

    public:
        EnemyAI(EnemyData& enemyData) : data(enemyData) {}

        void AiUpdate(float deltaTime, const point3d& heroPosition);
        point3d GetPosition() const;

    private:
        void Patrol(float deltaTime);
        void Chase(const point3d& heroPos);
        void UpdateRotation(const point3d& direction);
    };
}


extern XMVECTOR enemyPositions;
void updateEnemyPosition(float deltaTime, const point3d& heroPosition);