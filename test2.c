typedef struct {
    double lo;
    double hi;
} vec2d;

vec2d vec2d_add(vec2d a, vec2d b) {
    return (vec2d){ a.lo + b.lo, a.hi + b.hi };
}

int main() {
    vec2d a = {1.0, 2.0}, b = {3.0, 4.0};
    vec2d c = vec2d_add(a, b);
    return 0;
}
