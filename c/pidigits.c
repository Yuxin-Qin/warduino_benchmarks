extern void print_int(int val);
extern void print_string(const char* str, int len);

typedef unsigned int my_size_t;
#define MAX_DIGITS 1500  // 增加容量以支持更高精度

void my_memset(void *ptr, int value, my_size_t num) {
    unsigned char *p = (unsigned char*)ptr;
    for (my_size_t i = 0; i < num; i++) {
        p[i] = (unsigned char)value;
    }
}

void my_memcpy(void *dest, const void *src, my_size_t num) {
    unsigned char *d = (unsigned char*)dest;
    const unsigned char *s = (const unsigned char*)src;
    for (my_size_t i = 0; i < num; i++) {
        d[i] = s[i];
    }
}

typedef struct {
    int digits[MAX_DIGITS]; 
    int len;
    int negative;  // 添加符号位
} BigInt;

//函数前向声明
void bigint_sub(const BigInt *a, const BigInt *b, BigInt *res);

void* memcpy(void* dest, const void* src, my_size_t n) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    
    for (my_size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    
    return dest;
}

void* memset(void* dest, int val, my_size_t len) {
    unsigned char* ptr = (unsigned char*)dest;
    for(unsigned int i = 0; i < len; i++)
        ptr[i] = (unsigned char)val;
    return dest;
}

// 初始化大整数
void bigint_init(BigInt *a, int val) {
    my_memset(a->digits, 0, sizeof(a->digits));
    a->len = 0;
    a->negative = 0;
    
    if (val < 0) {
        a->negative = 1;
        val = -val;
    }
    
    if (val == 0) {
        a->len = 1;
        return;
    }
    
    while (val > 0) {
        a->digits[a->len++] = val % 10;
        val /= 10;
    }
}

// 复制大整数
void bigint_copy(BigInt *dst, const BigInt *src) {
    my_memcpy(dst, src, sizeof(BigInt));
}

// 加法，支持符号
void bigint_add(const BigInt *a, const BigInt *b, BigInt *res) {
    // 如果符号不同，转为减法
    if (a->negative != b->negative) {
        BigInt temp = *b;
        temp.negative = !temp.negative;
        bigint_sub(a, &temp, res);
        return;
    }
    
    int carry = 0, i;
    int maxlen = a->len > b->len ? a->len : b->len;

    my_memset(res->digits, 0, sizeof(res->digits));
    for (i = 0; i < maxlen || carry; ++i) {
        int sum = carry;
        if (i < a->len) sum += a->digits[i];
        if (i < b->len) sum += b->digits[i];
        res->digits[i] = sum % 10;
        carry = sum / 10;
    }
    res->len = i;
    res->negative = a->negative; // 符号与操作数相同
    
    while (res->len > 1 && res->digits[res->len-1] == 0) res->len--;
    
    // 如果结果为0，确保不是负数
    if (res->len == 1 && res->digits[0] == 0)
        res->negative = 0;
}

// 无符号比较：仅比较绝对值
int bigint_abs_cmp(const BigInt *a, const BigInt *b) {
    if (a->len != b->len) return a->len - b->len;
    for (int i = a->len-1; i >= 0; --i) {
        if (a->digits[i] != b->digits[i])
            return a->digits[i] - b->digits[i];
    }
    return 0;
}

// 有符号比较
int bigint_cmp(const BigInt *a, const BigInt *b) {
    if (a->negative && !b->negative) return -1;
    if (!a->negative && b->negative) return 1;
    return a->negative ? -bigint_abs_cmp(a, b) : bigint_abs_cmp(a, b);
}

