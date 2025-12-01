extern void print_string(const char *s, int len);

// The Computer Language Benchmarks Game
#define MAXIMUM_OUTPUT_LENGTH 4096

typedef int intnative_t;
typedef unsigned long long uint64_t;
typedef unsigned int uint32_t;
typedef unsigned long long uintmax_t;  // 移到前面来
typedef unsigned int size_t;  // 添加size_t的定义
#define NULL 0
// Replace snprintf in generate_Frequencies_For_Desired_Length_Oligonucleotides
int format_frequency_output(char *output, int max_length, const char *oligonucleotide, float frequency);

// Replace snprintf in generate_Count_For_Oligonucleotide
int format_count_output(char *output, int max_length, uintmax_t count, const char *oligonucleotide);

// my_ftoa
int my_ftoa(float value, char *buffer, int buffer_size, int precision);

// Replace snprintf in generate_Frequencies_For_Desired_Length_Oligonucleotides
int format_frequency_output(char *output, int max_length, const char *oligonucleotide, float frequency) {
    int pos = 0;
    
    // Copy
    while (*oligonucleotide && pos < max_length - 1) {
        output[pos++] = *oligonucleotide++;
    }
    
    // Insert space
    if (pos < max_length - 1) {
        output[pos++] = ' ';
    }
    
    // float to string
    char float_str[20];
    int float_len = my_ftoa(frequency, float_str, 20, 3);
    
    // copy float 
    for (int i = 0; i < float_len && pos < max_length - 2; i++) {
        output[pos++] = float_str[i];
    }
    
    // Insert /n
    if (pos < max_length - 1) {
        output[pos++] = '\n';
    }
    
    output[pos] = '\0';
    return pos;
}

// Replace snprintf in generate_Count_For_Oligonucleotide
int format_count_output(char *output, int max_length, uintmax_t count, const char *oligonucleotide) {
    int pos = 0;
    
    // 转换计数到字符串
    char count_str[32];
    int count_len = 0;
    
    // 处理特殊情况：0
    if (count == 0) {
        count_str[0] = '0';
        count_len = 1;
    } else {
        // 转换数字
        uintmax_t temp = count;
        while (temp > 0 && count_len < 31) {
            count_str[count_len++] = '0' + (temp % 10);
            temp /= 10;
        }
        
        // 反转字符串
        for (int i = 0; i < count_len / 2; i++) {
            char tmp = count_str[i];
            count_str[i] = count_str[count_len - 1 - i];
            count_str[count_len - 1 - i] = tmp;
        }
    }
    
    count_str[count_len] = '\0';
    
    // 复制计数字符串
    for (int i = 0; i < count_len && pos < max_length - 3; i++) {
        output[pos++] = count_str[i];
    }
    
    // 添加制表符
    if (pos < max_length - 2) {
        output[pos++] = '\t';
    }
    
    // 复制寡核苷酸序列
    while (*oligonucleotide && pos < max_length - 1) {
        output[pos++] = *oligonucleotide++;
    }
    
    output[pos] = '\0';
    return pos;
}

// 另外需要实现my_ftoa函数
int my_ftoa(float value, char *buffer, int buffer_size, int precision) {
    int pos = 0;
    
    // 处理负数
    if (value < 0) {
        buffer[pos++] = '-';
        value = -value;
    }
    
    // 整数部分
    int integer_part = (int)value;
    float fractional_part = value - integer_part;
    
    // 整数转字符串
    if (integer_part == 0) {
        buffer[pos++] = '0';
    } else {
        char integer_str[20];
        int integer_len = 0;
        
        int temp = integer_part;
        while (temp > 0 && integer_len < 19) {
            integer_str[integer_len++] = '0' + (temp % 10);
            temp /= 10;
        }
        
        // 反转整数字符串
        for (int i = integer_len - 1; i >= 0; i--) {
            buffer[pos++] = integer_str[i];
        }
    }
    
    // 小数点
    if (precision > 0) {
        buffer[pos++] = '.';
        
        // 小数部分
        for (int i = 0; i < precision; i++) {
            fractional_part *= 10;
            int digit = (int)fractional_part;
            buffer[pos++] = '0' + digit;
            fractional_part -= digit;
        }
    }
    
    buffer[pos] = '\0';
    return pos;
}

void* memcpy(void* dest, const void* src, size_t n) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    
    return dest;
}

#define TINY_BLOCK_SIZE      32      // 32 byte
#define SMALL_BLOCK_SIZE     128     // 128 byte
#define MEDIUM_BLOCK_SIZE    512     // 512 byte
#define LARGE_BLOCK_SIZE     2048    // 2KB
#define HUGE_BLOCK_SIZE      8192    // 8KB

