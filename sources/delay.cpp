#include <iostream> 
#include <thread>
#include <chrono>
#include <fstream>

#define ACTIVEWAIT

void delay(std::chrono::microseconds m) {
#ifdef ACTIVEWAIT
  auto active_wait = [] (std::chrono::microseconds ms) {
		       long msecs = ms.count();
		       auto start = std::chrono::high_resolution_clock::now();
		       auto end   = false;
		       while(!end) {
			 auto elapsed = std::chrono::high_resolution_clock::now() - start;
			 auto msec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
			 if(msec>msecs)
			   end = true;
		       }
		       return;
		     };
  active_wait(m);
#else
  std::this_thread::sleep_for(m);
#endif
  return;
}



