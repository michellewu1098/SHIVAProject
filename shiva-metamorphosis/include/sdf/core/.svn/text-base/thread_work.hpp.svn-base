#ifndef SDF_CORE_THREAD_WORK_INCLUDED
#define SDF_CORE_THREAD_WORK_INCLUDED


#include <sdf/core/config.hpp>


#ifndef SDF_NO_BOOST_THREAD

#include <boost/thread.hpp>

namespace sdf
{
	namespace thread
	{
		//----------------------------------------------------------------------------------------------------------------------
		/// @class work_queue "include/sdf/core/thread_work.hpp"
		/// @brief The work queue can be used to simplify the work when many similar tasks are to be done
		///			Each task is grabbed when one is done. If the tasks are too small, the process will not be efficient: the mutex lock is an expensive call
		/// @author Mathieu Sanchez
		/// @version 1.0
		/// @date Last Revision 28/06/11 Initial revision
		//----------------------------------------------------------------------------------------------------------------------
		class work_queue
		{
		public :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Constructor - takes in the number of tasks to do (should be a high number)
			/// @param[in] i_num_tasks Number of tasks to process
			//----------------------------------------------------------------------------------------------------------------------
			work_queue(unsigned int i_num_tasks);
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Get the next task id
			/// @param[out] o_id The task id
			/// @return True if there is a new task, False otherwise
			//----------------------------------------------------------------------------------------------------------------------
			bool next_work_id(unsigned int* o_id);
		private :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Internal - Mutex to lock when updating the current task id
			//----------------------------------------------------------------------------------------------------------------------
			boost::mutex m_work_mutex;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Read an obj file from a filename
			//----------------------------------------------------------------------------------------------------------------------
			unsigned int m_current_work_id;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief The max work id - the number of tasks to process
			//----------------------------------------------------------------------------------------------------------------------
			unsigned int m_max_work_id;
		};

		//----------------------------------------------------------------------------------------------------------------------
		/// @class work_split "include/sdf/core/thread_work.hpp"
		/// @brief The work split will split the work equally amongst the threads, and each thread will do its part.
		///			It s good when the tasks are not long enough to use work_queue.
		/// @author Mathieu Sanchez
		/// @version 1.0
		/// @date Last Revision 28/06/11 Initial revision
		//----------------------------------------------------------------------------------------------------------------------
		class work_split
		{
		public :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Constructor
			/// @param[in] i_num_tasks The number of tasks
			/// @param[in] i_num_threads The number of threads to use - by default it will use the hardware concurrency number.
			//----------------------------------------------------------------------------------------------------------------------
			work_split(unsigned int i_num_tasks, unsigned int i_num_threads = boost::thread::hardware_concurrency());

			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Get the number of threads working
			/// @return Number of threads
			//----------------------------------------------------------------------------------------------------------------------
			unsigned int num_threads() const { return m_num_work_threads; }

			//----------------------------------------------------------------------------------------------------------------------
			/// @brief The initial id of the first task
			/// @return The first task id to start for the thread
			//----------------------------------------------------------------------------------------------------------------------
			unsigned int start_id() const { return 0; }
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief End marker id for the first thread
			/// @return End marker
			//----------------------------------------------------------------------------------------------------------------------
			unsigned int end_id() const { return m_num_tasks_per_thread+m_num_extra_tasks; }
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief The number of tasks a thread *should* be doing (first thread often gets more)
			/// @return The number of tasks a thread has to process
			//----------------------------------------------------------------------------------------------------------------------
			unsigned int num_tasks_per_thread() const { return m_num_tasks_per_thread; }

			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Wait for the threads to finish
			//----------------------------------------------------------------------------------------------------------------------
			void join_all();

		private :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Number of threads working (does not count the managing thread)
			//----------------------------------------------------------------------------------------------------------------------
			unsigned int m_num_work_threads;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Number of tasks to process
			//----------------------------------------------------------------------------------------------------------------------
			unsigned int m_num_tasks;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Number of tasks one thread should process
			//----------------------------------------------------------------------------------------------------------------------
			unsigned int m_num_tasks_per_thread;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief Number of extra tasks the first thread will have to eat
			//----------------------------------------------------------------------------------------------------------------------
			unsigned int m_num_extra_tasks;
		};
	}
}

#endif /* SDF_NO_BOOST_THREAD */
#endif /* SDF_CORE_THREAD_WORK_INCLUDED */
