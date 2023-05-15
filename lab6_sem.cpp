/*
* Autor: Daniel Gonzalez
* Carne: 20293
* Descripcion: Uso de semaforos para sincronziacion.
*/

#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <semaphore.h>
#include <iostream>
#include <fstream>

#define CANT_ID 100
#define NUM_ITERS 10
#define NUM_THREADS 10

using namespace std;

sem_t semaphore, write_to_file_sem;
ofstream output_file;

void write_to_file(string log) {
    sem_wait(&write_to_file_sem);
    output_file.open("logs_sem.txt", ios::app | ios::out);
    if (output_file.is_open()) {
        output_file << log << endl;
        output_file.close();
    }
    sem_post(&write_to_file_sem);
}

void* consume(void* arg) {
    int id = *((int *) arg);
    free(arg);
    string log;
    for (int i = 0; i < NUM_ITERS; i++) {
        sem_wait(&semaphore);
        log = "h" + to_string(id) + ": iniciando iteracion " + to_string(i);
        write_to_file(log);
        log = "h" + to_string(id) + ": Semaforo abierto con exito. Se obtuvo recurso.";
        write_to_file(log);
        float sleep_time = (rand() % 10) * 0.1;
        sleep(sleep_time);
        log = "h" + to_string(id) + ": Recurso usado, finaliza iteracion.";
        write_to_file(log);
        sem_post(&semaphore);
    }
    pthread_exit(NULL);
}

/*
* En este caso no uso directamente una variable que representaba la cantidad
* de recursos disponibles. En cambio use semaforos contadores, cuyo contador es
* la cantidad de recursos disponibles.
*/

int main() {
    sem_init(&semaphore, 0, CANT_ID);
    sem_init(&write_to_file_sem, 0, 1);
    output_file.open("logs_sem.txt", ios::out);
    output_file << "";
    output_file.close();
    pthread_t threads[NUM_THREADS];
    // Creacion de threads.
    for (int i = 0; i < NUM_THREADS; i++) {
        string log = "Inicializando thread " + to_string(i);
        write_to_file(log);
        int* index = (int *) malloc(sizeof(int));
        *index = i;
        pthread_create(&threads[i], NULL, &consume, (void *) index);
    }
    for(int i = 0; i < NUM_THREADS;i++) {
        pthread_join(threads[i], NULL);
    }
    sem_destroy(&semaphore);
    sem_destroy(&write_to_file_sem);
}