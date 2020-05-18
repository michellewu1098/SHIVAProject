#include "Command.h"

//----------------------------------------------------------------------------------

Command::Command()
{
	m_state = READY;
	m_object = NULL;
	m_totemController = NULL;
}

//----------------------------------------------------------------------------------

Command::~Command()
{
	// This is a bit messy, I have to say
	// I hate pointers
	// Delete only objects that are not referenced any more in the tree
	//if( IsObjectDetached() )
	//	delete m_object;
}

//----------------------------------------------------------------------------------

bool Command::IsObjectDetached()
{
	if( m_object->IsValid() && m_object->GetChild() == NULL && m_object->GetParent() == NULL )
	{
		return true;
	}
	else
		return false;
}

//----------------------------------------------------------------------------------

// ADD OBJECT COMMAND

AddObjectCommand::AddObjectCommand() : m_primID( 0 ), m_nGUIControllers( 0 )
{ }

//----------------------------------------------------------------------------------

AddObjectCommand::~AddObjectCommand()
{
	delete m_object;
}

//----------------------------------------------------------------------------------

void AddObjectCommand::Execute( Totem::Controller* _controller )
{
	#ifdef _DEBUG
	std::cout << "AddObjectCommand EXECUTE asked to execute" << std::endl;
	#endif
	
	m_totemController = _controller;

	if( m_totemController != NULL )
	{
		m_totemController->AddObjectToTop( m_primID, m_nGUIControllers );
		m_totemController->SelectTopObject();

		m_object = m_totemController->GetSelected();
	}
}

//----------------------------------------------------------------------------------

void AddObjectCommand::Undo()
{
	#ifdef _DEBUG
	std::cout << "AddObjectCommand UNDO asked to execute" << std::endl;
	#endif

	if( m_totemController != NULL )
	{
		Totem::Object* currentSelected = m_totemController->GetSelected();

		if( currentSelected == m_object )
		{
			m_totemController->DeleteSelectedObject();
		}
		else 
		{
			m_totemController->SetSelectedObject( m_object );
			m_totemController->DeleteSelectedObject();
		}
	}
}

//----------------------------------------------------------------------------------

void AddObjectCommand::Redo()
{
	#ifdef _DEBUG
	std::cout << "AddObjectCommand REDO asked to execute" << std::endl;
	#endif
	
	if( m_totemController != NULL )
	{
		m_totemController->ReAddObjectToTop( m_object );
		m_totemController->SelectTopObject();
	}
}

//----------------------------------------------------------------------------------

// DUPLICATE OBJECT COMMAND

DuplicateObjectCommand::DuplicateObjectCommand() : m_primID( 0 ), m_nGUIControllers( 0 )
{ }

//----------------------------------------------------------------------------------

DuplicateObjectCommand::~DuplicateObjectCommand()
{
	delete m_object;
}

//----------------------------------------------------------------------------------

void DuplicateObjectCommand::Execute( Totem::Controller* _controller )
{
	#ifdef _DEBUG
	std::cout << "DuplicateObjectCommand EXECUTE asked to execute" << std::endl;
	#endif

	// EXPLANATION
	// 1. Identify the currently selected object (this is the one which will be duplicated)
	// 2. Find out the primitive type of the selected object.  This is a digit between 0 and 4:
	//		0 Sphere
	//		1 Cone
	//		2 Cylinder
	//		3 Cube
	//		4 Cuboid
	// 3. Add the duplicated object to the top of the pole
	// 4. Set the duplicated objects scale and rotation to the same as the selected object

	m_totemController = _controller;

	if( m_totemController != NULL )
	{
		float scaleX = 1;
		float scaleY = 1;
		float scaleZ = 1;

		float rotX = 0;
		float rotY = 0;
		float rotZ = 0;

//		 _currentNode->getTypeID() == CYLINDER_NODE_ID

		m_object = m_totemController->GetSelected();
//		int x = m_totemController->get

		if( m_object != NULL )
		{
			m_primID = m_object->GetPrimTypeID();
			
			scaleX = m_object->GetScaleX();
			scaleY = m_object->GetScaleY();
			scaleZ = m_object->GetScaleZ();

			m_object->GetRotation(rotX, rotY, rotZ);


			m_totemController->AddObjectToTop( m_primID, m_nGUIControllers );
			m_totemController->SelectTopObject();

//		Totem::Object* duplicatedObject = m_totemController->GetSelected();
//		duplicatedObject = m_object;

			m_object = m_totemController->GetSelected();
			if( m_object != NULL )
			{
				m_object->SetRotation(rotX, rotY, rotZ);
				m_object->SetScale(scaleX, scaleY, scaleZ);
			}
		}
	}
}

//----------------------------------------------------------------------------------

