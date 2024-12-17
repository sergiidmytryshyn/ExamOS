// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com


#include "mpi_mng.hpp"

int main(int argc, char* argv[]) {

    MPIManager mng(argc, argv);
    int mod = mng.get_mode();
    int prcs = mng.get_procs();
    int rnk = mng.get_rank();

    int x = 123;
    int y;
    if (rnk == 0) {
        mng.send(rnk + 1, x);
        std::cout << "Process " << rnk << " sent " << x << "\n";
        mng.receive(prcs - 1, y);
        std::cout << "Process " << rnk << " recieved " << y << "\n";
    }
    else if (rnk < prcs - 1) {
        mng.receive(rnk - 1, y);
        std::cout << "Process " << rnk << " recieved " << y << "\n";
        mng.send(rnk + 1, y);
        std::cout << "Process " << rnk << " sent " << x << "\n";
    }
    else {
        mng.receive(rnk - 1, y);
        std::cout << "Process " << rnk << " recieved " << y << "\n";
        mng.send(0, y);
        std::cout << "Process " << rnk << " sent " << x << "\n";
    }

    mng.wait_barrier();
    return 0;
}
