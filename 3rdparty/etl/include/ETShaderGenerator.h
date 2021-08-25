#ifndef __ET_SHADERGENERATOR_H__
#define __ET_SHADERGENERATOR_H__
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

#include <OgreCommon.h>
#include <OgrePixelFormat.h>


namespace ET
{
  enum ShaderModel
  {
    SM_11 = 0,
    SM_14 = 1,
    SM_20 = 2
  };
  
  /**
   * The ShaderGenerator creates shaders for use in splatting material.
   * It's written to support the SplattingMaterial class, but you may
   * use it for your own if you wish to. Create an instance of the generator
   * with the static create() function.
   * This is an abstract interface. A concrete implementation is instantiated
   * behind the curtains. Right now, the implementation creates Cg shaders.
   * In the future this might be changed to separate generators depending on
   * the render system which can generate HLSL and GLSL shaders as needed.
   * Shaders are only generated once globally, independent of the generator
   * instance. Any further calls to regenerate a specific shader will return
   * the previous instance.
   */
  class ShaderGenerator
  {
  public:
    /** Create a new generator instance */
    static ShaderGenerator* create();
    /** Destroy a generator instance */
    static void destroy(ShaderGenerator* generator);
    
    /** 
     * Generate a vertex shader with the given requirements. Returns the name
     * of the generated shader.
     * @param numSplatMaps Number of splatting maps
     * @param numTextures Number of splatting textures
     * @param patchLocalCoords Use texcoord1 for splatting maps?
     * @param vertexMorphing Use vertex LOD morphing?
     * @param sm Target shader model
     */
    virtual const Ogre::String& createVertexShader(int numSplatMaps, int numTextures,
      bool patchLocalCoords, bool vertexMorphing, ShaderModel sm) = 0;
    
    /**
     * Generate a fragment shader with the given requirements. Returns the name
     * of the generated shader.
     * @param format Pixel format of the splatting maps
     * @param numSplatMaps Number of splatting maps
     * @param numTextures Number of splatting textures
     * @param startChannel First channel in the first splatting map to be used
     * @param sm Target shader model
     */
    virtual const Ogre::String& createFragmentShader(Ogre::PixelFormat format,
      int numSplatMaps, int numTextures, int startChannel, ShaderModel sm) = 0;
     
  protected:
    ShaderGenerator() {}
    virtual ~ShaderGenerator() {}
  };
}

#endif