void DuplicateObjectCommand::Undo()
{
	#ifdef _DEBUG
	std::cout << "DuplicateObjectCommand UNDO asked to execute" << std::endl;
	#endif

	if( m_totemController != NULL )
	{
		Totem::Object* currentSelected = m_totemController->GetSelected();

		if( currentSelected == m_object )
		{
			m_totemController->DeleteSelectedObject();
		}
		else 
		{
			m_totemController->SetSelectedObject( m_object );
			m_totemController->DeleteSelectedObject();
		}
	}
}

//----------------------------------------------------------------------------------

void DuplicateObjectCommand::Redo()
{
	#ifdef _DEBUG
	std::cout << "DuplicateObjectCommand REDO asked to execute" << std::endl;
	#endif
	
	if( m_totemController != NULL )
	{
		m_totemController->ReAddObjectToTop( m_object );
		m_totemController->SelectTopObject();
	}
}

//----------------------------------------------------------------------------------

// NUDGE ACTIVITY

NudgeCommand::NudgeCommand() : m_x( 0.f ), m_y( 0.f ), m_z( 0.f )
{ }

//----------------------------------------------------------------------------------

void NudgeCommand::Execute( Totem::Controller* _controller )
{
	#ifdef _DEBUG
	std::cout << "NudgeCommand EXECUTE asked to execute" << std::endl;
	#endif

	m_totemController = _controller;
	 
	if( m_totemController != NULL )
	{
		m_object = m_totemController->GetSelected();
		m_totemController->MoveSelectedObject( m_x, m_y, m_z );
	}
}

//----------------------------------------------------------------------------------

void NudgeCommand::Undo()
{
	#ifdef _DEBUG
	std::cout << "NudgeCommand UNDO asked to execute" << std::endl;
	#endif
	
	if( m_totemController != NULL )
	{
		Totem::Object* currentSelected = m_totemController->GetSelected();

		if( currentSelected != m_object )
		{
			currentSelected->SetDrawBBox( false );
			m_totemController->SetSelectedObject( m_object );
			m_object->SetDrawBBox( true );
		}
		m_totemController->MoveSelectedObject( -m_x, -m_y, -m_z );
	}
}

//----------------------------------------------------------------------------------

void NudgeCommand::Redo()
{
	#ifdef _DEBUG
	std::cout << "NudgeCommand REDO asked to execute" << std::endl;
	#endif
	
	if( m_totemController != NULL )
	{
		Totem::Object* currentSelected = m_totemController->GetSelected();

		if( currentSelected != m_object )
		{
			currentSelected->SetDrawBBox( false );
			m_totemController->SetSelectedObject( m_object );
			m_object->SetDrawBBox( true );
		}

		m_totemController->MoveSelectedObject( m_x, m_y, m_z );
	}
}

//----------------------------------------------------------------------------------

// RESET NUDGE ACTIVITY

ResetNudgeCommand::ResetNudgeCommand() : m_offsetX( 0.f ), m_offsetY( 0.f ), m_offsetZ( 0.f )
{ }

//----------------------------------------------------------------------------------

void ResetNudgeCommand::Execute( Totem::Controller* _controller )
{
	#ifdef _DEBUG
	std::cout << "ResetNudgeCommand EXECUTE asked to execute" << std::endl;
	#endif
	
	m_totemController = _controller;

	if( m_totemController != NULL )
	{
		m_object = m_totemController->GetSelected();

		m_totemController->GetSelectedObjectOffset( m_offsetX, m_offsetY, m_offsetZ );
		m_totemController->ResetMoveSelected();
	}
}

//----------------------------------------------------------------------------------

void ResetNudgeCommand::Undo()
{
	#ifdef _DEBUG
	std::cout << "ResetNudgeCommand UNDO asked to execute" << std::endl;
	#endif
	
	if( m_totemController != NULL )
	{
		Totem::Object* currentSelected = m_totemController->GetSelected();

		if( currentSelected != m_object )
		{
			currentSelected->SetDrawBBox( false );
			m_totemController->SetSelectedObject( m_object );
			m_object->SetDrawBBox( true );
		}

		m_totemController->MoveSelectedObject( m_offsetX, m_offsetY, m_offsetZ );
	}
}

//----------------------------------------------------------------------------------

void ResetNudgeCommand::Redo()
{
	#ifdef _DEBUG
	std::cout << "ResetNudgeCommand REDO asked to execute" << std::endl;
	#endif
	
	if( m_totemController != NULL )
	{
		Totem::Object* currentSelected = m_totemController->GetSelected();

		if( currentSelected != m_object )
		{
			currentSelected->SetDrawBBox( false );
			m_totemController->SetSelectedObject( m_object );
			m_object->SetDrawBBox( true );
		}

		m_totemController->GetSelectedObjectOffset( m_offsetX, m_offsetY, m_offsetZ );
		m_totemController->ResetMoveSelected();
	}
}

