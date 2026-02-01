#include "utils.h"

float clamp(float x, float a, float b) {
    return fmax(fmin(x, b), a);
}

float smoothstep(float edge0, float edge1, float x) {
    // Scale, bias and saturate x to 0..1 range
    clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    // Evaluate polynomial
    return x * x * (3 - 2 * x);
}

float fract(float a) {
    return a - floor(a);
}

float lerp(float x1, float x2, float a) {
    return x1 * (1 - a) + x2 * a;
}

float degreesToRadians(float degrees) {
    return degrees * PI / 180.0f;
}

point3d quaternionToEuler(float x, float y, float z, float w) {
    float magintude = sqrt(x * x + y * y + z * z);
    x /= magintude;
    y /= magintude;
    z /= magintude;

    float pitch = atan2(2 * (w * x + y * z), 1 - 2 * (pow(x, 2) + pow(y, 2)));
    float yaw = asin(2 * (w * y - z * x));
    float roll = atan2(2 * (w * z + x * y), 1 - 2 * (pow(y, 2) + pow(z, 2)));

    return point3d(pitch, yaw, roll);
}

DirectX::XMVECTOR eulerToQuanternion(float x, float y, float z) {
    DirectX::XMVECTOR qRotation = DirectX::XMQuaternionRotationRollPitchYaw(x, y, z);
    qRotation = DirectX::XMQuaternionNormalize(qRotation);

    return qRotation;
}

int getRandom(int min, int max) {
    if (max > min)
    {
        return rand() % (max - min) + min;
    }
    else
    {
        return rand() % min;
    }
}

point3d rotatePoint(point3d pos, float angle)
{
    float radius = pos.magnitude();
    float angle0 = atan2(pos.y, pos.x);
    angle0 += angle;

    return point3d(cos(angle0), sin(angle0), 0) * radius;
}

Transform GetWorldTransform(Entity* entity) {
    Transform worldTransform = Transform();

    Transform* originTransform = entity->GetComponent<Transform>();
    if (originTransform != nullptr) {

        vector<Transform*> transforms = { originTransform };

        Entity* ancestor = entity->parent;
        while (ancestor != nullptr) {

            Transform* parentTransform = ancestor->GetComponent<Transform>();
            if (parentTransform != nullptr) {
                transforms.push_back(parentTransform);
                ancestor = ancestor->parent;
            }
            else {
                break;
            }
        }

        for (int i = transforms.size() - 1; i >= 0; i--) {
            worldTransform += *transforms[i];
        }
    }

    return worldTransform;
}

Transform2D GetWorldTransform2D(Entity* entity) {
    Transform2D worldTransform2D = Transform2D();

    Transform2D* originTransform2D = entity->GetComponent<Transform2D>();
    if (originTransform2D != nullptr) {

        vector<Transform2D*> transforms2d = { originTransform2D };

        Entity* ancestor = entity->parent;
        while (ancestor != nullptr) {

            Transform2D* parentTransform2D = ancestor->GetComponent<Transform2D>();
            if (parentTransform2D != nullptr) {
                transforms2d.push_back(parentTransform2D);
                ancestor = ancestor->parent;
            }
            else {
                break;
            }
        }

        for (int i = transforms2d.size() - 1; i >= 0; i--) {
            worldTransform2D += *transforms2d[i];

            /*Transform2D other = *transforms2d[i];

            point3d aspectCorrection;
            switch (worldTransform2D.ratio)
            {
            case ScreenAspectRatio::XY:
                aspectCorrection = point3d(1, 1, 0);
                break;
            case ScreenAspectRatio::YX:
                aspectCorrection = point3d(ConstBuf::frame.aspect.x, ConstBuf::frame.aspect.y, 0);
                break;
            case ScreenAspectRatio::XX:
                aspectCorrection = point3d(1, ConstBuf::frame.aspect.y, 0);
                break;
            case ScreenAspectRatio::YY:
                aspectCorrection = point3d(ConstBuf::frame.aspect.x, 1, 0);
                break;
            }

            worldTransform2D.position += other.position * worldTransform2D.scale * aspectCorrection;
            worldTransform2D.position.x *= ConstBuf::frame.aspect.y;
            worldTransform2D.position = rotatePoint(worldTransform2D.position, worldTransform2D.rotation);
            worldTransform2D.position.x *= ConstBuf::frame.aspect.x;

            worldTransform2D.anchorPoint = other.anchorPoint;
            worldTransform2D.scale *= other.scale;
            worldTransform2D.rotation += other.rotation;
            worldTransform2D.ratio = other.ratio;*/
        }
    }

    return worldTransform2D;
}