#define TINY_POOL_COUNT      65536   // 2MB
#define SMALL_POOL_COUNT     32768   // 4MB
#define MEDIUM_POOL_COUNT    8192    // 4MB
#define LARGE_POOL_COUNT     2048    // 4MB
#define HUGE_POOL_COUNT      512     // 4MB

// 内存块头部信息
typedef struct BlockHeader {
    unsigned int size;               // 内存块大小
    unsigned int pool_type;          // 池类型标识
    struct BlockHeader *next;        // 指向下一个空闲块
} BlockHeader;

// 定义不同大小的内存块类型
typedef struct {
    char data[TINY_BLOCK_SIZE - sizeof(BlockHeader)];
} TinyBlock;

typedef struct {
    char data[SMALL_BLOCK_SIZE - sizeof(BlockHeader)];
} SmallBlock;

typedef struct {
    char data[MEDIUM_BLOCK_SIZE - sizeof(BlockHeader)];
} MediumBlock;

typedef struct {
    char data[LARGE_BLOCK_SIZE - sizeof(BlockHeader)];
} LargeBlock;

typedef struct {
    char data[HUGE_BLOCK_SIZE - sizeof(BlockHeader)];
} HugeBlock;

// 各种大小的内存池
static char tiny_pool[TINY_POOL_COUNT * TINY_BLOCK_SIZE];
static char small_pool[SMALL_POOL_COUNT * SMALL_BLOCK_SIZE];
static char medium_pool[MEDIUM_POOL_COUNT * MEDIUM_BLOCK_SIZE];
static char large_pool[LARGE_POOL_COUNT * LARGE_BLOCK_SIZE];
static char huge_pool[HUGE_POOL_COUNT * HUGE_BLOCK_SIZE];

// 空闲链表
static BlockHeader *tiny_freelist = NULL;
static BlockHeader *small_freelist = NULL;
static BlockHeader *medium_freelist = NULL;
static BlockHeader *large_freelist = NULL;
static BlockHeader *huge_freelist = NULL;

// 内存池类型标识
#define POOL_TYPE_TINY    1
#define POOL_TYPE_SMALL   2
#define POOL_TYPE_MEDIUM  3
#define POOL_TYPE_LARGE   4
#define POOL_TYPE_HUGE    5
#define POOL_TYPE_SYSTEM  6  // 系统malloc分配的内存

// 标记是否已经初始化
static int mem_pool_initialized = 0;

// 初始化特定大小的内存池
static void init_pool(void *pool_ptr, unsigned int block_size, 
                     unsigned int block_count, unsigned int pool_type,
                     BlockHeader **freelist) {
    char *p = (char *)pool_ptr;
    
    // 初始化所有块并链接到空闲列表
    for (unsigned int i = 0; i < block_count; i++) {
        BlockHeader *header = (BlockHeader *)p;
        header->size = block_size;
        header->pool_type = pool_type;
        header->next = (i < block_count - 1) ? (BlockHeader *)(p + block_size) : NULL;
        p += block_size;
    }
    
    // 设置空闲列表指向第一个块
    *freelist = (BlockHeader *)pool_ptr;
}

// 初始化所有内存池
void init_mem_pool() {
    if (mem_pool_initialized) return;
    
    // 初始化每种大小的内存池
    init_pool(tiny_pool, TINY_BLOCK_SIZE, TINY_POOL_COUNT, POOL_TYPE_TINY, &tiny_freelist);
    init_pool(small_pool, SMALL_BLOCK_SIZE, SMALL_POOL_COUNT, POOL_TYPE_SMALL, &small_freelist);
    init_pool(medium_pool, MEDIUM_BLOCK_SIZE, MEDIUM_POOL_COUNT, POOL_TYPE_MEDIUM, &medium_freelist);
    init_pool(large_pool, LARGE_BLOCK_SIZE, LARGE_POOL_COUNT, POOL_TYPE_LARGE, &large_freelist);
    init_pool(huge_pool, HUGE_BLOCK_SIZE, HUGE_POOL_COUNT, POOL_TYPE_HUGE, &huge_freelist);
    
    mem_pool_initialized = 1;
}

/*
 * my_malloc: 智能内存分配
 * 根据请求的大小选择合适的内存池
 */
void *my_malloc(unsigned int num_bytes) {
    // 如果内存池未初始化，则初始化
    if (!mem_pool_initialized) {
        init_mem_pool();
    }
    
    // 需要加上头部大小
    unsigned int total_size = num_bytes + sizeof(BlockHeader);
    BlockHeader *block = NULL;
    
    // 根据请求大小选择合适的内存池
    if (total_size <= TINY_BLOCK_SIZE && tiny_freelist != NULL) {
        block = tiny_freelist;
        tiny_freelist = tiny_freelist->next;
    } else if (total_size <= SMALL_BLOCK_SIZE && small_freelist != NULL) {
        block = small_freelist;
        small_freelist = small_freelist->next;
    } else if (total_size <= MEDIUM_BLOCK_SIZE && medium_freelist != NULL) {
        block = medium_freelist;
        medium_freelist = medium_freelist->next;
    } else if (total_size <= LARGE_BLOCK_SIZE && large_freelist != NULL) {
        block = large_freelist;
        large_freelist = large_freelist->next;
    } else if (total_size <= HUGE_BLOCK_SIZE && huge_freelist != NULL) {
        block = huge_freelist;
        huge_freelist = huge_freelist->next;
    }

    if (block != NULL) {
        // 返回数据区域指针（跳过头部）
        return (void *)((char *)block + sizeof(BlockHeader));
    }
    
    return NULL;
}

