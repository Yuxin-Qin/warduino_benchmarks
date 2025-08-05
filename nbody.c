/* The Computer Language Benchmarks Game
   http://benchmarksgame.alioth.debian.org/

   contributed by Mark C. Lewis
   modified slightly by Chad Whipkey
   converted from java to c++,added sse support, by Branimir Maksimovic
   converted from c++ to c, by Alexey Medvedchikov 
*/

#define PI 3.141592653589793
#define SOLAR_MASS ( 4 * PI * PI )
#define DAYS_PER_YEAR 365.24
#define Z 50000000

void print_int(int);
void print_string(const char*);

struct body {
   float x[3], fill, v[3], mass;
};

static struct body solar_bodies[] = {
   /* sun */
   {
      .x = { 0., 0., 0. },
      .v = { 0., 0., 0. },
      .mass = SOLAR_MASS
   },
   /* jupiter */
   {
      .x = { 4.84143144246472090e+00,
         -1.16032004402742839e+00,
         -1.03622044471123109e-01 },
      .v = { 1.66007664274403694e-03 * DAYS_PER_YEAR,
         7.69901118419740425e-03 * DAYS_PER_YEAR,
         -6.90460016972063023e-05 * DAYS_PER_YEAR },
      .mass = 9.54791938424326609e-04 * SOLAR_MASS
   },
   /* saturn */
   {
      .x = { 8.34336671824457987e+00,
         4.12479856412430479e+00,
         -4.03523417114321381e-01 },
      .v = { -2.76742510726862411e-03 * DAYS_PER_YEAR,
         4.99852801234917238e-03 * DAYS_PER_YEAR,
         2.30417297573763929e-05 * DAYS_PER_YEAR },
      .mass = 2.85885980666130812e-04 * SOLAR_MASS
   },
   /* uranus */
   {
      .x = { 1.28943695621391310e+01,
         -1.51111514016986312e+01,
         -2.23307578892655734e-01 },
      .v = { 2.96460137564761618e-03 * DAYS_PER_YEAR,
         2.37847173959480950e-03 * DAYS_PER_YEAR,
         -2.96589568540237556e-05 * DAYS_PER_YEAR },
      .mass = 4.36624404335156298e-05 * SOLAR_MASS
   },
   /* neptune */
   {
      .x = { 1.53796971148509165e+01,
         -2.59193146099879641e+01,
         1.79258772950371181e-01 },
      .v = { 2.68067772490389322e-03 * DAYS_PER_YEAR,
         1.62824170038242295e-03 * DAYS_PER_YEAR,
         -9.51592254519715870e-05 * DAYS_PER_YEAR },
      .mass = 5.15138902046611451e-05 * SOLAR_MASS
   }
};

static const int BODIES_SIZE = sizeof(solar_bodies) / sizeof(solar_bodies[0]);

float sqrt_approx(float x) {
  float guess = x / 2.0;
  for (int i = 0; i < 10; ++i) {
    guess = 0.5 * (guess + x / guess);
  }
  return guess;
}

void offset_momentum(struct body *bodies, unsigned int nbodies)
{
   unsigned int i, k;
   for (i = 0; i < nbodies; ++i)
      for (k = 0; k < 3; ++k)
         bodies[0].v[k] -= bodies[i].v[k] * bodies[i].mass
            / SOLAR_MASS;
}

typedef struct {
   float val[2];
   float lo;
   float hi;
   float x;
   float y;
} vec2d;

vec2d vec2d_load(float a, float b) {
   vec2d v;
   v.val[0] = a;
   v.val[1] = b;
   return v;
}

void vec2d_store(float *out, vec2d v) {
  out[0] = v.x;
  out[1] = v.y;
}

vec2d vec2d_add(vec2d a, vec2d b) {
   return (vec2d){ .val = { a.val[0] + b.val[0], a.val[1] + b.val[1] } };
}

vec2d vec2d_sub(vec2d a, vec2d b) {
   return (vec2d){ .val = { a.val[0] - b.val[0], a.val[1] - b.val[1] } };
}

vec2d vec2d_mul(vec2d a, vec2d b) {
   return (vec2d){ .val = { a.val[0] * b.val[0], a.val[1] * b.val[1] } };
}

vec2d vec2d_sqrt(vec2d a) {
   return (vec2d){ .val = { sqrt_approx(a.val[0]), sqrt_approx(a.val[1]) } };
}

// Optional scalar broadcast
vec2d vec2d_set1(float x) {
   return (vec2d){ .val = { x, x } };
}

// Load low 64 bits from pointer into lower part of v
vec2d vec2d_loadl(vec2d v, const float *ptr) {
   v.val[0] = *ptr;
   return v;
}

// Load low 64 bits from pointer into upper part of v
vec2d vec2d_loadh(vec2d v, const float *ptr) {
   v.val[1] = *ptr;
   return v;
}

vec2d vec2d_set(float val) {
    return (vec2d){ val, val };
}

