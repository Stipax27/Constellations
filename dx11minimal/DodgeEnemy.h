bool isDamageTaken = false;
bool isShaking = false;

float shakeIntensity = .1;
float shakeDuration = 600;
float shakeTime = 0;
float shakeStartTime = 0;

void Shaking(point3d& p)
{
    if (isShaking) {


        float shakeAmp = shakeIntensity * (1. - (currentTime - shakeStartTime) / shakeDuration);

        float shakeOffsetX = ((rand() % 100) / 100. - .5) * shakeAmp;
        float shakeOffsetY = ((rand() % 100) / 100. - .5) * shakeAmp;

        p += shakeOffsetX;

    }
}