bool IsEntityValid(Entity* entity) {
    return entity != nullptr && entity->IsActive();
}

bool IsKeyPressed(const int Key) {
    return GetAsyncKeyState(Key) & 0x8000;
}

vector<string> split(string s, string temp) {
    vector<string> tokens;
    size_t pos = 0;
    string token;

    while ((pos = s.find(temp)) != string::npos) {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + temp.length());
    }
    tokens.push_back(s);

    return tokens;
}

void SetLookVector(Transform* transform, point3d direction){
    point3d currentLookVector = transform->GetLookVector();
    point3d rotationAxis = currentLookVector.cross(direction).normalized();
    DirectX::XMVECTOR rotationAxisVector = DirectX::XMVectorSet(rotationAxis.x, rotationAxis.y, rotationAxis.z, 0.0f);
    float angleBetweenVectors = acosf(currentLookVector.dot(direction));
    DirectX::XMVECTOR quaternionRotation = DirectX::XMQuaternionRotationAxis(rotationAxisVector, angleBetweenVectors);
    DirectX::XMMATRIX matrixRotation = DirectX::XMMatrixRotationQuaternion(quaternionRotation);

    transform->mRotation = matrixRotation * transform->mRotation;
}

vector<point3d> smoothCornersPath(const vector<point3d>& pointsBefore, int numberIterations) {

    if (pointsBefore.size() <= 1) return pointsBefore;

    vector<point3d> pointsAfter;

    for (int i = 0; i < pointsBefore.size(); i++) {

        int iPrevious;
        int iNext;

        if (i == 0) {
            iPrevious = pointsBefore.size() - 1;
            iNext = i + 1;
        }
        else if (i == pointsBefore.size() - 1) {
            iPrevious = i - 1;
            iNext = 0;
        }
        else {
            iPrevious = i - 1;
            iNext = i + 1;
        }

        point3d previousPoint = pointsBefore[iPrevious];
        point3d currentPoint = pointsBefore[i];
        point3d nextPoint = pointsBefore[iNext];

        point3d localStartPoint = previousPoint.lerp(currentPoint, 0.6666f);
        point3d localEndPoint = currentPoint.lerp(nextPoint, 0.3333f);
        point3d supportivePoint = localStartPoint.lerp(localEndPoint, 0.5f);
        point3d localMiddlePoint = supportivePoint.lerp(currentPoint, 0.5f);

        pointsAfter.push_back(localStartPoint);
        pointsAfter.push_back(localMiddlePoint);
        pointsAfter.push_back(localEndPoint);

    }

    if (numberIterations <= 1) return pointsAfter;
    else return smoothCornersPath(pointsAfter, numberIterations-1);
}

float hueToRgb(float p, float q, float t) {
    if (t < 0.0f) t += 1.0f;
    if (t > 1.0f) t -= 1.0f;

    if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
    if (t < 1.0f / 2.0f) return q;
    if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
    return p;
}

point3d hslToRgb(const point3d& hsl) {
    float h = hsl.x;
    float s = hsl.y;
    float l = hsl.z;

    float r, g, b;

    if (s == 0.0f) {
        r = g = b = l;
    }
    else {
        float q = (l < 0.5f) ? l * (1.0f + s) : l + s - l * s;
        float p = 2.0f * l - q;

        r = hueToRgb(p, q, h + 1.0f / 3.0f);
        g = hueToRgb(p, q, h);
        b = hueToRgb(p, q, h - 1.0f / 3.0f);
    }

    return point3d(r, g, b);
}

point3d rgbToHsl(const point3d& rgb) {
    float r = rgb.x;
    float g = rgb.y;
    float b = rgb.z;

    float maxVal = max(r, g, b);
    float minVal = min(r, g, b);
    float h = 0.0f, s = 0.0f, l = (maxVal + minVal) / 2.0f;

    if (maxVal != minVal) {
        float diff = maxVal - minVal;
        s = (l > 0.5f) ? diff / (2.0f - maxVal - minVal) : diff / (maxVal + minVal);

        if (maxVal == r) {
            h = (g - b) / diff + ((g < b) ? 6.0f : 0.0f);
        }
        else if (maxVal == g) {
            h = (b - r) / diff + 2.0f;
        }
        else {
            h = (r - g) / diff + 4.0f;
        }

        h /= 6.0f;
    }

    return point3d(h, s, l);
}