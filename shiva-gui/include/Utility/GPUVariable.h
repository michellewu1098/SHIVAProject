///-----------------------------------------------------------------------------------------------
/// \file GPUVariable.h
/// \brief Class to handle uniforms of shaders
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_UTILITY_GPUVARIABLE__
#define __SHIVA_UTILITY_GPUVARIABLE__

#include <GL/glew.h>
#include <GL/glu.h>
#include <iostream>
#include <string>

namespace Utility
{

	class GPUVariable
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Ctor
		/// \param [in] _program Shader program ID
		/// \param [in] _shader Fragment or vertex shader
		/// \param [in] _variableName This is the name in the shader of the uniform that is to be bound
		//----------------------------------------------------------------------------------
		GPUVariable( GLuint _programID, std::string _variableName );
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		~GPUVariable();
		//----------------------------------------------------------------------------------
		/// \brief Pass value of uniform variable int
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void UniformInt( int _value );
		//----------------------------------------------------------------------------------
		/// \brief Pass value of uniform variable float
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void UniformFloat( float _value );
		//----------------------------------------------------------------------------------
		/// \brief Pass value of uniform variable float3
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void UniformFloat3( float* _value );
		//----------------------------------------------------------------------------------
		/// \brief Pass value of uniform variable float4
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void UniformFloat4( float* _value );
		//----------------------------------------------------------------------------------
		/// \brief Pass value of uniform variable mat4
		/// \param [in] _value
		//----------------------------------------------------------------------------------
		void UniformMatrix4( float* _value );
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Uniform location
		//----------------------------------------------------------------------------------
		GLint m_loc;
		//----------------------------------------------------------------------------------

	};
}

#endif
