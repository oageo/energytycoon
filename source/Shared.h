#ifndef SHARED_H
#define SHARED_H

/*-----------------------------------------------------------------------------------------------*/

// Data path for different builds
#ifdef WIN32
	#ifdef _DEBUG
		const std::string cDataDirPre = "../../data/";
	#else
		const std::string cDataDirPre = "../data/"; 
	#endif
#else
	const std::string cDataDirPre = "/usr/share/games/energytycoon/data/"; 
#endif

const int cDemoSeconds = 900;

/*-----------------------------------------------------------------------------------------------*/

//! Convert something to a string
template<typename T> std::string toString(T pData)
{
	std::stringstream lStream;
	lStream << pData;
	return lStream.str();
}

/*-----------------------------------------------------------------------------------------------*/

//! Convert something to a string
template<typename T> std::string toStringSigned(T pData)
{
	std::stringstream lStream;
	lStream << pData;
	std::string lReturn = lStream.str();
	if (pData > 0)
		return "+" + lReturn;
	return lReturn;
}

/*-----------------------------------------------------------------------------------------------*/

//! Convert something to a wstring
template<typename T> std::wstring toStringW(T pData)
{
	std::wstringstream lStream;
	lStream << pData;
	return lStream.str();
}

/*-----------------------------------------------------------------------------------------------*/

//! Convert a string to a number
template<typename T> T toNumber(std::string pString)
{
	T lReturn;
	std::stringstream lStream;
	lStream << pString;
	lStream >> lReturn;
	return lReturn;
}

/*-----------------------------------------------------------------------------------------------*/

//! Representing a rotated rectangle
class RotatedRect
{
public:

	//! Create from 4 points
	RotatedRect(Ogre::Vector2 pUL, Ogre::Vector2 pUR, Ogre::Vector2 pBL, Ogre::Vector2 pBR):
	  mUL(pUL), mUR(pUR), mBL(pBL), mBR(pBR)
	{}

    //! Create from axis-aligned rectangle
	RotatedRect(Ogre::Rect pNorm)
	{
		mUL.x = pNorm.left;
		mUL.y = pNorm.top;

		mUR.x = pNorm.right;
		mUR.y = pNorm.top;

		mBL.x = pNorm.left;
		mBL.y = pNorm.bottom;

		mBR.x = pNorm.right;
		mBR.y = pNorm.bottom;
	}

	Ogre::Vector2 mUL, mUR, mBL, mBR;
};

/*-----------------------------------------------------------------------------------------------*/

