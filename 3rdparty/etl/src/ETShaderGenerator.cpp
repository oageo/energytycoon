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
#include "ETShaderGenerator.h"
#include <OgreHighLevelGpuProgramManager.h>
#include <OgreStringConverter.h>
#include <sstream>

using namespace Ogre;
using namespace std;


namespace ET
{
  namespace
  {
    const char* NAME_PREFIX = "ET/Programs/";
    const char* VS_PREFIX = "Vertex/";
    const char* PS_PREFIX = "Fragment/";
    const char* MODEL_PREFIX[] = {"SM11_", "SM14_", "SM20_"};
    const char* GROUP = "General";
  }
  
  struct MapFormatDesc
  {
    int numChannels;
    String channels;
    String type;
    MapFormatDesc(PixelFormat format)
    {
      numChannels = 0;
      int rgba[4];
      PixelUtil::getBitDepths(format, rgba);
      if (rgba[0])
      {
        channels += 'r';
        ++numChannels;
      }
      if (rgba[1])
      {
        channels += 'g';
        ++numChannels;
      }
      if (rgba[2])
      {
        channels += 'b';
        ++numChannels;
      }
      if (rgba[3])
      {
        channels += 'a';
        ++numChannels;
      }
      type = "float";
      if (numChannels > 1)
        type += StringConverter::toString(numChannels);
    }
  };
  

  class CgShaderGenerator : public ShaderGenerator
  {
  public:
  
