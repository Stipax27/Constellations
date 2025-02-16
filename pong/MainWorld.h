
std::vector <point3d> UrsaMajor =
{
    {-.25, -.25, 0.},
    {-.1, -.2, 0.},
    {-.05, .05, 0.},
    {.25, .05, 0.},
    {.35, .25, 0.},
    {.05, .25, 0.},
    {-.05, .05, 0.}
};
std::vector <float> UrsaMajor_health = { 1,1,1,1,1,1,1 };

std::vector <point3d> Aries =
{
    {0, 0, 0.},
    {.21, .05, 0.},
    {.35, .12, 0.},
    {.43, .27, 0.}
};

std::vector <std::vector <float>> Aries_indices =
{
    {0, 1},
    {1, 2},
    {2, 3},
    {3, 4}
};

std::vector <float> Aries_health = { 1,1,1,1 };

std::vector <point3d> Taurus =
{
    {-1, -.7, 0},
    {-.3, -.3, 0},
    {-.1, -.1, 0},
    {0, 0, 0},
    {-.2, -.01, 0},
    {-1.2, -.1, 0},
    {-.2, -.01, 0},
    {0, 0, 0},
    {.3, .1, 0},
    {.05, .4, 0},
    {.3, .5, 0},
    {.05, .4, 0},
    {.3, .1, 0},
    {.75, .05, 0},
    {.9, .25,0}
};

std::vector <std::vector <float>> Taurus_indices =
{
    {0, 1},
    {1, 2},
    {2, 3},
    {3, 4},
    {4, 5},
    {3, 8},
    {8, 9},
    {9, 10},
    {8, 13},
    {13, 14}
};

std::vector <float> Taurus_health = { 1, 1,1,1.5,1,1,1,1,1,1,1,1,1,1,1 };

std::vector <point3d> Gemini =
{
    {-.15, 1.15, 0},
    {-.15, .85, 0},
    {-.3, 1.2, 0},
    {-.15, .85, 0},
    {-.3, .6, 0},
    {-.2, 0, 0},
    {-.5, .05, 0},
    {-.2, 0, 0},
    {-.25, -.2, 0},
    {-.2, 0, 0},
    {0, 0, 0},
    {.4, -.01, 0},
    {.35, -.3, 0},
    {.4, -.01, 0},
    {.9, -.01, 0},
    {.4, -.01, 0},
    {.4, .7, 0},
    {.35, 1.1, 0},
    {.4, .7, 0},
    {.5, 1, 0},
    {.65, 1.1, 0}
};

std::vector <std::vector <float>> Gemini_indices =
{
    {0, 1},
    {1, 2},
    {1, 4},
    {4, 5},
    {5, 6},
    {5, 8},
    {5, 10},
    {10, 11},
    {11, 12},
    {11, 14},
    {11, 16},
    {16, 17},
    {16, 19},
    {19, 20}
};

std::vector <float> Gemini_health = { 1,1,1,1,1,1,1,1,1,1,1.5,1,1,1,1,1,1,1,1,1,1 };

std::vector <point3d> Cancer =
{
    {-.09, -.7, 0},
    {-.05, -.15, 0},
    {0, 0, 0},
    {-.4, .5, 0},
    {0, 0, 0},
    {.4, .3, 0}
    
};

std::vector <std::vector <float>> Cancer_indices =
{
    {0,1},
    {1,2},
    {2,3},
    {2,5}
};

std::vector <float> Cancer_health = { 1,1,1,1,1,1 };

std::vector <point3d> Leo =
{
    {.8, -.2, 0},
    {.75, -.33, 0},
    {.35, -.2, 0},
    {.3, .01, 0},
    {0, 0, 0},
    {-.3, -.01, 0},
    {-.75, .3, 0},
    {-.3, .29, 0},
    {.5, .45, 0},
    {.5, .2, 0},
    {.3, .01, 0}
};

