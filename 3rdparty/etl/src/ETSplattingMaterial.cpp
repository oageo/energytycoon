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

#include "ETSplattingMaterial.h"
#include <iostream>
#include <OgreStringConverter.h>
#include <OgreMaterialManager.h>
#include <OgreGpuProgramManager.h>
#include <OgreTechnique.h>
#include <OgrePass.h>

using namespace Ogre;
using namespace std;


namespace ET
{
  namespace
  {
    const int NUM_TEXUNITS[] = {4, 6, 12};
  }

  SplattingMaterial::SplattingMaterial(const String& name, const String& group,
    PixelFormat format)
  : mName(name), mGroup(group), mFormat(format), mPatchLocalCoords(false),
    mVertexMorphing(false), mSplatRepeatX(1), mSplatRepeatZ(1), mShaderModel(SM_11)
  {
    // create new material
    mMaterial = MaterialPtr (MaterialManager::getSingleton().create(name, group));
    // check whether we can use shader model 2
    GpuProgramManager* gpuMgr = GpuProgramManager::getSingletonPtr();
    if (gpuMgr->isSyntaxSupported("arbfp1") || gpuMgr->isSyntaxSupported("ps_2_0"))
      mShaderModel = SM_20;
    else if (gpuMgr->isSyntaxSupported("ps_1_4") || gpuMgr->isSyntaxSupported("fp20"))
      mShaderModel = SM_14;
      
    mShaderGenerator = ShaderGenerator::create();
  }
  
  SplattingMaterial::~SplattingMaterial()
  {
    ShaderGenerator::destroy(mShaderGenerator);
    MaterialManager::getSingleton().remove(mName);
  }
  
  void SplattingMaterial::setSplattingMapFormat(PixelFormat format)
  {
    mFormat = format;
  }
  
  void SplattingMaterial::setUsePatchLocalTexCoords(bool patchLocal)
  {
    mPatchLocalCoords = patchLocal;
  }
  
  void SplattingMaterial::setUseVertexMorphing(bool vertexMorphing)
  {
    mVertexMorphing = vertexMorphing;
  }
  
  void SplattingMaterial::setBaseMaterial(const MaterialPtr& material)
  {
    mBaseMaterial = material;
  }
  
  void SplattingMaterial::setBaseMaterial(const String& materialName)
  {
    mBaseMaterial = MaterialPtr(MaterialManager::getSingleton().getByName(materialName));
  }
  
  void SplattingMaterial::setSplattingMaps(const TextureList& maps)
  {
    mMaps = maps;
  }
  
  void SplattingMaterial::setSplattingTextures(const TextureList& textures)
  {
    mTextures = textures;
  }
  
  void SplattingMaterial::setTextureRepeat(float repeatX, float repeatZ)
  {
    mSplatRepeatX = repeatX;
    mSplatRepeatZ = repeatZ;
  }
  
  
  void SplattingMaterial::createMaterial()
  {
    cout << "Creating material with maps:\n";
    for (size_t i = 0; i < mMaps.size(); ++i)
      cout << "  " << mMaps[i] << "\n";
    cout << "and splatting textures:\n";
    for (size_t i = 0; i < mTextures.size(); ++i)
      cout << "  " << mTextures[i] << "\n";
    // prepare material from empty or base material
    mMaterial->removeAllTechniques();
    if (!mBaseMaterial.isNull())
      mBaseMaterial->copyDetailsTo(mMaterial);
    mMaterial->load();

    cout << flush;

    if (mTextures.empty() || mMaps.empty())
      return;
      
    // get the technique to apply splatting to
    Technique* t = mMaterial->getBestTechnique();
    if (!t)
      t = mMaterial->createTechnique();
    
    cout << "Checking whether to split passes..." << endl;
    bool splitMapsAcrossPasses = checkMapSplitSaves();
    // create passes
    int curTex = 0;
    int curMap = 0;
    int curChan = 0;
    int curPass = 0;
    cout << "Creating passes..." << endl;
    while (curTex < mTextures.size() && curMap < mMaps.size())
    {
      cout << "New pass at curTex = " << curTex << " and curMap = " << curMap << endl;
      if (!splitMapsAcrossPasses)
        createPassNoSplit(t, curPass, curTex, curMap);
      else
        createPassSplit(t, curPass, curTex, curMap, curChan);
      ++curPass;
    }
  }
  
