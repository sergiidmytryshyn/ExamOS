//
// Created by serhii on 17.12.24.
//

#include "mpi_mng.hpp"


MPIManager::MPIManager(int argc, char** argv) {
    rank = std::stoi(argv[1]);
    parse_conf(argv);
    create_shm();
}

MPIManager::~MPIManager() {
    if (mode == 0) {
        if (shm) {
            shm->destroy<bip::interprocess_semaphore>("my_sem");
            for (int i = 0; i < procs; ++i) {
                std::string sem_name = "sem_" + std::to_string(i);
                shm->destroy<bip::interprocess_semaphore>(sem_name.c_str());
            }
            bip::shared_memory_object::remove(comm.c_str());
        }
    }
}

void MPIManager::parse_conf(char* argv[]) {
    std::ifstream file(argv[2]);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open config" << std::endl;
        return;
    }

    if (!(file >> mode)) {
        std::cerr << "Error: Failed to read mode from config file." << std::endl;
        return;
    }

    if (!(file >> procs)) {
        std::cerr << "Error: Failed to read number of procs." << std::endl;
        return;
    }

    if (mode == 0) {
        if (!(file >> comm)) {
            std::cerr << "Error: Failed to read comm." << std::endl;
            return;
        }
    }

    if (mode == 1) {
        for (int i = 0; i <= rank; ++i) {
            if (!(file >> comm)) {
                std::cerr << "Error: Failed to read IP address " << i+1 << " from config file." << std::endl;
                return;
            }
        }
    }

    file.close();
}


int MPIManager::get_rank() {
    return rank;
}

int MPIManager::get_procs() {
    return procs;
}

int MPIManager::get_mode() {
    return mode;
}


void MPIManager::create_shm() {
    shm = new bip::managed_shared_memory(bip::open_or_create, comm.c_str(), shm_size);
    sem = shm->find_or_construct<bip::interprocess_semaphore>("my_sem")(0);
    sem_vector.resize(procs);
    for (int i = 0; i < procs; ++i) {
        std::string sem_name = "sem_" + std::to_string(i);
        sem_vector[i] = shm->find_or_construct<bip::interprocess_semaphore>(sem_name.c_str())(0);
    }
}

void MPIManager::wait_barrier() {
    int* i = shm->find_or_construct<int>("counter")(0);
    (*i)++;
    if (*i == procs) {
        *i = 0;
        sem->post();
    } else {
        sem->wait();
    }
}