std::vector <std::vector <float>> Leo_indices =
{
    {0, 1},
    {1, 2},
    {2, 3},
    {3, 4},
    {4, 5},
    {5, 6},
    {6, 7},
    {7, 8},
    {8, 9},
    {9, 3}
};

std::vector <float> Leo_health = { 1,1,1,1,1,1,1,1,1,1,1 };

std::vector <point3d> Virgo =
{
    {-1.9, -.61, 0},
    {-1.2, -.6, 0},
    {-.6, -.5, 0},
    {.5, -.9, 0},
    {.4, -1.9, 0},
    {.5, -.9, 0},
    {.8, -.25, 0},
    {2, -.4, 0},
    {.8, -.25, 0},
    {0, 0, 0},
    {-.35, .4, 0},
    {-.6, -.5, 0},
    {-.35, .4, 0},
    {-1.3, .38, 0},
    {-1.4, .01, 0},
    {-1.85, 0, 0}
};

std::vector <std::vector <float>> Virgo_indices =
{
    {0, 1},
    {1, 2},
    {2, 3},
    {3, 4},
    {3, 6},
    {6, 7},
    {6, 9},
    {9, 10},
    {10, 2},
    {10, 11},
    {11, 12},
    {12, 13}
};

std::vector <float> Virgo_health{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1.5, 1, 1, 1, 1, 1, 1 };

std::vector <point3d> Libra =
{
    {-.2,.7,0},
    {.2,.3,0},
    {.3,-.1,0},
    {-.2,-.3,0},
    {0, 0, 0},
    {.2,.3,0},
    {-.2,-.3,0},
    {-.4,-.1,0},
    {-.6, -.05, 0}
};

std::vector <std::vector <float>> Libra_indices =
{
    {0, 1},
    {1, 2},
    {2, 3},
    {3, 4},
    {4, 1},
    {3, 8},
    {8, 9}

};

std::vector <float> Libra_health = { 1,1,1,1,1.5,1,1,1,1,1 };

std::vector <point3d> LibraCopy1 ={};
std::vector <point3d> LibraHeroCopy = {};

// Add edges
std::vector<std::vector<float>> Libra_edges =
{
    {0, 1},
    {0, 2},
    {0, 3},
    {0, 4},
    {0, 5},
    {0, 6},
    {0, 7},
    {0, 8},
    {0, 9},
};


std::vector <float> Libra_health = { 1,1,1,1,1.5,1,1,1,1};



std::vector <point3d> Scorpius =
{
    {-.95, .7, 0},
    {-1, .8, 0},
    {-1.05, .9, 0},
    {-.8, 1.15, 0},
    {-.5, 1.1, 0},
    {-.32, 1, 0},
    {-.31, .8, 0},
    {-.3, .6, 0},
    {-.08, .1, 0},
    {0, 0, 0},
    {.1, -.04, 0},
    {.13, .07, 0},
    {.35, .15, 0},
    {.1, -.04, 0},
    {.1, -.15, 0},
    {.12, -.3, 0},
    {.25, -.28, 0},
    {.1, -.15, 0}
};

std::vector <std::vector <float>> Scorpius_indices =
{
    {0,1},
    {1,2},
    {2,3},
    {3,4},
    {4,5},
    {5,6},
    {6,7},
    {7,8},
    {8,9},
    {9,10},
    {10,11},
    {11,12},
    {12,10},
    {10,14},
    {14,15},
    {15,16},
    {16,14}
};


std::vector <float> Scorpius_health{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1.5, 1, 1, 1, 1, 1, 1, 1, 1 };

