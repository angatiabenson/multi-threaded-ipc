#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>
#include <cstring>

using namespace std;

const char *shm_name = "/my_shared_memory";
const size_t shm_size = 65536;
const char *sem_to_server = "/sem_to_server";
const char *sem_from_server = "/sem_from_server";

sem_t *to_server_sem;
sem_t *from_server_sem;

int shm_fd;
void *shared_memory;

void signal_handler(int sig)
{
    // Cleanup resources on SIGINT
    cout << "Server cleanup initiated." << endl;
    sem_close(to_server_sem);
    sem_close(from_server_sem);
    shm_unlink(shm_name);
    sem_unlink(sem_to_server);
    sem_unlink(sem_from_server);
    exit(0);
}

int main()
{
    signal(SIGINT, signal_handler); // Register signal handler

    cout << "SERVER STARTED" << endl;

    // Open semaphores
    to_server_sem = sem_open(sem_to_server, O_CREAT, 0666, 0);
    from_server_sem = sem_open(sem_from_server, O_CREAT, 0666, 0);

    while (true)
    {
        sem_wait(to_server_sem); // Wait for signal from client

        cout << endl
             << "CLIENT REQUEST RECEIVED" << endl;

        // Setup shared memory
        shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
        ftruncate(shm_fd, shm_size);
        shared_memory = mmap(0, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

        cout << "\tMEMORY OPEN" << endl;

        // Read file path and num_lines from shared memory
        char buffer[1024] = {0};
        strncpy(buffer, (char *)shared_memory, sizeof(buffer) - 1);
        string fileInfo(buffer);
        size_t separator = fileInfo.find('\n');
        if (separator == string::npos)
        {
            cerr << "Invalid data format in shared memory." << endl;
            continue;
        }

        string filePath = fileInfo.substr(0, separator);
        int num_lines = stoi(fileInfo.substr(separator + 1));

        cout << "\tOPENING: " << filePath << endl;

        ifstream file(filePath);
        if (!file.is_open())
        {
            cerr << "Failed to open file: " << filePath << endl;
            sem_post(from_server_sem); // Signal client about failure
            continue;
        }

        char *shm_ptr = static_cast<char *>(shared_memory);
        string line;
        int line_count = 0;

        while (getline(file, line) && line_count < num_lines)
        {
            // Write each line to shared memory, ensure not to exceed shm_size
            if ((shm_ptr - static_cast<char *>(shared_memory)) + line.length() + 1 < shm_size)
            {
                // cout<<line<<endl;
                strncpy(shm_ptr, line.c_str(), line.length());
                shm_ptr += line.length();
                *shm_ptr = '\n'; // Add newline character
                shm_ptr += 1;
                line_count++;
            }
            else
            {
                cerr << "Shared memory limit exceeded." << endl;
                break;
            }
        }

        clog << "\tFILE CLOSED." << endl;

        file.close();
        // Signal client that the data is ready
        sem_post(from_server_sem);
        clog << "\tMEMORY CLOSED." << endl;
    }

    return 0;
}
