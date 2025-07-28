// CHERI-compatible N-body benchmark (WARDuino-compatible)
// Converted from original C with minimal changes for CHERI

void print_string(const char *s);
void print_int(int n);
void print_double(double d);

#define PI 3.141592653589793
#define SOLAR_MASS (4 * PI * PI)
#define DAYS_PER_YEAR 365.24

struct body {
  double x[3], fill, v[3], mass;
};

static struct body solar_bodies[] = {
  {{0., 0., 0.}, 0., {0., 0., 0.}, SOLAR_MASS},
  {{4.84143144246472090, -1.16032004402742839, -1.03622044471123109e-01}, 0.,
   {1.66007664274403694e-03 * DAYS_PER_YEAR,
    7.69901118419740425e-03 * DAYS_PER_YEAR,
    -6.90460016972063023e-05 * DAYS_PER_YEAR},
   9.54791938424326609e-04 * SOLAR_MASS},
  {{8.34336671824457987, 4.12479856412430479, -0.403523417114321381}, 0.,
   {-2.76742510726862411e-03 * DAYS_PER_YEAR,
    4.99852801234917238e-03 * DAYS_PER_YEAR,
    2.30417297573763929e-05 * DAYS_PER_YEAR},
   2.85885980666130812e-04 * SOLAR_MASS},
  {{12.8943695621391310, -15.1111514016986312, -0.223307578892655734}, 0.,
   {2.96460137564761618e-03 * DAYS_PER_YEAR,
    2.37847173959480950e-03 * DAYS_PER_YEAR,
    -2.96589568540237556e-05 * DAYS_PER_YEAR},
   4.36624404335156298e-05 * SOLAR_MASS},
  {{15.3796971148509165, -25.9193146099879641, 0.179258772950371181}, 0.,
   {2.68067772490389322e-03 * DAYS_PER_YEAR,
    1.62824170038242295e-03 * DAYS_PER_YEAR,
    -9.51592254519715870e-05 * DAYS_PER_YEAR},
   5.15138902046611451e-05 * SOLAR_MASS},
};

#define BODIES_SIZE 5

void offset_momentum(struct body *bodies, int nbodies) {
  for (int i = 0; i < nbodies; i++)
    for (int k = 0; k < 3; k++)
      bodies[0].v[k] -= bodies[i].v[k] * bodies[i].mass / SOLAR_MASS;
}

double sqrt_approx(double x) {
  double guess = x / 2.0;
  for (int i = 0; i < 10; i++) guess = 0.5 * (guess + x / guess);
  return guess;
}

void bodies_advance(struct body *bodies, int nbodies, double dt) {
  for (int i = 0; i < nbodies; i++) {
    for (int j = i + 1; j < nbodies; j++) {
      double dx[3];
      for (int k = 0; k < 3; k++) dx[k] = bodies[i].x[k] - bodies[j].x[k];

      double dist_sqr = dx[0]*dx[0] + dx[1]*dx[1] + dx[2]*dx[2];
      double dist = sqrt_approx(dist_sqr);
      double mag = dt / (dist_sqr * dist);

      for (int k = 0; k < 3; k++) {
        double d = dx[k] * mag;
        bodies[i].v[k] -= d * bodies[j].mass;
        bodies[j].v[k] += d * bodies[i].mass;
      }
    }
  }
  for (int i = 0; i < nbodies; i++)
    for (int k = 0; k < 3; k++)
      bodies[i].x[k] += dt * bodies[i].v[k];
}

double bodies_energy(struct body *bodies, int nbodies) {
  double e = 0.0;
  for (int i = 0; i < nbodies; i++) {
    double v2 = bodies[i].v[0]*bodies[i].v[0] +
                bodies[i].v[1]*bodies[i].v[1] +
                bodies[i].v[2]*bodies[i].v[2];
    e += 0.5 * bodies[i].mass * v2;

    for (int j = i + 1; j < nbodies; j++) {
      double dx[3];
      for (int k = 0; k < 3; k++) dx[k] = bodies[i].x[k] - bodies[j].x[k];
      double dist = sqrt_approx(dx[0]*dx[0] + dx[1]*dx[1] + dx[2]*dx[2]);
      e -= (bodies[i].mass * bodies[j].mass) / dist;
    }
  }
  return e;
}

void print_double(double x) {
  int scaled = (int)(x * 1e9);
  print_int(scaled);
  print_string(" (scaled x1e9)\n");
}

void start() {
  int steps = 50000000; // hardcoded for now
  offset_momentum(solar_bodies, BODIES_SIZE);

  print_string("Initial energy:\n");
  print_double(bodies_energy(solar_bodies, BODIES_SIZE));

  for (int i = 0; i < steps; i++)
    bodies_advance(solar_bodies, BODIES_SIZE, 0.01);

  print_string("Final energy:\n");
  print_double(bodies_energy(solar_bodies, BODIES_SIZE));
}