/**
 * my_free: 释放内存
 * 根据内存块的池类型将其放回正确的空闲列表
 */
void my_free(void *ptr) {
    if (ptr == NULL) return;
    
    // 计算块头部地址
    BlockHeader *block = (BlockHeader *)((char *)ptr - sizeof(BlockHeader));
    
    // 根据池类型归还到相应的空闲列表
    switch (block->pool_type) {
        case POOL_TYPE_TINY:
            block->next = tiny_freelist;
            tiny_freelist = block;
            break;
        case POOL_TYPE_SMALL:
            block->next = small_freelist;
            small_freelist = block;
            break;
        case POOL_TYPE_MEDIUM:
            block->next = medium_freelist;
            medium_freelist = block;
            break;
        case POOL_TYPE_LARGE:
            block->next = large_freelist;
            large_freelist = block;
            break;
        case POOL_TYPE_HUGE:
            block->next = huge_freelist;
            huge_freelist = block;
            break;
        case POOL_TYPE_SYSTEM:
            // 系统分配的内存直接释放
            break;
    }
}

/***全局内存池***/
// 定义一个足够大的全局数组来存储polynucleotide数据
#define MAX_POLYNUCLEOTIDE_CAPACITY (128 * 1024 * 1024)  // 128MB，留有余量
static char polynucleotide[MAX_POLYNUCLEOTIDE_CAPACITY];
static intnative_t polynucleotide_Length = 0;