  void SplattingMaterial::createPassNoSplit(Technique* t, int curPass, 
    int& curTex, int& curMap)
  {
    size_t numChannels = PixelUtil::getComponentCount(mFormat);
    size_t numPerPass = NUM_TEXUNITS[mShaderModel];
    Pass* p = t->createPass();
    t->movePass(p->getIndex(), curPass);
    if (curPass != 0)
      p->setSceneBlending(SBT_ADD);
    p->setLightingEnabled(false);
    int numMaps = min(numPerPass / (numChannels+1), mMaps.size()-curMap);
    int numTex = min(numMaps * numChannels, mTextures.size()-curTex);
    p->setVertexProgram(mShaderGenerator->createVertexShader(numMaps, numTex, 
      mPatchLocalCoords, mVertexMorphing, mShaderModel));
    p->setFragmentProgram(mShaderGenerator->createFragmentShader(mFormat, numMaps,
      numTex, 0, mShaderModel));
    cout << flush;
    for (int i = curMap; i < curMap+numMaps; ++i)
      p->createTextureUnitState(mMaps[i]);
    for (int i = curTex; i < curTex+numTex; ++i)
      p->createTextureUnitState(mTextures[i]);
    GpuProgramParametersSharedPtr params = p->getVertexProgramParameters();
    params->setIgnoreMissingParams(true);
    params->setNamedConstant("splatRepeatX", mSplatRepeatX);
    params->setNamedConstant("splatRepeatZ", mSplatRepeatZ);
    curTex += numTex;
    curMap += numMaps;
  }
  
  void SplattingMaterial::createPassSplit(Technique* t, int curPass, int& curTex, 
    int& curMap, int& curChan)
  {
    size_t numChannels = PixelUtil::getComponentCount(mFormat);
    Pass* p = t->createPass();
    t->movePass(p->getIndex(), curPass);
    if (curPass != 0)
      p->setSceneBlending(SBT_ADD);
    p->setLightingEnabled(false);
    int numMaps = 1;
    int numTex = 0;
    int numUnitsLeft = NUM_TEXUNITS[mShaderModel]-1;
    if (curChan == numChannels)
      curChan = 0;
    while (numUnitsLeft > 0 && curTex + numTex < mTextures.size())
    {
      if (curChan == numChannels)
      {
        curChan = 0;
        if (numUnitsLeft < 2)
          break;
        ++numMaps;
        --numUnitsLeft;
      }
      ++numTex;
      --numUnitsLeft;
      ++curChan;
    }
    numMaps = min(numMaps, (int)mMaps.size() - curMap);
    p->setVertexProgram(mShaderGenerator->createVertexShader(numMaps, numTex, 
      mPatchLocalCoords, mVertexMorphing, mShaderModel));
    p->setFragmentProgram(mShaderGenerator->createFragmentShader(mFormat, numMaps,
      numTex, curChan, mShaderModel));
    for (int i = curMap; i < curMap+numMaps; ++i)
      p->createTextureUnitState(mMaps[i]);
    for (int i = curTex; i < curTex+numTex; ++i)
      p->createTextureUnitState(mTextures[i]);
    GpuProgramParametersSharedPtr params = p->getVertexProgramParameters();
    params->setIgnoreMissingParams(true);
    params->setNamedConstant("splatRepeatX", mSplatRepeatX);
    params->setNamedConstant("splatRepeatZ", mSplatRepeatZ);
    curTex += numTex;
    curMap += numMaps;
  }
  
  
  bool SplattingMaterial::checkMapSplitSaves()
  {
    size_t numChannels = PixelUtil::getComponentCount(mFormat);
    size_t numPerPass = NUM_TEXUNITS[mShaderModel];
    
    // estimate pass count for conservative approach
    int numTexPerPassNoSplit = (numPerPass / (numChannels+1)) * numChannels;
    if (numTexPerPassNoSplit == 0)
      return true;  // can't do without splitting
    int numPassesNoSplit = (mTextures.size()+numTexPerPassNoSplit-1) / numTexPerPassNoSplit;
    
    // estimate pass count for map splitting approach
    int numPassesSplit = 0;
    int numUnitsLeft = 0;
    int curChannel = 0;
    for (size_t i = 0; i < mTextures.size(); ++i)
    {
      if (curChannel == numChannels)
      {
        curChannel = 0;
        --numUnitsLeft;
      }
      if (numUnitsLeft <= 0)
      {
        ++numPassesSplit;
        numUnitsLeft = numPerPass-1;
      }
      ++curChannel;
      --numUnitsLeft;
    }
    
    std::cout << "Estimate: Conservative " << numPassesNoSplit << " passes, ";
    std::cout << "Split " << numPassesSplit << " passes.\n";    
    
    return (numPassesSplit > numPassesNoSplit);    
  }


  void SplattingMaterial::notifySplattingTextures(const TextureList& textures)
  {
    cout << "New splatting textures:\n";
    for (size_t i = 0; i < textures.size(); ++i)
      cout << textures[i] << "\n";
    cout << flush;
    setSplattingTextures(textures);
    createMaterial();
  }

  void SplattingMaterial::notifySplattingMaps(const TextureList& maps)
  {
    cout << "New splatting maps:\n";
    for (size_t i = 0; i < maps.size(); ++i)
      cout << maps[i] << "\n";
    cout << flush;
    setSplattingMaps(maps);
    createMaterial();
  }
}
