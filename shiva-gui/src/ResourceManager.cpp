#include "ResourceManager.h"
#include "GUIManager.h"
#include "System/Activity.h"
#include "GUI/Drawables/Drawable.h"
#include "GUI/Drawables/BitmapDrawable.h"
#include "GUI/Drawables/RectDrawable.h"
#include "GUI/Drawables/NinePatch.h"
#include "GUI/Drawables/StateListDrawable.h"
#include "GUI/Drawables/LayeredImageDrawable.h"
#include "GUI/Views/View.h"
#include "GUI/Views/ImageButton.h"
#include "GUI/Views/TextButton.h"
#include "GUI/Views/TextView.h"
#include "GUI/Views/ViewGroups/LinearLayout.h"
#include "GUI/Views/ViewGroups/CustomLayout.h"
#include "GUI/Views/Slider.h"
#include "GUI/Views/ViewGroups/ScrollContainer.h"
#include "GUI/Views/ViewGroups/ViewGroup.h"
#include "GUI/Views/ColourSelector.h"
#include "GUI/Views/AdapterViews/ListView.h"
#include "GUI/Views/AdapterViews/LayoutAdapterView.h"
#include "Utility/tinyxml.h"

//////////////////////////////////////////////////////////////////////////
#include <GL/GLee.h>
#include <GL/glu.h>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>


ShivaGUI::ResourceManager::ResourceManager(ShivaGUI::GUIManager *guiManager, unsigned int windowIndex )
{
	_GUIManager = guiManager;
	_windowIndex = windowIndex;
	_mipmapImages = true;
	_currentThemeNode = NULL;

	TTF_Init();
}

ShivaGUI::ResourceManager::~ResourceManager()
{
}


void ShivaGUI::ResourceManager::RegisterViewID(View *view, std::string ID)
{
	if( _GUIManager != NULL )
	{
		Activity *currentActivity = _GUIManager->GetCurrentActivity();
		if( currentActivity != NULL )
		{
			GUIController *currentGUIController = currentActivity->GetGUIController(_windowIndex);
			if( currentGUIController != NULL )
			{
				currentGUIController->RegisterViewID(view,ID);
			}
			else
				std::cerr<<"ShivaGUI::ResourceManager::RegisterViewID failed to retrieve currentGUIController"<<std::endl;
		}
		else
			std::cerr<<"ShivaGUI::ResourceManager::RegisterViewID failed to retrieve currentActivity"<<std::endl;
	}
	else
		std::cerr<<"ShivaGUI::ResourceManager::RegisterViewID failed to retrieve _GUIManager"<<std::endl;
}

ShivaGUI::View* ShivaGUI::ResourceManager::GetViewFromID(std::string ID)
{
	if( _GUIManager != NULL )
	{
		Activity *currentActivity = _GUIManager->GetCurrentActivity();
		if( currentActivity != NULL )
		{
			GUIController *currentGUIController = currentActivity->GetGUIController(_windowIndex);
			if( currentGUIController != NULL )
			{
				return currentGUIController->GetViewFromID(ID);
			}
			else
				std::cerr<<"ShivaGUI::ResourceManager::GetViewFromID failed to retrieve currentGUIController"<<std::endl;
		}
		else
			std::cerr<<"ShivaGUI::ResourceManager::GetViewFromID failed to retrieve currentActivity"<<std::endl;
	}
	else
		std::cerr<<"ShivaGUI::ResourceManager::GetViewFromID failed to retrieve _GUIManager"<<std::endl;
	return NULL;
}


void ShivaGUI::ResourceManager::ReloadTextures()
{
	for( std::map<std::string,unsigned int>::iterator it = _glTextures.begin(); it != _glTextures.end(); ++it )
	{
		bool mipmap = false;
		bool addAlpha = false;
		bool repeat = false;

		std::cout<<"INFO: ResourceManager Reloading Image to OpenGL: "<< it->first <<std::endl;

		SDL_Surface *image;

		// Load the BMP file into a surface
		//image = SDL_LoadBMP(filename.c_str());

		image = IMG_Load( it->first.c_str() );

		if( image == NULL )
		{
			std::cerr<<"WARNING: ResourceManager Couldn't load: "<< it->first <<" "<< SDL_GetError()<<std::endl;
		}

		SDLSurfaceToOpenGLID(image, it->second, mipmap, addAlpha, repeat );

		// Free the allocated BMP surface
		SDL_FreeSurface(image);
	}
}

