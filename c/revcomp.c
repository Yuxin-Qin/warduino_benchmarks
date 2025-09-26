// path: bcscripts/examples/revcomp.c
// Build (export memory so host can read strings):
//   clang --target=wasm32 -nostdlib \
//     -Wl,--no-entry -Wl,--export-all -Wl,--export-memory \
//     -Wno-implicit-function-declaration -Wl,--allow-undefined \
//     -O2 -o revcomp.wasm revcomp.c

// Enhanced WebAssembly version of revcomp for FASTA format processing

// Pool / sequence configuration
#define CELL_SIZE 64
#define POOL_SIZE_IN_PAGES 64      // 64 * 4096 = 256 KiB
#define PAGE_SIZE 4096
#define MAX_SEQUENCE_LENGTH 100000
#define LINE_WRAP 60

// Explicit env imports (why: match host signatures exactly)
__attribute__((import_module("env"), import_name("print_string")))
extern void print_string(const char *p);   // (i32) -> ()

__attribute__((import_module("env"), import_name("print_int")))
extern void print_int(int v);              // (i32) -> ()

// Globals
static char complement_table[256];
static char mem_pool[POOL_SIZE_IN_PAGES * PAGE_SIZE];
static int pool_offset = 0;

static char file_buffer[MAX_SEQUENCE_LENGTH];
static char sequence_buffer[MAX_SEQUENCE_LENGTH];
static char output_buffer[MAX_SEQUENCE_LENGTH];

// Bump allocator (unused by main flow; kept for parity)
static void* enhanced_malloc(int size) {
    if (size <= 0) return 0;
    if (pool_offset + size > (int)sizeof(mem_pool)) return 0;
    void* ptr = &mem_pool[pool_offset];
    pool_offset += size;
    return ptr;
}
static void reset_memory_pool(void) { pool_offset = 0; }

// Init complement table
static void init_complement(void) {
    for (int i = 0; i < 256; i++) complement_table[i] = (char)i;
    complement_table['A'] = 'T'; complement_table['T'] = 'A';
    complement_table['C'] = 'G'; complement_table['G'] = 'C';
    complement_table['a'] = 't'; complement_table['t'] = 'a';
    complement_table['c'] = 'g'; complement_table['g'] = 'c';
    complement_table['R'] = 'Y'; complement_table['Y'] = 'R';
    complement_table['S'] = 'S'; complement_table['W'] = 'W';
    complement_table['K'] = 'M'; complement_table['M'] = 'K';
    complement_table['B'] = 'V'; complement_table['D'] = 'H';
    complement_table['H'] = 'D'; complement_table['V'] = 'B';
    complement_table['N'] = 'N';
}

// Safe helpers
static int enhanced_strlen(const char* s) {
    if (!s) return 0;
    int len = 0;
    while (s[len] != '\0' && len < MAX_SEQUENCE_LENGTH) len++;
    return len;
}
static void safe_strcpy(char* dest, const char* src, int max_len) {
    if (!dest || !src || max_len <= 0) return;
    int i = 0;
    while (src[i] != '\0' && i < max_len - 1) { dest[i] = src[i]; i++; }
    dest[i] = '\0';
}

// Reverse-complement in place
static void enhanced_reverse_complement(char* seq, int len) {
    if (!seq || len <= 0) return;
    for (int i = 0; i < len / 2; i++) {
        int j = len - 1 - i;
        char ci = complement_table[(unsigned char)seq[i]];
        char cj = complement_table[(unsigned char)seq[j]];
        seq[i] = cj;
        seq[j] = ci;
    }
    if (len & 1) {
        int mid = len / 2;
        seq[mid] = complement_table[(unsigned char)seq[mid]];
    }
}

// Print one FASTA record, wrapped
static void process_fasta_sequence(const char* header, const char* sequence, int seq_len) {
    if (!sequence || seq_len <= 0) return;

    safe_strcpy(output_buffer, sequence, MAX_SEQUENCE_LENGTH);
    enhanced_reverse_complement(output_buffer, seq_len);

    print_string(header);
    print_string("\n");

    int pos = 0;
    while (pos < seq_len) {
        int line_len = (seq_len - pos > LINE_WRAP) ? LINE_WRAP : (seq_len - pos);

        // Temporarily NUL-terminate slice within global buffer (why: avoid stack pointer to host)
        char saved = output_buffer[pos + line_len];
        output_buffer[pos + line_len] = '\0';
        print_string(&output_buffer[pos]);
        output_buffer[pos + line_len] = saved;

        print_string("\n");
        pos += line_len;
    }
}