//----------------------------------------------------------------------------------

// SELECT ACTIVITY

SelectCommand::SelectCommand()
{
	m_selection = "";
}

//----------------------------------------------------------------------------------

void SelectCommand::Execute( Totem::Controller* _controller )
{
	#ifdef _DEBUG
	std::cout << "SelectCommand EXECUTE asked to execute" << std::endl;
	#endif
	
	m_totemController = _controller;
	
	if( m_totemController != NULL )
	{
		if( m_selection == std::string( "below" ) )
		{
			m_totemController->SelectObjectBelow();
		}
		else if( m_selection == std::string( "above" ) )
		{
			m_totemController->SelectObjectAbove();
		}

		m_object = m_totemController->GetSelected();
	}
}

//----------------------------------------------------------------------------------

void SelectCommand::Undo()
{
	#ifdef _DEBUG
	std::cout << "SelectCommand UNDO asked to execute" << std::endl;
	#endif
	
	if( m_totemController != NULL )
	{
		Totem::Object *currentObject = m_totemController->GetSelected();

		if( currentObject != m_object )
		{
			currentObject->SetDrawBBox( false );
			m_totemController->SetSelectedObject( m_object );
			m_object->SetDrawBBox( true );
			currentObject = m_object;
		}

		if( m_selection == std::string( "below" ) )
		{
			m_totemController->SelectObjectAbove();
		}
		else if( m_selection == std::string( "above" ) )
		{
			m_totemController->SelectObjectBelow();
		}
	}
}

//----------------------------------------------------------------------------------

void SelectCommand::Redo()
{
	#ifdef _DEBUG
	std::cout << "SelectCommand REDO asked to execute" << std::endl;
	#endif

	if( m_totemController != NULL )
	{
		Totem::Object *currentObject = m_totemController->GetSelected();

		if( currentObject != m_object )
		{
			currentObject->SetDrawBBox( false );
			m_totemController->SetSelectedObject( m_object );
			m_object->SetDrawBBox( true );
			currentObject = m_object;
		}

		if( m_selection == std::string( "below" ) )
		{
			m_totemController->SelectObjectBelow();
		}
		else if( m_selection == std::string( "above" ) )
		{
			m_totemController->SelectObjectAbove();
		}
	}
}

//----------------------------------------------------------------------------------

// SELECT WITH MOUSE ACTIVITY

SelectMouseCommand::SelectMouseCommand() 
{
	m_prevSelected = NULL;
	m_selectVecOrigX = m_selectVecOrigY = m_selectVecOrigZ = 0.f;
	m_selectVecDirX = m_selectVecDirY = m_selectVecDirZ = 0.f;
}

//----------------------------------------------------------------------------------

void SelectMouseCommand::SetSelectionVec( const float &_origX, const float &_origY, const float &_origZ, const float &_dirX, const float &_dirY, const float &_dirZ )
{
	m_selectVecOrigX = _origX;
	m_selectVecOrigY = _origY;
	m_selectVecOrigZ = _origZ;

	m_selectVecDirX = _dirX;
	m_selectVecDirY = _dirY;
	m_selectVecDirZ = _dirZ;
}

//----------------------------------------------------------------------------------

void SelectMouseCommand::Execute( Totem::Controller* _controller )
{
	#ifdef _DEBUG
	std::cout << "SelectMouseCommand EXECUTE asked to execute" << std::endl;
	#endif

	m_totemController = _controller;

	if( m_totemController != NULL )
	{
		m_prevSelected = m_totemController->GetSelected();
		m_totemController->SelectIntersectingObject( m_selectVecOrigX, m_selectVecOrigY, m_selectVecOrigZ, m_selectVecDirX, m_selectVecDirY, m_selectVecDirZ );
		m_object = m_totemController->GetSelected();
	}
}

//----------------------------------------------------------------------------------

void SelectMouseCommand::Undo()
{
	#ifdef _DEBUG
	std::cout << "SelectMouseCommand UNDO asked to execute" << std::endl; 
	#endif

	if( m_totemController != NULL )
	{
		Totem::Object *currentObject = m_totemController->GetSelected();
		currentObject->SetDrawBBox( false );
		m_totemController->SetSelectedObject( m_prevSelected );
		m_prevSelected->SetDrawBBox( true );
	}
}

//----------------------------------------------------------------------------------

void SelectMouseCommand::Redo()
{
	#ifdef _DEBUG
	std::cout << "SelectMouseCommand REDO asked to execute" << std::endl;
	#endif

	if( m_totemController != NULL )
	{
		Totem::Object *currentObject = m_totemController->GetSelected();

		if( currentObject != m_object )
		{
			currentObject->SetDrawBBox( false );
			m_totemController->SetSelectedObject( m_object );
			m_object->SetDrawBBox( true );
		}
	}
}

//----------------------------------------------------------------------------------

