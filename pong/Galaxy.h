
class SpiralGalaxy {
private:

    float tiltAngle;
    float rotationAngle;
    point3d position;
    int armsCount;
    float armWidth;
    float rotationSpeed;
   
    void generateStar(point3d& p) {

        float diskRadius = 1000.0f;
        float coreRadius = 80.0f;
        float thickness = 40.0f;


        float distance = coreRadius + (rand() % (int)(diskRadius - coreRadius));
        float height = (rand() % (int)thickness) - thickness / 2;


        int arm = rand() % armsCount;
        float armAngle = arm * (360.0f / armsCount);


        float spiralFactor = 3.5f * log(distance / coreRadius + 1);
        float angle = armAngle + spiralFactor * (distance / diskRadius) * 360;


        float randomOffset = (rand() % 100) / 100.0f * armWidth * distance;
        angle += randomOffset;


        p.x = distance * cos(angle * PI / 180.);
        p.y = distance * sin(angle * PI / 180.);
        p.z = height;


        if (rand() % 100 < 20) {// Процент Звёзд в центре 
            float coreDistance = rand() % (int)coreRadius;
            float coreAngle = rand() % 360;
            p.x = coreDistance * cos(coreAngle * PI / 180.);
            p.y = coreDistance * sin(coreAngle * PI / 180.);
        }
    }
public:

    SpiralGalaxy() {
        tiltAngle = 45.0f;
        rotationAngle = 0;
        position.x = window.width / 6;
        position.y = window.height / 6;
        position.z = 1000;
        armsCount; 
        armWidth; 
        rotationSpeed;
    }

    void update() { 
        rotationAngle += rotationSpeed;
        if (rotationAngle > 360) 
        {
            rotationAngle -= 360;        
        }
    }

    void draw() {

        srand(10);

        for (int i = 0; i < 6000; i++) {

            point3d star;
            generateStar(star);

            
            float distance = sqrt(star.x * star.x + star.y * star.y);   // Растояние от центра до края 
            float normalizedDist = min(distance / 300.0f, 1.0f);

            COLORREF starColor;

            
            if (distance < 100) {   // Цвета центра рандом для красоты
                int red = 255;
                int green = 100 + rand() % 156;
                int blue = rand() % 50;

                starColor = RGB(red, green, blue);
            }
            else if(distance < 400) {  // Цвета Спиралей тоже с рандомом
                int blue = 100 + rand() % 55;
                starColor = RGB(200 + rand() % 55, 100 + rand() % 44, blue);
            }
            else 
            {
                int blue = 200 + rand() % 55;
                starColor = RGB(150 + rand() % 105, 150 + rand() % 105, blue);
            }

            HBRUSH hBrush = CreateSolidBrush(starColor);
            HPEN hPen = CreatePen(PS_SOLID, 1, starColor);

            SelectObject(window.context, hPen);
            SelectObject(window.context, hBrush);

            
            star.rotateZ(star, rotationAngle);
            star.rotateX(star, tiltAngle);

            star.x += position.x;
            star.y += position.y;
            star.z += position.z;

            modelTransform(star, Aries);
            modelProject(star);

            
            int size = 1;
            if (distance < 80) size = 2 + rand() % 2;

            star.draw(star, size);

            DeleteObject(hPen);
            DeleteObject(hBrush);
        }
    }
    // Всё управление галактикой 
    void setTiltAngle(float angle) { tiltAngle = angle; } 
    float getTiltAngle() const { return tiltAngle; }

    void setArmCount(int count) { armsCount = (count > 0) ? count : 1; }
    int getArmCount() const { return armsCount; }

    void setArmWidth(float width) { armWidth = (width >= 0 && width <= 1) ? width : 0.15f; }
    float getArmWidth() const { return armWidth; }

    void setPosition(float x, float y, float z) {
        position.x = x;
        position.y = y;
        position.z = z;
    }

    void setRotationSpeed(float speed) { rotationSpeed = speed; }
    float getRotationSpeed() const { return rotationSpeed; }

};