#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_key_t key;
pthread_key_t key2;

struct test_struct {
    int i;
    float k;
}struct_data;

int temp;

int setspecificvar () { /* Set specific data for threads */
    
    struct test_struct *pst = malloc(sizeof(struct_data));
    int *pi = malloc(sizeof(int *));
    memset(pst, 0, sizeof(struct_data));
    memset(pi, 0, sizeof(int));

    pthread_setspecific (key, pst);
    pthread_setspecific (key2, pi);

    return 0;
}
int changedata (int i, float k, int tempvar) { /* Change specific data for threads */
    
    struct test_struct *pstruct_data = pthread_getspecific(key);
    int *ptemp = pthread_getspecific(key2);

    *ptemp = tempvar;
    pstruct_data->i = i;
    pstruct_data->k = k;

    return 0;
}

int printdata (int t) {  /* print specific data for threads */

    printf ("******************************************************\n");
    
    printf ("The addres in child%d returned from pthread_getspecific(key):0x%p\n",                           \
            t, (struct test_struct *)pthread_getspecific(key));

    printf ("The value of members in structure bound to \"key\" in  child%d:\nstruct_data.i:%d\nstruct_data.k: %f\n", \
            t, ((struct test_struct *)pthread_getspecific (key))->i,                            \
            ((struct test_struct *)pthread_getspecific(key))->k);

    printf ("------------------------------------------------------\n");

    printf ("The addres in child%d returned from pthread_getspecific(key2):0x%p\n",                          \
            t, (int *)pthread_getspecific(key2));
    printf ("The value of \"temp\" bound to \"key2\" in child%d:%d\n", \
            t, *((int *)pthread_getspecific(key2)));
    
    return 0;
}

void *child1 (void *arg)
{
    setspecificvar ();
    changedata(10, 3.141500, 110); /* Should not change the data in child2 */
    printdata(1);
}

void *child2 (void *arg)
{
    setspecificvar ();

    changedata(12, 2.141500, 120); /* Should not change the data in child1 */
    printdata(2);

    changedata (122, 22.141500, 1220); /* Should not change the data in child1 */
    printdata (2);
}

int create_key () {
    pthread_key_create (&key, NULL);
    pthread_key_create (&key2, NULL);
    return 0;
}

int delete_key () {

    pthread_key_delete (key);
    pthread_key_delete (key2);
    return 0;
}

int main (void)
{
    pthread_t tid1, tid2;

    create_key ();
    pthread_create (&tid1, NULL, (void *)child1, NULL);
    pthread_create (&tid2, NULL, (void *)child2, NULL);
    pthread_join (tid1, NULL);
    pthread_join (tid2, NULL);

    delete_key ();

    return 0;
}

