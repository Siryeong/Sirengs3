/*TODO*/
#include <pthread.h>
#include <sys/types.h>


typedef struct {
	//TODO
	pthread_mutex_t mu;
	pthread_cond_t a;
	int num;
} mysem_t ;


void
mysem_init (mysem_t * sem, int init)
{
	//TODO
	pthread_mutex_init(&sem->mu, 0x0);
	pthread_cond_init(&sem->a, 0x0);
	sem->num = init;
}

void
mysem_post (mysem_t * sem)
{
	//TODO
	sem->num++;
	pthread_cond_signal(&sem->a);
}

void
mysem_wait (mysem_t * sem)
{
	//TODO
	pthread_mutex_lock(&sem->mu);
	sem->num--;
	if(sem->num <= 0){
		while(sem->num <=0)
			pthread_cond_wait(&sem->a, &sem->mu);
	pthread_mutex_unlock(&sem->mu);
}