//! Check if two rotated rectangles intersect
static bool intersectRotated(RotatedRect &pRect1, RotatedRect &pRect2)
{
	if (abs(pRect1.mBL.x - pRect2.mBL.x) > 70.0)
		return false;

	if (abs(pRect1.mBL.y - pRect2.mBL.y) > 70.0)
		return false;

	Ogre::Vector2 lAxis[8];

	lAxis[0] = (pRect1.mUR - pRect1.mUL).perpendicular();
	lAxis[1] = (pRect1.mBR - pRect1.mBL).perpendicular();
	lAxis[2] = (pRect1.mUR - pRect1.mBR).perpendicular();
	lAxis[3] = (pRect1.mUL - pRect1.mBL).perpendicular();
	lAxis[4] = (pRect2.mUR - pRect2.mUL).perpendicular();
	lAxis[5] = (pRect2.mBR - pRect2.mBL).perpendicular();
	lAxis[6] = (pRect2.mUR - pRect2.mBR).perpendicular();
	lAxis[7] = (pRect2.mUL - pRect2.mBL).perpendicular();

	for (int i = 0; i < 8; i++) {
		Ogre::Vector2 lProjUR1, lProjUL1, lProjBL1, lProjBR1, lProjUR2, lProjUL2, lProjBL2, lProjBR2;
		float lScalUR1, lScalUL1, lScalBL1, lScalBR1, lScalUR2, lScalUL2, lScalBL2, lScalBR2;

		lProjUL1.x = ( ( (pRect1.mUL.x * lAxis[i].x) + (pRect1.mUL.y * lAxis[i].y) ) 
			/ ((lAxis[i].x * lAxis[i].x) + (lAxis[i].y * lAxis[i].y)) ) * lAxis[i].x;
		lProjUL1.y = ( ( (pRect1.mUL.x * lAxis[i].x) + (pRect1.mUL.y * lAxis[i].y) ) 
			/ ((lAxis[i].x * lAxis[i].x) + (lAxis[i].y * lAxis[i].y)) ) * lAxis[i].y;
		lProjBL1.x = ( ( (pRect1.mBL.x * lAxis[i].x) + (pRect1.mBL.y * lAxis[i].y) ) 
			/ ((lAxis[i].x * lAxis[i].x) + (lAxis[i].y * lAxis[i].y)) ) * lAxis[i].x;
		lProjBL1.y = ( ( (pRect1.mBL.x * lAxis[i].x) + (pRect1.mBL.y * lAxis[i].y) ) 
			/ ((lAxis[i].x * lAxis[i].x) + (lAxis[i].y * lAxis[i].y)) ) * lAxis[i].y;
		lProjUR1.x = ( ( (pRect1.mUR.x * lAxis[i].x) + (pRect1.mUR.y * lAxis[i].y) ) 
			/ ((lAxis[i].x * lAxis[i].x) + (lAxis[i].y * lAxis[i].y)) ) * lAxis[i].x;
		lProjUR1.y = ( ( (pRect1.mUR.x * lAxis[i].x) + (pRect1.mUR.y * lAxis[i].y) ) 
			/ ((lAxis[i].x * lAxis[i].x) + (lAxis[i].y * lAxis[i].y)) ) * lAxis[i].y;
		lProjBR1.x = ( ( (pRect1.mBR.x * lAxis[i].x) + (pRect1.mBR.y * lAxis[i].y) ) 
			/ ((lAxis[i].x * lAxis[i].x) + (lAxis[i].y * lAxis[i].y)) ) * lAxis[i].x;
		lProjBR1.y = ( ( (pRect1.mBR.x * lAxis[i].x) + (pRect1.mBR.y * lAxis[i].y) ) 
			/ ((lAxis[i].x * lAxis[i].x) + (lAxis[i].y * lAxis[i].y)) ) * lAxis[i].y;

		lProjUL2.x = ( ( (pRect2.mUL.x * lAxis[i].x) + (pRect2.mUL.y * lAxis[i].y) ) 
			/ ((lAxis[i].x * lAxis[i].x) + (lAxis[i].y * lAxis[i].y)) ) * lAxis[i].x;
		lProjUL2.y = ( ( (pRect2.mUL.x * lAxis[i].x) + (pRect2.mUL.y * lAxis[i].y) ) 
			/ ((lAxis[i].x * lAxis[i].x) + (lAxis[i].y * lAxis[i].y)) ) * lAxis[i].y;
		lProjBL2.x = ( ( (pRect2.mBL.x * lAxis[i].x) + (pRect2.mBL.y * lAxis[i].y) ) 
			/ ((lAxis[i].x * lAxis[i].x) + (lAxis[i].y * lAxis[i].y)) ) * lAxis[i].x;
		lProjBL2.y = ( ( (pRect2.mBL.x * lAxis[i].x) + (pRect2.mBL.y * lAxis[i].y) ) 
			/ ((lAxis[i].x * lAxis[i].x) + (lAxis[i].y * lAxis[i].y)) ) * lAxis[i].y;
		lProjUR2.x = ( ( (pRect2.mUR.x * lAxis[i].x) + (pRect2.mUR.y * lAxis[i].y) ) 
			/ ((lAxis[i].x * lAxis[i].x) + (lAxis[i].y * lAxis[i].y)) ) * lAxis[i].x;
		lProjUR2.y = ( ( (pRect2.mUR.x * lAxis[i].x) + (pRect2.mUR.y * lAxis[i].y) ) 
			/ ((lAxis[i].x * lAxis[i].x) + (lAxis[i].y * lAxis[i].y)) ) * lAxis[i].y;
		lProjBR2.x = ( ( (pRect2.mBR.x * lAxis[i].x) + (pRect2.mBR.y * lAxis[i].y) ) 
			/ ((lAxis[i].x * lAxis[i].x) + (lAxis[i].y * lAxis[i].y)) ) * lAxis[i].x;
		lProjBR2.y = ( ( (pRect2.mBR.x * lAxis[i].x) + (pRect2.mBR.y * lAxis[i].y) ) 
			/ ((lAxis[i].x * lAxis[i].x) + (lAxis[i].y * lAxis[i].y)) ) * lAxis[i].y;

		lScalUR1 = lProjUR1.dotProduct(lAxis[i]);
		lScalUL1 = lProjUL1.dotProduct(lAxis[i]);
		lScalBL1 = lProjBL1.dotProduct(lAxis[i]);
		lScalBR1 = lProjBR1.dotProduct(lAxis[i]);
		lScalUR2 = lProjUR2.dotProduct(lAxis[i]);
		lScalUL2 = lProjUL2.dotProduct(lAxis[i]);
		lScalBL2 = lProjBL2.dotProduct(lAxis[i]);
		lScalBR2 = lProjBR2.dotProduct(lAxis[i]);

		if ((std::min(std::min(lScalUR2, lScalUL2), std::min(lScalBL2, lScalBR2)) 
			>= std::max(std::max(lScalUR1, lScalUL1), std::max(lScalBL1, lScalBR1)))
			|| (std::min(std::min(lScalUR1, lScalUL1), std::min(lScalBL1, lScalBR1)) 
			>= std::max(std::max(lScalUR2, lScalUL2), std::max(lScalBL2, lScalBR2)))) {
			return false;
		}
	}

	return true;
}

