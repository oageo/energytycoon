#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

/*-----------------------------------------------------------------------------------------------*/

//! Supertype of game objects
enum eGameObjectType { eCity, ePowerplant, eRiver, eStreet, eTransfer, eStatic,
					   ePole, eLine, eResource, eCompany};

/*-----------------------------------------------------------------------------------------------*/

//! Subtype of objects
enum eCompanyObjectType { ePPNuclearSmall, ePPNuclearLarge, 
						  ePPCoalSmall, ePPCoalLarge,
						  ePPGasSmall, ePPGasLarge, 
						  ePPWindSmall, ePPWindLarge, ePPWindOffshore,
						  ePPWater,
						  ePPBio,
						  ePPSolarSmall, ePPSolarLarge,
						  ePPSolarUpdraft,
						  ePPNuclearFusion,
						  eREGasSmall, eREGasLarge,
						  eREUraniumSmall, eREUraniumLarge,
						  eRECoalSmall, eRECoalLarge,
						  eDIPoleSmall, eDIPoleLarge, 
						  eDIIntersectionSmall, eDIIntersectionLarge,
						  eCOHeadquarters, eCOResearch, eCOPublicRelations, eNull };

/*-----------------------------------------------------------------------------------------------*/

//! Selected marker scale
const float cSelectorScale[] = {	0.5, 0.5,
									0.5, 0.5,
									0.5, 0.5,
									0.25, 0.5, 0.5,
									0.5,
									0.5,
									0.5, 0.5,
									0.5,
									0.5,
									0.5, 0.5,
									0.5, 0.5,
									0.5, 0.5,
									0.5, 0.5,
									0.5, 0.5,
									0.5, 0.5, 0.5, 0.5 };

/*-----------------------------------------------------------------------------------------------*/

//! Mesh for specific subtype
const std::string cMeshFilename[] = { "nuc_small.mesh", "pp_nuclear_large.mesh",
								 "pp_coal_small.mesh", "pp_coal_large.mesh",
								 "pp_gas_small.mesh", "pp_gas_large.mesh",
								 "pp_wind_small.mesh", "pp_wind_large.mesh", "pp_wind_offshore.mesh",
								 "pp_water.mesh",
								 "pp_bio.mesh",
								 "pp_solar_small.mesh", "pp_solar_large.mesh",
								 "pp_solar_updraft.mesh",
								 "pp_nuclear_fusion.mesh",
								 "re_gas_small.mesh", "re_gas_large.mesh",
								 "re_uranium_small.mesh", "re_uranium_large.mesh",
								 "re_coal_small.mesh", "re_coal_large.mesh",
								 "strommastklein.mesh", "strommast.mesh",
								 "di_intersection_small.mesh", "di_intersection_large.mesh",
								 "co_headquarters.mesh", "co_research.mesh", "co_public_relations.mesh" };

/*-----------------------------------------------------------------------------------------------*/

//! Subtype border length
const size_t cStructureBorderLength[] = { 34, 34,
									   26, 26,
									   26, 26,
									   12, 16, 16,
									   26,
									   26,
									   16, 33,
									   33,
									   26,
									   16, 26,
									   16, 26,
									   16, 26,
									   6, 8,
									   6, 8,
									   26, 26, 26 };

/*-----------------------------------------------------------------------------------------------*/

//! Subtype structure scale
const Ogre::Vector3 cStructureScale[] = { Ogre::Vector3(1.0, 1.0, 1.0), Ogre::Vector3(0.65, 0.65, 0.65), 
								  Ogre::Vector3(1.0, 1.0, 1.0), Ogre::Vector3(1.0, 1.0, 1.0),
								  Ogre::Vector3(1.0, 1.0, 1.0), Ogre::Vector3(1.0, 1.0, 1.0),
								  Ogre::Vector3(0.75,0.75,0.75), Ogre::Vector3(0.75,0.75,0.75), Ogre::Vector3(0.75,0.75,0.75),
								  Ogre::Vector3(1.0, 1.0, 1.0),
								  Ogre::Vector3(1.0, 1.0, 1.0),
								  Ogre::Vector3(1.0, 1.0, 1.0), Ogre::Vector3(0.6, 0.6, 0.6),
								  Ogre::Vector3(0.9, 0.9, 0.9),
								  Ogre::Vector3(0.6, 0.6, 0.6),
								  Ogre::Vector3(0.85, 0.85, 0.85), Ogre::Vector3(0.85, 0.85, 0.85),
								  Ogre::Vector3(0.85, 0.85, 0.85), Ogre::Vector3(0.85, 0.85, 0.85),
								  Ogre::Vector3(0.85, 0.85, 0.85), Ogre::Vector3(0.85, 0.85, 0.85),
								  Ogre::Vector3(1.0, 1.0, 1.0), Ogre::Vector3(0.1, 0.1, 0.1),
								  Ogre::Vector3(1.0, 1.0, 1.0), Ogre::Vector3(1.0, 1.0, 1.0),
								  Ogre::Vector3(1.0, 1.0, 1.0), Ogre::Vector3(1.7, 1.7, 1.7), Ogre::Vector3(1.0, 1.0, 1.0) };

/*-----------------------------------------------------------------------------------------------*/

//! Interface for all game objects
class GameObject {
public:

	//! Create it
	GameObject(void)
	{
		mID = mIDCounter;
		mIDCounter++;
	}

	//! Return ID
	std::string getID(void)
	{
		std::stringstream lTemp;
		lTemp << mID;

		return lTemp.str();
	}

	//! Set ID, only for loading games
	void setID(size_t pID)
	{
		if(mIDCounter <= pID)
			mIDCounter = pID+1;

		mID = pID;
	}

	//! Reset ID counter (for restarting game)
	static void resetIDCounter(void)
	{
		mIDCounter = 0;
	}

	//! Get Position (center)
	virtual Ogre::Vector2 getPosition(void) = 0;

	//! Is the object selectable on the map
	virtual bool isSelectable(void) = 0;

	//! Get supertype
	virtual eGameObjectType getType(void) = 0;

	//! Get subtype
	virtual eCompanyObjectType getSubtype(void) = 0;

private:

	size_t mID;
	static size_t mIDCounter;
};

/*-----------------------------------------------------------------------------------------------*/

//! Highlightable interface
class Highlightable {
public:

	//! Highlight object
	virtual void highlight(bool pHighlight) = 0;
};

/*-----------------------------------------------------------------------------------------------*/

#endif // GAMEOBJECT_H
