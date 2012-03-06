
#include <iostream>
#include <sstream>
#include <string>
#include <boost/asio.hpp>
#include "boost/bind.hpp"
#include <cstdlib>
#include "boost/bind.hpp"
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

#include "../redis4cpp/interface.h"
#include "../redis4cpp/dataaccess.h"

using namespace std;

boost::shared_mutex _print;


std::ostringstream results;

void PrintResult(redis4cpp::CommandBasePtr cmd)
{
    try
    {
        boost::lock_guard<boost::shared_mutex> guard(_print);
        std::cout << "//command: " << cmd->OutputBuffer() << "//result: " << cmd->Result() << std::endl;
        std::cout << "******************************************************************" << std::endl;
    }
    catch(std::exception& e)
    {
        std::cerr << "PrintResult Exception: " << e.what() << "\n";
    }
}

void ExecutionGetLoop(redis4cpp::DataAccess* db)
{
    try
    {
        int count = 0;
        while(count < 10) 
        {
            redis4cpp::CommandBase* get = new redis4cpp::CommandBase("EVAL", boost::bind(&PrintResult, _1));
            get->AddArgument("return redis.call('get', 'nmykey');");
            get->AddArgument(0);
            db->AsyncCommand(get);
        
            count++;
        }
        
    }
    catch(std::exception& e)
    {
        std::cerr << "ExecutionLoop Exception: " << e.what() << "\n";
    }
}

void ExecutionSetLoop(redis4cpp::DataAccess* db)
{
    try
    {
        int count = 0;
        while(count < 10) 
        {
            redis4cpp::CommandBase* set = new redis4cpp::CommandBase("SET", boost::bind(&PrintResult, _1));
            set->AddArgument("nmykey");
            set->AddArgument(count);
            db->AsyncCommand(set);
            
            count++;
        }
        
    }
    catch(std::exception& e)
    {
        std::cerr << "ExecutionLoop Exception: " << e.what() << "\n";
    }
}

int main(int argc, char** argv) {

    try
    {            
        boost::asio::io_service io_service;
        redis4cpp::DataAccess db(io_service);

        // Create a pool of threads to run all of the io_services.
        boost::thread_group threads;

        // Create threads for read/write async operations
        for (std::size_t i = 0; i < 2; ++i)
            threads.create_thread(boost::bind(&boost::asio::io_service::run, &io_service));

        boost::thread_group datathreads;
        datathreads.create_thread(boost::bind(&ExecutionSetLoop,  &db));
        datathreads.create_thread(boost::bind(&ExecutionGetLoop,  &db));
        datathreads.join_all();
        
        // wait for them
        threads.join_all();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

  return 0;
}

