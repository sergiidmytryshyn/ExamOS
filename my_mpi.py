import sys
import subprocess

def create_config_file(args):
    with open(f"data/{args[1]}", 'w') as cfile:
        cfile.write(f"{args[2]}\n")
        cfile.write(f"{args[3]}\n")
        if args[2] == "0":
            cfile.write(args[4])
        elif args[2] == "1":
            for i, arg in enumerate(args):
                if i > 4:
                    cfile.write(f"{arg}\n")
        else:
            print("Wrong usage")
            return False
    return True


if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Usage: python3 script.py <config_file> <mode> <num_processes> <shm_path or adresses>")
        sys.exit(1)
    if not create_config_file(sys.argv):
        sys.exit(1)
    res1 = subprocess.run(['bash', "./compile.sh"], check=True)
    res2 = subprocess.run(['bash', "./run_procs.sh", sys.argv[3], f"./data/{sys.argv[1]}"], check=True)


"""
python3 my_mpi.py 1 test0.cfg 0 5 /sharmemseg123
python3 my_mpi.py 1 test1.cfg 1 3 127.0.0.1 127.0.0.1 127.0.0.1
"""