/***硬编码数据区域***/
// 硬编码存储输入数据
static const char input_data[] = 
">THREE Homo sapiens frequency\n"
"cccataactacaatagtcggcaatcttttattacccagaactaacgtttttatttcccgg\n"
"tacgtatcacattaatcttaatttaatgcgtgagagtaacgatgaacgaaagttatttat\n"
"gtttaagccgcttcttgagaatacagattactgttagaatgaaggcatcataactagaac\n"
"accaacgcgcacctcgcacattactctaatagtagctttattcagtttaatatagacagt\n"
"atttgaaccaggcgctaatgttaaggcccccttcgaaaaccttgttatgttattccatgt\n"
"ggtcggaggatttgcggggcgatagcgctgggcggggatcaacaatttcgttcatgcgag\n"
"cgcccccataaccagtaggtacagttcggaaaagaaaaccccacgcactcgctagaagtg\n"
"ttacaatcacatcacttcgtaccgaagggactactgtattccgtcttggggatgtaacag\n"
"actgattacagtcttatgatgaagcctcattcatctaaaattagttgatttattccacgg\n"
"atactatcacactcctatagaaagagttaccaccgtgggaagctagatataataaataaa\n"
"agacatacaatattagtatggctcatgatctacacttactcggatctctctttttttata\n"
"accagtagatcgcattacacgtattgttgttccgcatcaggccctaggggctcaaacttc\n"
"catggtggataactaaaacgtccgtcactaaacgaagatattaatagatgaaatacacgg\n"
"gtttacttgatttctgttcagtcattcacgggaaatcctaggagtctttcataacggcgg\n"
"tcttagtaggaatgtagtcaagctctgtagaggtctcgacggaattggtatttcctggca\n"
"tcacaatttacctagtattggagatcacttaaaataatgttgagataataatcaggatat\n"
"ttctagtatgtgacaaacctctatttagtgattgtgattttcaattaaacaagacgtagg\n"
"ggtcaaattaacgactacatgttggaaagaaggccgaattgtaatatctaactcatgtac\n"
"taagaagaagtgctttcgtttaaggctttctgtctaacattctaacgtcaattcctatgt\n"
"aatactactgtaaccaagttattactcggctgcgtagataaagtctcatgtaaatgacgg\n"
"tttatctgttacttttgggtttcaacctagctaggacgccggtactaattacgacacctg\n"
"cgtatagtgcagggtgttcaatgtgcctttttatgtccggattataaccatccctctccc\n"
"acttggaatatcaccgggttcttaatgacttagttcgtcttccttattttccgggtaaga\n"
"tcgctgtggaccggacccattttgatctagtctaaaaaggtatatagcgtttcgtctggc\n"
"ccgcttacgttcactgaaacttagattaatcaatgcactgcactggattaacaagaacat\n"
"gttatagtgtactgacacatgttagactaagaggtctgttcgggttagccgacttatatg\n"
"tttaaccgattttgacaactgggttgagagataacaatgaagagtgaggactgtagaaga\n"
"tcttaaaactgtaccatagtgctcaattcgctaatggcttgaattatttaattgttctaa\n"
"ccctggcgtcgaatttttttggttcgaaaatacttagcacagcgtattgttcaacgagat\n"
"gcacaactgtaccgttagaaagcggcttaatgacaaggcagtattgtgactattgacagg\n"
"gaatcctaaaaagctactcgaattggtatatggaagaggtatgtactgagaggtcgcgcc\n"
"tattagtcaaattctgccaaagaagagtcaaaagcttaactagtttgatggtatgaggtt\n"
"taatgctaggtggtctataccaccaaaaagtatatgggatatcccagaatttatcgactt\n"
"tcaatcgtctaccgtcacgacgtacactaggcagccctaatccaaaacttttgaggatga\n"
"gtactgccactattatactgtaccatttgtaacttacattttatatcttcaaagaggtag\n"
"atattgtcggccattactgtcacttacactaagggtagcttgattactgatacctctcat\n"
"ggtaaaaagtaatttaagaacctatttttttacataacctctgctactaccgttagtgtt\n"
"ttagtcggttcaagtcacaaaatccctgtagcgcacccctataagcagaaggaaacctta\n"
"atgcggataaaaacttttgccggaaccgttaatcctatgagaataccactcttggaatcg\n"
"gtcctttaggctgaggatatagaacgaggggaacgcatcaatctaggttaggtgagagaa\n"
"ctttgtatcaaaacgcaagtaccatatgccgtcctcagtaaattgccaaatgcagaaatc\n"
"ttacactcttttcttaactaagtatgagagcaacctcactcctgaacagcttgttaccta\n"
"acgagaagaggctttaagtagcctggagcctcaaccggatatccggatttgactctcatc\n"
"cacttacatgatgattacggtcattacatctcatgattttctgagtgccctatagactgg\n"
"gaatttaatctaccctgtttctatttgttaacaaggagaaccactggtcaagatgacgcg\n"
"cttccatttatgccaccataagtaagttctcggaacccttacatgattggcctaccaacc\n"
"tatatatgtgaccaatgtacggtacatagagtgtggcctatcatattcaggtcatcgagc\n"
"tcagtatttaaagattatatggtcgctgggggtattcagtgcgcgatggaagactaacat\n"
"tggaaatcaacggaattgacaacacgctcactttaataacctatctcaggataagtttaa\n"
"tgtaattagacggaactttctctaactccgtgtactaactctttgaaaataatgtgggta\n"
"tttttatttcatctagatttgtctgtatcgaaagaaagtattggtccaaataatcctcag\n"
"taaaatcaagtcataaatataaaatttagatcttaggacagaggaaagtgctttcccgag\n"
"cataggatctggcctacgccagtagttcatgcttgtgttaaaagttgttactgtttatag\n"
"tccgtactcagggtagtgttcgatactcagcggggaactgacatattacactaaggaatc\n"
"aaggcccttcgtatgggtcatgtttatatatttaattacttacgctatttgatcgagaat\n"
"agctatagtaacgtcgtaagaatgcaggatgcgattcgagtttgtaaattcacagatact\n"
"gtgtatcatattattatagatgttaaggcatagaattattggtattgatgtacaaaaaat\n"
"tatgggtgggcagtaccgataggcattacgagcagtgcagcttggaagaactggatgtat\n"
"cctataactagtaagagccttaaaggtactacatacccagggatgttaccatcattaatt\n"
"tggccatcttcaatcttcgcaatgcatactttcttctacaagatgccttttagaagacaa\n"
"aataagtgtcaacaataacgctgtaacttaactctgttgtacgtggaatcaagtctcact\n"
"aaagcaactaacattccgacatgcaaacgcaggactactagattattaaattcgccagcc\n"
"cgcctcgtttaatataacatcataaaaattctaagtaatatctcacacactaatccgcca\n"
"tcgtccatagcatcagtcacctgtcttacacaaacacatgtttaatcgatgttgttatgc\n"
"caagctagtttcgcgaccatgtaactaattgtggaaagctgctaccttgaacgacatcaa\n"
"ccatcctacctttgtacaacagaccaacatctctgtactggtaaatagatctgaaaagtt\n"
"ataaatataactgttttcacattgatagaaaaacagctatgtgctatttgtatatactat\n"
"aataaattaagcgaaacatggagattaaaacagtgttttctcatcctccacctcttgttc\n";

// 全局变量用于跟踪读取位置
static size_t input_position = 0;

