bool isShaking = false;
float shakeIntensity = 10; 
float shakeDuration = 5000;   
float shakeTime = 0;  

void Shaking(point3d &p) 
{
    if (isShaking) {
        float shakeOffsetX = (rand() % 100 / 100 - .5) * shakeIntensity;
        float shakeOffsetY = (rand() % 100 / 100 - .5) * shakeIntensity;
        p.x += shakeOffsetX;
        p.y += shakeOffsetY;
        shakeTime += 0.016; 
        if (shakeTime >= shakeDuration) {
            isShaking = false;
        }
    }
}
void startShake() {
    isShaking = true;
    shakeTime = 0;
}

void updateConstellation(std::vector<point3d>& starsCords) {
    for (int i = 0; i < starsCords.size(); i++) {
        Shaking(starsCords[i]);
    }
}