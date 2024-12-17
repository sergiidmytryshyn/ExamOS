//
// Created by serhii on 17.12.24.
//

#ifndef EXAMOS_MPI_MNG_HPP
#define EXAMOS_MPI_MNG_HPP


#include <iostream>
#include <vector>
#include <thread>
#include <map>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>

namespace bip = boost::interprocess;

class MPIManager {

private:
    int rank;
    int mode;
    int procs;
    std::string comm; // shared memory name or network address
    size_t shm_size = 4096;
    bip::managed_shared_memory* shm;
    bip::interprocess_semaphore* sem;
    std::vector<bip::interprocess_semaphore*> sem_vector;
    void create_shm();
    void delete_shm();

public:
    MPIManager(int argc, char* argv[]);
    ~MPIManager();

    void parse_conf(char* argv[]);

    int get_rank();
    int get_procs();
    int get_mode();

    template<typename T>
    void send(int dest, T& value) {
        std::string dest_name = "data_" + std::to_string(rank);
        T* addr = shm->construct<T>(dest_name.c_str())(value);
        std::memcpy(addr, &value, sizeof(T));
        sem_vector[rank]->post();
    };

    template<typename T>
    void receive(int src, T& value) {
        sem_vector[src]->wait();
        std::string src_name = "data_" + std::to_string(src);
        T* addr = shm->find<T>(src_name.c_str()).first;
        std::memcpy(&value, addr, sizeof(T));
    };


    template<typename T>
    void send_arr(int dest, T* value, size_t sz) {
        std::string dest_name = "data_" + std::to_string(rank);
        auto addr = shm->construct<T>(dest_name.c_str())[sz]();
        std::memcpy(addr, value, sizeof(T) * sz);
        sem_vector[rank]->post();
    };

    template<typename T>
    void receive_arr(int src, T* value, size_t sz) {
        sem_vector[src]->wait();
        std::string src_name = "data_" + std::to_string(src);
        T* addr = shm->find<T>(src_name.c_str()).first;
        std::memcpy(value, addr, sizeof(T) * sz);
    };

    void wait_barrier();

    template<typename T>
    void sync_send(int dest, T& value) {
        std::string dest_name = "data_" + std::to_string(dest);
        T* addr = shm->construct<T>(dest_name.c_str())(value);
        std::memcpy(addr, &value, sizeof(T));
//        sem_vector[dest]->post();
        sem_vector[rank]->wait();
    }

    template<typename T>
    void sync_receive(int src, T& value) {
        sem_vector[src]->wait();
        std::string src_name = "data_" + std::to_string(src);
        T* addr = shm->find<T>(src_name.c_str()).first;
        if (addr) {
            std::memcpy(&value, addr, sizeof(T));
            shm->destroy<T>(src_name.c_str());
        } else {
            std::cerr << "Error: Data not found in shared memory for source " << src << std::endl;
        }

        sem_vector[src]->post();
    }

};




#endif //EXAMOS_MPI_MNG_HPP
