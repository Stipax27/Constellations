int SPIRAL_NOISE_ITER = 6;

float DEG_TO_RAD = 0.0174699591855f;

// Unit = 10 UA
int   GALAXY_FIELD_VOXEL_STEPS = 16;
int   GALAXY_FIELD_VOXEL_STEPS_HD = 7;
float GALAXY_FIELD_VOXEL_STEP_SIZE = 250000.0f; // 2,500,000 AL
float GALAXY_RADIUS = 0.015f;  // (% of 250000)  50,000 AL

int   STAR_FIELD_VOXEL_STEPS = 22;
float STAR_FIELD_VOXEL_STEP_SIZE = 0.5f;  // 5AL 
float STAR_RADIUS = 0.01f; // 2e-8 in true life !   // (% of 5)   1e-8

float PLANET_FIELD_SCALE = 75.;
int   PLANET_FIELD_VOXEL_STEPS = 10;
float PLANET_FIELD_VOXEL_STEP_SIZE = 0.5f;  // 5AL 
float PLANET_RADIUS = 0.04f;

float3 SUN_COLOR = float3(0.3f, 0.21f, 0.165f);

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


//-----------------------------------------------------
// Noise functions
//----------------------------------------------------------------------------------------

float fract(float x)
{
    return x - floor(x);
}

float2 fract(float2 x)
{
    return x - floor(x);
}

float3 fract(float3 x)
{
    return x - floor(x);
}

float4 fract(float4 x)
{
    return x - floor(x);
}

#define HASHSCALE3 float3(0.1031f, 0.1030f, 0.0973f)
#define HASHSCALE4 float4(1031.0f, 0.1030f, 0.0973f, 0.1099f)
#define HASHSCALE1 0.1031f

float hash(float p)
{
	float3 p3  = fract(float3(p, p, p) * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19f);
    return fract((p3.x + p3.y) * p3.z);
}