bool ShivaGUI::ResourceManager::GetManagedTexture(std::string name, unsigned int *texID)
{
	if( _glExternalTextures.find(name) != _glExternalTextures.end() )
	{
		*texID = _glExternalTextures[name];
		return false;
	}

	glGenTextures(1, texID);
	_glExternalTextures[name] = *texID;
	return true;
}

void ShivaGUI::ResourceManager::ClearManagedTexture(std::string name)
{
	if( _glExternalTextures.find(name) != _glExternalTextures.end() )
	{
		unsigned int texID = _glExternalTextures[name];

		glDeleteTextures(1, &texID);
		_glExternalTextures.erase(name);
	}
}



unsigned int ShivaGUI::ResourceManager::GetBitmap(std::string filename)
{
	if( _glTextures.find(filename) != _glTextures.end() )
	{
		return _glTextures[filename];
	}

	// I've had this function for so long I can't remember where it was from originally
	// I think it was based on SDL example usage code

	bool mipmap = false;
	bool addAlpha = false;
	bool repeat = false;

	std::cout<<"INFO: ResourceManager Loading Image to OpenGL: "<<filename<<std::endl;

	SDL_Surface *image;

	// Load the BMP file into a surface
	//image = SDL_LoadBMP(filename.c_str());

	image = IMG_Load(filename.c_str());

	if( image == NULL )
	{
		std::cerr<<"WARNING: ResourceManager Couldn't load: "<<filename<<" "<< SDL_GetError()<<std::endl;
		return 0;
	}

	unsigned int texName = SDLSurfaceToOpenGL(image, mipmap, addAlpha, repeat );

	// Free the allocated BMP surface
	SDL_FreeSurface(image);

	if( texName != 0 )
		_glTextures[filename] = texName;

	return texName;
}

unsigned int ShivaGUI::ResourceManager::GetSimpleText(std::string text, std::string fontfilename, unsigned int fontsize, unsigned int fontColour)
{
	TTF_Font *font = LoadFont(fontfilename,fontsize);
	if( font == NULL )
	{
		std::cerr<<"WARNING: ResourceManager::GetSimpleText could not load font: "<<fontfilename<<std::endl;
		return 0;
	}

	SDL_Color foregroundColour = { (fontColour & 0xFF000000) >> 24, (fontColour & 0x00FF0000) >> 16, (fontColour & 0x0000FF00) >> 8, (fontColour & 0x000000FF) };//{255,255,255,255};

	//std::cout<<"R = "<<(int)foregroundColour.r<<" G = "<<(int)foregroundColour.g<<" B = "<<(int)foregroundColour.b<<std::endl;

	SDL_Surface *surf = TTF_RenderText_Blended(font, text.c_str(), foregroundColour);//TTF_RenderText_Shaded( font, text.c_str(), foregroundColour, backgroundColour );

	return SDLSurfaceToOpenGL(surf,false,false,false,true);
}

TTF_Font* ShivaGUI::ResourceManager::LoadFont(std::string filename, unsigned int size)
{
	if( filename.empty() )
		filename = _defaultFont;
	std::stringstream key;
	key << filename<< size;

	std::map<std::string, TTF_Font*>::iterator fontIt = _fontcache.find( key.str() );
	if( fontIt != _fontcache.end() )
	{
		return fontIt->second;
	}
	TTF_Font *currentFont = TTF_OpenFont(filename.c_str(),size);

	if( currentFont != NULL )
		_fontcache[ key.str() ] = currentFont;

	return currentFont;
}