// 自定义fgets函数，从硬编码数组中读取一行
char* my_fgets(char* buffer, int size, void* unused_stream) {
    if (input_position >= sizeof(input_data) - 1) {
        return NULL; // 已到达数据末尾
    }
    
    int i = 0;
    while (i < size - 1 && input_position < sizeof(input_data) - 1) {
        buffer[i] = input_data[input_position];
        input_position++;
        
        if (buffer[i] == '\n') {
            i++;
            break;
        }
        i++;
    }
    
    buffer[i] = '\0';
    return (i > 0) ? buffer : NULL;
}


/***string库简单实现***/
// 返回值: 0表示相等，<0表示s1<s2，>0表示s1>s2
int my_memcmp(const void *s1, const void *s2, size_t n) {
    const unsigned char *p1 = s1, *p2 = s2;
    
    // 逐字节比较
    while (n--) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    
    return 0; // 内存块相等
}

// 计算字符串长度（不包括'\0'结束符）
size_t my_strlen(const char *str) {
    const char *s = str;
    
    // 计数直到遇到结束符
    while (*s) {
        s++;
    }
    
    return s - str;
}




// 简单的快速排序实现
static void _my_qsort_swap(char *a, char *b, size_t size) {
    char tmp;
    size_t i;
    for (i = 0; i < size; i++) {
        tmp = a[i];
        a[i] = b[i];
        b[i] = tmp;
    }
}

void my_qsort(void *base, size_t nmemb, size_t size, 
              int (*compar)(const void *, const void *)) {
    if (nmemb <= 1) return;
    
    char *cbase = (char *)base;
    char *pivot = cbase + (nmemb - 1) * size;
    size_t i = 0;
    
    for (size_t j = 0; j < nmemb - 1; j++) {
        if (compar(cbase + j * size, pivot) <= 0) {
            _my_qsort_swap(cbase + i * size, cbase + j * size, size);
            i++;
        }
    }
    
    _my_qsort_swap(cbase + i * size, pivot, size);
    
    my_qsort(cbase, i, size, compar);
    if (i + 1 < nmemb) {
        my_qsort(cbase + (i + 1) * size, nmemb - i - 1, size, compar);
    }
}

/***哈希表的实现***/
// 自定义哈希函数
#define CUSTOM_HASH_FUNCTION(key) ((key) ^ (key)>>7)

// 哈希表的初始容量和最大负载因子
#define HASH_INITIAL_SIZE 1024
#define HASH_MAX_LOAD_FACTOR 0.75

// 哈希表项结构
typedef struct HashEntry {
    uint64_t key;
    uint32_t value;
    struct HashEntry *next;
    int used;  // 标记此项是否被使用
} HashEntry;

// 哈希表结构
typedef struct {
    HashEntry **buckets;
    size_t size;        // 当前桶数
    size_t count;       // 当前元素数
    size_t capacity;    // 最大容量 (size * HASH_MAX_LOAD_FACTOR)
} HashMap;

// 创建一个新的哈希表
HashMap* hashmap_create() {
    HashMap* map = (HashMap*)my_malloc(sizeof(HashMap));
    if (!map) return NULL;
    
    map->size = HASH_INITIAL_SIZE;
    map->count = 0;
    map->capacity = (size_t)(HASH_INITIAL_SIZE * HASH_MAX_LOAD_FACTOR);
    
    map->buckets = (HashEntry**)my_malloc(map->size * sizeof(HashEntry*));
    if (!map->buckets) {
        my_free(map);
        return NULL;
    }
    
    // 初始化所有桶为NULL
    for (size_t i = 0; i < map->size; i++) {
        map->buckets[i] = NULL;
    }
    
    return map;
}

// 计算键的哈希值
size_t hashmap_hash(HashMap* map, uint64_t key) {
    return CUSTOM_HASH_FUNCTION(key) % map->size;
}

// 扩展哈希表
void hashmap_resize(HashMap* map) {
    size_t old_size = map->size;
    HashEntry** old_buckets = map->buckets;
    
    // 创建新的桶数组，大小为原来的两倍
    map->size = old_size * 2;
    map->capacity = (size_t)(map->size * HASH_MAX_LOAD_FACTOR);
    map->buckets = (HashEntry**)my_malloc(map->size * sizeof(HashEntry*));
    
    if (!map->buckets) {
        // 分配失败，恢复旧值
        map->size = old_size;
        map->capacity = (size_t)(old_size * HASH_MAX_LOAD_FACTOR);
        map->buckets = old_buckets;
        return;
    }
    
    // 初始化新桶
    for (size_t i = 0; i < map->size; i++) {
        map->buckets[i] = NULL;
    }
    
    // 重新散列所有现有项
    for (size_t i = 0; i < old_size; i++) {
        HashEntry* entry = old_buckets[i];
        while (entry) {
            HashEntry* next = entry->next;
            
            // 计算新的桶索引
            size_t new_index = hashmap_hash(map, entry->key);
            
            // 将项添加到新桶
            entry->next = map->buckets[new_index];
            map->buckets[new_index] = entry;
            
            entry = next;
        }
    }
    
    // 释放旧的桶数组
    my_free(old_buckets);
}