// Demo data
static void process_test_fasta(void) {
    const char* header = ">ONE Homo sapiens alu";
    const char* full_sequence =
        "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCGAGGCGGGCGGA"
        "TCACCTGAGGTCAGGAGTTCGAGACCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACT"
        "AAAAATACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCAGCTACTCGGGAG"
        "GCTGAGGCAGGAGAATCGCTTGAACCCGGGAGGCGGAGGTTGCAGTGAGCCGAGATCGCG"
        "CCACTGCACTCCAGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAAGGCCGGGCGCGGT"
        "GGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCGAGGCGGGCGGATCACCTGAGGTCA"
        "GGAGTTCGAGACCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAATACAAAAA"
        "TTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCAGCTACTCGGGAGGCTGAGGCAGGAG"
        "AATCGCTTGAACCCGGGAGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCCA"
        "GCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAAGGCCGGGCGCGGTGGCTCACGCCTGT"
        "AATCCCAGCACTTTGGGAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGACC"
        "AGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAATACAAAAATTAGCCGGGCGTG"
        "GTGGCGCGCGCCTGTAATCCCAGCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACC"
        "CGGGAGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCCAGCCTGGGCGACAG"
        "AGCGAGACTCCGTCTCAAAAAGGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTT"
        "TGGGAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGACCAGCCTGGCCAACA"
        "TGGTGAAACCCCGTCTCTACTAAAAATACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCT"
        "GTAATCCCAGCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGGAGGCGGAGG"
        "TTGCAGTGAGCCGAGATCGCGCCACTGCACTCCAGCCTGGGCGACAGAGCGAGACTCCGT"
        "CTCAAAAAGGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCGAGG"
        "CGGGCGGATCACCTGAGGTCAGGAGTTCGAGACCAGCCTGGCCAACATGGTGAAACCCCG"
        "TCTCTACTAAAAATACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCAGCTA"
        "CTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGGAGGCGGAGGTTGCAGTGAGCCG"
        "AGATCGCGCCACTGCACTCCAGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAAGGCCG"
        "GGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCGAGGCGGGCGGATCACC"
        "TGAGGTCAGGAGTTCGAGACCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAA"
        "TACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCAGCTACTCGGGAGGCTGA"
        "GGCAGGAGAATCGCTTGAACCCGGGAGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACT"
        "GCACTCCAGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAAGGCCGGGCGCGGTGGCTC"
        "ACGCCTGTAATCCCAGCACTTTGGGAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGT"
        "TCGAGACCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAATACAAAAATTAGC"
        "CGGGCGTGGTGGCGCGCGCCTGTAATCCCAGCTACTCGGGAGGCTGAGGCAGGAGAATCG"
        "CTTGAACCCGGGAGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCCAGCCTG"
        "GGCGACAGAGCGAGACTCCGTCTCAAAAAGGCCGGGCGCGGTGGCTCACGCCTGTAATCC"
        "CAGCACTTTGGGAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGACCAGCCT"
        "GGCCAACATGGTGAAACCCCGTCTCTACTAAAAATACAAAAATTAGCCGGGCGTGGTGGC"
        "GCGCGCCTGTAATCCCAGCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGGA"
        "GGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCCAGCCTGGGCGACAGAGCGA"
        "GACTCCGTCTCAAAAAGGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGGGA"
        "GGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGACCAGCCTGGCCAACATGGTG"
        "AAACCCCGTCTCTACTAAAAATACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAAT"
        "CCCAGCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGGAGGCGGAGGTTGCA"
        "GTGAGCCGAGATCGCGCCACTGCACTCCAGCCTGGGCGACAGAGCGAGACTCCGTCTCAA"
        "AAAGGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCGAGGCGGGC"
        "GGATCACCTGAGGTCAGGAGTTCGAGACCAGCCTGGCCAACATGGTGAAACCCCGTCTCT"
        "ACTAAAAATACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCAGCTACTCGG"
        "GAGGCTGAGGCAGGAGAATCGCTTGAACCCGGGAGGCGGAGGTTGCAGTGAGCCGAGATC"
        "GCGCCACTGCACTCCAGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAAGGCCGGGCGC"
        "GGTGGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCGAGGCGGGCGGATCACCTGAGG"
        "TCAGGAGTTCGAGACCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAATACAA"
        "AAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCAGCTACTCGGGAGGCTGAGGCAG"
        "GAGAATCGCTTGAACCCGGGAGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACT"
        "CCAGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAAGGCCGGGCGCGGTGGCTCACGCC"
        "TGTAATCCCAGCACTTTGGGAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAG"
        "ACCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAATACAAAAATTAGCCGGGC"
        "GTGGTGGCGCGCGCCTGTAATCCCAGCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGA"
        "ACCCGGGAGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCCAGCCTGGGCGA"
        "CAGAGCGAGACTCCGTCTCAAAAAGGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCA"
        "CTTTGGGAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGACCAGCCTGGCCA"
        "ACATGGTGAAACCCCGTCTCTACTAAAAATACAAAAATTAGCCGGGCGTGGTGGCGCGCG"
        "CCTGTAATCCCAGCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGGAGGCGG"
        "AGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCCAGCCTGGGCGACAGAGCGAGACTC"
        "CGTCTCAAAAAGGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCG"
        "AGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGACCAGCCTGGCCAACATGGTGAAACC"
        "CCGTCTCTACTAAAAATACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCAG"
        "CTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGGAGGCGGAGGTTGCAGTGAG"
        "CCGAGATCGCGCCACTGCACTCCAGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAAGG"
        "CCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCGAGGCGGGCGGATC"
        "ACCTGAGGTCAGGAGTTCGAGACCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAA"
        "AAATACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCAGCTACTCGGGAGGC"
        "TGAGGCAGGAGAATCGCTTGAACCCGGGAGGCGGAGGTTGCAGTGAGCCGAGATCGCGCC";

    int seq_len = enhanced_strlen(full_sequence);
    int copy_len = (seq_len > MAX_SEQUENCE_LENGTH - 1) ? (MAX_SEQUENCE_LENGTH - 1) : seq_len;

    safe_strcpy(sequence_buffer, full_sequence, copy_len + 1);

    print_string("Processing user-provided FASTA sequence...\n");
    print_string("Original length: ");
    print_int(copy_len);
    print_string("\n");

    process_fasta_sequence(header, sequence_buffer, copy_len);

    print_string("\nFASTA processing completed.\n");
}

// Exported entrypoint for wdcli --invoke start
__attribute__((export_name("start")))
void start(void) {
    init_complement();
    reset_memory_pool();

    print_int(POOL_SIZE_IN_PAGES * PAGE_SIZE);  // simple numeric banner first
    print_string("\n");

    print_string("Enhanced FASTA revcomp started\n");
    print_string("Memory pool size: ");
    print_int(POOL_SIZE_IN_PAGES * PAGE_SIZE);
    print_string(" bytes\n");

    process_test_fasta();
}
