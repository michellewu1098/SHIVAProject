#include <sdf/core/config.hpp>
#ifndef SDF_NO_BOOST_THREAD
#include <sdf/core/thread_work.hpp>


//----------------------------------------------------------------------------------------------------------------------
sdf::thread::work_queue::work_queue(unsigned int i_num_tasks) : m_current_work_id(0), m_max_work_id(i_num_tasks)
{
}

//----------------------------------------------------------------------------------------------------------------------
bool sdf::thread::work_queue::next_work_id(unsigned int* o_id)
{
	
	{
		boost::mutex::scoped_lock lock(m_work_mutex);

		if (m_current_work_id>=m_max_work_id)
			return false;

		*o_id = m_current_work_id++;
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
sdf::thread::work_split::work_split(
							unsigned int i_num_tasks,
							unsigned int i_num_threads)
							: m_num_work_threads(i_num_threads), m_num_tasks(i_num_tasks)
{
	assert(i_num_tasks>=i_num_threads);
	assert(i_num_threads>0);
	
	m_num_tasks_per_thread = m_num_tasks/m_num_work_threads;
	m_num_extra_tasks = m_num_tasks%m_num_work_threads;
}

#endif /* SDF_NO_BOOST_THREAD */
