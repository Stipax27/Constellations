
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
    //{3, 4}
};
std::vector <float> Aries_health = { 1,1,1,1 };
std::vector <point3d> AriesCopy = {};


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

std::vector <float> Gemini_health = { 1,1,1,1,1,1,1,1,1,1,1.5,1,1,1,1,1,1,1,1,1,1 };
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

std::vector <float> Virgo_health{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1.5, 1, 1, 1, 1, 1, 1 };
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
std::vector <point3d> ScorpiusCopy = {};

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
std::vector <point3d> CapricornusCopy = {};

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
   arrange—onstellation(Aries, AriesCopy, 100, 0, 0);
   arrange—onstellation(UrsaMajor, UrsaMajorCopy, 100, -60, 0);
   arrange—onstellation(Cancer, CancerCopy, 100, -20, 0);
   arrange—onstellation(Taurus, TaurusCopy, 100, 40, 50);
   arrange—onstellation(Leo,LeoCopy, 100, -150, -100);
   arrange—onstellation(Gemini,GeminiCopy, 100, 0, 0);
   arrange—onstellation(Libra,LibraCopy, 100, 0, 0);
   arrange—onstellation(Virgo,VirgoCopy, 100, 250, 0);
   arrange—onstellation(Scorpius,ScorpiusCopy, 100, 70, 0);
   arrange—onstellation(Sagittarius,SagittariusCopy, 100, 0, 0);
   arrange—onstellation(Capricornus, CapricornusCopy, 100, 600, 0);
   arrange—onstellation(Aquarius, AquariusCopy, 100, 800, 0);
   arrange—onstellation(Pisces, PiscesCopy, 100, 0, 0);
}

using namespace std;

enum ZodiacSign {
    ARIES, TAURUS, GEMINI, CANCER, LEO, VIRGO,
    LIBRA, SCORPIO, SAGITTARIUS, CAPRICORN, AQUARIUS, PISCES
};
enum MonthSign {
    January, February, March, April, May, June, July, August, September, October, November,December

};
enum DaySign {
    one,
    two,	    
    three,	
    four,	
    five,	
    six,	    
    seven,	
    eight,	
    nine,	
    ten,	    
    eleven,	
    twelve,	
    thirteen,
    fourteen,
    fifteen,	
    sixteen,	
    seventeen,	
    eighteen,
    nineteen,
    twenty,	
    twentyone,
    twentytwo,
    twentythree,
    twentyfour,
    twentyfive,
    twentysix,
    twentyseven,
    twentyeight,
    twentynine,
    thirty,	
    thirtyone
};

bool monthIsSelected = false;
MonthSign player_month = MonthSign::January;

bool dayIsSelected = false;
int player_daynumber = one;

DaySign player_day = DaySign::one;

ZodiacSign player_sign = ZodiacSign::ARIES;

ZodiacSign getZodiacSign(int day, int month) {
    if ((month == March && day >= twentyone)       || (month == April && day <= nineteen))       return ARIES;
    if ((month == April && day >= twenty)          || (month == May  && day <= twenty))          return TAURUS;
    if ((month == May && day >= twentyone)         || (month == June  && day <= twenty))         return GEMINI;
    if ((month == June && day >= twentyone)        || (month == July  && day <= twentytwo))      return CANCER;
    if ((month == July && day >= twentythree)      || (month == August  && day <= twentytwo))    return LEO;
    if ((month == August && day >= twentythree)    || (month == September  && day <= twentytwo)) return VIRGO;
    if ((month == September && day >= twentythree) || (month == October && day <= twentytwo))    return LIBRA;
    if ((month == October && day >= twentythree)   || (month == November && day <= twentyone))   return SCORPIO;
    if ((month == November && day >= twentytwo)    || (month == December  && day <= twentyone))  return SAGITTARIUS;
    if ((month == December && day >= twentytwo)    || (month == January && day <= nineteen))     return CAPRICORN;
    if ((month == January && day >= twenty)        || (month == February && day <= eighteen))    return AQUARIUS;
    if ((month == February && day >= nineteen)     || (month == March && day <= twenty))         return PISCES;

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
string mounthToString(MonthSign mounth) {
    switch (mounth) {
    case January    : return "ﬂÌ‚‡¸";
    case February   : return "‘Â‚‡Î¸";
    case March      : return "Ã‡Ú";
    case April      : return "¿ÔÂÎ¸";
    case May        : return "Ã‡È";
    case June       : return "»˛Ì¸";
    case July       : return "»˛Î¸";
    case August     : return "¿‚„ÛÒÚ";
    case September  : return "—ÂÌÚˇ·¸";
    case October    : return "ŒÍÚˇ·¸";
    case November   : return "ÕÓˇ·¸";
    case December   : return "ƒÂÍ‡·¸";
    default: return "ÕÂËÁ‚ÂÒÚÌ˚È ÏÂÒˇˆ";
    }
}
string dayToString(DaySign day) {
    switch (day) {
    case one: return "1 ";
    case two: return "2 ";
    case three: return "3 ";
    case four: return "4 ";
    case five: return "5 ";
    case six: return "6 ";
    case seven: return "7 ";
    case eight: return "8 ";
    case nine: return "9 ";
    case ten: return "10";
    case eleven: return "11";
    case twelve: return "12";
    case thirteen: return "13";
    case fourteen: return "14";
    case fifteen: return "15";
    case sixteen: return "16";
    case seventeen: return "17";
    case eighteen: return "18";
    case nineteen: return "19";
    case twenty:   return "20";
    case twentyone: return "21";
    case twentytwo: return "22";
    case twentythree: return "23";
    case twentyfour: return "24";
    case twentyfive: return "25";
    case twentysix: return "26";
    case twentyseven: return "27";
    case twentyeight: return "28";
    case twentynine: return "29";
    case thirty: return "30";
    case thirtyone: return "31";
    default: return "ÕÂËÁ‚ÂÒÚÌ˚È ‰ÂÌ¸";
    }
}