vec2d vec2d_div(vec2d a, vec2d b) {
    return (vec2d){ a.lo / b.lo, a.hi / b.hi };
}

vec2d vec2d_rsqrt(vec2d d){
      vec2d guess = vec2d_div(vec2d_set(1.0), d); // rough 1/sqrt
      for (int i = 0; i < 2; i++) {
         guess = vec2d_mul(vec2d_set(1.5),
            vec2d_sub(guess,vec2d_mul(
               vec2d_div(vec2d_set(0.5), d),
                  vec2d_mul(guess, vec2d_mul(guess, guess))
               )
            )
         );
      }
return guess;
}

void bodies_advance(struct body *bodies, unsigned int nbodies, float dt)
{
   unsigned int N = (nbodies - 1) * nbodies / 2;
   static struct {
      float dx[3], fill;
   } r[1000];
   //static __attribute__((aligned(16))) float mag[1000];
   static float mag[1000];

   unsigned int i, j, k, m;
   vec2d dx[3], dsquared, distance, dmag;

   for(k = 0, i = 0; i < nbodies - 1; ++i)
      for(j = i + 1; j < nbodies; ++j, ++k)
         for ( m = 0; m < 3; ++m)
            r[k].dx[m] = bodies[i].x[m] - bodies[j].x[m];

   for (i = 0; i < N; i += 2) {
      for (m = 0; m < 3; ++m) {
         dx[m] = vec2d_loadl(dx[m], &r[i].dx[m]);
         dx[m] = vec2d_loadh(dx[m], &r[i+1].dx[m]);
      }

      //dsquared = dx[0] * dx[0] + dx[1] * dx[1] + dx[2] * dx[2];
      vec2d dsquared =  vec2d_add(
                        vec2d_add(
                        vec2d_mul(dx[0], dx[0]),
                        vec2d_mul(dx[1], dx[1])
                        ),
                        vec2d_mul(dx[2], dx[2])
      );

      //distance = _mm_cvtps_pd(_mm_rsqrt_ps(_mm_cvtpd_ps(dsquared)));
      //distance = (vec2d){5.0, 5.0}; for test @Yuxin
      distance = vec2d_rsqrt(dsquared);

      for (j = 0; j < 2; ++j)
      /*
         distance = distance * vec2d_set(1.5)
            - ((vec2d_set(0.5) * dsquared) * distance)
            * (distance * distance);
      */
      distance = vec2d_mul(distance, vec2d_set(1.5));
      vec2d half_dsq = vec2d_mul(vec2d_set(0.5), dsquared);
      vec2d triple = vec2d_mul(half_dsq, distance);
      vec2d square = vec2d_mul(distance, distance);
      distance = vec2d_sub(distance, vec2d_mul(triple, square));

      //dmag = _mm_set1_pd(dt) / (dsquared) * distance;
      vec2d dt_vec = (vec2d){dt, dt};
      dmag = vec2d_mul(vec2d_div(dt_vec, dsquared), distance);

      vec2d_store(&mag[i], dmag);
   }

   for (i = 0, k = 0; i < nbodies - 1; ++i)
      for ( j = i + 1; j < nbodies; ++j, ++k)
         for ( m = 0; m < 3; ++m) {
            bodies[i].v[m] -= r[k].dx[m] * bodies[j].mass
               * mag[k];
            bodies[j].v[m] += r[k].dx[m] * bodies[i].mass
               * mag[k];
         }

   for (i = 0; i < nbodies; ++i)
      for ( m = 0; m < 3; ++m)
         bodies[i].x[m] += dt * bodies[i].v[m];
}

float my_sqrt(float x) {
    float guess = x > 1.0 ? x : 1.0;
    for (int i = 0; i < 10; ++i) {
        guess = 0.5 * (guess + x / guess);
    }
    return guess;
}

float bodies_energy(struct body *bodies, unsigned int nbodies) {
   float dx[3], distance, e = 0.0;
   unsigned int i, j, k;

   for (i=0; i < nbodies; ++i) {
      e += bodies[i].mass * ( bodies[i].v[0] * bodies[i].v[0]
         + bodies[i].v[1] * bodies[i].v[1]
         + bodies[i].v[2] * bodies[i].v[2] ) / 2.;

      for (j=i+1; j < nbodies; ++j) {
         for (k = 0; k < 3; ++k)
            dx[k] = bodies[i].x[k] - bodies[j].x[k];

         float distance = my_sqrt(dx[0]*dx[0] + dx[1]*dx[1] + dx[2]*dx[2]);

         e -= (bodies[i].mass * bodies[j].mass) / distance;
      }
   }
   return e;
}

void start() {
    offset_momentum(solar_bodies, BODIES_SIZE);
    print_int((int)bodies_energy(solar_bodies, BODIES_SIZE));
    for (int i = 0; i < Z; ++i)
        bodies_advance(solar_bodies, BODIES_SIZE, 0.01);
    print_int((int)bodies_energy(solar_bodies, BODIES_SIZE));
}