/*-----------------------------------------------------------------------------------------------*/

//! Check if two lines intersect ( (p1-p2) vs (p3-p4) )
static bool intersectLines(Ogre::Vector2 p1, Ogre::Vector2 p2, Ogre::Vector2 p3, Ogre::Vector2 p4)
{
	float x1, x2, x3, x4, y1, y2, y3, y4;

	x1 = p1.x;
	x2 = p2.x;
	x3 = p3.x;
	x4 = p4.x;

	y1 = p1.y;
	y2 = p2.y;
	y3 = p3.y;
	y4 = p4.y;

	float lDenominator = ((y4-y3)*(x2-x1)) - ((x4-x3)*(y2-y1));

	if (abs(lDenominator) > 0.00001) {
		float lUa = (((x4-x3)*(y1-y3)) - ((y4-y3)*(x1-x3)))
			/ lDenominator;

		float lUb = (((x2-x1)*(y1-y3)) - ((y2-y1)*(x1-x3)))
			/ lDenominator;

		if ((lUa <= 0.9999) && (lUb <= 0.9999) && (lUa >= 0.0001) && (lUb >= 0.0001))
			return true;
	}

	return false;
}

/*-----------------------------------------------------------------------------------------------*/

//! Calculate determinant
static float det(Ogre::Vector2 p1, Ogre::Vector2 p2)
{
	return (p1.x * p2.y) - (p1.y * p2.x);
}

/*-----------------------------------------------------------------------------------------------*/

//! Find intersection point
static Ogre::Vector2 intersection(Ogre::Vector2 p1, Ogre::Vector2 p2, Ogre::Vector2 p3,
                                  Ogre::Vector2 p4)
{
	Ogre::Vector2 lA1 = p2 - p1;
	Ogre::Vector2 lA2 = p4 - p3;

	float lDetS = det(p3 - p1, -1 * lA2);
	float lDet = det(lA1, -1 * lA2);
	float lS = lDetS / lDet;

	Ogre::Vector2 lResult;
	lResult = p1 + (lS * lA1);
	return lResult;
}

/*-----------------------------------------------------------------------------------------------*/

//! Random bool shortcut
static bool randBool(void)
{
	return ((rand() % 2) == 0) ? true : false;
}

/*-----------------------------------------------------------------------------------------------*/

//! Random from percentage shortcut (pPercent% chance for true)
static bool randPercent(double pPercent)
{
	// sanitize input
	if (pPercent > 100.0)
		pPercent = 100.0;
	else if (pPercent < 0.0)
		pPercent = 0.0;

	int lRandomValue = rand();

	if (lRandomValue < ((RAND_MAX/100)*pPercent))
		return true;
	return false;
}

/*-----------------------------------------------------------------------------------------------*/

//! Random from interval shortcut
static int randomInt(int pFrom, int pTo)
{
	return (rand() % (pFrom-pTo) + pFrom);
}

/*-----------------------------------------------------------------------------------------------*/

//! Create axis-aligned rect from point
static Ogre::Rect rectFromPoint2(Ogre::Vector2 pPosition, float pBorderLength)
{
	Ogre::Rect lTemp;
	
	lTemp.left = pPosition.x - (pBorderLength/2);
	lTemp.right = pPosition.x + (pBorderLength/2);
	lTemp.top = pPosition.y - (pBorderLength/2);
	lTemp.bottom = pPosition.y + (pBorderLength/2);

	return lTemp;
}

/*-----------------------------------------------------------------------------------------------*/

//! Create axis-aligned rect from point
static Ogre::Rect rectFromPoint3(Ogre::Vector3 pPosition, float pBorderLength)
{
	Ogre::Rect lTemp;
	
	lTemp.left = pPosition.x - (pBorderLength/2);
	lTemp.right = pPosition.x + (pBorderLength/2);
	lTemp.top = pPosition.z - (pBorderLength/2);
	lTemp.bottom = pPosition.z + (pBorderLength/2);

	return lTemp;
}

/*-----------------------------------------------------------------------------------------------*/

