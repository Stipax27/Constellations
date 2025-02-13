
std::vector <float> Aries = { 0, 0, 0., .21, .05, 0., .35, .12, 0., .43, .27, 0. };
std::vector <float> Aries_health = { 1,1,1,1 };
std::vector <float> UrsaMajor = { -.25, -.25, 0., -.1, -.2, 0.,-.05, .05, 0.,.25, .05,0., .35, .25,0., .05, .25,0., -.05, .05,0. };
std::vector <float> UrsaMajor_health = { 1,1,1,1,1,1,1 };
std::vector <float> Cancer = { 0, 0, 0, .04, .25, 0, .08, .7, 0, .04, .25, 0, .5, .5, 0 };
std::vector <float> Cancer_health = { 1,1.5,1,1,1,1 };
std::vector <float> Taurus = { -1, -.7, 0, -.3, -.3, 0, -.1, -.1, 0, 0,0,0, -.2, -.01, 0, -1.2, -.1, 0, -.2, -.01, 0, 0,0,0, .3, .1, 0, .05, .4, 0, .3, .5, 0, .05, .4, 0, .3, .1, 0, .75, .05, 0, .9, .25,0 };
std::vector <float> Taurus_health = { 1, 1,1,1.5,1,1,1,1,1,1,1,1,1,1,1 };
std::vector <float> Gemini = {-.15, 1.15, 0, -.15, .85, 0, -.3, 1.2, 0, -.15, .85, 0, -.3, .6, 0, -.2, 0, 0, -.5, .05, 0, -.2, 0, 0, -.25, -.2, 0, -.2, 0, 0, 0, 0, 0, .4, -.01, 0, .35, -.3, 0,
.4, -.01, 0, .9, -.01, 0, .4, -.01, 0, .4, .7, 0, .35, 1.1, 0, .4, .7, 0, .5, 1, 0, .65, 1.1, 0};
std::vector <float> Gemini_health = {1,1,1,1,1,1,1,1,1,1,1.5,1,1,1,1,1,1,1,1,1,1};
std::vector <float> Leo = {.8, -.2, 0, .75, -.33, 0, .35, -.2, 0, .3, .01, 0, 0,0,0, -.3, -.01, 0, -.75, .3, 0, -.3, .29, 0, .5, .45, 0, .5, .2, 0, .3, .01, 0};
std::vector <float> Leo_health = {1,1,1,1,1,1,1,1,1,1,1};
std::vector <float> Libra = { -.2,.7,0, .2,.3,0, .3,-.1,0, -.2,-.3,0,  0,0,0, .2,.3,0, 0,0,0, -.2,-.3,0, -.4,-.1,0, -.6,-.05,0 };
std::vector <float> Libra_health = { 1,1,1,1,1.5,1,1,1,1,1 };
std::vector <float> Virgo = {-1.9, -.61, 0, -1.2, -.6, 0, -.6, -.5, 0, .5, -.9, 0, .4, -1.9, 0, .5, -.9, 0, .8, -.25, 0, 2, -.4, 0, .8, -.25, 0, 0, 0, 0, -.35, .4, 0, -.6, -.5, 0, -.35, .4, 0,
-1.3, .38, 0, -1.4, .01, 0, -1.85, 0, 0};
std::vector <float> Virgo_health{1, 1, 1, 1, 1, 1, 1, 1, 1, 1.5, 1, 1, 1, 1, 1, 1};
std::vector <float> Scorpius{-.95, .7, 0, -1, .8, 0, -1.05, .9, 0, -.8, 1.15, 0, -.5, 1.1, 0, -.32, 1, 0, -.31, .8, 0, -.3, .6, 0, -.08, .1, 0, 0, 0, 0, .1, -.04, 0, .13, .07, 0, .35, .15, 0, .1, -.04, 0, .1, -.15, 0,
.12, -.3, 0, .25, -.28, 0, .1, -.15, 0};
std::vector <float> Scorpius_health{1, 1, 1, 1, 1, 1, 1, 1, 1, 1.5, 1, 1, 1, 1, 1, 1, 1, 1};

void arrange—onstellation(std::vector <float>& starArray, float angleX, float angleY, float angleZ)
{
    int starsCount = starArray.size() / 3;
    float scale = 1000;
    for (int i = 0; i < starsCount; i++)// –¿ÁÏÂ˘ÂÌËÂ ÀËÌËÈ.
    {
        point3d p = { starArray[i * 3 + 0], starArray[i * 3 + 1], starArray[i * 3 + 2] };

        move(p, 0, 0, 3000. / scale);
        rotateX(p, angleX);
        rotateY(p, angleY);
        rotateZ(p, angleZ);

        starArray[i * 3 + 0] = p.x * scale;
        starArray[i * 3 + 1] = p.y * scale;
        starArray[i * 3 + 2] = p.z * scale;
    }
}

void initWorld()
{
   arrange—onstellation(Aries, -17, -25, 0);
   arrange—onstellation(UrsaMajor, -15, -60, 0);
   arrange—onstellation(Cancer, -20, -20, 0);
   arrange—onstellation(Taurus, -90, 40, 50);
   arrange—onstellation(Leo, 400, -150, -100);
   arrange—onstellation(Gemini, -200, 0, 0);
   arrange—onstellation(Libra, -300, 0, 0);
   arrange—onstellation(Virgo, 250, 250, 0);
   arrange—onstellation(Scorpius, 50, 70, 0);

}