ShivaGUI::Drawable* ShivaGUI::ResourceManager::GetDrawable(std::string filename)
{
	// If the first character is an '@' we will assume it's a constant for replacement
	if( filename.at(0) == '@' )
		filename = GetInflationAttribute(filename);


	if( filename.empty() )
	{
		std::cerr<<"WARNING: ResourceManager::GetDrawable given empty filename"<<std::endl;
		return NULL;
	}

	if( filename.compare( filename.size()-4,4,".png") == 0 )
	{
		// We are being asked to load a png file
		Drawable *output = new BitmapDrawable( GetBitmap(std::string("Resources/Drawables/")+filename) );
		output->SetFilename(filename);
		return output;
	}
	else if( filename.compare( filename.size()-4,4,".xml") == 0 )
	{
		// We are being asked to load a drawable from xml file

		TiXmlDocument doc( (std::string("Resources/Drawables/")+filename).c_str() );

		if( doc.LoadFile() )
		{
			Drawable *output = ParseDrawableElement(&doc);
			output->SetFilename(filename);
			return output;
		}
		else
		{
			std::cerr<<"WARNING: ResourceManager::GetDrawable Could not load drawable from xml: "<<filename<<std::endl;
			return NULL;
		}

	}

	return NULL;
}

ShivaGUI::Drawable* ShivaGUI::ResourceManager::CreateDrawable(std::string drawableName)
{
	if( drawableName == "BitmapDrawable" )
	{
		return new BitmapDrawable();
	}
	else if( drawableName == "StateListDrawable" )
	{
		return new StateListDrawable();
	}
	else if( drawableName == "NinePatch" )
	{
		return new NinePatch();
	}
	else if( drawableName == "RectDrawable" )
	{
		return new RectDrawable();
	}
	else if( drawableName == "LayeredImageDrawable" )
	{
		return new LayeredImageDrawable();
	}
	return NULL;
}

ShivaGUI::View* ShivaGUI::ResourceManager::CreateView(std::string viewName)
{
	if( viewName == "View" )
	{
		return new View();
	}
	if( viewName == "AbsoluteLayout" )
	{
		//return new AbsoluteLayout();
	}
	if( viewName == "ImageButton" )
	{
		return new ImageButton();
	}
	if( viewName == "TextButton" )
	{
		return new TextButton();
	}
	if( viewName == "LinearLayout" )
	{
		return new LinearLayout();
	}
	if( viewName == "Slider" )
	{
		return new Slider();
	}
	if( viewName == "ColourSelector" )
	{
		return new ColourSelector();
	}
	if( viewName == "TextView" )
	{
		return new TextView();
	}
	if( viewName == "CustomLayout" )
	{
		return new CustomLayout();
	}
	if( viewName == "ScrollContainer" )
	{
		return new ScrollContainer();
	}
	if( viewName == "ListView" )
	{
		return new ListView();
	}
	if( viewName == "LayoutAdapterView" )
	{
		return new LayoutAdapterView();
	}

	View *customView = _GUIManager->CreateView(viewName);
	if( customView != NULL )
		return customView;

	std::cerr<<"WARNING: ResourceManager::GetView could not create View of unknown type: "<<viewName<<std::endl;
	return NULL;
}

ShivaGUI::AudioClip* ShivaGUI::ResourceManager::GetAudioClip(std::string filename)
{
	return _GUIManager->GetAudioManager()->GetSample(filename);
}

ShivaGUI::View* ShivaGUI::ResourceManager::GetLayout(std::string filenameInput )
{
	// Strip off any leading path stuff initially:
	boost::filesystem::path filename(filenameInput);
	filename = filename.filename();

	// First, see if a version of the layout exists in the options directory:
	boost::filesystem::path optionsPath( _GUIManager->GetProfileOptionsDir() );
	optionsPath = optionsPath / "Layout" / filename;

	TiXmlDocument doc;
	boost::filesystem::path pathToLoad;
	if( boost::filesystem::exists(optionsPath) )
	{
		pathToLoad = optionsPath;
	}
	else
	{
		// Version in main resources dir
		pathToLoad = "Resources/Layout" / filename;
	}

	if( doc.LoadFile(pathToLoad.string()) )
	{
		View *rootView = ParseLayoutElement(&doc,true);
		DoPostEvaluationLinks();
		return rootView;
	}
	else
	{
		std::cerr<<"WARNING: ResourceManager::GetLayout Could not load layout from xml: "<<filename<<std::endl;
		return NULL;
	}

}

