procs=$1
cfg=$2

for ((i = 0; i < procs; i++)); do
    ./bin/exam "$i" "$cfg" &
done
