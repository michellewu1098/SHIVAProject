#include "CommandManager.h"

//----------------------------------------------------------------------------------

// Initialise statics

Totem::CommandManager* Totem::CommandManager::m_instance = NULL;

//----------------------------------------------------------------------------------

Totem::CommandManager* Totem::CommandManager::Init()
{
	m_instance = new Totem::CommandManager();
	return m_instance;
}

//----------------------------------------------------------------------------------

void Totem::CommandManager::UnInit()
{
	delete m_instance;
}

//----------------------------------------------------------------------------------

Totem::CommandManager::CommandManager()
{
	m_totemController = Totem::Controller::GetInstance();
	//m_currentCmd = NULL;
}

//----------------------------------------------------------------------------------

Totem::CommandManager::~CommandManager()
{
	Clear();
}

//----------------------------------------------------------------------------------

void Totem::CommandManager::Clear()
{
	while( !m_undoStack.empty() )
	{
		//delete m_undoStack.top();		
		m_undoStack.pop();
	}                             

	while( !m_redoStack.empty() )
	{
		//delete m_redoStack.top();
		m_redoStack.pop();
	}
}

//----------------------------------------------------------------------------------

void Totem::CommandManager::Execute( Command* _currentCmd )
{
	_currentCmd->Execute( m_totemController );
	m_undoStack.push( _currentCmd );

	// Clear redo stack
	while( !m_redoStack.empty() )
	{
		delete m_redoStack.top();
		m_redoStack.pop();
	}
}

//----------------------------------------------------------------------------------

void Totem::CommandManager::Undo()
{
	if( !m_undoStack.empty() )
	{
		// Put current command in the redo stack 
		//m_redoStack.push( m_currentCmd );
		
		Command* cmd = ( Command* )m_undoStack.top();
		m_redoStack.push( cmd );
		m_undoStack.pop();

		cmd->Undo();
	}
	else
	{
		std::cout << "WARNING: No more undos to perform." << std::endl;
		return;
	}
}

//----------------------------------------------------------------------------------

void Totem::CommandManager::Redo()
{
	if( !m_redoStack.empty() ) 
	{
		// Put current command in the undo stack
		//m_undoStack.push( m_currentCmd );

		Command* cmd = ( Command* )m_redoStack.top();
		m_undoStack.push( cmd );
		m_redoStack.pop();

		cmd->Redo();
	}
	else
	{
		std::cout << "WARNING: No more redos to perform." << std::endl;
		return;
	}
}

//----------------------------------------------------------------------------------