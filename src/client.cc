#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <cstring>
#include <cstdlib>

#include "../include/calculator.h"

using namespace std;

// Define shared memory size and name
const char *shm_name = "/my_shared_memory";
const size_t shm_size = 65536;

// Semaphore names
const char *sem_to_server = "/sem_to_server";
const char *sem_from_server = "/sem_from_server";

// Thread function prototype
void *thread_function(void *arg);

// Structure to pass data to threads
struct ThreadData
{
    int *sum_array;
    char *shared_mem_ptr;
    int start_line;
    int end_line;
    int thread_id;
};

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <file_path> <num_lines>\n";
        return 1;
    }

    const char *file_path = argv[1];
    int num_lines = std::stoi(argv[2]);

    // Open shared memory
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, shm_size);
    void *shared_memory = mmap(0, shm_size, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (shared_memory == MAP_FAILED)
    {
        std::cerr << "Shared memory mapping failed\n";
        return 1;
    }

    // Initialize shared memory with file path and num_lines
    memset(shared_memory, 0, shm_size);
    sprintf(static_cast<char *>(shared_memory), "%s\n%d", file_path, num_lines);
    std::cout << "SHARED MEMORY ALLOCATED: " << shm_size << " BYTES\n";

    // Open semaphores
    sem_t *to_server_sem = sem_open(sem_to_server, O_CREAT, 0666, 0);
    sem_t *from_server_sem = sem_open(sem_from_server, O_CREAT, 0666, 0);

    // Signal server that the data is ready
    sem_post(to_server_sem);
    // Wait for server to process the file
    sem_wait(from_server_sem);

    // Create thread attributes
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    // Calculate lines per thread
    int lines_per_thread = num_lines / 4;
    pthread_t threads[4];
    ThreadData data[4];
    int sum_array[4] = {0};

    for (int i = 0; i < 4; ++i)
    {
        data[i].sum_array = sum_array;
        data[i].shared_mem_ptr = static_cast<char *>(shared_memory);
        data[i].start_line = i * lines_per_thread;
        data[i].end_line = (i + 1) * lines_per_thread - 1;
        data[i].thread_id = i;
        pthread_create(&threads[i], &attr, thread_function, &data[i]);
    }

    std::cout << "THREADS CREATED\n";

    // Wait for threads to finish
    for (int i = 0; i < 4; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    int total = 0;
    for (int sum : sum_array)
    {
        total += sum;
    }
    cout << "\tSUM: " << total << endl;

    return 0;
}

void *thread_function(void *arg)
{
    ThreadData *data = (ThreadData *)arg;
    std::istringstream stream(data->shared_mem_ptr);
    std::string line;
    double total_sum = 0;
    int line_count = 0;

    //  Skip to the start line
    for (int i = 0; i < data->start_line && std::getline(stream, line); ++i)
    {
    }

    // Process each line within the thread's range
    while (std::getline(stream, line) && line_count < data->end_line - data->start_line + 1)
    {
        double line_sum = calculator::calculate_from_string(line);
        total_sum += line_sum;
        line_count++;
    }

    data->sum_array[data->thread_id]=total_sum;

    // Output the thread's results
    std::cout << "\tTHREAD " << data->thread_id << ": " << line_count << " LINES, " << total_sum << std::endl;

    return nullptr;
}
