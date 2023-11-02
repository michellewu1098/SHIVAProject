# SHIVA Project
## Sculpture for Health-care: Interaction and Virtual Art in 3D

The **SHIVA project** was designed to provide virtual sculpting tools for children with complex disabilities to allow them to engage with artistic and creative activities that they might otherwise never be able to access. Modern *3D printing* then allows us to physically build their creations. 

These tools were deployed in a school for students with complex disabilities and used for a variety of artistic, education and developmental purposes. 

![SHIVA software in use](http://i.imgur.com/GAk94SF.jpg)

The software can be installed to use by downloading the installer from the 'Installers' folder.

## To download and modify the project:
**NOTE**: As of November 2023 - The details here need reviewing as the project has now been upgraded to VS 2019 and boost 1.83.0.

boost now needs to be added via NuGet in VS 2019 so the boost libraries do not need to be downloaded from GitHub now.

OUTDATED: *If cloning or downloading the project, be aware that we're including `boost`, consequently the download will be > 4GB in size and therefore the whole thing could take a long time.*

## Requirements
* Visual Studio 2019
* Windows 10 (11?)
* `glew`
* `SDL`
* `SDL_image`
* `SDL_mixer`
* `SDL_ttf`
* `boost 1.83.0` installed via NuGet in VS 2019
