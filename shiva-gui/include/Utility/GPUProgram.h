///-----------------------------------------------------------------------------------------------
/// \file GPUProgram.h
/// \brief Class for shader program
/// \author Leigh McLoughlin
/// \version 1.0
///-----------------------------------------------------------------------------------------------

#ifndef __SHIVA_UTILITY_GPUPROGRAM__
#define __SHIVA_UTILITY_GPUPROGRAM__

#include <string>
#include "GPUShader.h"

namespace Utility
{
	class GPUProgram
	{
	public:

		//----------------------------------------------------------------------------------
		/// \brief Type of shader program
		//----------------------------------------------------------------------------------
		enum ProgramType
		{
			VERTEX,
			VERTEX_AND_FRAGMENT,
			FRAGMENT
		};
		//----------------------------------------------------------------------------------
		/// \brief Ctor
		//----------------------------------------------------------------------------------
		GPUProgram();
		//----------------------------------------------------------------------------------
		/// \brief Dtor
		//----------------------------------------------------------------------------------
		~GPUProgram();
		//----------------------------------------------------------------------------------
		/// \brief Create shader program
		/// \param [in] _programFileName
		/// \param [in] _type 
		//----------------------------------------------------------------------------------
		bool Create( const std::string _programFileName, ProgramType _type = FRAGMENT );
		//----------------------------------------------------------------------------------
		/// \brief Bind shader program
		//----------------------------------------------------------------------------------
		void Bind();
		//----------------------------------------------------------------------------------
		/// \brief Unbind shader program
		//----------------------------------------------------------------------------------
		void Unbind();
		//----------------------------------------------------------------------------------
		/// \brief Get fragment shader
		/// \return m_fs Fragment shader
		//----------------------------------------------------------------------------------
		GPUShader* GetFragShader() { return m_fs; }
		//----------------------------------------------------------------------------------
		/// \brief Get vertex shader
		/// \return m_vs Vertex shader
		//----------------------------------------------------------------------------------
		GPUShader* GetVertShader() { return m_vs; }
		//----------------------------------------------------------------------------------
		/// \brief Get shader program ID
		/// \return m_program
		//----------------------------------------------------------------------------------
		GLuint GetProgramID() const { return m_program; }
		//----------------------------------------------------------------------------------
		bool ValidateProgram( GLuint _program, GLuint _vs, GLuint _fs );
		//----------------------------------------------------------------------------------
		bool LinkProgram();
		//----------------------------------------------------------------------------------

	protected:

		//----------------------------------------------------------------------------------
		/// \brief Shader program id
		//----------------------------------------------------------------------------------
		GLuint m_program;
		//----------------------------------------------------------------------------------
		/// \brief Vertex shader
		//----------------------------------------------------------------------------------
		GPUShader *m_vs;
		//----------------------------------------------------------------------------------
		/// \brief Fragment shader        
		//----------------------------------------------------------------------------------	
		GPUShader *m_fs;
		//----------------------------------------------------------------------------------	
	};
}

#endif