// SWAP ACTIVITY

SwapCommand::SwapCommand() : m_moveUp( false )
{ }

//----------------------------------------------------------------------------------

void SwapCommand::Execute( Totem::Controller* _controller )
{
	#ifdef _DEBUG
	std::cout << "SwapCommand EXECUTE asked to execute" << std::endl;
	#endif
	
	m_totemController = _controller;

	if( m_totemController != NULL )
	{
		m_object = m_totemController->GetSelected();

		if( m_moveUp)
		{
			m_totemController->ReorderSelectedObject( true );
		}
		else
		{
			m_totemController->ReorderSelectedObject( false );
		}
	}
}

//----------------------------------------------------------------------------------

void SwapCommand::Undo()
{
	#ifdef _DEBUG
	std::cout << "SwapCommand UNDO asked to execute" << std::endl;
	#endif

	if( m_totemController != NULL )
	{
		Totem::Object *currentObject = m_totemController->GetSelected();

		if( currentObject != m_object )
		{
			currentObject->SetDrawBBox( false );
			m_totemController->SetSelectedObject( m_object );
			m_object->SetDrawBBox( true );
			currentObject = m_object;
		}

		if( m_moveUp)
		{
			m_totemController->ReorderSelectedObject( false );
		}
		else
		{
			m_totemController->ReorderSelectedObject( true );
		}
	}
}

//----------------------------------------------------------------------------------

void SwapCommand::Redo()
{
	#ifdef _DEBUG
	std::cout << "SwapCommand REDO asked to execute" << std::endl;
	#endif

	if( m_totemController != NULL )
	{
		Totem::Object *currentObject = m_totemController->GetSelected();

		if( currentObject != m_object )
		{
			currentObject->SetDrawBBox( false );
			m_totemController->SetSelectedObject( m_object );
			m_object->SetDrawBBox( true );
			currentObject = m_object;
		}

		if( m_moveUp)
		{
			m_totemController->ReorderSelectedObject( true );
		}
		else
		{
			m_totemController->ReorderSelectedObject( false );
		}
	}
}

//----------------------------------------------------------------------------------

// RESET SCALE ACTIVITY

ResetScaleCommand::ResetScaleCommand() : m_scaleX( 0.f ), m_scaleY( 0.f ), m_scaleZ( 0.f )
{ }

//----------------------------------------------------------------------------------

void ResetScaleCommand::Execute( Totem::Controller* _controller )
{
	#ifdef _DEBUG
	std::cout << "ResetScaleCommand EXECUTE asked to execute" << std::endl;
	#endif

	m_totemController = _controller;

	if( m_totemController != NULL )
	{
		Totem::Object *currentObject = m_totemController->GetSelected();
		m_object = currentObject;

		if( currentObject != NULL )
		{
			m_scaleX = currentObject->GetScaleX();
			m_scaleY = currentObject->GetScaleY();
			m_scaleZ = currentObject->GetScaleZ();

			#ifdef _DEBUG
			std::cout << "INFO: scaling reset object to: 1" << std::endl;
			#endif
			currentObject->SetScale( 1.0f, 1.0f, 1.0f );
		}
	}
}

//----------------------------------------------------------------------------------

void ResetScaleCommand::Undo()
{
	#ifdef _DEBUG
	std::cout << "ResetScaleCommand UNDO asked to execute" << std::endl;
	#endif

	if( m_totemController != NULL )
	{
		Totem::Object *currentObject = m_totemController->GetSelected();

		if( currentObject != m_object )
		{
			currentObject->SetDrawBBox( false );
			m_totemController->SetSelectedObject( m_object );
			m_object->SetDrawBBox( true );
			currentObject = m_object;
		}

		if( currentObject != NULL )
		{
			#ifdef _DEBUG
			std::cout << "INFO: scaling reset object to: 1" << std::endl;
			#endif
			currentObject->SetScale( m_scaleX, m_scaleY, m_scaleZ );
		}
	}
}

//----------------------------------------------------------------------------------

void ResetScaleCommand::Redo()
{
	#ifdef _DEBUG
	std::cout << "ResetScaleCommand REDO asked to execute" << std::endl;
	#endif

	if( m_totemController != NULL )
	{
		Totem::Object *currentObject = m_totemController->GetSelected();

		if( currentObject != m_object )
		{
			currentObject->SetDrawBBox( false );
			m_totemController->SetSelectedObject( m_object );
			m_object->SetDrawBBox( true );
			currentObject = m_object;
		}

		if( currentObject != NULL )
		{
			m_scaleX = currentObject->GetScaleX();
			m_scaleY = currentObject->GetScaleY();
			m_scaleZ = currentObject->GetScaleZ();

			#ifdef _DEBUG
			std::cout << "INFO: scaling reset object to: 1" << std::endl;
			#endif
			currentObject->SetScale( 1.0f, 1.0f, 1.0f );
		}
	}
}