// 向哈希表中插入键值对，如果键已存在则更新值
// 返回：0 表示更新现有项，1 表示插入新项
int hashmap_put(HashMap* map, uint64_t key, uint32_t value, int* was_unused) {
    // 检查是否需要扩展哈希表
    if (map->count >= map->capacity) {
        hashmap_resize(map);
    }
    
    size_t index = hashmap_hash(map, key);
    HashEntry* entry = map->buckets[index];
    
    // 检查键是否已存在
    while (entry) {
        if (entry->key == key) {
            // 更新现有项
            entry->value = value;
            if (was_unused) *was_unused = 0;
            return 0;
        }
        entry = entry->next;
    }
    
    // 创建新项
    entry = (HashEntry*)my_malloc(sizeof(HashEntry));
    if (!entry) return -1;  // 内存分配失败
    
    entry->key = key;
    entry->value = value;
    entry->used = 1;
    entry->next = map->buckets[index];
    map->buckets[index] = entry;
    map->count++;
    
    if (was_unused) *was_unused = 1;
    return 1;
}

// 从哈希表中获取键对应的值
// 返回：-1 表示键不存在，否则返回项的索引
int hashmap_get(HashMap* map, uint64_t key) {
    size_t index = hashmap_hash(map, key);
    HashEntry* entry = map->buckets[index];
    
    while (entry) {
        if (entry->key == key) {
            return index;  // 找到键
        }
        entry = entry->next;
    }
    
    return -1;  // 键不存在
}

// 获取哈希表中特定位置项的值
uint32_t hashmap_value(HashMap* map, int index, uint64_t key) {
    if (index < 0 || index >= (int)map->size) return 0;
    
    HashEntry* entry = map->buckets[index];
    while (entry) {
        if (entry->key == key) {
            return entry->value;
        }
        entry = entry->next;
    }
    
    return 0;
}

// 获取哈希表的大小（元素数量）
size_t hashmap_size(HashMap* map) {
    return map->count;
}

// 定义用于遍历哈希表项的回调函数类型
typedef void (*HashMapIterFunc)(uint64_t key, uint32_t value, void* user_data);

// 遍历哈希表中的所有项
void hashmap_foreach(HashMap* map, HashMapIterFunc func, void* user_data) {
    for (size_t i = 0; i < map->size; i++) {
        HashEntry* entry = map->buckets[i];
        while (entry) {
            if (entry->used) {
                func(entry->key, entry->value, user_data);
            }
            entry = entry->next;
        }
    }
}

// 销毁哈希表
void hashmap_destroy(HashMap* map) {
    if (!map) return;
    
    for (size_t i = 0; i < map->size; i++) {
        HashEntry* entry = map->buckets[i];
        while (entry) {
            HashEntry* next = entry->next;
            my_free(entry);
            entry = next;
        }
    }
    
    my_free(map->buckets);
    my_free(map);
}

// 表示散列表末尾的常量
#define HASHMAP_END -1

//定义一个键值对类型
typedef struct {
    uint64_t key;
    uint32_t value;
} element;

// Macro to convert a nucleotide character to a code. Note that upper and lower
// case ASCII letters only differ in the fifth bit from the right and we only
// need the three least significant bits to differentiate the letters 'A', 'C',
// 'G', and 'T'. Spaces in this array/string will never be used as long as
// characters other than 'A', 'C', 'G', and 'T' aren't used.
//#define code_For_Nucleotide(nucleotide) (" \0 \1\3  \2"[nucleotide & 0x7])
// 替换原来的宏
// char code_For_Nucleotide(char nucleotide) {
//     switch(nucleotide & 0x7) {
//         case 1: return 0; // 'A'
//         case 3: return 1; // 'C'
//         case 7: return 2; // 'G'  
//         case 4: return 3; // 'T'
//         default: return 0; // 默认为'A'
//     }
// }
// 修正版的code_For_Nucleotide函数
char code_For_Nucleotide(char nucleotide) {
    switch(nucleotide) {
        case 'A': case 'a': return 0;
        case 'C': case 'c': return 1;
        case 'G': case 'g': return 2;  
        case 'T': case 't': return 3;
        default: return 0; // 默认为'A'
    }
}
// And one more macro to convert the codes back to nucleotide characters.
#define nucleotide_For_Code(code) ("ACGT"[code & 0x3])

// 用于收集哈希表项的结构和回调
typedef struct {
    element* array;
    size_t index;
} CollectContext;

void collect_element(uint64_t key, uint32_t value, void* user_data) {
    CollectContext* context = (CollectContext*)user_data;
    context->array[context->index++] = (element){key, value};
}

