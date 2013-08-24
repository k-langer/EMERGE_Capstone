#include "boost/date_time/posix_time/posix_time.hpp"
namespace pt = boost::posix_time;
 
int main()
{
             
    pt::ptime current_date_microseconds = pt::microsec_clock::local_time();
    std::cout << current_date_microseconds << std::endl;
}
