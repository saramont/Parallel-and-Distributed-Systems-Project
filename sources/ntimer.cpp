#include <iostream>
#include <chrono>

#define START(timename) auto timename = std::chrono::system_clock::now();
#define STOPNANO(timename,elapsed) auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - timename).count();


class ntimer {
    std::chrono::system_clock::time_point start;
    std::chrono::system_clock::time_point stop;
    std::string message; 
    using usecs = std::chrono::microseconds;
    using msecs = std::chrono::milliseconds;
    using nsecs = std::chrono::nanoseconds;
private:
    long * us_elapsed;
 
public:
    ntimer(const std::string m) : message(m),us_elapsed((long *)NULL) {
    start = std::chrono::system_clock::now();
 }
 
 ntimer(const std::string m, long * us) : message(m),us_elapsed(us) {
    start = std::chrono::system_clock::now();
 }
 ~ntimer() {
    stop = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = stop - start;
    auto musec = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
    
    std::cout << message << "TIME " << musec << " nsec " << std::endl;
    if(us_elapsed != NULL)
        (*us_elapsed) = musec;
}
};