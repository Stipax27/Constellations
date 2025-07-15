cbuffer frame : register(b4)
{
    float4 time;
    float4 aspect;
};

cbuffer camera : register(b3)
{
    float4x4 world[2];
    float4x4 view[2];
    float4x4 proj[2];
};


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

float kU2G = 500000.0f;
float kG2U = 0.000002f;


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

float pn(float3 x) {
    float3 p = floor(x), f = fract(x);
	f *= f * (3.0f - f - f);
	float2 uv = (p.xy + float2(37.0f, 17.0f) * p.z) + f.xy,
	     //rg = textureLod(tex, (uv + 0.5f) / 256.0f, - 100.0f).yx;
         rg = float2(1.0f, 1.0f);
	return 2.4f * lerp(rg.x, rg.y, f.z) - 1.0f;
}

float mapIntergalacticCloud(float3 p, float4 id) {
	float k = 2.0f * id.w + 0.1f;  // p/=k;
    return k * (0.5f + SpiralNoiseC(p.zxy * 0.4132f + 333.0f, id) * 3.0f + pn(p * 8.5f) * 0.12f);
}


// ---------------------------------------------------
// Render intergalactic clouds
// ---------------------------------------------------


float3 hsv2rgb(float x, float y, float z) {	
	return z + z * y * (clamp(abs(((x * 6.0f + float3(0, 4, 2)) % 6.0f) - 3.0f) - 1.0f, 0.0f, 1.0f) - 1.0f);
}


float4 renderIntergalacticClouds(float3 ro, float3 rd, float tmax, float4 id) {
    float max_dist = min(tmax, float(STAR_FIELD_VOXEL_STEPS)),
		  td=0., d, t, noi, lDist, a, sp = 9.,         
    	  rRef = 2.*id.x,
          h = .05+.25*id.z;
    float3 pos, lightColor;   
    float4 sum = float4(0, 0, 0, 0);
   	
    t = .1 * hash(hash(rd)); 

    for (int i = 0; i < 100; i++)  {
	    if (td > .9 ||  sum.w > .99 || t > max_dist) break;
        a = smoothstep(max_dist, 0., t);
        pos = ro + t * rd;
        d = abs(mapIntergalacticCloud(pos, id))+.07;

        // Light calculations
        lDist = max(length(fmod(pos+sp*.5,sp)-sp*.5), .001); // TODO add random offset
        noi = pn(.05*pos);
        lightColor = lerp(hsv2rgb(noi,.5,.6), 
                         hsv2rgb(noi+.3,.5,.6), 
                         smoothstep(rRef*.5,rRef*2.,lDist));
        sum.xyz += a * lightColor/exp(lDist*lDist*lDist*.08)/30.;
		// Edges coloring
        if (d < h) {
			td += (1.-td)*(h-d)+.005;  // accumulate density
            sum.xyz += sum.w * sum.xyz * .25 / lDist;  // emission	
			sum += (1.-sum.w) * .02 * td * a;  // uniform scale density + alpha blend in contribution 
        } 
        td += .015;
        t += max(d * .08 * max( min(lDist, d), 2.), .01);  // trying to optimize step size
    }
    
   	sum = clamp(sum, 0., 1.);   
    sum.xyz *= sum.xyz * (3. - sum.xyz - sum.xyz);
	return sum;
}


//-----------------------------------------------------
//        Coordinate system conversions
//-----------------------------------------------------


struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint   starID : COLOR0;
    float4 worldpos : POSITION1;
};


float4 PS( VS_OUTPUT input ) : SV_Target
{
	float2 uv = input.uv;
    float2 p = -1.0f + 2.0f * uv;
    //p.x *= iResolution.x/iResolution.y;
    p.x *= aspect.x;
    
    float3 color = float3(0, 0, 0);

// camera ----------------------------------------------

    float4x4 v = view[0];

    float3 ro, rdcam, up;
    ro = float3(v._m30, v._m31, v._m32);
    rdcam = float3(v._m20, v._m21, v._m22);
    up = float3(v._m10, v._m11, v._m12);

    float3 ww = normalize( rdcam ),
         uu = normalize( cross(ww, up) ),
         vv = normalize( cross(uu, ww)),
         rd = mul(float4(p.x, p.y, 1, 0), view[0]).xyz;

    rd = normalize(rd);
       
// - rendu des etoiles -----------------------------------

	float4 star = float4(0, 0, 0, 0);

    float dStar = 9999.0f;
    float4 clouds = renderIntergalacticClouds(ro, rd, dStar, float4(0.5f, 0.4f, 0.16f, 0.7f));
    star = clouds + (1.0f - clouds.w) *sqrt(star) * star.w;

// - rendu des galaxies ----------------------------------

    color = star.xyz;

    float noise = frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
    return float4(noise, noise, noise, 1.0);

    return star;
    
    return float4((float3(0.03f, 0.1f, 0.1f) + color), 1.0f);
}
