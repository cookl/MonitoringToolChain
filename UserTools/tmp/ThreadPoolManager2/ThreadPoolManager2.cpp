#include "ThreadPoolManager2.h"

ThreadPoolManager2_args::ThreadPoolManager2_args() : Thread_args() {}

ThreadPoolManager2_args::~ThreadPoolManager2_args() {}

ThreadPoolManager2::ThreadPoolManager2() : Tool() {}

bool ThreadPoolManager2::Initialise(std::string configfile, DataModel& data) {
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

bool ThreadPoolManager2::Execute() {
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

bool ThreadPoolManager2::Finalise() {
  for (unsigned int i = 0; i < args.size(); i++)
    m_util->KillThread(args.at(i));

  args.clear();

  delete m_util;
  m_util = 0;

  return true;
}

void ThreadPoolManager2::CreateThread() {
  ThreadPoolManager2_args* tmparg = new ThreadPoolManager2_args();
  tmparg->busy = 0;
  tmparg->job=0;
  tmparg->job_queue=&job_queue;
  args.push_back(tmparg);
  std::stringstream tmp;
  tmp << "T" << m_threadnum;
  m_util->CreateThread(tmp.str(), &Thread, args.at(args.size() - 1));
  m_threadnum++;
}

void ThreadPoolManager2::DeleteThread(unsigned int pos) {
  m_util->KillThread(args.at(pos));
  delete args.at(pos);
  args.at(pos) = 0;
  args.erase(args.begin() + (pos - 1));
}

void ThreadPoolManager2::Thread(Thread_args* arg) {
  ThreadPoolManager2_args* args = reinterpret_cast<ThreadPoolManager2_args*>(arg);

  if (!args->job_queue->size()) usleep(100);
  else {
    args->job=args->job_queue->GetJob();
    args->busy = true;
    args->job->func(args->job->args); 
    args->busy = false;
  }
}
