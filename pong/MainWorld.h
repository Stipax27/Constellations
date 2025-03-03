
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
std::vector <std::vector <float>> UrsaMajor_indeces =
{
    {0,1},
    {1,2},
    {2,3},
    {3,4},
    {4,5},
    {5,6},
    {6,2},
};
std::vector <float> UrsaMajor_health = { 1,1,1,1,1,1,1 };
std::vector <point3d> UrsaMajorCopy = {};


//std::vector <point3d> Aries =
//{
//    {0, 0, 0.},
//    {.21, .05, 0.},
//    {.35, .12, 0.},
//    {.43, .27, 0.}
//};
//std::vector <std::vector <float>> Aries_indices =
//{
//    {0, 1},
//    {1, 2},
//    {2, 3},
//    //{3, 4}
//};
//std::vector <float> Aries_health = { 1,1,1,1 };
//std::vector <point3d> AriesCopy = {};


//Constellation Aries(
//Aries,
//    Aries_health, Aries_indices
//);

Constellation Aries(
{
    {0, 0, 0.},
    {.21, .05, 0.},
    {.35, .12, 0.},
    {.43, .27, 0.}
},
    { 1,1,1,1 },
{
    {0, 1},
    {1, 2},
    {2, 3}
}
);


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

std::vector <float> Taurus_health = { 1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
std::vector <point3d> TaurusCopy = {};

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

std::vector <float> Gemini_health = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
std::vector <point3d> GeminiCopy = {};

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
std::vector <point3d> CancerCopy = {};

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
std::vector <point3d> LeoCopy = {};

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

std::vector <float> Virgo_health{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
std::vector <point3d> VirgoCopy = {};

std::vector <point3d> Libra =
{
    {-.2,.7,0},
    {.2,.3,0},
    {.3,-.1,0},
    {-.2,-.3,0},
    {0, 0, 0},
    {-.4,-.1,0},
    {-.6, -.05, 0}
};
std::vector <std::vector <float>> Libra_indices =
{
    {1, 4},
    {1, 2},
    {0, 1},
    {4, 3},
    {2, 3},
    {3, 5},
    {5, 6},
};
std::vector <point3d> LibraCopy ={};
std::vector <point3d> LibraHeroCopy = {};




std::vector <float> Libra_health = { 1,1,1,1,1,1,1,1,1};



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


std::vector <float> Scorpius_health{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
std::vector <point3d> ScorpiusCopy = {};

std::vector <point3d> Sagittarius =
{
    {1, 1, 0},
    {.8, .9, 0},
    {.8, .3, 0},
    {1.2, .5, 0},
    {1.6, .2, 0},
    {.4, -.1, 0},
    {1,-.6 , 0},
    {0, 0, 0},
    {-.3,.3, 0},
    {-.5,.15, 0},
    {-.3,-.3, 0},
    {-.3,.3, 0},
    {-1.5,.15, 0},
    {-1.7, 1.1, 0},
    {-1, 1.6, 0},
    {-.5, 1.3, 0},
    {-.4, 1.6, 0},
};

std::vector <std::vector <float>> Sagittarius_indices =
{
    {0,1},
    {1,2},
    {2,3},
    {3,4},
    {2,5},
    {5,6},
    {5,7},
    {7,8},
    {8,9},
    {9,10},
    {10,7},
    {9,12},
    {12,13},
    {13,14},
    {14,15},
    {14,16}
};

std::vector <float> Sagittarius_health{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
std::vector <point3d> SagittariusCopy = {};

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
std::vector <point3d> CapricornusCopy = {};

std::vector <point3d> Aquarius =
{
   {1.2, .6, 0},
   {.6, .3, 0},
   {0,0,0},
   {-.2, .4, 0},
   {0, .7, 0},
   {-.3, .1, 0},
   {-1, .3, 0},
   {-1.3, .75, 0},
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
    {2,5},
    {5,6},
    {6,7},
    {6,8},
    {8,9},
    {9,10}

};

std::vector <float> Aquarius_health{1,1,1,1,1,1,1,1,1,1,1,1,1,1};
std::vector <point3d> AquariusCopy = {};

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
std::vector <point3d> PiscesCopy = {};


//std::vector<point3d> MorphArray ={}; Îòêëþ÷åíî
//std::vector <std::vector <float>> Morp_indices = {}; Îòêëþ÷åíî
//std::vector <float> Morp_health{}; Îòêëþ÷åíî


//std::vector <point3d> *constStarArray = { &Aries }; Îòêëþ÷åíî
//std::vector <float> *constHealthArray = { &Aries_health }; Îòêëþ÷åíî
//std::vector <std::vector <float>> *constIndArray = { &Aries_indices }; Îòêëþ÷åíî

// Íàâåðíîå ìîæíî óäàëÿòü
        //void arrangeÑonstellation(std::vector <point3d>& starArrayData, std::vector <point3d>& starArrayRendering, float angleX, float angleY, float angleZ) // 
        //{
        //    starArrayRendering = starArrayData;
        //    int starsCount = starArrayRendering.size();
        //    float scale = 1000;
        //    for (int i = 0; i < starsCount; i++)// ÐÀçìåùåíèå Ëèíèé.
        //    {
        //        point3d p = { starArrayRendering[i].x, starArrayRendering[i].y, starArrayRendering[i].z };
        //
        //        move(p, 0, 0, 3000. / scale);
        //        rotateX(p, angleX);
        //        rotateY(p, angleY);
        //        rotateZ(p, angleZ);
        //
        //        starArrayRendering[i].x = p.x * scale;
        //        starArrayRendering[i].y = p.y * scale;
        //        starArrayRendering[i].z = p.z * scale;
        //    }
        //}
//

void initWorld()
{
    Aries.setStarsRenderedCords(-17, -25, 0);
   //arrangeÑonstellation(Aries, AriesCopy, -17, -25, 0);
   //arrangeÑonstellation(UrsaMajor, -15, -60, 0);
   //arrangeÑonstellation(Cancer, CancerCopy,  -20, -20, 0);
   //arrangeÑonstellation(Taurus, TaurusCopy, -90, 40, 50);
   //arrangeÑonstellation(Leo, LeoCopy, 40, 50, 0);
   //arrangeÑonstellation(Gemini, GeminiCopy, -200, 0, 0);
   //arrangeÑonstellation(Libra, LibraCopy, 0, 0, 0);
   //arrangeÑonstellation(Libra, LibraHeroCopy, -19, -55, -5);

   //arrangeÑonstellation(Virgo, VirgoCopy, 250, 250, 0);
   //arrangeÑonstellation(Libra, 100, 250, 250);
   //arrangeÑonstellation(Scorpius, ScorpiusCopy, 400, 110, 0);
   //arrangeÑonstellation(Sagittarius, SagittariusCopy, 0, 400, 0);
   //arrangeÑonstellation(Capricornus, CapricornusCopy, 400, 0, 0);
   //arrangeÑonstellation(Aquarius, AquariusCopy, 100, 0, 0);
   //arrangeÑonstellation(Pisces, PiscesCopy, 0, 100, 0);
}

using namespace std;

enum ZodiacSign {
    ARIES, TAURUS, GEMINI, CANCER, LEO, VIRGO,
    LIBRA, SCORPIO, SAGITTARIUS, CAPRICORN, AQUARIUS, PISCES
};
enum MonthSign {
    January, February, March, April, May, June, July, August, September, October, November,December

};


bool confirm = false;
bool monthIsSelected = false;
MonthSign player_month = MonthSign::January;

bool dayIsSelected = false;
int player_day = 1;

ZodiacSign player_sign = ZodiacSign::ARIES;

ZodiacSign getZodiacSign(int day, int month) {
    if ((month == March && day >= 21)       || (month == April && day <= 19))       return ARIES;
    if ((month == April && day >= 20)          || (month == May  && day <= 20))          return TAURUS;
    if ((month == May && day >= 21)         || (month == June  && day <= 20))         return GEMINI;
    if ((month == June && day >= 21)        || (month == July  && day <= 22))      return CANCER;
    if ((month == July && day >= 23)      || (month == August  && day <= 22))    return LEO;
    if ((month == August && day >= 23)    || (month == September  && day <= 22)) return VIRGO;
    if ((month == September && day >= 23) || (month == October && day <= 22))    return LIBRA;
    if ((month == October && day >= 23)   || (month == November && day <= 21))   return SCORPIO;
    if ((month == November && day >= 22)    || (month == December  && day <= 21))  return SAGITTARIUS;
    if ((month == December && day >= 22)    || (month == January && day <= 19))     return CAPRICORN;
    if ((month == January && day >= 20)        || (month == February && day <= 18))    return AQUARIUS;
    if ((month == February && day >= 19)     || (month == March && day <= 20))         return PISCES;

    throw invalid_argument("Invalid date");
}

std::string SignString[] = { "Îâåí" , "Òåëåö","Áëèçíåöû", "Ðàê", "Ëåâ", "Äåâà", "Âåñû", "Ñêîðïèîí", "Ñòðåëåö", "Êîçåðîã", "Âîäîëåé", "Ðûáû" };

string zodiacSignToString(ZodiacSign sign) {

    if (sign<0 || sign >11) return "Íåèçâåñòíûé çíàê";
    return SignString[(int)sign];
   
}
std::string mounthString[] = { "ßíâàðü", "Ôåâðàëü", "Ìàðò", "Àïðåëü", "Ìàé", "Èþíü", "Èþëü", "Àâãóñò", "Ñåíòÿáðü", "Îêòÿáðü", "Íîÿáðü", "Äåêàáðü" };

string mounthToString(MonthSign mounth) {

    if (mounth<0 || mounth>11) return "Íåèçâåñòíûé ìåñÿö";
    return mounthString[(int)mounth];
}

