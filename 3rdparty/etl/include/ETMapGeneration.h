#ifndef __ET_MAPGENERATION_H__
#define __ET_MAPGENERATION_H__
/********************************************************************************
EDITABLE TERRAIN LIBRARY v3 for Ogre

Copyright (c) 2008 Holger Frydrych <frydrych@oddbeat.de>

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
********************************************************************************/

#include "ETCommon.h"

namespace ET
{
  class FlatTerrain;

  /**
   * Generate a normal map for the given terrain.
   * @param array The colour array in which to store the generated map
   * @param terrain The Terrain for which to generate the normal map
   */
  void generateNormalMap(ColourArray2D& array, FlatTerrain* terrain);

  /**
   * Generates a lightmap for the given terrain assuming a directional light.
   * @param array The colour array in which to store the generated map
   * @param terrain The terrain for which to generate the lightmap
   * @param lightDirection The direction from which the light is shining
   * @param lightColour The colour of the light
   * @param ambient The ambient scene colour
   */
  void generateLightMap(ColourArray2D& array, FlatTerrain* terrain, 
      Ogre::Vector3 lightDirection, const Ogre::ColourValue& lightColour, 
      const Ogre::ColourValue& ambient);

  /**
   * Generates a shadow map for the given terrain assuming a directional light.
   * The array dimensions set the size of the generated map. Areas in shadow
   * are set to the given ambient colour, all the other pixels are left untouched.
   * So as input you will likely want to use a white colour array or a generated
   * lightmap.
   * @param array The colour array in which to store the generated map
   * @param terrain The terrain for which to generate the lightmap
   * @param lightDirection The direction from which the light is shining
   * @param ambient The ambient scene colour
   */
  void generateShadowMap(ColourArray2D& array, FlatTerrain* terrain,
      Ogre::Vector3 lightDirection, const Ogre::ColourValue& ambient);
}

#endif