//----------------------------------------------------------------------------------

// SCALE ACTIVITY

ScaleCommand::ScaleCommand()
{
	m_scaleX = m_scaleY = m_scaleZ = 0.f;
	m_scaling = "";
}

//----------------------------------------------------------------------------------

void ScaleCommand::SetScaleStepSize( const float &_scaleX, const float &_scaleY, const float &_scaleZ )
{
	m_scaleX = _scaleX;
	m_scaleY = _scaleY;
	m_scaleZ = _scaleZ;
}

//----------------------------------------------------------------------------------

void ScaleCommand::Execute( Totem::Controller* _controller )
{
	#ifdef _DEBUG
	std::cout << "ScaleCommand EXECUTE asked to execute" << std::endl;
	#endif

	m_totemController = _controller;
	m_object = m_totemController->GetSelected();

	if( m_totemController != NULL )
	{
		Totem::Object *currentObject = m_totemController->GetSelected();
		if( currentObject != NULL )
		{
			float scaleX = currentObject->GetScaleX();
			float scaleY = currentObject->GetScaleY();
			float scaleZ = currentObject->GetScaleZ();
			
			if( m_scaling == std::string( "up" ) )
			{
				scaleX += m_scaleX;
				scaleY += m_scaleY;
				scaleZ += m_scaleZ;
				if( scaleX > 2.0f )
					scaleX = 2.0f;
				if( scaleY > 2.0f )
					scaleY = 2.0f;
				if( scaleZ > 2.0f )
					scaleZ = 2.0f;
				#ifdef _DEBUG
				std::cout << "INFO: scaling up object to: " << scaleX << " " << scaleY << " " << scaleZ << std::endl;
				#endif
			}
			else if( m_scaling == std::string( "down" ) ) 
			{
				scaleX -= m_scaleX;
				scaleY -= m_scaleY;
				scaleZ -= m_scaleZ;
				if( scaleX < 0.1f )
					scaleX = 0.1f;
				if( scaleY < 0.1f )
					scaleY = 0.1f;
				if( scaleZ < 0.1f )
					scaleZ = 0.1f;
				#ifdef _DEBUG
				std::cout << "INFO: scaling down object to: " << scaleX << " " << scaleY << " " << scaleZ << std::endl;
				#endif
			}
			currentObject->SetScale( scaleX, scaleY, scaleZ );
		}
	}
}

//----------------------------------------------------------------------------------

void ScaleCommand::Undo()
{
	#ifdef _DEBUG
	std::cout << "ScaleCommand UNDO asked to execute" << std::endl;
	#endif

	if( m_totemController != NULL )
	{
		Totem::Object *currentObject = m_totemController->GetSelected();

		if( currentObject != m_object )
		{
			currentObject->SetDrawBBox( false );
			m_totemController->SetSelectedObject( m_object );
			m_object->SetDrawBBox( true );
			currentObject = m_object;
		}

		if( currentObject != NULL )
		{
			float scaleX = currentObject->GetScaleX();
			float scaleY = currentObject->GetScaleY();
			float scaleZ = currentObject->GetScaleZ();
			
			if( m_scaling == std::string( "up" ) )
			{
				scaleX -= m_scaleX;
				scaleY -= m_scaleY;
				scaleZ -= m_scaleZ;
				if( scaleX < 0.1f )
					scaleX = 0.1f;
				if( scaleY < 0.1f )
					scaleY = 0.1f;
				if( scaleZ < 0.1f )
					scaleZ = 0.1f;

				#ifdef _DEBUG
				std::cout << "INFO: scaling up object to: " << scaleX << " " << scaleY << " " << scaleZ << std::endl;
				#endif
			}
			else if( m_scaling == std::string( "down" ) ) 
			{
				scaleX += m_scaleX;
				scaleY += m_scaleY;
				scaleZ += m_scaleZ;
				if( scaleX > 2.0f )
					scaleX = 2.0f;
				if( scaleY > 2.0f )
					scaleY = 2.0f;
				if( scaleZ > 2.0f )
					scaleZ = 2.0f;
				#ifdef _DEBUG
				std::cout << "INFO: scaling down object to: " << scaleX << " " << scaleY << " " << scaleZ << std::endl;
				#endif
			}
			currentObject->SetScale( scaleX, scaleY, scaleZ );
		}
	}
}

//----------------------------------------------------------------------------------

