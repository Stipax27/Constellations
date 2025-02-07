
std::vector <float> Aries = { 0, 0, 0., .21, .05, 0., .35, .12, 0., .43, .27, 0. };
std::vector <float> Aries_health = { 1,1,1,1 };
std::vector <float> UrsaMajor = { -.25, -.25, 0., -.1, -.2, 0.,-.05, .05, 0.,.25, .05,0., .35, .25,0., .05, .25,0., -.05, .05,0. };
std::vector <float> UrsaMajor_health = { 1,1,1,1,1,1,1 };
std::vector <float> PieceOfCancer1 = { 0, 0, 0, .04, .25, 0, .08, .7, 0 };
std::vector <float> PieceOfCancer1_health = { 1,1,1 };
std::vector <float> Taurus = { 0,0,0, .08, .7, .25, .04 };
std::vector <float> Taurus_health = { 1,1 };
std::vector <float> Gemini = {};
std::vector <float> Gemini_health = {};
std::vector <float> Leo = {};
std::vector <float> Leo_health = {};

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
   arrange—onstellation(PieceOfCancer1, -20, -20, 0);
   arrange—onstellation(Taurus, 20, -30, 0);

}
