const int SPIRAL_NOISE_ITER = 6

const float PI = 3.14159265359;
const float DEG_TO_RAD = (PI / 180.0);

// Unit = 10 UA
const int   GALAXY_FIELD_VOXEL_STEPS = 16;
const int   GALAXY_FIELD_VOXEL_STEPS_HD = 7;
const float GALAXY_FIELD_VOXEL_STEP_SIZE = 250000.; // 2,500,000 AL
const float GALAXY_RADIUS = .015;  // (% of 250000)  50,000 AL

const int   STAR_FIELD_VOXEL_STEPS = 22;
const float STAR_FIELD_VOXEL_STEP_SIZE = .5;  // 5AL 
const float STAR_RADIUS = .01; // 2e-8 in true life !   // (% of 5)   1e-8

const float PLANET_FIELD_SCALE = 75.;
const int   PLANET_FIELD_VOXEL_STEPS = 10;
const float PLANET_FIELD_VOXEL_STEP_SIZE = .5;  // 5AL 
const float PLANET_RADIUS = .04;


const float kU2G = GALAXY_FIELD_VOXEL_STEP_SIZE/STAR_FIELD_VOXEL_STEP_SIZE;
const float kG2U = STAR_FIELD_VOXEL_STEP_SIZE/GALAXY_FIELD_VOXEL_STEP_SIZE;

const vec3  SUN_COLOR = vec3(.3, .21, .165);

float time;


//-----------------------------------------------------


#define IN_UNIVERSE 1.
#define IN_GALAXY 2.
#define IN_SOLAR_SYSTEM 3.

#define MOVING     1.
#define STATIONARY 2.

#define NONE     0.
#define GALAXY   1.
#define STAR     2.
#define PLANET   3.


struct Config {
 	float movingMode;
    float targetType;
    float coordSystem;
    float time;
    vec3 ro_cam;
    vec3 rd_cam;
    vec3 target_pos;
    vec3 galaxy_pos;
};


#define CONF(id)  texture(iChannel0, vec2(id+.5,.5)/ iChannelResolution[0].xy, -100.0).xyz;
#define CONF4(id) texture(iChannel0, vec2(id+.5,.5)/ iChannelResolution[0].xy, -100.0);

Config getConfig() { 
    vec4 v1 = CONF4(0.);
    Config cfg;
    
    cfg.movingMode = v1.x > 1.5 ? STATIONARY : 
                     MOVING;
    cfg.targetType = v1.y > 2.5 ? PLANET : 
    				 v1.y > 1.5 ? STAR:
    				 v1.y > 0.5 ? GALAXY:
    				 NONE;
    cfg.coordSystem = v1.z > 2.5 ? IN_SOLAR_SYSTEM :
    				  v1.z > 1.5 ? IN_GALAXY :				
                      IN_UNIVERSE;
    cfg.time = v1.w;
    cfg.ro_cam = CONF(1.);
    cfg.rd_cam = CONF(2.);
    cfg.target_pos = CONF(3.);
    cfg.galaxy_pos = CONF(4.);
    return cfg;
}


//-----------------------------------------------------
// Noise functions
//----------------------------------------------------------------------------------------

#define HASHSCALE3 vec3(.1031, .1030, .0973)
#define HASHSCALE4 vec4(1031, .1030, .0973, .1099)
#define HASHSCALE1 .1031

