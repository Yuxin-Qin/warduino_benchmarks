/* 685_1.c – CWE-685: Function Call With Incorrect Number of Arguments
 *
 * Host (WARDuino primitive) expects: warduino_sum2(i32, i32) -> i32
 * Here we *lie* to the compiler and declare it as taking only one argument.
 * The Wasm import type becomes (i32) -> i32, so the host and guest signatures
 * are inconsistent -> CWE-685.
 */

extern int  warduino_sum2(int a);   /* WRONG prototype: 1 param instead of 2 */

/* WARDuino entry point */
void start(void) {
    /* This compiles as a 1-argument call at the Wasm level. The host
       primitive still assumes 2 parameters and 1 result. */
    int r = warduino_sum2(10);
}