void ScaleCommand::Redo()
{
	#ifdef _DEBUG
	std::cout << "ScaleCommand REDO asked to execute" << std::endl;
	#endif	
	
	if( m_totemController != NULL )
	{
		Totem::Object *currentObject = m_totemController->GetSelected();

		if( currentObject != m_object )
		{
			currentObject->SetDrawBBox( false );
			m_totemController->SetSelectedObject( m_object );
			m_object->SetDrawBBox( true );
			currentObject = m_object;
		}

		if( currentObject != NULL )
		{
			float scaleX = currentObject->GetScaleX();
			float scaleY = currentObject->GetScaleY();
			float scaleZ = currentObject->GetScaleZ();
			
			if( m_scaling == std::string( "up" ) )
			{
				scaleX += m_scaleX;
				scaleY += m_scaleY;
				scaleZ += m_scaleZ;
				if( scaleX > 2.0f )
					scaleX = 2.0f;
				if( scaleY > 2.0f )
					scaleY = 2.0f;
				if( scaleZ > 2.0f )
					scaleZ = 2.0f;
				#ifdef _DEBUG
				std::cout << "INFO: scaling up object to: " << scaleX << " " << scaleY << " " << scaleZ << std::endl;
				#endif
			}
			else if( m_scaling == std::string( "down" ) ) 
			{
				scaleX -= m_scaleX;
				scaleY -= m_scaleY;
				scaleZ -= m_scaleZ;
				if( scaleX < 0.1f )
					scaleX = 0.1f;
				if( scaleY < 0.1f )
					scaleY = 0.1f;
				if( scaleZ < 0.1f )
					scaleZ = 0.1f;
				#ifdef _DEBUG
				std::cout << "INFO: scaling down object to: " << scaleX << " " << scaleY << " " << scaleZ << std::endl;
				#endif
			}
			currentObject->SetScale( scaleX, scaleY, scaleZ );
		}
	}
}

//----------------------------------------------------------------------------------

// ROTATE OBJECT ACTIVITY

RotateCommand::RotateCommand() : m_rotX( 0.f ), m_rotY( 0.f ), m_rotZ( 0.f )
{ }

//----------------------------------------------------------------------------------

void RotateCommand::Execute( Totem::Controller* _controller )
{
	#ifdef _DEBUG
	std::cout << "RotateCommand EXECUTE asked to execute" << std::endl; 
	#endif

	m_totemController = _controller;

	if( m_totemController != NULL )
	{
		Totem::Object *currentObject = m_totemController->GetSelected();
		m_object = currentObject;

		if( currentObject != NULL )
		{
			#ifdef _DEBUG
			std::cout << "INFO: adding object rotation: " << m_rotX << " " << m_rotY << " " << m_rotZ << std::endl;
			#endif
			currentObject->AddRotation( m_rotX, m_rotY, m_rotZ );
		}
	}
}

//----------------------------------------------------------------------------------

void RotateCommand::Undo()
{
	#ifdef _DEBUG
	std::cout << "RotateCommand UNDO asked to execute" << std::endl; 
	#endif
	
	if( m_totemController != NULL )
	{
		Totem::Object *currentObject = m_totemController->GetSelected();

		if( currentObject != m_object )
		{
			currentObject->SetDrawBBox( false );
			m_totemController->SetSelectedObject( m_object );
			m_object->SetDrawBBox( true );
			currentObject = m_object;
		}

		if( currentObject != NULL )
		{
			currentObject->AddRotation( -m_rotX, -m_rotY, -m_rotZ );
		}
	}
}

//----------------------------------------------------------------------------------

void RotateCommand::Redo()
{
	#ifdef _DEBUG
	std::cout << "RotateCommand REDO asked to execute" << std::endl; 
	#endif

	if( m_totemController != NULL )
	{
		Totem::Object *currentObject = m_totemController->GetSelected();

		if( currentObject != m_object )
		{
			currentObject->SetDrawBBox( false );
			m_totemController->SetSelectedObject( m_object );
			m_object->SetDrawBBox( true );
			currentObject = m_object;
		}

		if( currentObject != NULL )
		{
			#ifdef _DEBUG
			std::cout << "INFO: adding object rotation: " << m_rotX << " " << m_rotY << " " << m_rotZ << std::endl;
			#endif
			currentObject->AddRotation( m_rotX, m_rotY, m_rotZ );
		}
	}
}


//----------------------------------------------------------------------------------

// RESET ROTATION ACTIVITY

ResetRotationCommand::ResetRotationCommand() : m_rotX( 0.f ), m_rotY( 0.f ), m_rotZ( 0.f )
{ }


//----------------------------------------------------------------------------------

void ResetRotationCommand::Execute( Totem::Controller* _controller )
{
	#ifdef _DEBUG
	std::cout << "ResetRotationCommand EXECUTE asked to execute" << std::endl;
	#endif

	m_totemController = _controller;

	if( m_totemController != NULL )
	{
		Totem::Object *currentObject = m_totemController->GetSelected();
		m_object = currentObject;

		if( currentObject != NULL )
		{
			// Remember rotation settings
			currentObject->GetRotation(m_rotX, m_rotY, m_rotZ);
//			m_rotY = currentObject->GetScaleY();
//			m_rotZ = currentObject->GetScaleZ();

			#ifdef _DEBUG
			std::cout << "INFO: rotation reset object to: 0" << std::endl;
			#endif

			currentObject->SetRotation( 0.0f, 0.0f, 0.0f );
		}
	}
}