// 减法，支持符号
void bigint_sub(const BigInt *a, const BigInt *b, BigInt *res) {
    // 如果符号不同，转为加法
    if (a->negative != b->negative) {
        BigInt temp = *b;
        temp.negative = !temp.negative;
        bigint_add(a, &temp, res);
        return;
    }
    
    // 决定实际操作
    int cmp = bigint_abs_cmp(a, b);
    const BigInt *larger, *smaller;
    
    if (cmp >= 0) {
        larger = a;
        smaller = b;
        res->negative = a->negative;
    } else {
        larger = b;
        smaller = a;
        res->negative = !a->negative;
    }
    
    int borrow = 0;

    my_memset(res->digits, 0, sizeof(res->digits));
    for (int i = 0; i < larger->len; i++) {
        int diff = larger->digits[i] - (i < smaller->len ? smaller->digits[i] : 0) - borrow;
        if (diff < 0) {
            diff += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        res->digits[i] = diff;
    }
    res->len = larger->len;
    while (res->len > 1 && res->digits[res->len-1] == 0) res->len--;
    
    // 如果结果为0，确保不是负数
    if (res->len == 1 && res->digits[0] == 0)
        res->negative = 0;
}

// 乘以无符号整数
void bigint_mul_ui(const BigInt *a, unsigned int b, BigInt *res) {
    if (b == 0) {
        bigint_init(res, 0);
        return;
    }
    
    unsigned long long carry = 0;

    my_memset(res->digits, 0, sizeof(res->digits));
    int i;
    for (i = 0; i < a->len || carry; ++i) {
        unsigned long long prod = carry;
        if (i < a->len) prod += (unsigned long long)a->digits[i] * b;
        res->digits[i] = prod % 10;
        carry = prod / 10;
    }
    res->len = i;
    res->negative = a->negative;
    
    while (res->len > 1 && res->digits[res->len-1] == 0) res->len--;
    
    // 如果结果为0，确保不是负数
    if (res->len == 1 && res->digits[0] == 0)
        res->negative = 0;
}

// 计算 res = a - b*d
void bigint_submul_ui(const BigInt *a, const BigInt *b, unsigned int d, BigInt *res) {
    BigInt tmp;
    bigint_mul_ui(b, d, &tmp);
    bigint_sub(a, &tmp, res);
}

// 计算 res = a + b*c
void bigint_addmul_ui(const BigInt *a, const BigInt *b, unsigned int c, BigInt *res) {
    BigInt tmp;
    bigint_mul_ui(b, c, &tmp);
    bigint_add(a, &tmp, res);
}

// 准确实现整数除法: res = a / b (向零截断)
void bigint_tdiv_q(const BigInt *a, const BigInt *b, BigInt *res) {
    // 检查除数为0的情况
    if (b->len == 1 && b->digits[0] == 0) {
        return; // 除以零未定义
    }
    
    // 处理符号
    int res_negative = (a->negative != b->negative);
    
    // 创建工作副本
    BigInt abs_a = *a;
    BigInt abs_b = *b;
    abs_a.negative = 0;
    abs_b.negative = 0;
    
    // 如果被除数小于除数，结果为0
    if (bigint_abs_cmp(&abs_a, &abs_b) < 0) {
        bigint_init(res, 0);
        return;
    }
    
    // 创建临时变量
    BigInt remainder, quotient, shifted_divisor, temp;
    bigint_copy(&remainder, &abs_a);
    my_memset(&quotient, 0, sizeof(quotient));
    quotient.len = 1;
    
    // 找到最大的位移
    int max_shift = remainder.len - abs_b.len;
    
    // 执行长除法
    for (int shift = max_shift; shift >= 0; shift--) {
        // 创建左移的除数: shifted_divisor = abs_b * 10^shift
        my_memset(&shifted_divisor, 0, sizeof(shifted_divisor));
        shifted_divisor.len = abs_b.len + shift;
        for (int i = 0; i < abs_b.len; i++) {
            shifted_divisor.digits[i + shift] = abs_b.digits[i];
        }
        
        // 求当前位的商
        int digit = 0;
        while (bigint_abs_cmp(&remainder, &shifted_divisor) >= 0) {
            bigint_sub(&remainder, &shifted_divisor, &temp);
            bigint_copy(&remainder, &temp);
            digit++;
        }
        
        // 将数字放入商的适当位置
        if (digit > 0 || quotient.len > 1) {
            // 确保商有足够空间
            if (shift + 1 > quotient.len) {
                quotient.len = shift + 1;
            }
            quotient.digits[shift] = digit;
        }
    }
    
    // 设置结果符号
    quotient.negative = res_negative;
    bigint_copy(res, &quotient);
    
    // 如果结果为0，确保不是负数
    if (res->len == 1 && res->digits[0] == 0)
        res->negative = 0;
}

// 获取无符号整数值(最低位)
unsigned int bigint_get_ui(const BigInt *a) {
    return a->digits[0];
}

// 全局变量
BigInt tmp1, tmp2, acc, den, num;
typedef unsigned int ui;

// 精确实现mpz_mul_ui, mpz_add, mpz_tdiv_q的提取数字功能
ui extract_digit(ui nth) {
    bigint_mul_ui(&num, nth, &tmp1);
    bigint_add(&tmp1, &acc, &tmp2);
    bigint_tdiv_q(&tmp2, &den, &tmp1);
    return bigint_get_ui(&tmp1);
}

// 精确实现mpz_submul_ui, mpz_mul_ui的消除数字功能
void eliminate_digit(ui d) {
    BigInt temp;
    bigint_submul_ui(&acc, &den, d, &temp);
    bigint_copy(&acc, &temp);
    bigint_mul_ui(&acc, 10, &tmp1);
    bigint_copy(&acc, &tmp1);
    bigint_mul_ui(&num, 10, &tmp1);
    bigint_copy(&num, &tmp1);
}

// 精确实现mpz_addmul_ui, mpz_mul_ui的下一项功能
void next_term(ui k) {
    ui k2 = k * 2U + 1U;
    
    // acc = acc + num * 2U
    BigInt temp;
    bigint_addmul_ui(&acc, &num, 2U, &temp);
    bigint_copy(&acc, &temp);
    
    // acc = acc * k2
    bigint_mul_ui(&acc, k2, &tmp1);
    bigint_copy(&acc, &tmp1);
    
    // den = den * k2
    bigint_mul_ui(&den, k2, &tmp1);
    bigint_copy(&den, &tmp1);
    
    // num = num * k
    bigint_mul_ui(&num, k, &tmp1);
    bigint_copy(&num, &tmp1);
}

int start() {
    ui d, k, i;
    int n = 10;  // 减少到只计算5位
    unsigned int iterations = 0;
    
    
    bigint_init(&tmp1, 0);
    
    bigint_init(&tmp2, 0);
    
    bigint_init(&acc, 0);
    
    bigint_init(&den, 1);
    
    bigint_init(&num, 1);
    
    for (i = k = 0; i < n;) {
        iterations++;
        
        // 防止无限循环
        if (iterations > 50000) {
            print_string("Too many iterations\n", 21);
            return 1;
        }
        
        // 显示进度
        if (iterations % 1000 == 0) {
            print_string(".", 1);
            if ((iterations / 1000) % 50 == 0) {
                print_string("\n", 2);
            }
        }
        
        // 每10000次迭代显示详细状态
        if (iterations % 10000 == 0) {
            print_string("\nIteration: ", 13);
            print_int(iterations);
            print_string("\n", 2);
        }
        
        // 计算下一项
        next_term(++k);
        
        // 检查是否可以提取数字
        int cmp = bigint_cmp(&num, &acc);
        if (cmp > 0)
            continue;
        
        // 尝试提取数字
        d = extract_digit(3);
        int d2 = extract_digit(4);
        
        // 检查连续的数字是否相同
        if (d != d2)
            continue;
        
        // 输出数字
         //print_string('0' + d ,1);
         // 另一种正确方式
	char digit[1];
	digit[0] = '0' + d;
	print_string(digit, 10);
        
        if (++i % 10 == 0)
            print_string("\n", 2);
        
        eliminate_digit(d);
    }
    
    print_string("\nCalculation complete\n", 24);
    return 0;
}
