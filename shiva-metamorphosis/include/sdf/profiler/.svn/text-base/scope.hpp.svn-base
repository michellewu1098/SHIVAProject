#ifndef SDF_PROFILER_SCOPE_INCLUDED
#define SDF_PROFILER_SCOPE_INCLUDED


#include <cstdarg>
#include <iostream>
#include <vector>
#include <string>
#include <boost/timer.hpp>

namespace sdf
{
	namespace profiler
	{
		//----------------------------------------------------------------------------------------------------------------------
		/// @class scope "include/sdf/profiler/scope.hpp"
		/// @brief Profiles a scope (based on boost::timer)
		/// @author Mathieu Sanchez
		/// @version 1.0
		/// @date Last Revision 28/06/11 Initial revision
		//----------------------------------------------------------------------------------------------------------------------
		class scope
		{
		public :
			scope();
			scope(const std::string& i_name);
			scope(const char* i_name, ...);
			~scope();

			void restart();
			float query() const;
			void print_time() const;
		private :
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief  A name to identify and allow nested profilers
			//----------------------------------------------------------------------------------------------------------------------
			std::string m_name;
			//----------------------------------------------------------------------------------------------------------------------
			/// @brief  Start time for profiling
			//----------------------------------------------------------------------------------------------------------------------
			boost::timer m_start;
		};
	}
}

// DO NOT USE SPACES IN name !
#define SDF_PROFILE(name) sdf::profiler::scope sdf_profiler_macro_generated_name_##name(#name);

#endif /* SDF_PROFILER_SCOPE_INCLUDED */
