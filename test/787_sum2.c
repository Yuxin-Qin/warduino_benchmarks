/* 685_1.c – CWE-685: Function Call With Incorrect Number of Arguments
 *
 * Host (WARDuino primitive) expects: warduino_sum2(i32, i32) -> i32
 * Here we *lie* to the compiler and declare it as taking only one argument.
 * The Wasm import type becomes (i32) -> i32, so the host and guest signatures
 * are inconsistent -> CWE-685.
 */

extern int  warduino_sum2(int a);   /* WRONG prototype: 1 param instead of 2 */
extern void print_int(int);         /* WARDuino primitive */

/* WARDuino entry point */
void start(void) {
    /* This compiles as a 1-argument call at the Wasm level. The host
       primitive still assumes 2 parameters and 1 result. */
    int r = warduino_sum2(10);

    /* Whatever garbage / UB result we get, we print it. On a “lucky” run
       you might see nondeterministic numbers or a crash. */
    print_int(r);
}
