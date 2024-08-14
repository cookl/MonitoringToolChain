#ifndef THREAD_POOL_MANAGER2_H
#define THREAD_POOL_MANAGER2_H

#include <string>
#include <iostream>

#include "Tool.h"
#include <queue>
#include <mutex>
#include "DataModel.h"

struct Job{

void (*func)(void*);
void* args;

};

class JobQueue{

public:

~JobQueue(){
  m_lock.lock();
  while(m_jobs.size()){
    delete m_jobs.front();
    m_jobs.front()=0;
    m_jobs.pop();
  }
 m_lock.unlock();

}
  
void AddJob(Job* job){
  m_lock.lock();
  m_jobs.push(job);
  m_lock.unlock();}

Job* GetJob(){
  m_lock.lock();
  if(!m_jobs.size()){
    m_lock.unlock();
    return 0;
  } 
  Job* ret = m_jobs.front();
  m_jobs.front()=0;
  m_jobs.pop();
  m_lock.unlock();
  return ret;
};

unsigned int size(){
  m_lock.lock();
  unsigned int tmp= m_jobs.size();
  m_lock.unlock(); 
  return tmp; 
}

private:

  std::queue<Job*> m_jobs;
  std::mutex m_lock;
  
};

/**
 * \struct ThreadPoolManager2_args
 *
 * This is a struct to place data you want your thread to acess or exchange with it. The idea is the datainside is only used by the threa\
d and so will be thread safe
*
* $Author: B.Richards $
* $Date: 2019/05/28 10:44:00 $
*/

struct ThreadPoolManager2_args:Thread_args{

  ThreadPoolManager2_args();
  ~ThreadPoolManager2_args();
  bool busy;
 Job* job;
 JobQueue* job_queue;

};

/**
 * \class ThreadPoolManager2
 *
 * This is a template for a Tool that dynamically more or less threads, such that there is always 1 available thread.This can therefore be used to scale to your worklaod, however be carefull when using more than one of these tools and to apply upperlimits if necessary both locally within this tool and globally so that more threads than is practical are created causing massive inefficency. Please fill out the descripton and author information.
 *
 * $Author: B.Richards $
 * $Date: 2019/05/28 10:44:00 $
 */

class ThreadPoolManager2: public Tool {


 public:

  ThreadPoolManager2(); ///< Simple constructor
  bool Initialise(std::string configfile,DataModel &data); ///< Initialise Function for setting up Tool resorces. @param configfile The path and name of the dynamic configuration file to read in. @param data A reference to the transient data class used to pass information between Tools.
  bool Execute(); ///< Executre function used to perform Tool perpose. 
  bool Finalise(); ///< Finalise funciton used to clean up resorces.

  JobQueue job_queue;
  //std::queue<Job*> job_queue;
  //std::vector<Thread_Args*> threads;

 private:

  void CreateThread(); ///< Function to Create Thread
  void DeleteThread(unsigned int pos); ///< Function to delete thread @param pos is the position in the args vector below

  static void Thread(Thread_args* arg); ///< Function to be run by the thread in a loop. Make sure not to block in it
  Utilities* m_util; ///< Pointer to utilities class to help with threading
  std::vector<ThreadPoolManager2_args*> args; ///< Vector of thread args (also holds pointers to the threads)

  unsigned int m_freethreads; ///< Keeps track of free threads
  unsigned long m_threadnum; ///< Counter for unique naming of threads

};


#endif
