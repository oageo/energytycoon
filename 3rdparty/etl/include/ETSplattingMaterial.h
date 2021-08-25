#ifndef __ET_SPLATTINGMATERIAL_H__
#define __ET_SPLATTINGMATERIAL_H__
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
#include "ETShaderGenerator.h"
#include "ETSplattingManager.h"
#include <OgreCommon.h>
#include <OgrePixelFormat.h>
#include <OgreMaterial.h>


namespace ET
{
  /**
   * The SplattingMaterial class provides an easy, generic way of creating 
   * dynamic materials for texture splatting. You provide a base material
   * and some information about your splatting procedure. Then SplattingMaterial
   * will clone the base material and insert the necessary splatting passes
   * at the beginning of the best available technique of the base material.
   * The material will use shader model 2 if available and otherwise fall back 
   * to shader model 1. The necessary shaders are generated on the fly as needed
   * and strive to perfectly match the needs of each pass. You need to have the
   * Cg plugin loaded in order for this class to work properly.
   * Note that this is a general purpose implementation aimed for getting
   * splatting working as easily as possible. Custom tailored code for your
   * specific needs may suit you better.
   */
  class SplattingMaterial : public SplattingListener
  {
  public:
    /**
     * Creates and prepares a new material for splatting. Note that you must
     * provide further details to the class via the public interface and then
     * call createMaterial() before you can use the generated material.
     *
     * @param name Name of the material to create
     * @param group Resource group to place the material in
     * @param format The pixel format of the splatting maps
     */
    SplattingMaterial(const Ogre::String& name, const Ogre::String& group,
      Ogre::PixelFormat format);
    ~SplattingMaterial();
    
    /** 
     * Specify the pixel format used for the splatting map textures. 
     */
    void setSplattingMapFormat(Ogre::PixelFormat format);

    /** 
     * Specify which texture coord set to use for the splatting maps.
     * If you intend to apply this material to a single terrain patch,
     * then enable this.
     */
    void setUsePatchLocalTexCoords(bool patchLocal);

    /** 
     * Specify whether to use LOD morphing on the terrain. This requires
     * LOD morphing to be active for the terrain representation you apply
     * this material to.
     */
    void setUseVertexMorphing(bool vertexMorphing);

    /**
     * Specify the base material to use.
     * This material will be copied and the necessary splatting parts added to
     * it. Using a base material you can specify additional tasks the terrain
     * material needs to handle, like lighting and shadows. If you don't
     * specify a base material, the class will start from an empty material.
     */
    void setBaseMaterial(const Ogre::MaterialPtr& material);

    /**
     * Specify the base material to use.
     * This material will be copied and the necessary splatting parts added to
     * it. Using a base material you can specify additional tasks the terrain
     * material needs to handle, like lighting and shadows. If you don't
     * specify a base material, the class will start from an empty material.
     */
    void setBaseMaterial(const Ogre::String& materialName);

    /**
     * Specify the names of the splatting map textures to use.
     */
    void setSplattingMaps(const TextureList& maps);
    
    /**
     * Specify the names of the splatting textures to use. Need to be in the
     * order in which they apply to the splatting map channels.
     */
    void setSplattingTextures(const TextureList& textures);
    
    /** 
     * Set how often the splatting textures are repeated over the terrain.
     * @param repeatX Texture repetition in X direction
     * @param repeatZ Texture repetition in Z direction
     */
    void setTextureRepeat(float repeatX, float repeatZ);

    /**
     * Generate the material from the given information.
     */
    void createMaterial();


    /** Get the name of the created material. */
    const Ogre::String& getMaterialName() const { return mName; }
    /** Get the resource group of the created material. */
    const Ogre::String& getResourceGroup() const { return mGroup; }

    /** Return the generated material. */
    const Ogre::MaterialPtr& getMaterial() const { return mMaterial; }



    virtual void notifySplattingTextures(const TextureList& textures);
    virtual void notifySplattingMaps(const TextureList& maps);

  private:
    /** 
     * Tests how many passes are needed and whether passes can be saved if
     * splatting maps are split across passes. Returns whether to split maps
     * across passes.
     */ 
    bool checkMapSplitSaves();
    
    void createPassNoSplit(Ogre::Technique* t, int curPass, int& curTex, int& curMap);
    void createPassSplit(Ogre::Technique* t, int curPass, int& curTex, int& curMap, 
      int& curChan);
    
    Ogre::String mName, mGroup;
    Ogre::PixelFormat mFormat;
    Ogre::MaterialPtr mMaterial;
    Ogre::MaterialPtr mBaseMaterial;
    bool mPatchLocalCoords;
    bool mVertexMorphing;
    TextureList mMaps;
    TextureList mTextures;
    float mSplatRepeatX, mSplatRepeatZ;
    ShaderModel mShaderModel;
    ShaderGenerator* mShaderGenerator;
  };
}

#endif