bool ShivaGUI::ResourceManager::OutputLayout(std::string filename, View *rootNode)
{
	if( filename.empty() || rootNode == NULL )
		return false;

	std::cout<<"INFO: ResourceManager Deflating Layout to file: "<<filename<<std::endl;

	TiXmlDocument doc( filename.c_str() );

	doc.InsertEndChild( *rootNode->Deflate( this ) );

	return doc.SaveFile();
}

std::string ShivaGUI::ResourceManager::GetInflationAttribute(std::string originalAttrib)
{
	if( originalAttrib.at(0) == '@' )
	{
		//std::cout<<"*********INFO: Const replacement for: "<<originalAttrib<<std::endl;
		// Check against consts in Profile
		if( _attribConstsProfile.find(originalAttrib) != _attribConstsProfile.end() )
		{
			std::cout<<"INFO: Const replacement for: "<<originalAttrib<<" from Profile with: "<<_attribConstsProfile[originalAttrib]<<std::endl;
			return _attribConstsProfile[originalAttrib];
		}
		// Check against consts in Theme
		if( _attribConstsTheme.find(originalAttrib) != _attribConstsTheme.end() )
		{
			std::cout<<"INFO: Const replacement for: "<<originalAttrib<<" from Theme with: "<<_attribConstsTheme[originalAttrib]<<std::endl;
			return _attribConstsTheme[originalAttrib];
		}
	}
	return originalAttrib;
}

void ShivaGUI::ResourceManager::LoadProfileAttributeConsts(ProfileManager *profileManager)
{
	_attribConstsProfile.clear();

	/*
	xml for consts should look like this  (same format in profile and theme)

	<constGroup>
		<const>
			<key>@something</key>
			<value>somethingelse</value>
		</const>
		<const>
			<key>@something</key>
			<value>somethingelse</value>
		</const>
	</constGroup>

	*/

	profileManager->EnterOptionNode( "constGroup" );
	int numConsts = profileManager->ContainsOption( "const" );
	for( int i = 0; i < numConsts; i++ )
	{
		if( profileManager->EnterOptionNode( "const", i ) )
		{
			// We can now query our const replacements
			std::string key = profileManager->GetString( "key", "@INVALID" );
			std::string value = profileManager->GetString( "value", "@INVALID" );

			_attribConstsProfile[key] = value;

			std::cout<<"INFO: LoadProfileAttributeConsts, key: "<<key<<" value: "<<value<<std::endl;

			profileManager->ExitOptionNode();
		}
	}
	profileManager->ExitOptionNode();
}

void ShivaGUI::ResourceManager::SetTheme(std::string filename)
{
	_attribConstsTheme.clear();

	if( filename.empty() )
	{
		delete _currentThemeNode;
		_currentThemeNode = NULL;
		return;
	}
	TiXmlDocument doc( filename.c_str() );

	if( doc.LoadFile() )
	{
		delete _currentThemeNode;
		_currentThemeNode = NULL;
		// Grab a copy of the root element node
		_currentThemeNode = (TiXmlElement*) doc.RootElement()->Clone();

		// Load consts list
		/*
		xml for consts should look like this (same format in profile and theme)

		<constGroup>
			<const>
				<key>@something</key>
				<value>somethingelse</value>
			</const>
			<const>
				<key>@something</key>
				<value>somethingelse</value>
			</const>
		</constGroup>

		*/
		if( _currentThemeNode != NULL )
		{
			TiXmlElement *constContainerNode = _currentThemeNode->FirstChildElement("constGroup");
			if( constContainerNode != NULL )
			{
				TiXmlNode *child = 0;
				while( child = constContainerNode->IterateChildren( "const", child ) )
				{
					TiXmlElement *keyElement = child->FirstChildElement("key");
					if( keyElement != NULL )
					{
						TiXmlElement *valueElement = child->FirstChildElement("value");
						if( valueElement != NULL )
						{
							std::string key = keyElement->GetText();//Value();
							std::string value = valueElement->GetText();//Value();
							_attribConstsTheme[key] = value;

							std::cout<<"INFO: Load Theme Attribute Consts, key: "<<key<<" value: "<<value<<std::endl;
						}
					}
				}

				/*
				for( TiXmlAttribute *currentAttribute = constNode->FirstChildElement("Const"); currentAttribute != NULL; currentAttribute = currentAttribute->NextSiblingElement() )
				{
					std::string key = currentAttribute->Name();
					std::string value = currentAttribute->Value();
					_attribConstsTheme[key] = value;
				}
				*/
			}
		}
	}
	else
	{
		std::cerr<<"WARNING: ResourceManager::SetTheme Could not load theme from xml: "<<filename<<std::endl;
	}
}

