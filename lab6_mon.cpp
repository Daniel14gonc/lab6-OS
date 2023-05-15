/*
* Autor: Daniel Gonzalez
* Carne: 20293
* Descripcion: Uso de monitores con mutex y variables de condicion.
*/

#include <pthread.h>
#include <iostream>
#include <fstream>
#include <semaphore.h>
#include <cstdlib>
#include <unistd.h>

#define CANT_ID 100
#define NUM_ITERS 10
#define NUM_THREADS 10

using namespace std;

ofstream output_file;

int available_resources = CANT_ID;

sem_t write_to_file_sem;

pthread_cond_t condition;
pthread_mutex_t mutex;

// Funcion que permite escribir logs al archivo.

void write_to_file(string log) {
    sem_wait(&write_to_file_sem);
    output_file.open("logs_mon.txt", ios::app | ios::out);
    if (output_file.is_open()) {
        output_file << log << endl;
        output_file.close();
    }
    sem_post(&write_to_file_sem);
}

int decrease_count(int count) {
    string log;
    pthread_mutex_lock(&mutex);
    if (available_resources < count) {
        log = "Se intentaron obtener " + to_string(count) + " recursos pero solo hay " + to_string(available_resources);
        write_to_file(log);
    }
    while (available_resources < count) {
        pthread_cond_wait(&condition, &mutex);
    }

    available_resources -= count;
    pthread_mutex_unlock(&mutex);
    return 0;
}

int increase_count(int count) {
    pthread_mutex_lock(&mutex);
    available_resources += count;
    pthread_cond_broadcast(&condition);
    pthread_mutex_unlock(&mutex);
    return 0;
}

void* consume(void* arg) {
    int id = *((int *) arg);
    free(arg);
    string log;
    // Ejecucion de obtencion de recursos.
    for (int i = 0; i < NUM_ITERS; i++) {
        log = "h" + to_string(id) + ": iniciando iteracion " + to_string(i);
        write_to_file(log);
        int cant_resource = rand() % CANT_ID;
        log = "h" + to_string(id) + ": Se intentaran obtener " + to_string(cant_resource) + " recursos.";
        write_to_file(log);
        decrease_count(cant_resource);
        log = "h" + to_string(id) + ": Se lograron obtener los recursos.";
        write_to_file(log);
        float sleep_time = (rand() % 10) * 0.1;
        sleep(sleep_time);
        increase_count(cant_resource);
        log = "h" + to_string(id) + ": Recursos usados, finaliza iteracion.";
        write_to_file(log);
    }
    pthread_exit(NULL);
}


int main() {
    sem_init(&write_to_file_sem, 0, 1);
    condition = PTHREAD_COND_INITIALIZER;
    mutex = PTHREAD_MUTEX_INITIALIZER;
    output_file.open("logs_mon.txt", ios::out);
    output_file << "";
    output_file.close();
    pthread_t threads[NUM_THREADS];

    // Creacion de threads
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
    sem_destroy(&write_to_file_sem);
}