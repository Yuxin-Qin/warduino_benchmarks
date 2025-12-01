// WARDuinoÎÞ¿â°æ±Ÿ - fibonacci_warduino.c
extern void print_int(int val);
extern void print_string(const char* str, int len);

// ÊµÏÖ±ØÒªµÄÄÚŽæº¯Êý£š²Î¿Œpidigits.c£©
void* memset(void* dest, int val, unsigned int len) {
    unsigned char* ptr = (unsigned char*)dest;
    for (unsigned int i = 0; i < len; i++) {
        ptr[i] = (unsigned char)val;
    }
    return dest;
}

void* memcpy(void* dest, const void* src, unsigned int n) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    for (unsigned int i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

// Œòµ¥Ê±Œäº¯Êý£š²Î¿Œnbody1.cÖÐµÄ×ö·š£©
static unsigned int timer_counter = 0;

unsigned int simple_clock() {
    return timer_counter++;
}

// ŽóÕûÊýÖ§³Ö£š²Î¿Œpidigits.c£¬ÓÃÓÚŽŠÀíŽóFibonacciÊý£©
typedef struct {
    int digits[100];  // ×ã¹»ŽæŽ¢ŽóÊý
    int len;
} BigInt;

void bigint_init(BigInt *a, long long val) {
    memset(a->digits, 0, sizeof(a->digits));
    a->len = 0;
    
    if (val == 0) {
        a->len = 1;
        return;
    }
    
    while (val > 0) {
        a->digits[a->len++] = val % 10;
        val /= 10;
    }
}

void bigint_add(const BigInt *a, const BigInt *b, BigInt *res) {
    int carry = 0, i;
    int maxlen = a->len > b->len ? a->len : b->len;

    memset(res->digits, 0, sizeof(res->digits));
    for (i = 0; i < maxlen || carry; ++i) {
        int sum = carry;
        if (i < a->len) sum += a->digits[i];
        if (i < b->len) sum += b->digits[i];
        res->digits[i] = sum % 10;
        carry = sum / 10;
    }
    res->len = i;
}

void print_bigint(const BigInt *a) {
    for (int i = a->len - 1; i >= 0; i--) {
        char digit[2] = {'0' + a->digits[i], '\0'};
        print_string(digit, 100);
    }
}

// µüŽú·šŒÆËãFibonacci£šÖ§³ÖŽóÊý£©
void fibonacci_iterative_big(int n, BigInt *result) {
    if (n <= 1) {
        bigint_init(result, n);
        return;
    }
    
    BigInt a, b, c;
    bigint_init(&a, 0);
    bigint_init(&b, 1);
    
    for (int i = 2; i <= n; i++) {
        bigint_add(&a, &b, &c);
        memcpy(&a, &b, sizeof(BigInt));
        memcpy(&b, &c, sizeof(BigInt));
    }
    
    memcpy(result, &b, sizeof(BigInt));
}

// Œòµ¥µÄFibonacciŒÆËã£šÓÃÓÚÐ¡Êý×Ö£©
long long fibonacci_iterative_simple(int n) {
    if (n <= 1) return n;
    
    long long a = 0, b = 1, c;
    for (int i = 2; i <= n; i++) {
        c = a + b;
        a = b;
        b = c;
        // 每10次迭代输出进度
        if (i % 10 == 0) {
          print_string("Progress: ", 10);
          print_int(i);
          print_string("/", 1);
          print_int(n);
          print_string("\n", 1);
    }
    }
    return b;
}

// WARDuinoÈë¿Úµã
void start() {
    // 使用volatile防止优化
    volatile int n = 40;
    print_string("Calculating Fibonacci(", 22);
    print_int(n);
    print_string(")\n", 2);
    
    unsigned int start_time = simple_clock();
    
    // ¶ÔÓÚÐ¡Êý×ÖÊ¹ÓÃŒòµ¥°æ±Ÿ£¬ŽóÊý×ÖÊ¹ÓÃŽóÊý°æ±Ÿ
    if (n <= 92) { // 2^63-1ÒÔÄÚµÄFibonacciÊý
        long long result = fibonacci_iterative_simple(n);
        
        print_string("Fibonacci(", 10);
        print_int(n);
        print_string(") = ", 4);
        print_int((int)(result >> 32)); // ŽòÓ¡žß32Î»
        print_string(" ", 1);
        print_int((int)result); // ŽòÓ¡µÍ32Î»
        print_string("\n", 1);
    } else {
        BigInt result;
        fibonacci_iterative_big(n, &result);
        
        print_string("Fibonacci(", 11);
        print_int(n);
        print_string(") = ", 4);
        print_bigint(&result);
        print_string("\n", 1);
    }
    
    unsigned int end_time = simple_clock();
    unsigned int time_taken = end_time - start_time;
    
    print_string("Calculation took ", 18);
    print_int((int)time_taken);
    print_string(" time units\n", 12);
    
    print_string("Fibonacci calculation completed\n", 32);
}