std::vector <point3d> Sagittarius =
{
    {1, 1, 0},
    {.8, .9, 0},
    {.8, .3, 0},
    {1.2, .5, 0},
    {1.6, .2, 0},
    {1.2, .5, 0},
    {.8, .3, 0},
    {.4, -.1, 0},
    {1,-.6 , 0},
    {.4, -.1, 0},
    {0, 0, 0},
    {-.3,.3, 0},
    {-.5,.15, 0},
    {-.3,-.3, 0},
    {0, 0, 0},
    {-.3,.3, 0},
    {-.5,.15, 0},
    {-1.5,.15, 0},
    {-1.7, 1.1, 0},
    {-1, 1.6, 0},
    {-.5, 1.3, 0},
    {-1, 1.6, 0},
    {-.4, 1.6, 0},
};

std::vector <std::vector <float>> Sagittarius_indices =
{
    {0,1},
    {1,2},
    {2,3},
    {3,4},
    {2,7},
    {7,8},
    {7,10},
    {10,11},
    {11,12},
    {12,13},
    {13,10},
    {12,17},
    {17,18},
    {18,19},
    {19,20},
    {19,22}
};

std::vector <float> Sagittarius_health{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

std::vector <point3d> Capricornus =
{
   {1.6, -.5, 0}, 
   {1.5, -.3 ,0},
   {0,0,0},
   {-.5, -.1, 0},
   {-1, -.1, 0},
   {-1.3, -.2, 0},
   {-1.15, .1, 0},
   {-.55, .8, 0},
   {0, 1, 0},
   {.3, 1.2, 0},
   {.4, 1.1, 0},
   {1.3, .1, 0},
   {1.5, -.3 ,0},
};

std::vector <std::vector <float>> Capricornus_indices =
{
    {0,1},
    {1,2},
    {2,3},
    {3,4},
    {4,5},
    {5,6},
    {6,7},
    {7,8},
    {8,9},
    {9,10},
    {10,11},
    {11,1}
};

std::vector <float> Capricornus_health{1,1,1,1,1,1,1,1,1,1,1,1,1};

std::vector <point3d> Aquarius =
{
   {1.2, .6, 0},
   {.6, .3, 0},
   {0,0,0},
   {-.2, .4, 0},
   {0, .7, 0},
   {-.2, .4, 0},
   {0,0,0},
   {-.3, .1, 0},
   {-1, .3, 0},
   {-1.3, .75, 0},
   {-1, .3, 0},
   {-.8, .4, 0},
   {-.75, .7, 0},
   {-.9, .8, 0}

};

std::vector <std::vector <float>> Aquarius_indices =
{
    {0,1},
    {1,2},
    {2,3},
    {3,4},
    {2,7},
    {7,8},
    {8,9},
    {8,11},
    {11,12},
    {12,13}

};

std::vector <float> Aquarius_health{1,1,1,1,1,1,1,1,1,1,1,1,1,1};

std::vector<point3d> Pisces =
{
   {1.5, .1, 0},
   {1.3, .2, 0},
   {1.15, 0, 0},
   {1, .1, 0},
   {.8, .05, 0},
   {0,0,0},
   {-.5, 0, 0},
   {-1, .3, 0},
   {-1.2, .5, 0},
   {-1, -.3, 0},
   {-.9, -.6, 0},
   {-.9, -.8, 0},
   {-1, -1, 0},
   {-.8, -1.2, 0}  
};

std::vector <std::vector <float>> Pisces_indices =
{
    {0,1},
    {1,2},
    {2,3},
    {3,4},
    {4,5},
    {5,6},
    {6,7},
    {7,8},
    {8,9},
    {9,10},
    {10,11},
    {11,12},
    {12,13}
};

std::vector <float> Pisces_health{1,1,1,1,1,1,1,1,1,1,1,1,1,1};

void arrange—onstellation(std::vector <point3d>& starArrayData, std::vector <point3d>& starArrayRendering, float angleX, float angleY, float angleZ)
{
    starArrayRendering = starArrayData;
    int starsCount = starArrayRendering.size();
    float scale = 1000;
    for (int i = 0; i < starsCount; i++)// –¿ÁÏÂ˘ÂÌËÂ ÀËÌËÈ.
    {
        point3d p = { starArrayRendering[i].x, starArrayRendering[i].y, starArrayRendering[i].z };

        move(p, 0, 0, 3000. / scale);
        rotateX(p, angleX);
        rotateY(p, angleY);
        rotateZ(p, angleZ);

        starArrayRendering[i].x = p.x * scale;
        starArrayRendering[i].y = p.y * scale;
        starArrayRendering[i].z = p.z * scale;
    }
}

void initWorld()
{
   //arrange—onstellation(Aries, -17, -25, 0);
   //arrange—onstellation(UrsaMajor, -15, -60, 0);
   //arrange—onstellation(Cancer, -20, -20, 0);
   //arrange—onstellation(Taurus, -90, 40, 50);
   //arrange—onstellation(Leo, 400, -150, -100);
   //arrange—onstellation(Gemini, -200, 0, 0);
   arrange—onstellation(Libra, LibraCopy1, 0, 0, 0);
   arrange—onstellation(Libra, LibraHeroCopy, -19, -55, -5);

   //arrange—onstellation(Libra, 100, 250, 250);
   //arrange—onstellation(Virgo, 250, 250, 0);
   //arrange—onstellation(Scorpius, 50, 70, 0);
   //arrange—onstellation(Sagittarius, 600, 0, 0);
   //arrange—onstellation(Capricornus, 0, 600, 0);
   //arrange—onstellation(Aquarius, 200, 800, 0);
   //arrange—onstellation(Pisces, 0, 0, 0);
}

using namespace std;

enum ZodiacSign {
    ARIES, TAURUS, GEMINI, CANCER, LEO, VIRGO,
    LIBRA, SCORPIO, SAGITTARIUS, CAPRICORN, AQUARIUS, PISCES
};

ZodiacSign getZodiacSign(int day, int month) {
    if ((month == 3 && day >= 21) || (month == 4 && day <= 19)) return ARIES;
    if ((month == 4 && day >= 20) || (month == 5 && day <= 20)) return TAURUS;
    if ((month == 5 && day >= 21) || (month == 6 && day <= 20)) return GEMINI;
    if ((month == 6 && day >= 21) || (month == 7 && day <= 22)) return CANCER;
    if ((month == 7 && day >= 23) || (month == 8 && day <= 22)) return LEO;
    if ((month == 8 && day >= 23) || (month == 9 && day <= 22)) return VIRGO;
    if ((month == 9 && day >= 23) || (month == 10 && day <= 22)) return LIBRA;
    if ((month == 10 && day >= 23) || (month == 11 && day <= 21)) return SCORPIO;
    if ((month == 11 && day >= 22) || (month == 12 && day <= 21)) return SAGITTARIUS;
    if ((month == 12 && day >= 22) || (month == 1 && day <= 19)) return CAPRICORN;
    if ((month == 1 && day >= 20) || (month == 2 && day <= 18)) return AQUARIUS;
    if ((month == 2 && day >= 19) || (month == 3 && day <= 20)) return PISCES;

    throw invalid_argument("Invalid date");
}

string zodiacSignToString(ZodiacSign sign) {
    switch (sign) {
    case ARIES: return "Œ‚ÂÌ";
    case TAURUS: return "“ÂÎÂˆ";
    case GEMINI: return "¡ÎËÁÌÂˆ˚";
    case CANCER: return "–‡Í";
    case LEO: return "ÀÂ‚";
    case VIRGO: return "ƒÂ‚‡";
    case LIBRA: return "¬ÂÒ˚";
    case SCORPIO: return "—ÍÓÔËÓÌ";
    case SAGITTARIUS: return "—ÚÂÎÂˆ";
    case CAPRICORN: return " ÓÁÂÓ„";
    case AQUARIUS: return "¬Ó‰ÓÎÂÈ";
    case PISCES: return "–˚·˚";
    default: return "ÕÂËÁ‚ÂÒÚÌ˚È ÁÌ‡Í";
    }
}
