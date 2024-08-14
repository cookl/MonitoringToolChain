#include "ThreadPoolManager.h"

ThreadPoolManager_args::ThreadPoolManager_args() : Thread_args() {}

ThreadPoolManager_args::~ThreadPoolManager_args() {}

ThreadPoolManager::ThreadPoolManager() : Tool() {}

bool ThreadPoolManager::Initialise(std::string configfile, DataModel& data) {
  if (configfile != "") m_variables.Initialise(configfile);
  // m_variables.Print();

  m_data = &data;
  m_log = m_data->Log;

  if (!m_variables.Get("verbose", m_verbose)) m_verbose = 1;

  m_util = new Utilities();

  m_threadnum = 0;
  CreateThread();

  m_freethreads = 1;

  return true;
}

bool ThreadPoolManager::Execute() {
  if (job_queue.size() > 0) {
    for (unsigned int i = 0; i < args.size(); i++) {
      if (args.at(i)->busy == 0) {
        args.at(i)->job = job_queue.front();
        job_queue.pop();
        args.at(i)->busy = 1;
        break;
      }
    }
  }

  m_freethreads = 0;
  unsigned int lastfree = 0;
  for (unsigned int i = 0; i < args.size(); i++) {
    if (args.at(i)->busy == 0) {
      m_freethreads++;
      lastfree = i;
    }
  }

  if (m_freethreads < 1) CreateThread();
  if (m_freethreads > 1) DeleteThread(lastfree);

  (*m_log) << ML(3) << "free threads=" << m_freethreads << ":" << args.size() << std::endl;

  return true;
}

bool ThreadPoolManager::Finalise() {
  for (unsigned int i = 0; i < args.size(); i++)
    m_util->KillThread(args.at(i));

  args.clear();

  delete m_util;
  m_util = 0;

  return true;
}

void ThreadPoolManager::CreateThread() {
  ThreadPoolManager_args* tmparg = new ThreadPoolManager_args();
  tmparg->busy = 0;
  args.push_back(tmparg);
  std::stringstream tmp;
  tmp << "T" << m_threadnum;
  m_util->CreateThread(tmp.str(), &Thread, args.at(args.size() - 1));
  m_threadnum++;
}

void ThreadPoolManager::DeleteThread(unsigned int pos) {
  m_util->KillThread(args.at(pos));
  delete args.at(pos);
  args.at(pos) = 0;
  args.erase(args.begin() + (pos - 1));
}

void ThreadPoolManager::Thread(Thread_args* arg) {
  ThreadPoolManager_args* args = reinterpret_cast<ThreadPoolManager_args*>(arg);

  if (!args->busy)



    usleep(100);
  else {
    args->job->func(args->job->args);
    args->busy = 0;
  }
}