// Function to use when sorting elements with qsort() later. Elements with
// larger values will come first and in cases of identical values then elements
// with smaller keys will come first.
static int element_Compare(const element * const left_Element
  , const element * const right_Element){

    // Sort based on element values.
    if(left_Element->value < right_Element->value) return 1;
    if(left_Element->value > right_Element->value) return -1;

    // If we got here then both items have the same value so then sort based on
    // key.
    return left_Element->key > right_Element->key ? 1 : -1;
}

// Generate frequencies for all oligonucleotides in polynucleotide that are of
// desired_Length_For_Oligonucleotides and then save it to output.
static void generate_Frequencies_For_Desired_Length_Oligonucleotides(
  const char * const polynucleotide, const intnative_t polynucleotide_Length
  , const intnative_t desired_Length_For_Oligonucleotides, char * const output){

    HashMap* hash_Table = hashmap_create();

    uint64_t key=0;
    const uint64_t mask=((uint64_t)1<<2*desired_Length_For_Oligonucleotides)-1;

    // For the first several nucleotides we only need to append them to key in
    // preparation for the insertion of complete oligonucleotides to hash_Table.
    for(intnative_t i=0; i<desired_Length_For_Oligonucleotides-1; i++)
        key=(key<<2 & mask) | polynucleotide[i];

    // Add all the complete oligonucleotides of
    // desired_Length_For_Oligonucleotides to hash_Table and update the count
    // for each oligonucleotide.
    for(intnative_t i=desired_Length_For_Oligonucleotides-1
      ; i<polynucleotide_Length; i++){

        key=(key<<2 & mask) | polynucleotide[i];

        int element_Was_Unused;
        int idx = hashmap_get(hash_Table, key);
        
        if(idx == -1) {
            // Key doesn't exist, insert with value 1
            hashmap_put(hash_Table, key, 1, &element_Was_Unused);
        } else {
            // Key exists, increment value
            uint32_t value = hashmap_value(hash_Table, idx, key);
            hashmap_put(hash_Table, key, value + 1, &element_Was_Unused);
        }
    }

    // Create an array of elements from hash_Table.
    intnative_t elements_Array_Size = hashmap_size(hash_Table);
    element* elements_Array = my_malloc(elements_Array_Size * sizeof(element));
    
    // 收集哈希表中的所有项
    CollectContext context = {elements_Array, 0};
    hashmap_foreach(hash_Table, collect_element, &context);

    hashmap_destroy(hash_Table);

    // Sort elements_Array.
    my_qsort(elements_Array, elements_Array_Size, sizeof(element)
      , (int (*)(const void *, const void *)) element_Compare);

    // Print the frequencies for each oligonucleotide.
    for(intnative_t output_Position=0, i=0; i<elements_Array_Size; i++){

        // Convert the key for the oligonucleotide to a string.
        char oligonucleotide[desired_Length_For_Oligonucleotides+1];
        for(intnative_t j=desired_Length_For_Oligonucleotides-1; j>-1; j--){
            oligonucleotide[j]=nucleotide_For_Code(elements_Array[i].key);
            elements_Array[i].key>>=2;
        }
        oligonucleotide[desired_Length_For_Oligonucleotides]='\0';

        // // Output the frequency for oligonucleotide to output.
        // output_Position+=snprintf(output+output_Position
        //   , MAXIMUM_OUTPUT_LENGTH-output_Position, "%s %.3f\n", oligonucleotide
        //   , 100.0f*elements_Array[i].value
        //   /(polynucleotide_Length-desired_Length_For_Oligonucleotides+1));
// 在 generate_Frequencies_For_Desired_Length_Oligonucleotides 中替换 snprintf
        output_Position += format_frequency_output(output + output_Position,
            MAXIMUM_OUTPUT_LENGTH - output_Position, 
            oligonucleotide,
            100.0f * elements_Array[i].value / (polynucleotide_Length - desired_Length_For_Oligonucleotides + 1));

    }

    my_free(elements_Array);
}