#ifndef WIN32
//! unix find files function, not to be used directly, use findFilesInDirectory instead
static int getFilesFromDirectory(std::string pDirectory, std::vector<std::string> &oFiles)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(pDirectory.c_str())) == NULL)
    {
        std::cout << "Error(" << errno << ") opening " << pDirectory << std::endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL)
    {
        oFiles.push_back(std::string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}
#endif

/*-----------------------------------------------------------------------------------------------*/

//! Find all files in a directory
static std::vector<std::string> findFilesInDirectory(std::string pDirectory, bool pCutExt = true,
                                                     bool pRecursive = false)
{
	std::vector<std::string> lFilenames;

#ifdef WIN32
	HANDLE lHandle;
	WIN32_FIND_DATA lFindData;

	lHandle = FindFirstFile((pDirectory + "\\*").c_str(), &lFindData);

	do {
		if (!((lFindData.cFileName[0] == '.') 
			&& ((lFindData.cFileName[1] == '.' 
			&& lFindData.cFileName[2] == 0) 
			|| lFindData.cFileName[1] == 0 ))) {
			if (lFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if (pRecursive) {
					std::vector<std::string> lSubdirFilenames 
            = findFilesInDirectory(lFindData.cFileName, false, true);
					lFilenames.insert(lFilenames.end(), lSubdirFilenames.begin(), lSubdirFilenames.end());
				}
			} else {
				lFilenames.push_back(lFindData.cFileName);
			}
		}
	}
	while (FindNextFile(lHandle, &lFindData));
	FindClose(lHandle);
#else
  getFilesFromDirectory(pDirectory, lFilenames);
#endif

	for (size_t i = 0; i < lFilenames.size(); i++) {
		lFilenames[i] = lFilenames[i].substr(lFilenames[i].find("/") + 1);
		if (pCutExt) {
			lFilenames[i] = lFilenames[i].substr(0, lFilenames[i].find("."));
		}
	}
	return lFilenames;
}

/*-----------------------------------------------------------------------------------------------*/

//! Shortcut to get some data from mission XML file
static std::vector<std::string> getStartDataFromMission(std::string pMissionFilename,
                                                        std::string pLanguage)
{
	std::vector<std::string> lReturn;
	boost::shared_ptr<TiXmlDocument> lMission(new TiXmlDocument((cDataDirPre 
		+ "missions/" + pMissionFilename).c_str()));

	lMission->LoadFile(TIXML_ENCODING_UTF8);

	TiXmlNode* rootNode = lMission->FirstChildElement("mission");

	TiXmlNode* nameNode = rootNode->FirstChildElement("name");

	while (nameNode) {
		if (((std::string)nameNode->ToElement()->Attribute("language")) == pLanguage) {
			lReturn.push_back(nameNode->ToElement()->GetText());
			break;
		}

		nameNode = nameNode->NextSiblingElement("name");
	}

	lReturn.push_back(rootNode->ToElement()->Attribute("map"));

	TiXmlNode* node = rootNode->FirstChildElement("start_conditions");

	lReturn.push_back(node->ToElement()->Attribute("name"));

	return lReturn;
}

/*-----------------------------------------------------------------------------------------------*/

//! Shortcut to get mission goals
static std::vector<std::string> getMissionGoalsFromMission(std::string pMissionFilename,
                                                           std::string pLanguage,
                                                           std::string pNone)
{
	std::vector<std::string> lReturn;
	boost::shared_ptr<TiXmlDocument> lMission(new TiXmlDocument((cDataDirPre 
		+ "missions/" + pMissionFilename).c_str()));

	lMission->LoadFile(TIXML_ENCODING_UTF8);

	TiXmlNode* rootNode = lMission->FirstChildElement("mission");
	TiXmlNode* node = rootNode->FirstChildElement("mission_goal");

	while(node) {
		std::string lText = "";

		TiXmlNode* textNode = node->FirstChildElement("text");

		while(textNode) {
			if(((std::string)textNode->ToElement()->Attribute("language")) == pLanguage)
				lText = textNode->ToElement()->GetText();

			textNode = textNode->NextSiblingElement("text");
		}

		std::string lType = node->ToElement()->Attribute("type");
		lReturn.push_back(lText);
		node = node->NextSiblingElement("mission_goal");
	}

	return lReturn;
}

/*-----------------------------------------------------------------------------------------------*/

//! Log something
template<typename T> void logData(T pData)
{
	std::ofstream lFile;
	
	lFile.open("log.txt", std::ios::app);
	lFile << pData << std::endl;
	lFile.close();
}

/*-----------------------------------------------------------------------------------------------*/

#endif // SHARED_H