float hash(float p)
{
	vec3 p3  = fract(vec3(p) * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

//----------------------------------------------------------------------------------------
//  1 out, 2 in...
float hash(vec2 p)
{
	vec3 p3  = fract(vec3(p.xyx) * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

//----------------------------------------------------------------------------------------
//  1 out, 3 in...
float hash(vec3 p3)
{
	p3  = fract(p3 * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

vec4 hash4(const in vec4 n) { return fract(sin(n)*1399763.5453123); }


vec3 hash33(vec3 p3) {
	p3 = fract(p3 * HASHSCALE3);
    p3 += dot(p3, p3.yxz+19.19);
    return fract(vec3((p3.x + p3.y)*p3.z, (p3.x+p3.z)*p3.y, (p3.y+p3.z)*p3.x));
}

vec4 hash43(vec3 p) {
	vec4 p4 = fract(vec4(p.xyzx)  * HASHSCALE4);
    p4 += dot(p4, p4.wzxy+19.19);
	return fract(vec4((p4.x + p4.y)*p4.z, (p4.x + p4.z)*p4.y, (p4.y + p4.z)*p4.w, (p4.z + p4.w)*p4.x));
}

//----------------------------------------------------------------------------------------


float noise(const in vec3 x) {
    vec3 p = floor(x), f = fract(x);
	f *= f*(3.-f-f);
	vec2 uv = (p.xy+vec2(37.,17.)*p.z) + f.xy,
	     rg = textureLod( iChannel1, (uv+.5)/256., -100.).yx;
	return mix(rg.x, rg.y, f.z);
}

float pn(const in vec3 x) {
    vec3 p = floor(x), f = fract(x);
	f *= f*(3.-f-f);
	vec2 uv = (p.xy+vec2(37.,17.)*p.z) + f.xy,
	     rg = textureLod( iChannel1, (uv+.5)/256., -100.).yx;
	return 2.4*mix(rg.x, rg.y, f.z)-1.;
}

float bm(const in vec3 x) {
    vec3 p = floor(x), f = fract(x);
	f *= f*(3.-f-f);
	vec2 uv = (p.xy+vec2(37.,17.)*p.z) + f.xy,
	     rg = textureLod( iChannel1, (uv+ .5)/256., -100.).yx;
	return 1.-.82*mix(rg.x, rg.y, f.z);
}

float fpn(const in vec3 p) { 
    return pn(p*.06125)*.5 + 
           pn(p*.125)*.25 + 
           pn(p*.25)*.125;// + pn(p*.5)*.625;
}

float fbm(const in vec3 p) {
   return bm(p*.06125)*.5 + 
          bm(p*.125)*.25 + 
          bm(p*.25)*.125 + 
          bm(p*.4)*.2;
}

const mat3 msun = mat3(0., .8, .6, -.8, .36, -.48, -.6, -.48, .64);

float smoothNoise(in vec3 q){
	float f  = .5000*noise(q); q=msun*q*2.01;
          f += .2500*noise(q); q=msun*q*2.02;
          f += .1250*noise(q); q=msun*q*2.03;
          f += .0625*noise(q);
	return f;
}


// This spiral noise works by successively adding and rotating sin waves while increasing frequency.
// It should work the same on all computers since it's not based on a hash function like some other noises.
// It can be much faster than other noise functions if you're ok with some repetition.
const float nudge = 20.;	// size of perpendicular vector
float normalizer = 1.0 / sqrt(1.0 + nudge*nudge);	// pythagorean theorem on that perpendicular to maintain scale
float SpiralNoiseC(vec3 p, vec4 id) {
    float iter = 2., n = 2.-id.x; // noise amount
    for (int i = 0; i < SPIRAL_NOISE_ITER; i++) {
        n += -abs(sin(p.y*iter) + cos(p.x*iter)) / iter; // add sin and cos scaled inverse with the frequency (abs for a ridged look)
        p.xy += vec2(p.y, -p.x) * nudge; // rotate by adding perpendicular and scaling down
        p.xy *= normalizer;
        p.xz += vec2(p.z, -p.x) * nudge; // rotate on other axis
        p.xz *= normalizer;  
        iter *= id.y + .733733;          // increase the frequency
    }
    return n;
}

float mapIntergalacticCloud(vec3 p, vec4 id) {
	float k = 2.*id.w +.1;  // p/=k;
    return k*(.5 + SpiralNoiseC(p.zxy*.4132+333., id)*3. + pn(p*8.5)*.12);
}


// ---------------------------------------------------
// Render intergalactic clouds
// ---------------------------------------------------


vec3 hsv2rgb(float x, float y, float z) {	
	return z+z*y*(clamp(abs(mod(x*6.+vec3(0,4,2),6.)-3.)-1.,0.,1.)-1.);
}


vec4 renderIntergalacticClouds(vec3 ro, vec3 rd, float tmax, const vec4 id) {
    
    float max_dist= min(tmax, float(STAR_FIELD_VOXEL_STEPS)),
		  td=0., d, t, noi, lDist, a, sp = 9.,         
    	  rRef = 2.*id.x,
          h = .05+.25*id.z;
    vec3 pos, lightColor;   
    vec4 sum = vec4(0);
   	
    t = .1*hash(hash(rd)); 

    for (int i=0; i<100; i++)  {
	    if(td>.9 ||  sum.a > .99 || t>max_dist) break;
        a = smoothstep(max_dist,0.,t);
        pos = ro + t*rd;
        d = abs(mapIntergalacticCloud(pos, id))+.07;

        // Light calculations
        lDist = max(length(mod(pos+sp*.5,sp)-sp*.5), .001); // TODO add random offset
        noi = pn(.05*pos);
        lightColor = mix(hsv2rgb(noi,.5,.6), 
                         hsv2rgb(noi+.3,.5,.6), 
                         smoothstep(rRef*.5,rRef*2.,lDist));
        sum.rgb += a*lightColor/exp(lDist*lDist*lDist*.08)/30.;
		// Edges coloring
        if (d<h) {
			td += (1.-td)*(h-d)+.005;  // accumulate density
            sum.rgb += sum.a * sum.rgb * .25 / lDist;  // emission	
			sum += (1.-sum.a)*.02*td*a;  // uniform scale density + alpha blend in contribution 
        } 
        td += .015;
        t += max(d * .08 * max(min(lDist,d),2.), .01);  // trying to optimize step size
    }
    
   	sum = clamp(sum, 0., 1.);   
    sum.xyz *= sum.xyz*(3.-sum.xyz-sum.xyz);
	return sum;
}


//-----------------------------------------------------
//        Coordinate system conversions
//-----------------------------------------------------


vec3 galaxyToUniverse(vec3 galaxyPosU, vec3 coord) {
    return coord*kG2U + galaxyPosU;
}


vec3 universeToGalaxy(vec3 galaxyPosU, vec3 coord) {
    return (coord-galaxyPosU)*kU2G;
}


float4 PS( out vec4 fragColor, in vec2 fragCoord ) {
	vec2 uv = fragCoord.xy / iResolution.xy;
    vec2 p = -1. + 2. * uv;
    p.x *= iResolution.x/iResolution.y;
    
    vec3 col = vec3(0);
    
    
// Lecture de la configuration -------------------------
        Config cfg = getConfig();

		bool isU = cfg.coordSystem == IN_UNIVERSE,
			 isG = cfg.coordSystem == IN_GALAXY;

// camera ----------------------------------------------

    vec3 ro, rdcam, ta, up;
    ro = cfg.ro_cam; 
    rdcam = cfg.rd_cam;

    // Leger tangage
    up = normalize(vec3(.1*cos(.1*iTime), .3*cos(.1*iTime), 1.));
    vec3 ww = normalize( rdcam ),
         uu = normalize( cross(ww,up) ),
         vv = normalize( cross(uu,ww)),
         rd = normalize( -p.x*uu + p.y*vv + 2.*ww );
       
// - rendu des etoiles -----------------------------------

    bool isHitStar = false;
	vec3 starPosG, starId = vec3(90);  
	vec4 star = vec4(0);
    
    vec3 roG = isU ? universeToGalaxy(cfg.galaxy_pos, ro) : ro;    
    float dStar = 9999.;
    star = renderStarField(roG, rd, dStar, starId );

// - rendu des galaxies ----------------------------------

    vec3 targetPosU, targetId = vec3(90);
    // Le calcul se fait toujours en coordonnees univers
	vec3 roU = isG ? galaxyToUniverse(cfg.galaxy_pos, ro) : ro;
	vec4 colGalaxy = renderGalaxyField(roU, rd, targetPosU, targetId, isG);

    star.rgb += colGalaxy.rgb* (1. - star.a);

    col = star.rgb;
    
    
    return vec4((isU ? vec3(0.03,0.,.1)+col : col), 1.);
 
}
