//class SpaceCamera {
//public:
//    point3d position;  // Позиция в пространстве
//    point3d forward;   // Направление "вперед"
//    point3d right;     // Направление "вправо"
//    point3d up;        // Направление "вверх"
//
//    float moveSpeed = 0.1f;
//    float rotationSpeed = 1.0f;
//
//    SpaceCamera() {
//        
//        position = { 0, 0, 0 };
//        forward = { 0, 0, 1 };
//        right = { 1, 0, 0 };
//        up = { 0, 1, 0 };
//    }
//
//    
//    void updateVectors() {
//        
//        forward = forward / sqrt(forward.x * forward.x + forward.y * forward.y + forward.z * forward.z);
//
//        
//        right = {
//            forward.y * up.z - forward.z * up.y,
//            forward.z * up.x - forward.x * up.z,
//            forward.x * up.y - forward.y * up.x
//        };
//        right = right / sqrt(right.x * right.x + right.y * right.y + right.z * right.z);
//
//        
//        up = {
//            right.y * forward.z - right.z * forward.y,
//            right.z * forward.x - right.x * forward.z,
//            right.x * forward.y - right.y * forward.x
//        };
//    }
//
//    // Движение вперед/назад
//    void moveForward(float amount) {
//        position += forward * amount;
//    }
//
//    // Движение вправо/влево
//    void moveRight(float amount) {
//        position += right * amount;
//    }
//
//    // Движение вверх/вниз
//    void moveUp(float amount) {
//        position += up * amount;
//    }
//
//   
//    void pitch(float angle) {
//        float rad = angle * PI / 180.0f;
//        float cosA = cos(rad);
//        float sinA = sin(rad);
//
//        point3d newForward = {
//            forward.x,
//            forward.y * cosA - forward.z * sinA,
//            forward.y * sinA + forward.z * cosA
//        };
//
//        forward = newForward;
//        updateVectors();
//    }
//
//    
//    void yaw(float angle) {
//        float rad = angle * PI / 180.0f;
//        float cosA = cos(rad);
//        float sinA = sin(rad);
//
//        point3d newForward = {
//            forward.x * cosA + forward.z * sinA,
//            forward.y,
//            -forward.x * sinA + forward.z * cosA
//        };
//
//        forward = newForward;
//        updateVectors();
//    }
//
//    
//    void roll(float angle) {
//        float rad = angle * PI / 180.0f;
//        float cosA = cos(rad);
//        float sinA = sin(rad);
//
//        point3d newRight = {
//            right.x * cosA - right.y * sinA,
//            right.x * sinA + right.y * cosA,
//            right.z
//        };
//
//        right = newRight;
//        updateVectors();
//    }
//};
//
//void CamerhandleInput(SpaceCamera& camera) {
//    
//    if (GetKeyState('W') & 0x8000) {
//        camera.moveForward(camera.moveSpeed);
//    }
//    if (GetKeyState('S') & 0x8000) {
//        camera.moveForward(-camera.moveSpeed);
//    }
//    if (GetKeyState('A') & 0x8000) {
//        camera.moveRight(-camera.moveSpeed);
//    }
//    if (GetKeyState('D') & 0x8000) {
//        camera.moveRight(camera.moveSpeed);
//    }
//    if (GetKeyState('Q') & 0x8000) { 
//        camera.moveUp(-camera.moveSpeed);
//    }
//    if (GetKeyState('E') & 0x8000) {  
//        camera.moveUp(camera.moveSpeed);
//    }
//
//    // Вращение
//    if (GetKeyState(VK_UP) & 0x8000) {
//        camera.pitch(-camera.rotationSpeed);
//    }
//    if (GetKeyState(VK_DOWN) & 0x8000) {
//        camera.pitch(camera.rotationSpeed);
//    }
//    if (GetKeyState(VK_LEFT) & 0x8000) {
//        camera.yaw(-camera.rotationSpeed);
//    }
//    if (GetKeyState(VK_RIGHT) & 0x8000) {
//        camera.yaw(camera.rotationSpeed);
//    }
//    if (GetKeyState('Z') & 0x8000) {  
//        camera.roll(-camera.rotationSpeed);
//    }
//    if (GetKeyState('X') & 0x8000) { 
//        camera.roll(camera.rotationSpeed);
//    }
//}
//
//void renderScene(const SpaceCamera& camera) {
//    
//    RECT clientRect;
//    GetClientRect(window.hWnd, &clientRect);
//    HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0)); 
//    FillRect(window.context, &clientRect, brush);
//    DeleteObject(brush);
//
//   
//    for (auto& star : stars) {
//        
//        point3d viewSpace = star - camera.position;
//
//        // Проекция (только то, что перед камерой)
//        if (viewSpace.z > nearPlaneClip) {  
//            float scale = 1000.0f / viewSpace.z;
//
//            point3d screenPoint = {
//                window.width / 2 + viewSpace.x * scale,
//                window.height / 2 - viewSpace.y * scale,
//                viewSpace.z
//            };
//
//            
//            float starSize = def_size * (1000.0f / viewSpace.z);
//            starSize = std::clamp(starSize, 1.0f, def_size); // Ограничиваем размер
//
//            
//            screenPoint.draw(screenPoint, starSize);
//        }
//    }
//}