//----------------------------------------------------------------------------------

void ResetRotationCommand::Undo()
{
	#ifdef _DEBUG
	std::cout << "ResetRotateCommand UNDO asked to execute" << std::endl; 
	#endif
	
	if( m_totemController != NULL )
	{
		Totem::Object *currentObject = m_totemController->GetSelected();

		if( currentObject != m_object )
		{
			currentObject->SetDrawBBox( false );
			m_totemController->SetSelectedObject( m_object );
			m_object->SetDrawBBox( true );
			currentObject = m_object;
		}

		if( currentObject != NULL )
		{
			currentObject->SetRotation( m_rotX, m_rotY, m_rotZ );
		}
	}
}

//----------------------------------------------------------------------------------

void ResetRotationCommand::Redo()
{
	#ifdef _DEBUG
	std::cout << "ResetRotateCommand REDO asked to execute" << std::endl; 
	#endif

	if( m_totemController != NULL )
	{
		Totem::Object *currentObject = m_totemController->GetSelected();

		if( currentObject != m_object )
		{
			currentObject->SetDrawBBox( false );
			m_totemController->SetSelectedObject( m_object );
			m_object->SetDrawBBox( true );
			currentObject = m_object;
		}

		if( currentObject != NULL )
		{
			#ifdef _DEBUG
			std::cout << "INFO: adding object rotation: " << m_rotX << " " << m_rotY << " " << m_rotZ << std::endl;
			#endif

            // A redo of reset object rotation must put the object back to its original orienttion so set the rotation to zero
			currentObject->SetRotation( 0, 0, 0);
		}
	}
}

//----------------------------------------------------------------------------------

// RESET BLEND ACTIVITY

ResetBlendCommand::ResetBlendCommand() : m_blendAmount( 0.f )
{ }

//----------------------------------------------------------------------------------

void ResetBlendCommand::Execute( Totem::Controller* _controller )
{
	#ifdef _DEBUG
	std::cout << "ResetBlendCommand EXECUTE asked to execute" << std::endl; 
	#endif

	m_totemController = _controller;

	m_blendAmount = m_totemController->GetBlend();
	m_totemController->SetBlend( 0.0f );
}

//----------------------------------------------------------------------------------

void ResetBlendCommand::Undo()
{
	#ifdef _DEBUG
	std::cout << "ResetBlendCommand UNDO asked to execute" << std::endl;
	#endif

	m_totemController->SetBlend( m_blendAmount );
}

//----------------------------------------------------------------------------------

void ResetBlendCommand::Redo()
{
	#ifdef _DEBUG
	std::cout << "ResetBlendCommand REDO asked to execute" << std::endl; 
	#endif

	m_blendAmount = m_totemController->GetBlend();
	m_totemController->SetBlend( 0.0f );
}

//----------------------------------------------------------------------------------

// BLEND ACTIVITY

BlendCommand::BlendCommand() 
{
	m_blend = "";
}

//----------------------------------------------------------------------------------

void BlendCommand::Execute( Totem::Controller* _controller )
{
	#ifdef _DEBUG
	std::cout << "BlendCommand EXECUTE asked to execute" << std::endl;
	#endif

	m_totemController = _controller;

	if( m_blend == std::string( "increase" ) )
	{
		m_totemController->AdjustBlend( 0.05f );
		float currentBlend = m_totemController->GetBlend();
		if( currentBlend > 1.5f )
			m_totemController->SetBlend( 1.5f );
	}
	else if( m_blend == std::string( "decrease" ) )
	{
		m_totemController->AdjustBlend( -0.05f );
		float currentBlend = m_totemController->GetBlend();
		if( currentBlend < -1.0f )
			m_totemController->SetBlend( -1.0f );
	}
}

//----------------------------------------------------------------------------------

void BlendCommand::Undo()
{
	#ifdef _DEBUG
	std::cout << "BlendCommand UNDO asked to execute" << std::endl;
	#endif

	if( m_blend == std::string( "increase" ) )
	{
		m_totemController->AdjustBlend( -0.05f );
		float currentBlend = m_totemController->GetBlend();
		if( currentBlend < -1.0f )
			m_totemController->SetBlend( -1.0f );
	}
	else if( m_blend == std::string( "decrease" ) )
	{
		m_totemController->AdjustBlend( 0.05f );
		float currentBlend = m_totemController->GetBlend();
		if( currentBlend > 1.5f )
			m_totemController->SetBlend( 1.5f );
	}
}

//----------------------------------------------------------------------------------

