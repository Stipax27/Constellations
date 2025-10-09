namespace CameraTargeting
{
    TargetingData data;

    void StartTargeting(int enemyID)
    {
        if (enemyID >= 0 && enemyID < starSet.size() && starSet[enemyID])
        {
            data.isTargeting = true;
            data.targetEnemyID = enemyID;
            data.currentTransition = 0.0f;
        }
    }

    void StopTargeting()
    {
        data.isTargeting = false;
        data.targetEnemyID = -1;
        data.currentTransition = 0.0f;
    }

    void ToggleTargeting(int enemyID)
    {
        if (data.isTargeting && data.targetEnemyID == enemyID)
        {
            StopTargeting();
        }
        else
        {
            StartTargeting(enemyID);
        }
    }

    void UpdateTargeting(float deltaTime)
    {
        if (data.isTargeting && data.targetEnemyID != -1)
        {
            // Плавное увеличение прогресса таргетинга
            data.currentTransition += data.transitionSpeed * deltaTime / 1000.0f;
            if (data.currentTransition > 1.0f) data.currentTransition = 1.0f;

            // Проверяем, жив ли еще целевой враг
            if (data.targetEnemyID >= starSet.size() ||
                !starSet[data.targetEnemyID])
            {
                StopTargeting();
            }
        }
        else if (data.currentTransition > 0.0f)
        {
            // Плавное уменьшение при отмене таргетинга
            data.currentTransition -= data.transitionSpeed * deltaTime / 1000.0f;
            if (data.currentTransition < 0.0f) data.currentTransition = 0.0f;
        }
    }

    bool IsTargeting()
    {
        return data.isTargeting && data.targetEnemyID != -1;
    }

    int GetTargetEnemyID()
    {
        return data.targetEnemyID;
    }

    XMVECTOR GetTargetPosition()
    {
        if (IsTargeting() && data.targetEnemyID >= 0 && data.targetEnemyID < starSet.size() && starSet[data.targetEnemyID])
        {
            Constellation& targetEnemy = *starSet[data.targetEnemyID];
            XMMATRIX enemyTransform = CreateEnemyToWorldMatrix(targetEnemy);
            return enemyTransform.r[3]; // Возвращаем позицию врага
        }
        return XMVectorSet(0, 0, 0, 0);
    }
}