// Generate a count for the number of times oligonucleotide appears in
// polynucleotide and then save it to output.
static void generate_Count_For_Oligonucleotide(
  const char * const polynucleotide, const intnative_t polynucleotide_Length
  , const char * const oligonucleotide, char * const output){
    const intnative_t oligonucleotide_Length=my_strlen(oligonucleotide);

    HashMap* hash_Table = hashmap_create();

    uint64_t key=0;
    const uint64_t mask=((uint64_t)1<<2*oligonucleotide_Length)-1;

    // For the first several nucleotides we only need to append them to key in
    // preparation for the insertion of complete oligonucleotides to hash_Table.
    for(intnative_t i=0; i<oligonucleotide_Length-1; i++)
        key=(key<<2 & mask) | polynucleotide[i];

    // Add all the complete oligonucleotides of oligonucleotide_Length to
    // hash_Table and update the count for each oligonucleotide.
    for(intnative_t i=oligonucleotide_Length-1; i<polynucleotide_Length; i++){

        key=(key<<2 & mask) | polynucleotide[i];

        int element_Was_Unused;
        int idx = hashmap_get(hash_Table, key);
        
        if(idx == -1) {
            // Key doesn't exist, insert with value 1
            hashmap_put(hash_Table, key, 1, &element_Was_Unused);
        } else {
            // Key exists, increment value
            uint32_t value = hashmap_value(hash_Table, idx, key);
            hashmap_put(hash_Table, key, value + 1, &element_Was_Unused);
        }
    }

    // Generate the key for oligonucleotide.
    key=0;
    for(intnative_t i=0; i<oligonucleotide_Length; i++)
        key=(key<<2) | code_For_Nucleotide(oligonucleotide[i]);

    // Output the count for oligonucleotide to output.
    int idx = hashmap_get(hash_Table, key);
    uintmax_t count = (idx == -1) ? 0 : hashmap_value(hash_Table, idx, key);
    // snprintf(output, MAXIMUM_OUTPUT_LENGTH, "%ju\t%s", count, oligonucleotide);
    // 在 generate_Count_For_Oligonucleotide 中替换 snprintf
    format_count_output(output, MAXIMUM_OUTPUT_LENGTH, count, oligonucleotide);
    hashmap_destroy(hash_Table);
}



int start(){
    char buffer[4096];
    init_mem_pool();
    // //从文件里面获取
    // FILE *inputFile = fopen("knucleotide-input25000000.txt", "r");
    // if (inputFile == NULL) {
    //     perror("无法打开输入文件");
    //     return 1;
    // }
    
    // Find the start of the third polynucleotide.
    while(my_fgets(buffer, sizeof(buffer), 0) && my_memcmp(">THREE", buffer
      , sizeof(">THREE")-1));

    // Start with 1 MB of storage for reading in the polynucleotide and grow
    // geometrically.
    // intnative_t polynucleotide_Capacity=1048576;
    // intnative_t polynucleotide_Length=0;
    // char * polynucleotide=my_malloc(polynucleotide_Capacity);
    // Start reading and encoding the third polynucleotide.
    while(my_fgets(buffer, sizeof(buffer), 0) && buffer[0]!='>'){
        for(intnative_t i=0; buffer[i]!='\0'; i++)
            if(buffer[i]!='\n')
                polynucleotide[polynucleotide_Length++]
                  =code_For_Nucleotide(buffer[i]);

        // Make sure we still have enough memory allocated for any potential
        // nucleotides in the next line.
         // 显示进度（每处理1MB数据显示一次）
        // if(polynucleotide_Length % (1024*1024) == 0) {
        //     printf("已处理: %d MB\n", polynucleotide_Length/(1024*1024));
        // }
        // if(polynucleotide_Capacity-polynucleotide_Length<sizeof(buffer)){
		// 	//polynucleotide=realloc(polynucleotide, polynucleotide_Capacity+=1048576);
        //     printf("当前占用的内存为  %d\n", polynucleotide_Capacity/1024/1024);
        // }
    }

    // 关闭文件
   // fclose(inputFile);

    char output_Buffer[7][MAXIMUM_OUTPUT_LENGTH];

    // Do the following functions in parallel.
    // #pragma omp parallel sections
    // {
        //检测特定寡核苷酸的次数
        //#pragma omp section
        generate_Count_For_Oligonucleotide(polynucleotide
          , polynucleotide_Length, "GGTATTTTAATTTATAGT", output_Buffer[6]);
        //#pragma omp section
        generate_Count_For_Oligonucleotide(polynucleotide
          , polynucleotide_Length, "GGTATTTTAATT", output_Buffer[5]);
        //#pragma omp section
        generate_Count_For_Oligonucleotide(polynucleotide
          , polynucleotide_Length, "GGTATT", output_Buffer[4]);
        //#pragma omp section
        generate_Count_For_Oligonucleotide(polynucleotide
          , polynucleotide_Length, "GGTA", output_Buffer[3]);
        //#pragma omp section
        generate_Count_For_Oligonucleotide(polynucleotide
          , polynucleotide_Length, "GGT", output_Buffer[2]);

        //#pragma omp section
        //统计二聚体的频率
        generate_Frequencies_For_Desired_Length_Oligonucleotides(polynucleotide
          , polynucleotide_Length, 2, output_Buffer[1]);
        //#pragma omp section
        //统计单个碱基的频率
        generate_Frequencies_For_Desired_Length_Oligonucleotides(polynucleotide
          , polynucleotide_Length, 1, output_Buffer[0]);
    //}
        // 打印结果时添加分隔符
        for(intnative_t i=0; i<7; i++) {
            print_string(output_Buffer[i], 200);
            print_string("\n", 1);
        }

    return 0;
}
