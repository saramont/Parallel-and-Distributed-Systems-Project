ifndef FF_ROOT 
FF_ROOT		= ../fastflow/
endif

serial: bfs_par2.cpp utimer.cpp ntimer.cpp Graph.cpp delay.cpp
	g++ -std=c++17  -O3 bfs_par2.cpp -o bfs_SEQ.out -DSEQ=1

cpp_threads: bfs_par2.cpp utimer.cpp ntimer.cpp Graph.cpp delay.cpp
	g++ -std=c++17  -O3 bfs_par2.cpp -o bfs_PAR.out -pthread

fastflow: bfs_par_ff2.cpp utimer.cpp ntimer.cpp Graph.cpp delay.cpp
	g++ -std=c++17  -O3 bfs_par_ff2.cpp -o bfs_FF.out -I $(FF_ROOT) -pthread

all: serial cpp_threads fastflow

clean:
	rm -f bfs_*.out

