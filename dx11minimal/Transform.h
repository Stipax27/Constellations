#include "MainWindow.h"
#include "Point3d.h"
#include "mouse.h"
#include "Constellation.h"
#include "DodgeEnemy.h"
#include "StatusGame.h"
#include <algorithm>

using namespace std;


float finalStarRad = 0;
float attackStartTime = 0;
float line_hit;
float attack_time;
float attack_cooldown;

bool check_attack = true;
bool attack_collision = false;
bool attack_speed = false;
bool attack_start = false;
bool attackCooldown = true;
bool T = false;

void (*modelTransform)(point3d& p, Constellation& Constellation);
void (*modelProject)(point3d& p);
void (*uiFunc)(point3d& point, Constellation& Constellation, int i);


void placeToWorld(point3d& p, Constellation& Constellation)
{
    p.rotateX(p, mouse.Angle.y * 0.1);
    p.rotateY(p, mouse.Angle.x * 0.1);
}


void placeConstToWorld(point3d& p, Constellation& Constellation)
{
    Shaking(p);

    p.move(0, 0, 3000. / Constellation.scale);
    p.rotateX(p, Constellation.angle.x);
    p.rotateY(p, Constellation.angle.y);
    p *= Constellation.scale;

    p.rotateX(p, mouse.Angle.y * 0.1);
    p.rotateY(p, mouse.Angle.x * 0.1);
}


float get_lenghts(point3d& point1, point3d& point2)
{
    float a = point1.x - point2.x;
    float b = point1.y - point2.y;
    float c = sqrt(a * a + b * b);
    return c;
}


void placeHeroToWorld(point3d& p, Constellation& Constellation)
{
    ShakingHero(p);
    p.move(0, 0, 0. / Constellation.scale);

    // fightMove();

    p *= .13;

    p *= Constellation.scale;

    p.rotateX(p, mouse.Angle.y * 0.1);
    p.rotateY(p, mouse.Angle.x * 0.1);
}