ShivaGUI::ViewEventListener* ShivaGUI::ResourceManager::GetListener(std::string name)
{
	if( _GUIManager != NULL )
	{
		Activity *currentActivity = _GUIManager->GetCurrentActivity();
		if( currentActivity != NULL )
		{
			GUIController *currentGUIController = currentActivity->GetGUIController(_windowIndex);
			if( currentGUIController != NULL )
			{
				return currentGUIController->GetListener(name);
			}
		}
	}
	std::cerr<<"ShivaGUI::ResourceManager::GetListener could not retrieve current GUIController"<<std::endl;
	return NULL;
}

ShivaGUI::View* ShivaGUI::ResourceManager::ParseLayoutElement( TiXmlNode *parentNode, bool rootNode )
{
	// Useful tinyxml links:
	// http://www.grinninglizard.com/tinyxmldocs/index.html
	// http://www.grinninglizard.com/tinyxmldocs/tutorial0.html

	int nodeType = parentNode->Type();

	switch( nodeType )
	{
		case TiXmlNode::TINYXML_DOCUMENT:
			for( TiXmlNode *childNode = parentNode->FirstChild(); childNode != NULL; childNode = childNode->NextSibling() )
			{
				// We return the first element that is a valid view
				View *result = ParseLayoutElement( childNode, true );
				if( result != NULL )
					return result;
			}
			break;

		case TiXmlNode::TINYXML_ELEMENT:
			{
				View *parentView = CreateView( parentNode->Value() );
				if( parentView != NULL )
				{
					// If we have a themes file, we must send this for inflation before we send the actual Layout node
					// This sets the View to the 'defaults' provided by the theme,
					// but also gives our Layout a chance to override anything it wants
					if( _currentThemeNode != NULL )
						parentView->Inflate( _currentThemeNode, this, "", rootNode );
					parentView->Inflate( parentNode->ToElement(), this );

					// Only check for children if View is a ViewGroup
					ViewGroup *parentContainer = dynamic_cast<ViewGroup*>(parentView);
					if( parentContainer != NULL )
					{
						for( TiXmlNode *childNode = parentNode->FirstChild(); childNode != NULL; childNode = childNode->NextSibling())
						{
							View *childView = ParseLayoutElement( childNode );
							if( childView != NULL )
							{
								LayoutParams *childLayoutParams;
								if( _currentThemeNode != NULL )
								{
									childLayoutParams = parentContainer->InflateLayoutParams( _currentThemeNode, NULL, childView->GetThemePrefix() );
									parentContainer->InflateLayoutParams( childNode->ToElement(), childLayoutParams );
								}
								else
								{
									childLayoutParams = parentContainer->InflateLayoutParams( childNode->ToElement() );
								}

								if( childLayoutParams != NULL )
								{
									childView->SetLayoutParams( childLayoutParams );
									parentContainer->AddView(childView,this);
								}
								else
								{
									std::cerr<<"WARNING: ResourceManager::ParseLayoutElement could not create LayoutParams for child node"<<std::endl;
								}

							}
							else
							{
								std::cerr<<"WARNING: ResourceManager::ParseLayoutElement could not parse layout element of container node"<<std::endl;
							}
						}
					}

					return parentView;
				}
				else
				{
					std::cerr<<"WARNING: ResourceManager::ParseLayoutElement could not create View"<<std::endl;
				}
			}
			break;

	}

	return NULL;
}