//----------------------------------------------------------------------------------------
//  1 out, 2 in...
float hash(float2 p)
{
	float3 p3  = fract(float3(p.xyx) * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

//----------------------------------------------------------------------------------------
//  1 out, 3 in...
float hash(float3 p3)
{
	p3  = fract(p3 * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

float4 hash4(const in float4 n) { return fract(sin(n)*1399763.5453123); }


float3 hash33(float3 p3) {
	p3 = fract(p3 * HASHSCALE3);
    p3 += dot(p3, p3.yxz+19.19);
    return fract(float3((p3.x + p3.y)*p3.z, (p3.x+p3.z)*p3.y, (p3.y+p3.z)*p3.x));
}

float4 hash43(float3 p) {
	float4 p4 = fract(float4(p.xyzx)  * HASHSCALE4);
    p4 += dot(p4, p4.wzxy+19.19);
	return fract(float4((p4.x + p4.y)*p4.z, (p4.x + p4.z)*p4.y, (p4.y + p4.z)*p4.w, (p4.z + p4.w)*p4.x));
}


//----------------------------------------------------------------------------------------


// This spiral noise works by successively adding and rotating sin waves while increasing frequency.
// It should work the same on all computers since it's not based on a hash function like some other noises.
// It can be much faster than other noise functions if you're ok with some repetition.
float nudge = 20.0f;	// size of perpendicular vector
float normalizer = 0.0499376169438922f;	// pythagorean theorem on that perpendicular to maintain scale
float SpiralNoiseC(float3 p, float4 id) {
    float iter = 2.0f, n = 2.0f - id.x; // noise amount
    for (int i = 0; i < SPIRAL_NOISE_ITER; i++) {
        n += -abs(sin(p.y*iter) + cos(p.x*iter)) / iter; // add sin and cos scaled inverse with the frequency (abs for a ridged look)
        p.xy += float2(p.y, -p.x) * nudge; // rotate by adding perpendicular and scaling down
        p.xy *= normalizer;
        p.xz += float2(p.z, -p.x) * nudge; // rotate on other axis
        p.xz *= normalizer;  
        iter *= id.y + 0.733733f;          // increase the frequency
    }
    return n;
}

float4 tex = float4(1.0f, 1.0f, 1.0f, 1.0f);

float pn(const in float3 x) {
    float3 p = floor(x), f = fract(x);
	f *= f * (3.0f - f - f);
	float2 uv = (p.xy + float2(37.0f, 17.0f) * p.z) + f.xy,
	     rg = textureLod(tex, (uv + 0.5f) / 256.0f, - 100.0f).yx;
	return 2.4f * mix(rg.x, rg.y, f.z) - 1.0f;
}

float mapIntergalacticCloud(float3 p, float4 id) {
	float k = 2.0f * id.w + 0.1f;  // p/=k;
    return k * (0.5f + SpiralNoiseC(p.zxy * 0.4132f + 333.0f, id) * 3.0f + pn(p * 8.5f) * 0.12f);
}


// ---------------------------------------------------
// Render intergalactic clouds
// ---------------------------------------------------


float3 hsv2rgb(float x, float y, float z) {	
	return z + z * y * (clamp(abs(mod(x * 6.0f + float3(0, 4, 2), 6.0f) - 3.0f) - 1.0f, 0.0f, 1.0f) - 1.0f);
}


float4 renderIntergalacticClouds(float3 ro, float3 rd, float tmax, const float4 id) {
    
    float max_dist= min(tmax, float(STAR_FIELD_VOXEL_STEPS)),
		  td=0.0f, d, t, noi, lDist, a, sp = 9.0f,         
    	  rRef = 2.0f * id.x,
          h = 0.05f + 0.25f * id.z;
    float3 pos, lightColor;   
    float4 sum = float4(0, 0, 0, 0);
   	
    t = .1*hash(hash(rd)); 

    for (int i = 0; i < 100; i++)  {
	    if(td > 0.9f ||  sum.a > 0.99f || t > max_dist) break;
        a = smoothstep(max_dist, 0.0f, t);
        pos = ro + t * rd;
        d = abs(mapIntergalacticCloud(pos, id)) + 0.07f;

        // Light calculations
        lDist = max(length(mod(pos + sp * 0.5f, sp) - sp * 0.5f), 0.001f); // TODO add random offset
        noi = pn(0.05f * pos);
        lightColor = mix(hsv2rgb(noi, 0.5f, 0.6f), 
                         hsv2rgb(noi + 0.3f, 0.5f, 0.6f), 
                         smoothstep(rRef * 0.5, rRef * 2.0f, lDist));
        sum.rgb += a * lightColor / exp(lDist * lDist * lDist * 0.08f) / 30.0f;
		// Edges coloring
        if (d < h) {
			td += (1.0f - td) * (h - d) + 0.005f;  // accumulate density
            sum.rgb += sum.a * sum.rgb * 0.25f / lDist;  // emission	
			sum += (1.0f - sum.a) * 0.02f * td * a;  // uniform scale density + alpha blend in contribution 
        } 
        td += 0.015f;
        t += max(d * 0.08f * max(min(lDist, d), 2.0f), 0.01f);  // trying to optimize step size
    }
    
   	sum = clamp(sum, 0.0f, 1.0f);   
    sum.xyz *= sum.xyz * (3.0f - sum.xyz - sum.xyz);
	return sum;
}


//-----------------------------------------------------
//        Coordinate system conversions
//-----------------------------------------------------


float3 galaxyToUniverse(float3 galaxyPosU, float3 coord) {
    return coord * kG2U + galaxyPosU;
}


float3 universeToGalaxy(float3 galaxyPosU, float3 coord) {
    return (coord - galaxyPosU)*kU2G;
}


float4 PS( out float4 fragColor, in float2 fragCoord ) {
	float2 uv = fragCoord.xy / iResolution.xy;
    float2 p = -1.0f + 2.0f * uv;
    p.x *= iResolution.x/iResolution.y;
    
    float3 col = float3(0);
    
    
// Lecture de la configuration -------------------------
        Config cfg = getConfig();

		bool isU = cfg.coordSystem == IN_UNIVERSE,
			 isG = cfg.coordSystem == IN_GALAXY;

// camera ----------------------------------------------

    float3 ro, rdcam, ta, up;
    ro = cfg.ro_cam; 
    rdcam = cfg.rd_cam;

    // Leger tangage
    up = normalize(float3(.1*cos(.1*iTime), .3*cos(.1*iTime), 1.));
    float3 ww = normalize( rdcam ),
         uu = normalize( cross(ww,up) ),
         vv = normalize( cross(uu,ww)),
         rd = normalize( -p.x*uu + p.y*vv + 2.*ww );
       
// - rendu des etoiles -----------------------------------

    bool isHitStar = false;
	float3 starPosG, starId = float3(90);  
	float4 star = float4(0);
    
    float3 roG = isU ? universeToGalaxy(cfg.galaxy_pos, ro) : ro;    
    float dStar = 9999.;
    star = renderStarField(roG, rd, dStar, starId );

// - rendu des galaxies ----------------------------------

    float3 targetPosU, targetId = float3(90);
    // Le calcul se fait toujours en coordonnees univers
	float3 roU = isG ? galaxyToUniverse(cfg.galaxy_pos, ro) : ro;
	float4 colGalaxy = renderGalaxyField(roU, rd, targetPosU, targetId, isG);

    star.rgb += colGalaxy.rgb* (1. - star.a);

    col = star.rgb;
    
    
    return float4((isU ? float3(0.03,0.,.1)+col : col), 1.);
}
