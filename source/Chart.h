#ifndef CHART_H
#define CHART_H

/*-----------------------------------------------------------------------------------------------*/

#include "Event.h"

/*-----------------------------------------------------------------------------------------------*/

//! Simple chart class (renders to a OGRE texture)
class Chart {
public:

	//! Create a chart
	Chart(std::string pName, size_t pWidth, size_t pHeight, size_t pPo2Width, size_t pPo2Height,
		  bool pYearLine = false, bool pCommonScale = false, bool pNullLine = false);

	//! Add a set of values, first set sets number of values
	void addValueSet(std::vector<int> pValues);

	//! Set colours for the graphs
	void setColourSet(std::vector<Ogre::ColourValue> pColours);

	//! Get graph texture
	Ogre::TexturePtr getTexture();

	//! Set values which are visible, changes scale if necessary
	void setValuesVisible(std::vector<bool> pValues);

	//! Force repaint (i.e. on startup)
	void forceRepaint(void);

	//! Serialize
	void serializeIntoXMLElement(TiXmlElement* pParentElement);

	//! Load
	void deserializeFromXMLElement(TiXmlNode* pParentNode);

private:

	//! Draw everything
	void draw(void);

	//! Month passed event handler (inserts month-line)
	void monthPassed(EventData* pData);

	//! Year passed event handler (inserts year-line)
	void yearPassed(EventData* pData);

	//! Repaint everything
	void repaint(void);

	//! Get index for texture buffer
	size_t indexAt(size_t pX, size_t pY);

	//! Set colour on texture buffer
	void setColourAt(size_t pX, size_t pY, Ogre::ColourValue pColour);

	std::vector<Ogre::ColourValue> mColourSet; //!< The colour set
	std::vector<std::vector<int> > mValueData; //!< All the values
	std::vector<bool> mVisible; //!< Value visible?
	std::vector<bool> mMonthLine; //!< Insert month line this iteration?
	Ogre::TexturePtr mTexture; //!< Texture representation
	bool mYearLine; //!< Insert year line?
	size_t mPo2Width; //!< Smalles possible power-of-two size of texture (older gpus)
	size_t mPo2Height;
	size_t mWidth; //!< 'Real' graph size
	size_t mHeight;
	Ogre::uint8* mChartN; //!< Texture buffer for graphs
	Ogre::uint8* mChartBackgroundN; //!< Texture buffer for background
	size_t mBufferSize; //!< Texture buffer size
	bool mCommonScale; //!< Use a common scale for all values?
	bool mNullLine; //!< Show 0-line
};

/*-----------------------------------------------------------------------------------------------*/

#endif // CHART_H