ShivaGUI::Drawable* ShivaGUI::ResourceManager::ParseDrawableElement( TiXmlNode *parentNode )
{
	int nodeType = parentNode->Type();

	switch( nodeType )
	{
		case TiXmlNode::TINYXML_DOCUMENT:
			for( TiXmlNode *childNode = parentNode->FirstChild(); childNode != NULL; childNode = childNode->NextSibling() )
			{
				// We return the first element that is a valid view
				Drawable *result = ParseDrawableElement( childNode );
				if( result != NULL )
					return result;
			}
			break;

		case TiXmlNode::TINYXML_ELEMENT:
		{
			Drawable *drawable = CreateDrawable( parentNode->Value() );
			if( drawable != NULL )
			{
				drawable->Inflate( parentNode->ToElement(), this );

				return drawable;
			}
			else
			{
				std::cerr<<"WARNING: ResourceManager::ParseDrawableElement could not create Drawable"<<std::endl;
			}
		}
	}
	return NULL;
}


unsigned int ShivaGUI::ResourceManager::SDLSurfaceToOpenGL(SDL_Surface *image, bool mipmap, bool addAlpha, bool repeat, bool reverseOrder )
{
	unsigned int texName = 0;
	glGenTextures(1, &texName);
	SDLSurfaceToOpenGLID(image,texName,mipmap,addAlpha,repeat,reverseOrder);
	return texName;
}

void ShivaGUI::ResourceManager::SDLSurfaceToOpenGLID(SDL_Surface *image, unsigned int OpenGLTexID, bool mipmap, bool addAlpha, bool repeat, bool reverseOrder )
{

	GLenum type = GL_RGBA;

	if( image->format->BitsPerPixel == 32 )
	{
		type = reverseOrder ? GL_BGRA : GL_RGBA;
	}
	else if( image->format->BitsPerPixel == 24 )
	{
		type = reverseOrder ? GL_BGR : GL_RGB;
	}
	else
	{
		std::cerr<<"WARNING: ResourceManager::SDLSurfaceToOpenGLID Image is not RGB or RGBA"<<std::endl;
		//SDL_FreeSurface(image);
	}


	if( OpenGLTexID != 0 )
	{
		glBindTexture(GL_TEXTURE_2D, OpenGLTexID);

		if( repeat )
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if( mipmap )
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		if( addAlpha )
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, type, GL_UNSIGNED_BYTE, image->pixels);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, image->format->BitsPerPixel/8, image->w, image->h, 0, type, GL_UNSIGNED_BYTE, image->pixels);

		if( mipmap )
			if( gluBuild2DMipmaps(GL_TEXTURE_2D,image->format->BitsPerPixel/8,image->w, image->h,type,GL_UNSIGNED_BYTE, image->pixels) != 0 )
				std::cerr<<"WARNING: mipmap generation error"<<std::endl;

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void ShivaGUI::ResourceManager::RegisterPostInflationLink( View *src, std::string dstID, bool scanForward)
{
	_postInflationLinks.push_back( PostInflationLink(src,dstID,scanForward) );
}

void ShivaGUI::ResourceManager::RegisterPostInflationLink( View *src, std::string dstID, Definitions::FocusDirection focusDir)
{
	_postInflationLinks.push_back( PostInflationLink(src,dstID,focusDir) );
}

void ShivaGUI::ResourceManager::DoPostEvaluationLinks()
{
	for( std::vector< PostInflationLink >::iterator it = _postInflationLinks.begin(); it != _postInflationLinks.end(); ++it )
	{
		View *dstView = GetViewFromID( it->dstID );
		if( dstView != NULL && it->src != NULL )
		{
			if( it->scan )
			{
				it->src->SetNextScan(dstView,it->scanForward);
				dstView->SetNextScan(it->src,!it->scanForward);
			}
			if( it->focus )
			{
				it->src->SetNextFocus(dstView,it->focusDir);
				dstView->SetNextFocus(it->src,Definitions::GetOppositeFocusDirection(it->focusDir));
			}
		}
		else
		{
			std::cerr<<"WARNING: ResourceManager::DoPostEvaluationLinks() cannot resolve Views, dstID = "<<it->dstID<<std::endl;
		}
	}
	_postInflationLinks.clear();
}
