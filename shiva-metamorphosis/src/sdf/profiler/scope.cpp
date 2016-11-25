#include <sdf/profiler/scope.hpp>
#include <math.h>
#include <cstdio>

//----------------------------------------------------------------------------------------------------------------------
sdf::profiler::scope::scope() : m_name("unnamed"), m_start()
{
	std::cout<<"+profiling "<<m_name<<": start"<<std::endl;
}

//----------------------------------------------------------------------------------------------------------------------
sdf::profiler::scope::scope(const std::string& i_name)  : m_name(i_name), m_start()
{
	std::cout<<"+profiling "<<m_name<<": start"<<std::endl;
}

//----------------------------------------------------------------------------------------------------------------------
sdf::profiler::scope::scope(const char* i_name, ...)  : m_name(i_name), m_start()
{
	static const unsigned int MaxTempStringSize = 256;
	char tmp[MaxTempStringSize];
	va_list args;
	va_start(args, i_name);
#ifdef _MSC_VER
	vsprintf_s(tmp,MaxTempStringSize,i_name,args);
#else
        vsprintf(tmp,i_name,args);
#endif
	m_name = tmp;
	va_end(args);
	std::cout<<"+profiling "<<m_name<<": start"<<std::endl;
}

//----------------------------------------------------------------------------------------------------------------------
sdf::profiler::scope::~scope()
{
	print_time();
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::profiler::scope::restart()
{
	m_start.restart();
}

//----------------------------------------------------------------------------------------------------------------------
float sdf::profiler::scope::query() const
{
	const float timeinsec = static_cast<float>(m_start.elapsed());
	return timeinsec;
}

//----------------------------------------------------------------------------------------------------------------------
void sdf::profiler::scope::print_time() const
{
	const float time = query();
	if (time>=60.f)
	{
		const unsigned int mins = static_cast<unsigned int>(time/60.f);
		const float restsec = (float)fmod(time,60.f);
		
		std::cout<<"-profiling "<<m_name<<": "<<mins<<"min"<<restsec<<"s"<<std::endl;
	}
	else
	{
		std::cout<<"-profiling "<<m_name<<": "<<time<<"s"<<std::endl;
	}
}
