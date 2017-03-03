#include "VolumeTree/VolumeTree.h"
#include "VolumeTree/Leaves/ConeNode.h"
#include "VolumeTree/Leaves/CubeNode.h"
#include "VolumeTree/Leaves/CylinderNode.h"
#include "VolumeTree/Leaves/SphereNode.h"
#include "VolumeTree/Leaves/TorusNode.h"
#include "VolumeTree/Leaves/VolCacheNode.h"
#include "VolumeTree/Nodes/BlendCSG.h"
#include "VolumeTree/Nodes/CSG.h"
#include "VolumeTree/Nodes/TransformNode.h"

//----------------------------------------------------------------------------------

VolumeTree::Tree::Tree()
{
	m_rootNode = NULL;
	
	m_bboxExtents = new float[ 3 ];
	m_bboxCentre = new float[ 3 ];
	m_bboxBoundsMin = new float[ 3 ];
	m_bboxBoundsMax = new float[ 3 ];
	m_exportID = 0;
}

//----------------------------------------------------------------------------------

VolumeTree::Tree::Tree( Node *_rootNode )
{
	m_rootNode = _rootNode;
	
	m_bboxExtents = new float[ 3 ];
	m_bboxCentre = new float[ 3 ];
	m_bboxBoundsMin = new float[ 3 ];
	m_bboxBoundsMax = new float[ 3 ];
	m_exportID = 0;
}

//----------------------------------------------------------------------------------

VolumeTree::Tree::~Tree()
{
	// TODO: currently we need to leave the _rootNode because it is probably used externally :S

	delete [] m_bboxExtents;
	delete [] m_bboxCentre;
	delete [] m_bboxBoundsMin;
	delete [] m_bboxBoundsMax;
}

//----------------------------------------------------------------------------------