    virtual const Ogre::String& createVertexShader(int numSplatMaps, int numTextures,
      bool patchLocalCoords, bool vertexMorphing, ShaderModel sm)
    {
      ostringstream s;
      s << NAME_PREFIX << VS_PREFIX << MODEL_PREFIX[sm] << "S" << numSplatMaps;
      s << "T" << numTextures;
      if (patchLocalCoords)
        s << "P";
      if (vertexMorphing)
        s << "M";
      String shaderName = s.str();
      s.str("");
      
      HighLevelGpuProgramManager* hlgpMgr = HighLevelGpuProgramManager::getSingletonPtr();
      HighLevelGpuProgramPtr program (hlgpMgr->getByName(shaderName));
      if (program.isNull())      
      {
        program = hlgpMgr->createProgram(shaderName, GROUP, "cg", GPT_VERTEX_PROGRAM);
        program->setParameter("entry_point", "main");
        if (sm == SM_11)
          program->setParameter("profiles", "vs_1_1\nvp10\nglslv\narbvp1");
        else if (sm == SM_14)
          program->setParameter("profiles", "arbvp1\nvs_1_4\nvp20\nglslv");
        else
          program->setParameter("profiles", "vs_2_0\narbvp1\nvp30\nglslv");
        vertexHeaderInput(s, patchLocalCoords, vertexMorphing);
        vertexHeaderOutput(s, sm, numSplatMaps+numTextures, vertexMorphing);
        vertexBody(s, sm, numSplatMaps, numTextures, patchLocalCoords, vertexMorphing);
        program->setSource(s.str());
        cout << "Generated vertex shader " << shaderName << ":\n";
        cout << s.str() << "\n";
        program->load();
        GpuProgramParametersSharedPtr params = program->getDefaultParameters();
        params->setIgnoreMissingParams(true);
        params->setNamedAutoConstant("worldViewProj", GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
        if (vertexMorphing)
          params->setNamedAutoConstant("morphFactor", GpuProgramParameters::ACT_CUSTOM, 77);
      }
        
      return program->getName();
    }
    
    virtual const Ogre::String& createFragmentShader(Ogre::PixelFormat format,
      int numSplatMaps, int numTextures, int startChannel, ShaderModel sm)
    {
      ostringstream s;
      s << NAME_PREFIX << PS_PREFIX << MODEL_PREFIX[sm];
      s << PixelUtil::getFormatName(format) << "_";
      s << "S" << numSplatMaps;
      s << "T" << numTextures;
      s << "C" << startChannel;
      String shaderName = s.str();
      s.str("");

      HighLevelGpuProgramManager* hlgpMgr = HighLevelGpuProgramManager::getSingletonPtr();
      HighLevelGpuProgramPtr program (hlgpMgr->getByName(shaderName));
      if (program.isNull())      
      {
        program = hlgpMgr->createProgram(shaderName, GROUP, "cg", GPT_FRAGMENT_PROGRAM);
        program->setParameter("entry_point", "main");
        if (sm == SM_11)
          program->setParameter("profiles", "ps_1_1\nfp10\nglslf");
        else if (sm == SM_14)
          program->setParameter("profiles", "ps_1_4\nfp20\nglslf");
        else
          program->setParameter("profiles", "ps_2_0\narbfp1\nfp30\nglslf");
        fragmentHeader(s, sm, numSplatMaps, numTextures);
        fragmentBody(s, sm, format, numSplatMaps, numTextures, startChannel);
        program->setSource(s.str());
        cout << "Generated fragment shader " << shaderName << ":\n";
        cout << s.str() << "\n";
        program->load();
      }
        
      return program->getName();
    }
    
    
  private:
  
    void vertexHeaderInput(ostringstream& s, bool patchLocalCoords, bool vertexMorphing)
    {
      s << "void main( \n";
      s << "  float4 iPos: POSITION, \n";
      s << "  float2 iUV0: TEXCOORD0, \n";
      if (patchLocalCoords)
        s << "  float2 iUV1: TEXCOORD1, \n";
      if (vertexMorphing)
        s << "  float delta: BLENDWEIGHT,\n";
    }
    
    void vertexHeaderOutput(ostringstream& s, ShaderModel sm, int numTextures, bool vertexMorphing)
    {
      s << "  out float4 oPos: POSITION, \n";
      s << "  out float2 oUV0: TEXCOORD0, \n";
      s << "  out float2 oUV1: TEXCOORD1, \n";
      if (sm < SM_20)
      {
        for (int i = 2; i < numTextures; ++i)
          s << "  out float2 oUV" << i << ": TEXCOORD" << i << ", \n";
      }
      s << "  uniform float splatRepeatX, \n";
      s << "  uniform float splatRepeatZ, \n";
      if (vertexMorphing)
        s << "  uniform float morphFactor, \n";
      s << "  uniform float4x4 worldViewProj\n";
      s << ")\n";
    }
    
    void vertexBody(ostringstream& s, ShaderModel sm, int numSplatMaps,
      int numTextures, bool patchLocalCoords, bool vertexMorphing)
    {
      s << "{\n";
      if (vertexMorphing)
        s << "  iPos.y += (delta.x * morphFactor);\n";
      s << "  oPos = mul(worldViewProj, iPos);\n";
      int uvNum = 0;
      String uv = (patchLocalCoords ? "iUV1" : "iUV0");
      if (sm >= SM_20)
        numSplatMaps = numTextures = 1;
      for (; uvNum < numSplatMaps; ++uvNum)
        s << "  oUV" << uvNum << " = " << uv << ";\n";
      s << "  iUV0.x *= splatRepeatX;\n";
      s << "  iUV0.y *= splatRepeatZ;\n";
      for (; uvNum < numSplatMaps+numTextures; ++uvNum)
        s << "  oUV" << uvNum << " = iUV0;\n";
      s << "}\n";
    }
    
    
    void fragmentHeader(ostringstream& s, ShaderModel sm, int numSplatMaps,
      int numTextures)
    {
      s << "void main(\n";
      s << "  float2 uv0: TEXCOORD0,\n";
      s << "  float2 uv1: TEXCOORD1,\n";
      if (sm < SM_20)
      {
        for (int i = 2; i < numSplatMaps+numTextures; ++i)
          s << "  float2 uv" << i << ": TEXCOORD" << i << ",\n";
      }
      for (int i = 0; i < numSplatMaps; ++i)
        s << "  uniform sampler2D map" << i << ",\n";
      for (int i = 0; i < numTextures; ++i)
        s << "  uniform sampler2D tex" << i << ",\n";
      s << "  out float4 color: COLOR\n";
      s << ")\n";
    }
    
    void fragmentBody(ostringstream& s, ShaderModel sm, PixelFormat format,
      int numSplatMaps, int numTextures, int startChannel)
    {
      s << "{\n";
      MapFormatDesc desc (format);
      for (int i = 0; i < numSplatMaps; ++i)
        readSplatMap(s, sm, desc, i);
      int curUV = numSplatMaps;
      int curMap = 0;
      int curChan = startChannel;
      for (int curTex = 0; curTex < numTextures; ++curTex)
      {
        if (curChan == desc.numChannels)
        {
          curChan = 0;
          ++curMap;
        }
        readTexture(s, sm, curMap, curTex, curChan, curUV);
        ++curChan;
        ++curUV;
      }
      s << "  ;\n}\n";
    }
    
    void readSplatMap(ostringstream& s, ShaderModel sm, const MapFormatDesc& desc, int i)
    {
      s << "  " << desc.type << " cov" << i << " = tex2D(map" << i << ", ";
      if (sm < SM_20)
        s << "uv" << i;
      else
        s << "uv0";
      s << ")." << desc.channels << ";\n";
    }
    
    void readTexture(ostringstream& s, ShaderModel sm, int curMap, int curTex, 
      int curChan, int curUV)
    {
      static const char CHAN_MAP[4] = {'x','y','z','w'};

      if (curTex == 0)
        s << "  color = ";
      else
        s << "        + ";
      s << "tex2D(tex" << curTex << ", uv";
      if (sm < SM_20)
        s << curUV;
      else
        s << "1";
      s << ") * cov" << curMap << "." << CHAN_MAP[curChan] << "\n";
    }
    
  };
  
  
  
  ShaderGenerator* ShaderGenerator::create()
  {
    return new CgShaderGenerator;
  }
  
  void ShaderGenerator::destroy(ShaderGenerator* generator)
  {
    delete generator;
  }
}