void BlendCommand::Redo()
{
	#ifdef _DEBUG
	std::cout << "BlendCommand REDO asked to execute" << std::endl;
	#endif

	if( m_blend == std::string( "increase" ) )
	{
		m_totemController->AdjustBlend( 0.05f );
		float currentBlend = m_totemController->GetBlend();
		if( currentBlend > 1.5f )
			m_totemController->SetBlend( 1.5f );
	}
	else if( m_blend == std::string( "decrease" ) )
	{
		m_totemController->AdjustBlend( -0.05f );
		float currentBlend = m_totemController->GetBlend();
		if( currentBlend < -1.0f )
			m_totemController->SetBlend( -1.0f );
	}
}

//----------------------------------------------------------------------------------

// DRILL COMMAND

DrillCommand::DrillCommand()
{
	m_op = NULL;
}

//----------------------------------------------------------------------------------

DrillCommand::~DrillCommand()
{
	delete m_op;
}

//----------------------------------------------------------------------------------

void DrillCommand::Execute( Totem::Controller* _controller )
{
	#ifdef _DEBUG
	std::cout << "DrillCommand EXECUTE asked to execute" << std::endl;
	#endif

	m_totemController = _controller;

	if( m_op != NULL )
	{
		m_totemController->AddOperation( m_op );
	}
}

//----------------------------------------------------------------------------------

void DrillCommand::Undo()
{
	#ifdef _DEBUG
	std::cout << "DrillCommand UNDO asked to execute" << std::endl;
	#endif

	m_totemController->RemoveLastOperation();
}

//----------------------------------------------------------------------------------

void DrillCommand::Redo()
{
	#ifdef _DEBUG
	std::cout << "DrillCommand REDO asked to execute" << std::endl;
	#endif

	if( m_op != NULL )
	{
		m_totemController->AddOperation( m_op );
	}
}

//----------------------------------------------------------------------------------

// DELETE OBJECT COMMAND

DeleteObjectCommand::DeleteObjectCommand()
{
	m_victim = NULL;
}

//----------------------------------------------------------------------------------

//DeleteObjectCommand::~DeleteObjectCommand()
//{
	//delete m_victim;
//}

//----------------------------------------------------------------------------------

// Undo of delete should remove OBJ from Tree, but keep a pointer to it
// Redo of delete will put OBJ back in Tree
// Execute and Redo methods must add exactly the same OBJ to Tree
// http://sourcephile.blogspot.co.uk/2011/04/how-to-do-undo-and-redo.html

void DeleteObjectCommand::Execute( Totem::Controller* _controller )
{
	#ifdef _DEBUG
	std::cout << "DeleteObjectCommand EXECUTE asked to execute" << std::endl;
	#endif

	m_totemController = _controller;

	m_object = m_totemController->GetSelected();
	m_victim = m_totemController->DeleteSelectedObj();
}

//----------------------------------------------------------------------------------

void DeleteObjectCommand::Undo()
{
	#ifdef _DEBUG
	std::cout << "DeleteObjectCommand UNDO asked to execute" << std::endl;
	#endif

	Totem::Object* currentSelected = m_totemController->GetSelected();

	if( currentSelected != m_object )
	{
		if( currentSelected == NULL )
		{
			// This means that pole is empty
			m_totemController->ReAddObjectToTop( m_object );
			m_totemController->SelectTopObject();
		}
		else
		{
			currentSelected->SetDrawBBox( false );
			m_totemController->SetSelectedObject( m_object );
			m_object->SetDrawBBox( true );
			m_totemController->InsertObject( m_victim );

			// April 2020: Code to reverse offsets created during deletion of an object

			Totem::Object *prevParent;
			Totem::Object *currParent;
			
			prevParent = m_victim;
			currParent = prevParent->GetParent();

			if (currParent != NULL)
			{
				float currParentOffsetx;
				float currParentOffsety;
				float currParentOffsetz;

				while (currParent != NULL)
				{
					currParent->GetTranslationOffset(currParentOffsetx, currParentOffsety, currParentOffsetz);

					currParent->SetTranslationOffset(currParentOffsetx, currParentOffsety, currParentOffsetz - 0.5f);

					prevParent = currParent;
					currParent = currParent->GetParent();
				}
			}	
		}
	}
}

//----------------------------------------------------------------------------------

void DeleteObjectCommand::Redo()
{
	#ifdef _DEBUG
	std::cout << "DeleteObjectCommand REDO asked to execute" << std::endl;
	#endif

	Totem::Object* currentSelected = m_totemController->GetSelected();

	if( currentSelected != m_object )
	{
		currentSelected->SetDrawBBox( false );
		m_totemController->SetSelectedObject( m_object );
		m_object->SetDrawBBox( true );
	}

	m_victim = m_totemController->DeleteSelectedObj();
}

//----------------------------------------------------------------------------------