bool VolumeTree::Tree::Load( const char* _filename )
{
	if( _filename && !_filename[ 0 ] )
		return false;

	if( !boost::filesystem::is_regular_file( _filename ) )
	{
		std::cerr << "WARNING: VolumeTree::Tree cannot find .xml file to load: " << _filename << std::endl;
		return false;
	}

	std::cout << "INFO: VolumeTree::Tree Loading from .xml file: " << _filename << std::endl;

	if( ImportXML( _filename ) )
	{
		std::cout << "File " << _filename << " imported successfully." << std::endl;
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------------

bool VolumeTree::Tree::ImportXML( const char* _filename )
{
	Node *newTree = NULL;
	TiXmlDocument doc( _filename );

	if( doc.LoadFile() ) 
	{
		std::cout << "XML file loaded, starting to parse now.." << std::endl;
		TiXmlElement *rootElement = doc.RootElement() ;

		if( rootElement != NULL)
		{
			if( rootElement->ValueStr() == "Model" )
			{
				TiXmlElement* childNode = rootElement->FirstChildElement();
				if( childNode != NULL ) 
				{
					if( childNode->ValueStr() == "CSG" ) 
					{
						newTree =  importFromXML( childNode );
					}
				}
			}
		}
	}

	if( newTree != NULL )
	{
		if( m_rootNode != NULL )
		{
			delete m_rootNode;
		}
		m_rootNode = newTree;

		return true;
	}
	else {
		std::cerr << "ERROR: ImportFromXML failed to give valid root node from loaded .xml file" << std::endl;
		return false;
	}
}

//----------------------------------------------------------------------------------

VolumeTree::Node* VolumeTree::Tree::importFromXML( TiXmlElement *_root )
{
	if( !_root )
	{
		std::cerr << "ERROR: VolumeTree::Tree::importFromXML given a NULL root" << std::endl;
		return NULL;
	}

	VolumeTree::Node* currentImport = NULL;

	if( _root->ValueStr() == std::string( "Cone" ) )
	{
		double radius, height;
		
		if( _root->Type() == TiXmlNode::TINYXML_ELEMENT )
		{
			VolumeTree::ConeNode *currentImportTyped = new VolumeTree::ConeNode();

			for( TiXmlAttribute *currentAttribute = _root->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
			{
				if( currentAttribute->Name() == std::string( "Radius" ) ) 
				{
					if( currentAttribute->QueryDoubleValue( &radius ) != TIXML_SUCCESS ) { 
						std::cerr << "ERROR: Couldn't find attribute \"Radius\" in Cone node" << std::endl; return NULL;
					}
				}
				else if( currentAttribute->Name() == std::string( "Height" ) )
				{
					if( currentAttribute->QueryDoubleValue( &height ) != TIXML_SUCCESS ) {  
					std::cerr << "ERROR: Couldn't find attribute \"Height\" in Cone node" << std::endl; return NULL;
					}
				}
			}
			
			currentImportTyped->SetRadius( ( float )radius );
			currentImportTyped->SetLength( ( float )height );
			currentImport = dynamic_cast< VolumeTree::Node* >( currentImportTyped );
		}
	}
	else if( _root->ValueStr() == std::string( "Cube" ) )
	{
		double dimX, dimY, dimZ; 
		
		if( _root->Type() == TiXmlNode::TINYXML_ELEMENT )
		{
			VolumeTree::CubeNode *currentImportTyped = new VolumeTree::CubeNode();

			for( TiXmlAttribute *currentAttribute = _root->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
			{
				if( currentAttribute->Name() == std::string( "DimensionX" ) ) 
				{
					if( currentAttribute->QueryDoubleValue( &dimX ) != TIXML_SUCCESS ) { 
						std::cerr << "ERROR: Couldn't find attribute \"DimensionX\" in Cube node" << std::endl; return NULL;
					}
				}
				else if( currentAttribute->Name() == std::string( "DimensionY" ) )
				{
					if( currentAttribute->QueryDoubleValue( &dimY ) != TIXML_SUCCESS ) { 
						std::cerr << "ERROR: Couldn't find attribute \"DimensionY\" in Cube node" << std::endl; return NULL;
					}
				}
				else if( currentAttribute->Name() == std::string( "DimensionZ" ) )
				{
					if( currentAttribute->QueryDoubleValue( &dimZ ) != TIXML_SUCCESS ) { 
						std::cerr << "ERROR: Couldn't find attribute \"DimensionZ\" in Cube node" << std::endl; return NULL;
					}
				}
			}

			currentImportTyped->SetLength( ( float )dimX, ( float )dimY, ( float )dimZ ); 
			currentImport = dynamic_cast< VolumeTree::Node* >( currentImportTyped );
		}
	}
	else if( _root->ValueStr() == std::string( "Cylinder" ) )
	{
		double height, radiusX, radiusY; 
		int isPole = 0; 

		if( _root->Type() == TiXmlNode::TINYXML_ELEMENT )
		{
			VolumeTree::CylinderNode *currentImportTyped = new VolumeTree::CylinderNode();

			for( TiXmlAttribute *currentAttribute = _root->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
			{
				if( currentAttribute->Name() == std::string( "RadiusX" ) ) 
				{
					if( currentAttribute->QueryDoubleValue( &radiusX ) != TIXML_SUCCESS ) { 
						std::cerr << "ERROR: Couldn't find attribute \"RadiusX\" in Cylinder node" << std::endl; return NULL;
					}
				}
				else if( currentAttribute->Name() == std::string( "RadiusY" ) )
				{
					if( currentAttribute->QueryDoubleValue( &radiusY ) != TIXML_SUCCESS ) { 
					std::cerr << "ERROR: Couldn't find attribute \"RadiusY\" in Cylinder node" << std::endl; return NULL;
					}
				}
				else if( currentAttribute->Name() == std::string( "Height" ) )
				{
					if( currentAttribute->QueryDoubleValue( &height ) != TIXML_SUCCESS ) { 
						std::cerr << "ERROR: Couldn't find attribute \"Height\" in Cylinder node" << std::endl; return NULL;
					}
				}
				else if( currentAttribute->Name() == std::string( "isPole" ) )
				{
					if( currentAttribute->QueryIntValue( &isPole ) != TIXML_SUCCESS ) {
						std::cerr << "ERROR: Couldn't find attribute \"isPole\" in Cylinder node" << std::endl; return NULL;
					}
				}
			}

			currentImportTyped->SetRadius( ( float )radiusX, ( float )radiusY );
			currentImportTyped->SetLength( ( float )height );
			if( isPole != 0 )
			{
				currentImportTyped->SetPole( true );
			}

			currentImport = dynamic_cast< VolumeTree::Node* >( currentImportTyped );
		}
	
	}
	else if( _root->ValueStr() == std::string( "Sphere" ) )
	{
		double dimX, dimY, dimZ; 
		if( _root->Type() == TiXmlNode::TINYXML_ELEMENT )
		{
			VolumeTree::SphereNode *currentImportTyped = new VolumeTree::SphereNode();

			for( TiXmlAttribute *currentAttribute = _root->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
			{
				if( currentAttribute->Name() == std::string( "RadiusX" ) ) 
				{
					if( currentAttribute->QueryDoubleValue( &dimX ) != TIXML_SUCCESS ) { 
						std::cerr << "ERROR: Couldn't find attribute \"RadiusX\" in Ellipsoid node" << std::endl; return NULL; 
					}
				}
				else if( currentAttribute->Name() == std::string( "RadiusY" ) )
				{
					if( currentAttribute->QueryDoubleValue( &dimY ) != TIXML_SUCCESS ) { 
						std::cerr << "ERROR: Couldn't find attribute \"RadiusY\" in Ellipsoid node" << std::endl; return NULL; 
					}
				}
				else if( currentAttribute->Name() == std::string( "RadiusZ" ) )
				{
					if( currentAttribute->QueryDoubleValue( &dimZ ) != TIXML_SUCCESS ) { 
						std::cerr << "ERROR: Couldn't find attribute \"RadiusZ\" in Ellipsoid node" << std::endl; return NULL; 
					}
				}
			}

			currentImportTyped->SetRadius( ( float )dimX, ( float )dimY, ( float )dimZ );
			currentImport = dynamic_cast< VolumeTree::Node* >( currentImportTyped );
		}
	}
	else if( _root->ValueStr() == std::string( "BlendCSG" ) )
	{
		double a0, a1, a2; 
		VolumeTree::CSGNode::CSGType csgImportType;

		if( _root->Type() == TiXmlNode::TINYXML_ELEMENT )
		{
			VolumeTree::BlendCSGNode *currentImportTyped = new VolumeTree::BlendCSGNode();

			for( TiXmlAttribute *currentAttribute = _root->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
			{
				if( currentAttribute->Name() == std::string( "A0" ) || currentAttribute->Name() == std::string( "a0" ) ) 
				{
					if( currentAttribute->QueryDoubleValue( &a0 ) != TIXML_SUCCESS ) { 
						std::cerr << "ERROR: Couldn't find attribute \"A0\" in BlendCSG node" << std::endl; return NULL; 
					}
				}
				else if( currentAttribute->Name() == std::string( "A1" ) || currentAttribute->Name() == std::string( "a1" ) )
				{
					if( currentAttribute->QueryDoubleValue( &a1 ) != TIXML_SUCCESS ) { 
						std::cerr << "ERROR: Couldn't find attribute \"A1\" in BlendCSG node" << std::endl; return NULL; 
					}
				}
				else if( currentAttribute->Name() == std::string( "A2" ) || currentAttribute->Name() == std::string( "a2" ) )
				{
					if( currentAttribute->QueryDoubleValue( &a2 ) != TIXML_SUCCESS ) { 
						std::cerr << "ERROR: Couldn't find attribute \"A2\" in BlendCSG node" << std::endl; return NULL; 
					}
				}
				else if( currentAttribute->Name() == std::string( "Type" ) || currentAttribute->Name() == std::string( "type" ) )
				{
					std::string csgType = currentAttribute->ValueStr();
					if( csgType == std::string( "Union" ) || csgType == std::string( "union" ) )
						csgImportType = VolumeTree::CSGNode::CSG_UNION;
					else if( csgType == std::string( "Intersection" ) || csgType == std::string( "intersection" ) )
						csgImportType = VolumeTree::CSGNode::CSG_INTERSECTION;
					else if( csgType == std::string( "Subtraction" ) || csgType == std::string( "subtraction" ) )
						csgImportType = VolumeTree::CSGNode::CSG_SUBTRACTION;

					currentImportTyped->SetCSGType( csgImportType );
				}
			}

			currentImportTyped->SetBlendParams( ( float )a0, ( float )a1, ( float )a2 );

			// Recurse children
			if( _root->NoChildren() )
			{
				std::cerr << "WARNING: BlendCSG node without two child nodes, attempting to continue but results are undefined" << std::endl;
			}
			else
			{
				TiXmlElement* currentChild = _root->FirstChildElement();
				currentImportTyped->SetChildA( importFromXML( currentChild ) );
				currentImportTyped->SetChildB( importFromXML( currentChild->NextSiblingElement() ) );
			}

			currentImport = dynamic_cast< VolumeTree::Node* >( currentImportTyped );
		}
	}
	else if( _root->ValueStr() == std::string( "CSG" ) )
	{
		VolumeTree::CSGNode::CSGType csgImportType;

		if( _root->Type() == TiXmlNode::TINYXML_ELEMENT )
		{
			VolumeTree::CSGNode *currentImportTyped = new VolumeTree::CSGNode();

			for( TiXmlAttribute *currentAttribute = _root->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
			{
				if( currentAttribute->Name() == std::string( "Type" ) || currentAttribute->Name() == std::string( "type" ) )
				{
					std::string csgType = currentAttribute->ValueStr();
					if( csgType == std::string( "Union" ) || csgType == std::string( "union" ) )
						csgImportType = VolumeTree::CSGNode::CSG_UNION;
					else if( csgType == std::string( "Intersection" ) || csgType == std::string( "intersection" ) )
						csgImportType = VolumeTree::CSGNode::CSG_INTERSECTION;
					else if( csgType == std::string( "Subtraction" ) || csgType == std::string( "subtraction" ) )
						csgImportType = VolumeTree::CSGNode::CSG_SUBTRACTION;

					currentImportTyped->SetCSGType( csgImportType );
				}
			}

			// Recurse children
			if( _root->NoChildren() )
			{
				std::cerr << "WARNING: CSG node without two child nodes, attempting to continue but results are undefined" << std::endl;
			}
			else
			{
				TiXmlElement* currentChild = _root->FirstChildElement();
				currentImportTyped->SetChildA( importFromXML( currentChild ) );
				currentImportTyped->SetChildB( importFromXML( currentChild->NextSiblingElement() ) );
			}

			currentImport = dynamic_cast< VolumeTree::Node* >( currentImportTyped );
		}
	}
	else if( _root->ValueStr() == std::string( "Transform" ) )
	{
		double tx = 0.0f, ty = 0.0f, tz = 0.0f, 
			   rx = 0.0f, ry = 0.0f, rz = 0.0f, 
			   sx = 1.0f, sy = 1.0f, sz = 1.0f,
		       refrx = 0.0f, refry = 0.0f, refrz = 0.0f, 
			   refsx = 0.0f, refsy = 0.0f, refsz = 0.0f; 

		if( _root->Type() == TiXmlNode::TINYXML_ELEMENT )
		{
			VolumeTree::TransformNode *currentImportTyped = new VolumeTree::TransformNode();
			currentImportTyped->Reset();

			for( TiXmlAttribute *currentAttribute = _root->FirstAttribute(); currentAttribute != NULL; currentAttribute = currentAttribute->Next() )
			{
				if( currentAttribute->Name() == std::string( "transX" ) ) 
				{
					if( currentAttribute->QueryDoubleValue( &tx ) != TIXML_SUCCESS ) { 
						std::cerr << "ERROR: Couldn't find attribute \"transX\" in Transform node" << std::endl; return NULL; 
					}
				}
				else if( currentAttribute->Name() == std::string( "transY" ) )
				{
					if( currentAttribute->QueryDoubleValue( &ty ) != TIXML_SUCCESS ) { 
						std::cerr << "ERROR: Couldn't find attribute \"transY\" in Transform node" << std::endl; return NULL; 
					}
				}
				else if( currentAttribute->Name() == std::string( "transZ" ) )
				{
					if( currentAttribute->QueryDoubleValue( &tz ) != TIXML_SUCCESS ) { 
						std::cerr << "ERROR: Couldn't find attribute \"transZ\" in Transform node" << std::endl; return NULL; 
					}
				}
				else if( currentAttribute->Name() == std::string( "angleX" ) )
				{
					if( currentAttribute->QueryDoubleValue( &rx ) != TIXML_SUCCESS ) { 
						std::cerr << "ERROR: Couldn't find attribute \"angleX\" in Transform node" << std::endl; return NULL; 
					}
				}
				else if( currentAttribute->Name() == std::string( "angleY" ) )
				{
					if( currentAttribute->QueryDoubleValue( &ry ) != TIXML_SUCCESS ) { 
						std::cerr << "ERROR: Couldn't find attribute \"angleY\" in Transform node" << std::endl; return NULL; 
					}
				}
				else if( currentAttribute->Name() == std::string( "angleZ" ) )
				{
					if( currentAttribute->QueryDoubleValue( &rz ) != TIXML_SUCCESS ) { 
						std::cerr << "ERROR: Couldn't find attribute \"angleZ\" in Transform node" << std::endl; return NULL; 
					}
				}
				else if( currentAttribute->Name() == std::string( "scaleX" ) )
				{
					if( currentAttribute->QueryDoubleValue( &sx ) != TIXML_SUCCESS ) { 
						std::cerr << "ERROR: Couldn't find attribute \"scaleX\" in Transform node" << std::endl; return NULL; 
					}
				}
				else if( currentAttribute->Name() == std::string( "scaleY" ) )
				{
					if( currentAttribute->QueryDoubleValue( &sy ) != TIXML_SUCCESS ) { 
						std::cerr << "ERROR: Couldn't find attribute \"scaleY\" in Transform node" << std::endl; return NULL; 
					}
				}
				else if( currentAttribute->Name() == std::string( "scaleZ" ) )
				{
					if( currentAttribute->QueryDoubleValue( &sz ) != TIXML_SUCCESS ) { 
						std::cerr << "ERROR: Couldn't find attribute \"scaleZ\" in Transform node" << std::endl; return NULL; 
					}
				}
			}

			currentImportTyped->SetTransformParams( ( float )tx, ( float )ty, ( float )tz, 
													( float )rx, ( float )ry, ( float )rz, 
													( float )sx, ( float )sy, ( float )sz, 
													( float )refrx, ( float )refry, ( float )refrz, 
													( float )refsx, ( float )refsy, ( float )refsz );

			// Recursively add child
			if( _root->NoChildren() ) {
				std::cerr << "WARNING: Transform node without child node, attempting to continue but results are undefined" << std::endl;
			}
			else { 
				currentImportTyped->SetChild( importFromXML( _root->FirstChildElement() ) ); 
			}

			currentImport = dynamic_cast< VolumeTree::Node* >( currentImportTyped );
		}
	}

	// In case we have an unknown node type
	if( currentImport == NULL )
	{
		std::cerr << "ERROR: VolumeTree::Tree::importFromXML could not resolve current node, results undefined" << std::endl;
		return NULL;
	}

	return currentImport;
} 
	

//----------------------------------------------------------------------------------

void VolumeTree::Tree::SaveXML( std::string _filename )
{
	TiXmlDocument doc;
	TiXmlDeclaration *decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );

	TiXmlElement *root = new TiXmlElement( "Model" );
	doc.LinkEndChild( root );

	TiXmlComment *comment = new TiXmlComment( "This is the internal format used to save and load models built in SHIVA" );
	root->LinkEndChild( comment );

	exportToXML( m_rootNode, root ); 
	doc.SaveFile( _filename );
}


//----------------------------------------------------------------------------------

void VolumeTree::Tree::exportToXML( Node *_currentNode, TiXmlElement *_root )
{
	TiXmlElement *newRoot;
	
	// Create corresponding internal node and fill with data
	if( _currentNode != NULL )
	{
		std::string nodeTypeStr = _currentNode->GetNodeType();
		
		if( nodeTypeStr == "ConeNode" )
		{
			VolumeTree::ConeNode *currentTyped = dynamic_cast< VolumeTree::ConeNode * >( _currentNode );

			if( currentTyped != NULL )
			{
				newRoot = new TiXmlElement( "Cone" );
				_root->LinkEndChild( newRoot );

				newRoot->SetDoubleAttribute( "Radius", currentTyped->GetRadius() );
				newRoot->SetDoubleAttribute( "Height", currentTyped->GetLength() );
			}
			else {
				std::cerr << "ERROR: VolumeTree::Tree::exportToXML node string type does not match class type: " << nodeTypeStr << ", results undefined" << std::endl;
				return;
			}
		}
		else if( nodeTypeStr == "CubeNode" )
		{
			VolumeTree::CubeNode *currentTyped = dynamic_cast< VolumeTree::CubeNode * >( _currentNode );

			if( currentTyped != NULL )
			{
				newRoot = new TiXmlElement( "Cube" );
				_root->LinkEndChild( newRoot );

				newRoot->SetDoubleAttribute( "DimensionX", currentTyped->GetLengthX() );
				newRoot->SetDoubleAttribute( "DimensionY", currentTyped->GetLengthY() );
				newRoot->SetDoubleAttribute( "DimensionZ", currentTyped->GetLengthZ() );

			}
			else {
				std::cerr << "ERROR: VolumeTree::Tree::exportToXML node string type does not match class type: " << nodeTypeStr << ", results undefined" << std::endl;
				return;
			}
		}
		else if( nodeTypeStr == "CylinderNode" )
		{
			VolumeTree::CylinderNode *currentTyped = dynamic_cast< VolumeTree::CylinderNode * >( _currentNode );

			if( currentTyped != NULL )
			{
				newRoot = new TiXmlElement( "Cylinder" );
				_root->LinkEndChild( newRoot );

				newRoot->SetDoubleAttribute( "RadiusX", currentTyped->GetRadiusX() );
				newRoot->SetDoubleAttribute( "RadiusY", currentTyped->GetRadiusY() );
				newRoot->SetDoubleAttribute( "Height",  currentTyped->GetLength() );
				if( currentTyped->isPole() )
				{
					newRoot->SetAttribute( "isPole", 1 );
				}

			}
			else {
				std::cerr << "ERROR: VolumeTree::Tree::exportToXML node string type does not match class type: " << nodeTypeStr << ", results undefined" << std::endl;
				return;
			}
		}
		else if( nodeTypeStr == "SphereNode" )
		{
			VolumeTree::SphereNode *currentTyped = dynamic_cast< VolumeTree::SphereNode * >( _currentNode );

			if( currentTyped != NULL )
			{
				newRoot = new TiXmlElement( "Sphere" );
				_root->LinkEndChild( newRoot );

				newRoot->SetDoubleAttribute( "RadiusX", currentTyped->GetRadiusX() );
				newRoot->SetDoubleAttribute( "RadiusY", currentTyped->GetRadiusY() );
				newRoot->SetDoubleAttribute( "RadiusZ",  currentTyped->GetRadiusZ() );
			}
			else {
				std::cerr << "ERROR: VolumeTree::Tree::exportToXML node string type does not match class type: " << nodeTypeStr << ", results undefined" << std::endl;
				return;
			}
		}
		else if( nodeTypeStr == "BlendCSGNode" )
		{
			VolumeTree::BlendCSGNode *currentTyped = dynamic_cast< VolumeTree::BlendCSGNode * >( _currentNode );

			if( currentTyped != NULL )
			{
				newRoot = new TiXmlElement( "BlendCSG" );
				_root->LinkEndChild( newRoot );

				float a0,a1,a2;
				currentTyped->GetBlendParams( a0, a1, a2 );
				newRoot->SetDoubleAttribute( "A0", a0 );
				newRoot->SetDoubleAttribute( "A1", a1 );
				newRoot->SetDoubleAttribute( "A2", a2 );
				
				CSGNode::CSGType csgType = currentTyped->GetCSGType();

				if( csgType == CSGNode::CSG_UNION ) {
					newRoot->SetAttribute( "Type", "Union" );
				}
				else if( csgType == CSGNode::CSG_INTERSECTION ) {
					newRoot->SetAttribute( "Type", "Intersection" );
				}
				else if( csgType == CSGNode::CSG_SUBTRACTION ) {
					newRoot->SetAttribute( "Type", "Subtraction" );
				}
			}
			else {
				std::cerr << "ERROR: VolumeTree::Tree::exportToXML node string type does not match class type: " << nodeTypeStr << ", results undefined" << std::endl;
				return;
			}
		}
		else if( nodeTypeStr == "CSGNode" )
		{
			VolumeTree::CSGNode *currentTyped = dynamic_cast< VolumeTree::CSGNode * >( _currentNode );

			if( currentTyped != NULL )
			{
				newRoot = new TiXmlElement( "CSG" );
				_root->LinkEndChild( newRoot );

				CSGNode::CSGType csgType = currentTyped->GetCSGType();

				if( csgType == CSGNode::CSG_UNION ) {
					newRoot->SetAttribute( "Type", "Union" );
				}
				else if( csgType == CSGNode::CSG_INTERSECTION ) {
					newRoot->SetAttribute( "Type", "Intersection" );
				}
				else if( csgType == CSGNode::CSG_SUBTRACTION ) {
					newRoot->SetAttribute( "Type", "Subtraction" );
				}
			}
			else {
				std::cerr << "ERROR: VolumeTree::Tree::exportToXML node string type does not match class type: " << nodeTypeStr << ", results undefined" << std::endl;
				return;
			}
		}
		else if( nodeTypeStr == "TransformNode" )
		{
			VolumeTree::TransformNode *currentTyped = dynamic_cast< VolumeTree::TransformNode * >( _currentNode );

			if( currentTyped != NULL )
			{
				newRoot = new TiXmlElement( "Transform" );
				_root->LinkEndChild( newRoot );
				
				float tx = 0.0f, ty = 0.0f, tz = 0.0f, //translate, 0 by default
				rx = 0.0f, ry = 0.0f, rz = 0.0f, //rotate as euler angles in radians, 0 by default
				sx = 1.0f, sy = 1.0f, sz = 1.0f, //scale, 1 by default
				refrx = 0.0f, refry = 0.0f, refrz = 0.0f, //reference point for rotation, 0 by default
				refsx = 0.0f, refsy = 0.0f, refsz = 0.0f; //reference point for scale, 0 by default


				currentTyped->GetTransformParams(	tx, ty, tz, //translate, 0 by default
													rx, ry, rz, //rotate as euler angles in radians, 0 by default
													sx, sy, sz, //scale, 1 by default
													refrx, refry, refrz, //reference point for rotation, 0 by default
													refsx, refsy, refsz); //reference point for scale, 0 by default

				newRoot->SetDoubleAttribute( "transX", -tx ); newRoot->SetDoubleAttribute( "transY", -ty ); newRoot->SetDoubleAttribute( "transZ", -tz );
				newRoot->SetDoubleAttribute( "angleX", -rx ); newRoot->SetDoubleAttribute( "angleY", -ry ); newRoot->SetDoubleAttribute( "angleZ", -rz );
				newRoot->SetDoubleAttribute( "scaleX", sx );  newRoot->SetDoubleAttribute( "scaleY", sy );  newRoot->SetDoubleAttribute( "scaleZ", sz );

			}
			else {
				std::cerr << "ERROR: VolumeTree::Tree::exportToXML node string type does not match class type: " << nodeTypeStr << ", results undefined" << std::endl;
				return;
			}
		}
	}
	else {
		std::cerr << "ERROR: VolumeTree::Tree::exportToXML given NULL root, results undefined." << std::endl;
		return;
	}

	// Recurse children
	for( Node *currentChild = _currentNode->GetFirstChild(); currentChild != NULL; currentChild = _currentNode->GetNextChild( currentChild ) )
	{
		 exportToXML( currentChild, newRoot );
	}
}


//----------------------------------------------------------------------------------

bool VolumeTree::Tree::Save( std::string _filename )
{

	if( m_rootNode == NULL )
	{
		return false;
	}

	std::cout << "INFO: VolumeTree::Tree Saving to Vol file: " << _filename << std::endl;

	// Build up tree
	totemio::TotemNode *exportRootNode = BuildExportNode( m_rootNode );

	// Export tree
	bool retVal = saveVol( _filename.c_str(), exportRootNode );

	//delete exportRootNode;
	// TODO: freeing the tree causes a crash
	//totemio::freeTree( exportRootNode );

	return retVal;
}

//----------------------------------------------------------------------------------

void VolumeTree::Tree::CalcBoundingBox()
{
	if( m_rootNode != NULL )
	{
		m_rootNode->GetBounds( &( m_bboxBoundsMin[ 0 ] ), &( m_bboxBoundsMax[ 0 ] ), &( m_bboxBoundsMin[ 1 ] ), &( m_bboxBoundsMax[ 1 ] ), &( m_bboxBoundsMin[ 2 ] ), &( m_bboxBoundsMax[ 2 ] ) );
		m_rootNode->GetBoundSizes( &( m_bboxExtents[ 0 ] ), &( m_bboxExtents[ 1 ] ), &( m_bboxExtents[ 2 ] ) );

		for( unsigned int i = 0; i < 3; i++ )
		{
			m_bboxCentre[ i ] = m_bboxBoundsMin[ i ] + ( 0.5f * m_bboxExtents[ i ] );
		}
	}
	else
	{
		for( unsigned int i = 0; i < 3; i++ )
		{
			m_bboxCentre[ i ] = m_bboxBoundsMin[ i ] = m_bboxBoundsMax[ i ] = m_bboxExtents[ i ] = 0.0f;
		}
	}
}

//----------------------------------------------------------------------------------

void VolumeTree::Tree::GetBoundingBox( float *_minvals, float *_maxvals )
{
	for(unsigned int i = 0; i < 3; i++ )
	{
		_minvals[ i ] = m_bboxBoundsMin[ i ];
		_maxvals[ i ] = m_bboxBoundsMax[ i ];
	}
}

//----------------------------------------------------------------------------------

void VolumeTree::Tree::GetBoundingBoxSize( float *_boxSize )
{
	for(unsigned int i = 0; i < 3; i++ )
	{
		_boxSize[ i ] = m_bboxExtents[ i ];
	}
}

//----------------------------------------------------------------------------------

void VolumeTree::Tree::GetBoundingBoxCentre( float *_boxCentre )
{
	for(unsigned int i = 0; i < 3; i++ )
	{
		_boxCentre[ i ] = m_bboxCentre[ i ];
	}
}

//----------------------------------------------------------------------------------

float VolumeTree::Tree::GetBoundingBoxMaxDim()
{
	float bboxMaxDim = m_bboxExtents[ 0 ];
	for( unsigned int i = 1; i < 3; i++ )
	{
		if( m_bboxExtents[ i ] > bboxMaxDim )
		{
			bboxMaxDim = m_bboxExtents[ i ];
		}
	}
	return bboxMaxDim;
}

//----------------------------------------------------------------------------------

void VolumeTree::Tree::DrawBBoxes( cml::matrix44f_c &_proj, cml::matrix44f_c &_mv )
{
	if( m_rootNode != NULL )
	{
		m_rootNode->DrawBBoxes( _proj, _mv );
	}
}

//----------------------------------------------------------------------------------

float VolumeTree::Tree::GetFunctionValue( float _x, float _y, float _z )
{
	if( m_rootNode != NULL ) {
		return m_rootNode->GetFunctionValue( _x, _y, _z );
	}
	return 0.0f;
}

//----------------------------------------------------------------------------------

std::string VolumeTree::Tree::GetCachedFunctionGLSLString()
{
	std::stringstream functionString;
	functionString << "float GetField(vec3 samplePosition) { return ";

	if( m_rootNode != NULL )
	{
		functionString << m_rootNode->GetCachedFunctionGLSLString( "samplePosition" );
	}
	else
	{
		functionString << "0.0f";
	}
	functionString << ";}";

	return functionString.str();
}

//----------------------------------------------------------------------------------

std::string VolumeTree::Tree::GetFunctionGLSLString()
{
	std::stringstream functionString;
	functionString << "float GetField(vec3 samplePosition) { return ";

	if( m_rootNode != NULL )
	{
		functionString << m_rootNode->GetFunctionGLSLString( false, "samplePosition" );
	}
	else
	{
		functionString << "0";
	}
	functionString << ";}";

	return functionString.str();
}

//----------------------------------------------------------------------------------

void VolumeTree::Tree::BuildCaches( CachingPolicy *_policy, GLSLRenderer *_renderer )
{
	if( m_rootNode != NULL )
	{
		if( _policy != NULL )
		{
			/// The policy decides which nodes should be cached
			_policy->Process( m_rootNode, _renderer );
		}
		/// This will build the actual caches
		m_rootNode->BuildCaches( _renderer );
	}
}

//----------------------------------------------------------------------------------

int VolumeTree::Tree::GetNumNodes()
{
	if( m_rootNode != NULL )
	{
		int count = 0, depth = 0;
		Node *result = m_rootNode->RecursiveGetNode( -1, count, depth );
		#ifdef _DEBUG
			std::cout << "INFO: VolumeTree::Tree::GetNumNodes(): "<< count << " nodes" << std::endl;
		#endif
		return count + 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------

VolumeTree::Node* VolumeTree::Tree::GetNode( int _index )
{
	if( m_rootNode != NULL )
	{
		int count = 0, depth = 0;
		Node *result = m_rootNode->RecursiveGetNode( _index, count, depth );
		if( result == NULL )
		{
			std::cerr << "WARNING: VolumeTree::Tree::GetNode(" << _index << ") not found!" << std::endl;
		}
		return result;
	}
	return NULL;
}

//----------------------------------------------------------------------------------

int VolumeTree::Tree::GetNodeDepth( int _index )
{
	if( m_rootNode != NULL )
	{
		int count = 0, depth = 0;
		Node *result = m_rootNode->RecursiveGetNode( _index, count, depth );
		return depth;
	}
	return 0;
}

//----------------------------------------------------------------------------------

void VolumeTree::Tree::UpdateParameters( GLSLRenderer *_renderer )
{
	if( m_rootNode != NULL )
	{
		m_rootNode->UpdateParameters( _renderer );
	}
}

//----------------------------------------------------------------------------------

VolumeTree::Node* VolumeTree::Tree::BuildImportNode( totemio::TotemNode *_currentNode )
{
	if( _currentNode == NULL )
	{
		std::cerr << "ERROR: VolumeTree::Tree::BuildImportNode given NULL root, results undefined" << std::endl;
		return NULL;
	}

	// Create corresponding internal node and fill with data
	VolumeTree::Node *currentImport = NULL;
	if( _currentNode->getTypeID() == CONE_NODE_ID )
	{
		totemio::ConeNode *currentTyped = dynamic_cast< totemio::ConeNode * >( _currentNode );

		if( currentTyped != NULL )
		{
			float radius, height;
			currentTyped->getConeParams( radius, height );
			VolumeTree::ConeNode *currentImportTyped = new VolumeTree::ConeNode( height, radius );

			currentImport = ( VolumeTree::Node* ) currentImportTyped;
		}
		else
		{
			std::cerr << "ERROR: VolumeTree::Tree::BuildImportNode node ID type does not match class type: cone, results undefined" << std::endl;
			return NULL;
		}
	}
	else if( _currentNode->getTypeID() == BOX_NODE_ID )
	{
		totemio::BoxNode *currentTyped = dynamic_cast< totemio::BoxNode * >( _currentNode );

		if( currentTyped != NULL )
		{
			float dimx, dimy, dimz;
			currentTyped->getBoxParams( dimx, dimy, dimz );
			VolumeTree::CubeNode *currentImportTyped = new VolumeTree::CubeNode( dimx, dimy, dimz );

			currentImport = ( VolumeTree::Node* ) currentImportTyped;
		}
		else
		{
			std::cerr << "ERROR: VolumeTree::Tree::BuildImportNode node ID type does not match class type: cube (box), results undefined" << std::endl;
			return NULL;
		}
	}
	else if( _currentNode->getTypeID() == CYLINDER_NODE_ID )
	{
		totemio::CylinderNode *currentTyped = dynamic_cast< totemio::CylinderNode * >( _currentNode );

		if( currentTyped != NULL )
		{
			float radiusX, radiusY, height;
			currentTyped->getCylinderParams( radiusX, radiusY, height );
			VolumeTree::CylinderNode *currentImportTyped = new VolumeTree::CylinderNode( height, radiusX, radiusY );

			currentImport = ( VolumeTree::Node* ) currentImportTyped;
		}
		else
		{
			std::cerr << "ERROR: VolumeTree::Tree::BuildImportNode node ID type does not match class type: cylinder, results undefined" << std::endl;
			return NULL;
		}
	}
	else if( _currentNode->getTypeID() == ELLIPSOID_NODE_ID )
	{
		totemio::EllipsoidNode *currentTyped = dynamic_cast< totemio::EllipsoidNode * >( _currentNode );

		if( currentTyped != NULL )
		{
			float dimx, dimy, dimz;
			currentTyped->getEllipsoidParams( dimx, dimy, dimz );
			VolumeTree::SphereNode *currentImportTyped = new VolumeTree::SphereNode( dimx, dimy, dimz );

			currentImport = ( VolumeTree::Node* ) currentImportTyped;
		}
		else
		{
			std::cerr << "ERROR: VolumeTree::Tree::BuildImportNode node ID type does not match class type: sphere (ellipsoid), results undefined" << std::endl;
			return NULL;
		}
	}
	else if( _currentNode->getTypeID() == TORUS_NODE_ID )
	{
		totemio::TorusNode *currentTyped = dynamic_cast< totemio::TorusNode * >( _currentNode );

		if( currentTyped != NULL )
		{
			float radius_big, radius_small;
			currentTyped->getTorusParams( radius_big, radius_small );
			VolumeTree::TorusNode *currentImportTyped = new VolumeTree::TorusNode( radius_big, radius_small );

			currentImport = ( VolumeTree::Node* ) currentImportTyped;
		}
		else
		{
			std::cerr << "ERROR: VolumeTree::Tree::BuildImportNode node ID type does not match class type: torus, results undefined" << std::endl;
			return NULL;
		}
	}
	else if( _currentNode->getTypeID() == CACHE_NODE_ID )
	{
		totemio::CacheNode *currentTyped = dynamic_cast< totemio::CacheNode * >( _currentNode );

		if( currentTyped != NULL )
		{
			VolumeTree::VolCacheNode *currentImportTyped = new VolumeTree::VolCacheNode( currentTyped );
			
			currentImport = ( VolumeTree::Node* ) currentImportTyped;
		}
		else
		{
			std::cerr << "ERROR: VolumeTree::Tree::BuildImportNode node ID type does not match class type: cache node, results undefined" << std::endl;
			return NULL;
		}
	}
	else if( ( _currentNode->getTypeID() == CSG_NODE_ID ) || ( _currentNode->getTypeID() == BLENDCSG_NODE_ID ) )
	{
		totemio::CSGNode *currentTyped = dynamic_cast< totemio::CSGNode * >( _currentNode );

		if( currentTyped != NULL )
		{
			VolumeTree::CSGNode *currentImportTyped;
			
			if( _currentNode->getTypeID() == BLENDCSG_NODE_ID )
			{
				totemio::BlendCSGNode *blendCurrentTyped = dynamic_cast< totemio::BlendCSGNode * >( _currentNode );
				VolumeTree::BlendCSGNode *blendCurrentImportTyped = new VolumeTree::BlendCSGNode();

				if( blendCurrentTyped != NULL )
				{
					float a0, a1, a2;
					blendCurrentTyped->getBlendParams( a0, a1, a2 );
					blendCurrentImportTyped->SetBlendParams( a0, a1, a2 );

					currentImportTyped = ( VolumeTree::CSGNode* ) blendCurrentImportTyped;
				}
				else
				{
					std::cerr << "ERROR: VolumeTree::Tree::BuildImportNode node ID type does not match class type: BlendCSG, results undefined" << std::endl;
					return NULL;
				}
			}
			else
			{
				currentImportTyped = new VolumeTree::CSGNode();	
			}

			VolumeTree::CSGNode::CSGType csgImportType;
			int csgType;
			currentTyped->getCSGType( csgType );

			if( csgType == totemio::CSGNode::CSG_TYPE_UNION )
				csgImportType = VolumeTree::CSGNode::CSG_UNION;
			else if( csgType == totemio::CSGNode::CSG_TYPE_INTERSECTION )
				csgImportType = VolumeTree::CSGNode::CSG_INTERSECTION;
			else if( csgType == totemio::CSGNode::CSG_TYPE_SUBTRACTION )
				csgImportType = VolumeTree::CSGNode::CSG_SUBTRACTION;
			currentImportTyped->SetCSGType( csgImportType );

			currentImport = ( VolumeTree::Node* ) currentImportTyped;

			// Recurse children
			unsigned int numChildren = _currentNode->getChildrenSize();
			if( numChildren == 2 )
			{
				currentImportTyped->SetChildA( BuildImportNode( _currentNode->getChild( 0 ) ) );
				currentImportTyped->SetChildB( BuildImportNode( _currentNode->getChild( 1 ) ) );
			}
			else
			{
				std::cerr << "WARNING: VolumeTree::Tree::BuildImportNode CSG node without two child nodes, attempting to continue but results are undefined" << std::endl;
			}
		}
		else
		{
			std::cerr << "ERROR: VolumeTree::Tree::BuildImportNode node ID type does not match class type: CSG, results undefined" << std::endl;
			return NULL;
		}
	}
	else if( _currentNode->getTypeID() == TRANSFORM_NODE_ID )
	{
		totemio::TransformNode *currentTyped = dynamic_cast< totemio::TransformNode * >( _currentNode );

		if( currentTyped != NULL )
		{
			float tx = 0.0f, ty = 0.0f, tz = 0.0f, //translate, 0 by default
			rx = 0.0f, ry = 0.0f, rz = 0.0f, //rotate as euler angles in radians, 0 by default
			sx = 1.0f, sy = 1.0f, sz = 1.0f, //scale, 1 by default
			refrx = 0.0f, refry = 0.0f, refrz = 0.0f, //reference point for rotation, 0 by default
			refsx = 0.0f, refsy = 0.0f, refsz = 0.0f; //reference point for scale, 0 by default

			currentTyped->getTransformParams( tx, ty, tz, //translate, 0 by default
											  rx, ry, rz, //rotate as euler angles in radians, 0 by default
											  sx, sy, sz, //scale, 1 by default
											  refrx, refry, refrz, //reference point for rotation, 0 by default
											  refsx, refsy, refsz ); //reference point for scale, 0 by default

			VolumeTree::TransformNode *currentImportTyped = new VolumeTree::TransformNode();

			// TODO: complete this (need to complete the transform class first)
			//currentImportTyped->SetTranslate(tx,ty,tz);
			
			currentImportTyped->SetTransformParams( tx, ty, tz, //translate, 0 by default
											        rx, ry, rz, //rotate as euler angles in radians, 0 by default
											        sx, sy, sz, //scale, 1 by default
													refrx, refry, refrz, //reference point for rotation, 0 by default
													refsx, refsy, refsz); //reference point for scale, 0 by default

			currentImport = ( VolumeTree::Node* ) currentImportTyped;

			// Recursively add child
			if( _currentNode->getChildrenSize() == 1 )
			{
				currentImportTyped->SetChild( BuildImportNode( _currentNode->getChild( 0 ) ) );
			}
			else
			{
				std::cerr << "WARNING: VolumeTree::Tree::BuildImportNode Transform node without a child node, attempting to continue but results are undefined" << std::endl;
			}
		}
		else
		{
			std::cerr << "ERROR: VolumeTree::Tree::BuildImportNode node ID type does not match class type: transform node, results undefined" << std::endl;
			return NULL;
		}
	}

		// In case we have an unknown node type, don't know what to do
	if( currentImport == NULL )
	{
		std::cerr << "ERROR: VolumeTree::Tree::BuildImportNode could not resolve current node, results undefined" << std::endl;
		return NULL;
	}

	return currentImport;
} 

//----------------------------------------------------------------------------------

totemio::TotemNode* VolumeTree::Tree::BuildExportNode( Node *_currentNode )
{
	if( _currentNode == NULL )
	{
		std::cerr << "ERROR: VolumeTree::Tree::BuildExportNode given NULL root, results undefined" << std::endl;
		return NULL;
	}

	// Create corresponding totemio node and fill with data
	totemio::TotemNode *currentExport = NULL;
	std::string nodeTypeStr = _currentNode->GetNodeType();
	if( nodeTypeStr == "ConeNode" )
	{
		VolumeTree::ConeNode *currentTyped = dynamic_cast< VolumeTree::ConeNode * >( _currentNode );

		if( currentTyped != NULL )
		{
			totemio::ConeNode *currentExportTyped = new totemio::ConeNode( GetExportNodeID( _currentNode ).c_str() );
			currentExportTyped->setConeParams( currentTyped->GetRadius(), currentTyped->GetLength() );
			currentExport = ( totemio::TotemNode* ) currentExportTyped;
		}
		else
		{
			std::cerr << "ERROR: VolumeTree::Tree::BuildExportNode node string type does not match class type: " << nodeTypeStr << ", results undefined" << std::endl;
			return NULL;
		}
	}
	else if( nodeTypeStr == "CubeNode" )
	{
		VolumeTree::CubeNode *currentTyped = dynamic_cast< VolumeTree::CubeNode * >( _currentNode );

		if( currentTyped != NULL )
		{
			totemio::BoxNode *currentExportTyped = new totemio::BoxNode( GetExportNodeID( _currentNode ).c_str() );
			currentExportTyped->setBoxParams( currentTyped->GetLengthX(), currentTyped->GetLengthY(), currentTyped->GetLengthZ() );
			currentExport = ( totemio::TotemNode* ) currentExportTyped;
		}
		else
		{
			std::cerr << "ERROR: VolumeTree::Tree::BuildExportNode node string type does not match class type: " << nodeTypeStr << ", results undefined" << std::endl;
			return NULL;
		}
	}
	else if( nodeTypeStr == "CylinderNode" )
	{
		VolumeTree::CylinderNode *currentTyped = dynamic_cast< VolumeTree::CylinderNode * >( _currentNode );

		if( currentTyped != NULL )
		{
			totemio::CylinderNode *currentExportTyped = new totemio::CylinderNode( GetExportNodeID( _currentNode ).c_str() );
			currentExportTyped->setCylinderParams( currentTyped->GetRadiusX(), currentTyped->GetRadiusY(), currentTyped->GetLength() );
			currentExport = ( totemio::TotemNode* ) currentExportTyped;
		}
		else
		{
			std::cerr << "ERROR: VolumeTree::Tree::BuildExportNode node string type does not match class type: " << nodeTypeStr << ", results undefined" << std::endl;
			return NULL;
		}
	}
	else if( nodeTypeStr == "SphereNode" )
	{
		VolumeTree::SphereNode *currentTyped = dynamic_cast< VolumeTree::SphereNode * >( _currentNode );

		if( currentTyped != NULL )
		{
			totemio::EllipsoidNode *currentExportTyped = new totemio::EllipsoidNode( GetExportNodeID( _currentNode ).c_str() );
			currentExportTyped->setEllipsoidParams( currentTyped->GetRadiusX(), currentTyped->GetRadiusY(), currentTyped->GetRadiusZ() );
			currentExport = ( totemio::TotemNode* ) currentExportTyped;
		}
		else
		{
			std::cerr << "ERROR: VolumeTree::Tree::BuildExportNode node string type does not match class type: " << nodeTypeStr << ", results undefined" << std::endl;
			return NULL;
		}
	}
	else if( nodeTypeStr == "TorusNode" )
	{
		VolumeTree::TorusNode *currentTyped = dynamic_cast< VolumeTree::TorusNode * >( _currentNode );

		if( currentTyped != NULL )
		{
			totemio::TorusNode *currentExportTyped = new totemio::TorusNode( GetExportNodeID( _currentNode ).c_str() );
			currentExportTyped->setTorusParams( currentTyped->GetSweepradius(), currentTyped->GetCircleRadius() );
			currentExport = ( totemio::TotemNode* ) currentExportTyped;
		}
		else
		{
			std::cerr << "ERROR: VolumeTree::Tree::BuildExportNode node string type does not match class type: " << nodeTypeStr << ", results undefined" << std::endl;
			return NULL;
		}
	}
	else if( nodeTypeStr == "BlendCSGNode" )
	{
		VolumeTree::BlendCSGNode *currentTyped = dynamic_cast< VolumeTree::BlendCSGNode * >( _currentNode );

		if( currentTyped != NULL )
		{
			totemio::BlendCSGNode *currentExportTyped = new totemio::BlendCSGNode( GetExportNodeID( _currentNode ).c_str() );

			float a0,a1,a2;
			currentTyped->GetBlendParams( a0, a1, a2 );
			currentExportTyped->setBlendParams( a0, a1, a2 );

			CSGNode::CSGType csgType = currentTyped->GetCSGType();
			totemio::CSGNode::CSG_TYPE csgExportType;
			if( csgType == CSGNode::CSG_UNION )
				csgExportType = totemio::CSGNode::CSG_TYPE_UNION;
			else if( csgType == CSGNode::CSG_INTERSECTION )
				csgExportType = totemio::CSGNode::CSG_TYPE_INTERSECTION;
			else if( csgType == CSGNode::CSG_SUBTRACTION )
				csgExportType = totemio::CSGNode::CSG_TYPE_SUBTRACTION;
			currentExportTyped->setCSGType( csgExportType );

			currentExport = ( totemio::TotemNode* ) currentExportTyped;
		}
		else
		{
			std::cerr << "ERROR: VolumeTree::Tree::BuildExportNode node string type does not match class type: " << nodeTypeStr << ", results undefined" << std::endl;
			return NULL;
		}
	}
	else if( nodeTypeStr == "CSGNode" )
	{
		VolumeTree::CSGNode *currentTyped = dynamic_cast< VolumeTree::CSGNode * >( _currentNode );

		if( currentTyped != NULL )
		{
			totemio::CSGNode *currentExportTyped = new totemio::CSGNode( GetExportNodeID( _currentNode ).c_str() );

			CSGNode::CSGType csgType = currentTyped->GetCSGType();
			totemio::CSGNode::CSG_TYPE csgExportType;
			if( csgType == CSGNode::CSG_UNION )
				csgExportType = totemio::CSGNode::CSG_TYPE_UNION;
			else if( csgType == CSGNode::CSG_INTERSECTION )
				csgExportType = totemio::CSGNode::CSG_TYPE_INTERSECTION;
			else if( csgType == CSGNode::CSG_SUBTRACTION )
				csgExportType = totemio::CSGNode::CSG_TYPE_SUBTRACTION;
			currentExportTyped->setCSGType( csgExportType );

			currentExport = ( totemio::TotemNode* ) currentExportTyped;
		}
		else
		{
			std::cerr << "ERROR: VolumeTree::Tree::BuildExportNode node string type does not match class type: " << nodeTypeStr << ", results undefined" << std::endl;
			return NULL;
		}
	}
	else if( nodeTypeStr == "TransformNode" )
	{
		VolumeTree::TransformNode *currentTyped = dynamic_cast< VolumeTree::TransformNode * >( _currentNode );

		if( currentTyped != NULL )
		{
			totemio::TransformNode *currentExportTyped = new totemio::TransformNode( GetExportNodeID( _currentNode ).c_str() );

			//currentExportTyped->setTransformMatrix( cml::matrix44f_c().identity().data() );

			float tx = 0.0f, ty = 0.0f, tz = 0.0f, //translate, 0 by default
			rx = 0.0f, ry = 0.0f, rz = 0.0f, //rotate as euler angles in radians, 0 by default
			sx = 1.0f, sy = 1.0f, sz = 1.0f, //scale, 1 by default
			refrx = 0.0f, refry = 0.0f, refrz = 0.0f, //reference point for rotation, 0 by default
			refsx = 0.0f, refsy = 0.0f, refsz = 0.0f; //reference point for scale, 0 by default


			currentTyped->GetTransformParams(	tx, ty, tz, //translate, 0 by default
												rx, ry, rz, //rotate as euler angles in radians, 0 by default
												sx, sy, sz, //scale, 1 by default
												refrx, refry, refrz, //reference point for rotation, 0 by default
												refsx, refsy, refsz); //reference point for scale, 0 by default

			/*
			bool translationSet, rotationSet, scaleSet;
			if( currentTyped->GetTransformTypes(translationSet, rotationSet, scaleSet) )
			{
				// Need to send whole matrix
				//currentExportTyped->setTransformMatrix( cml::matrix44f_c( currentTyped->GetTransformMatrix() ).transpose().data() );
			}

			if( translationSet )
			{
				currentTyped->GetTranslate(tx,ty,tz);

				std::cout<<"INFO: ******** Translate node values: "<<tx<<" "<<ty<<" "<<tz<<std::endl;

				// Oleg todo:
				//currentExportTyped->setTranslate(x,y,z);
			}
			if( rotationSet )
			{
				
			}
			if( scaleSet )
			{

			}
			*/

			currentExportTyped->setTransformParams(	-tx, -ty, -tz, //translate, 0 by default
													-rx, -ry, -rz, //rotate as euler angles in radians, 0 by default
													sx, sy, sz, //scale, 1 by default
													refrx, refry, refrz, //reference point for rotation, 0 by default
													refsx, refsy, refsz); //reference point for scale, 0 by default
			

			// Temporary code for debugging
			/*
			{
				float *temp = cml::matrix44f_c( currentTyped->GetTransformMatrix() ).transpose().data();
				std::cout<<"INFO: ******** transform matrix values: ";
				for( unsigned int i = 0; i < 16; i++ )
				{
					std::cout<<" "<<temp[i];
				}
				temp[11] = - temp[11]; // no idea why this needs to be negative!
				std::cout<<std::endl;
				currentExportTyped->setTransformMatrix( temp );
			}
			*/

			currentExport = ( totemio::TotemNode* ) currentExportTyped;
		}
		else
		{
			std::cerr << "ERROR: VolumeTree::Tree::BuildExportNode node string type does not match class type: " << nodeTypeStr << ", results undefined" << std::endl;
			return NULL;
		}
	}

	// In case we have an unknown node type, don't know what to do
	if( currentExport == NULL )
	{
		std::cerr << "ERROR: VolumeTree::Tree::BuildExportNode could not resolve current node, results undefined" << std::endl;
		return NULL;
	}

	// Recurse children
	for( Node *currentChild = _currentNode->GetFirstChild(); currentChild != NULL; currentChild = _currentNode->GetNextChild( currentChild ) )
	{
		totemio::TotemNode *childExportNode = BuildExportNode( currentChild );
		currentExport->addChild( childExportNode );
	}

	return currentExport;
}

//----------------------------------------------------------------------------------

std::string VolumeTree::Tree::GetExportNodeID( Node *_currentNode )
{
	if( _currentNode == NULL || _currentNode->GetIDString().empty() )
	{
		std::stringstream IDstream;
		IDstream << m_exportID;
		m_exportID++;
		return IDstream.str();
	}
	else
	{
		// TODO: check string is unique!
		return _currentNode->GetIDString();
	}
}

//----------------------------------------------------------------------------------

std::queue< VolumeTree::Node* > VolumeTree::Tree::getReverseTree()
{
	std::stack< Node* > s;
	std::queue< Node* > q;
	Node* current = m_rootNode;

	while( !s.empty() || current )
	{
		if( current )
		{
			s.push( current );
			current = current->GetFirstChild();
		}
		else
		{
			current = s.top();
			s.pop();
			q.push( current );
			current = current->GetNextChild( current->GetFirstChild() );
		}
	}
	//std::stack< Node* > s;
	//std::queue< Node* > q;
	//q.push( _rootNode );

	//Node* root; 

	//while( q.empty() == false )
	//{
	//	root = q.front();
	//	q.pop();
	//	s.push( root );

	//	Node* leftChild = root->GetFirstChild();
	//	Node* rightChild = root->GetNextChild( leftChild );
	//	
	//	if( rightChild != NULL ){
	//		q.push( rightChild );
	//	}

	//	if( leftChild != NULL ) {
	//		q.push( leftChild );
	//	}
	//}

	//delete root;
	//return s;
	return q;
}

//----------------------------------------------------------------------------------