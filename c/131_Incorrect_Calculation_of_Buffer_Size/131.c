/* CWE-131 */
volatile int sink;
void start(void) {
    int arr[8];
    int *p = arr;
    int wrong_size = sizeof(p);  /* sizeof pointer, not array */
    for (int i = 0; i < wrong_size; i++) {
        arr[i] = i;  /* may exceed arr[7] */
    }
    sink = arr[0];
}

