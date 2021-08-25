#ifndef GAMECONFIG_H
#define GAMECONFIG_H

/*-----------------------------------------------------------------------------------------------*/

#include "StdAfx.h"
#include "GameObject.h"

/*-----------------------------------------------------------------------------------------------*/

const std::string cCOVImage = "_Image";
const std::string cCOVPower = "_Power";
const std::string cCOVOpCost = "_OpCost";
const std::string cCOVEnvCost = "_EnvCost";
const std::string cCOVCost = "_Cost";
const std::string cCOVRate = "_Rate";

/*-----------------------------------------------------------------------------------------------*/

//! Handles all game config parameters
class GameConfig {
public:

	//! Load the game.cfg file
	static void load(std::string pFilename) 
	{
		mConversionMap[(int)ePPNuclearSmall] = "PPNuclearSmall";
		mConversionMap[(int)ePPNuclearLarge] = "PPNuclearLarge";
		mConversionMap[(int)ePPCoalSmall] = "PPCoalSmall";
		mConversionMap[(int)ePPCoalLarge] = "PPCoalLarge";
		mConversionMap[(int)ePPGasSmall] = "PPGasSmall";
		mConversionMap[(int)ePPGasLarge] = "PPGasLarge";
		mConversionMap[(int)ePPWindSmall] = "PPWindSmall";
		mConversionMap[(int)ePPWindLarge] = "PPWindLarge";
		mConversionMap[(int)ePPWindOffshore] = "PPWindOffshore";
		mConversionMap[(int)ePPWater] = "PPWater";
		mConversionMap[(int)ePPBio] = "PPBio";
		mConversionMap[(int)ePPSolarSmall] = "PPSolarSmall";
		mConversionMap[(int)ePPSolarLarge] = "PPSolarLarge";
		mConversionMap[(int)ePPSolarUpdraft] = "PPSolarUpdraft";
		mConversionMap[(int)ePPNuclearFusion] = "PPNuclearFusion";
		mConversionMap[(int)eREGasSmall] = "REGasSmall";
		mConversionMap[(int)eREGasLarge] = "REGasLarge";
		mConversionMap[(int)eREUraniumSmall] = "REUraniumSmall";
		mConversionMap[(int)eREUraniumLarge] = "REUraniumLarge";
		mConversionMap[(int)eRECoalSmall] = "RECoalSmall";
		mConversionMap[(int)eRECoalLarge] = "RECoalLarge";
		mConversionMap[(int)eDIPoleSmall] = "DIPoleSmall";
		mConversionMap[(int)eDIPoleLarge] = "DIPoleLarge";
		mConversionMap[(int)eDIIntersectionSmall] = "DIIntersectionSmall";
		mConversionMap[(int)eDIIntersectionLarge] = "DIIntersectionLarge";
		mConversionMap[(int)eCOHeadquarters] = "COHeadquarters";
		mConversionMap[(int)eCOResearch] = "COResearch";
		mConversionMap[(int)eCOPublicRelations] = "COPublicRelations";
		mConversionMap[(int)eNull] = "";

		std::ifstream lConfigFile;
		std::string lLine;

		lConfigFile.open(pFilename.c_str());

		while (lConfigFile >> lLine) {
			if ((lLine.substr(0,2) != "//") && !lLine.empty()) {
				std::string lKey = lLine.substr(0, lLine.find("="));
				std::remove(lKey.begin(), lKey.end(), ' ');

				std::string lValue = lLine.substr(lLine.find("=")+1);
				std::remove(lValue.begin(), lValue.end(), ' ');

				mData[lKey] = lValue;
			}
		}

		lConfigFile.close();
	}

  //! Convert companyobjecttype to string
	static std::string cCOTS(eCompanyObjectType pType)
	{
		return mConversionMap[pType];
	}

	//! Get a double value
	static double getDouble(std::string pKey, std::string pAppend = "") 
	{
		if (mData.find(pKey + pAppend) == mData.end())
#ifdef WIN32
			MessageBox(0, pKey.c_str(), "CRITICAL WARNING: KEY NOT FOUND", MB_OK);
#else
      std::cout << "CRITICAL WARNING: KEY NOT FOUND: " << pKey << std::endl;
#endif

		return toNumber<double>(mData[pKey + pAppend]);
	}

	//! Get a bool value
	static bool getBool(std::string pKey, std::string pAppend = "") 
	{
		if (mData.find(pKey + pAppend) == mData.end())
#ifdef WIN32
			MessageBox(0, pKey.c_str(), "CRITICAL WARNING: KEY NOT FOUND", MB_OK);
#else
      std::cout << "CRITICAL WARNING: KEY NOT FOUND: " << pKey << std::endl;
#endif

		return toNumber<bool>(mData[pKey + pAppend]);
	}

	//! Get a int value
	static int getInt(std::string pKey, std::string pAppend = "") 
	{
		if (mData.find(pKey + pAppend) == mData.end())
#ifdef WIN32
			MessageBox(0, pKey.c_str(), "CRITICAL WARNING: KEY NOT FOUND", MB_OK);
#else
      std::cout << "CRITICAL WARNING: KEY NOT FOUND: " << pKey << std::endl;
#endif

		return toNumber<int>(mData[pKey + pAppend]);
	}

	//! Get a string value
	static std::string getString(std::string pKey, std::string pAppend = "") 
	{
		if (mData.find(pKey + pAppend) == mData.end())
#ifdef WIN32
			MessageBox(0, pKey.c_str(), "CRITICAL WARNING: KEY NOT FOUND", MB_OK);
#else
      std::cout << "CRITICAL WARNING: KEY NOT FOUND: " << pKey << std::endl;
#endif

		return mData[pKey + pAppend];
	}

  //! Get a OGRE vector3 value
  static Ogre::Vector3 getVector3(std::string pKey, std::string pAppend = "")
  {
		if (mData.find(pKey + pAppend) == mData.end())
#ifdef WIN32
			MessageBox(0, pKey.c_str(), "CRITICAL WARNING: KEY NOT FOUND", MB_OK);
#else
      std::cout << "CRITICAL WARNING: KEY NOT FOUND: " << pKey << std::endl;
#endif

    std::string lValue = mData[pKey + pAppend];

    std::string lX = lValue.substr(0, lValue.find_first_of(","));
    std::string lY = lValue.substr(lValue.find_first_of(",") + 1,
                                   lValue.find_last_of(",") - lValue.find_first_of(",") - 1);
    std::string lZ = lValue.substr(lValue.find_last_of(",") + 1,
                                   lValue.length() - lValue.find_last_of(","));

    Ogre::Vector3 lReturn = Ogre::Vector3(toNumber<double>(lX), toNumber<double>(lY),
                                          toNumber<double>(lZ));
    return lReturn;
  }

  static bool hasKey(std::string pKey)
  {
    return (mData.find(pKey) != mData.end());
  }

private:

	static std::map<std::string, std::string> mData;
	static std::map<int, std::string> mConversionMap;
};

/*-----------------------------------------------------------------------------------------------*/

#endif // GAMECONFIG_H
