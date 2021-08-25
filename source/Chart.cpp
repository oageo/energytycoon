#include "StdAfx.h"
#include "Chart.h"

/*-----------------------------------------------------------------------------------------------*/

Chart::Chart(std::string pName, size_t pWidth, size_t pHeight, size_t pPo2Width,
             size_t pPo2Height, bool pYearLine, bool pCommonScale, bool pNullLine)
: mPo2Width(pPo2Width),
  mPo2Height(pPo2Height),
  mWidth(pWidth),
  mHeight(pHeight),
  mBufferSize(0),
	mYearLine(pYearLine),
  mCommonScale(pCommonScale),
  mNullLine(pNullLine)
{
	// some default colours...
	mColourSet.push_back(Ogre::ColourValue(1.0f, 0.0f, 0.0f));
	mColourSet.push_back(Ogre::ColourValue(0.0f, 1.0f, 0.0f));
	mColourSet.push_back(Ogre::ColourValue(0.0f, 0.0f, 1.0f));
	mColourSet.push_back(Ogre::ColourValue(1.0f, 1.0f, 0.0f));
	mColourSet.push_back(Ogre::ColourValue(1.0f, 0.0f, 1.0f));
	mColourSet.push_back(Ogre::ColourValue(0.0f, 1.0f, 1.0f));
	mColourSet.push_back(Ogre::ColourValue(1.0f, 1.0f, 1.0f));

	// default visiblity
	mVisible.push_back(true);
	mVisible.push_back(true);
	mVisible.push_back(true);
	mVisible.push_back(true);
	mVisible.push_back(true);
	mVisible.push_back(true);
	mVisible.push_back(true);

	mBufferSize = pPo2Width*pPo2Height*4;

	mChartBackgroundN = new Ogre::uint8[mBufferSize];
	mChartN = new Ogre::uint8[mBufferSize];

	for (size_t a = 0; a < mBufferSize; a++)
		mChartBackgroundN[a] = 0;

	for (size_t i = 0; i < pWidth; i++) {
		for (size_t j = 0; j < pHeight; j++) {
			mChartBackgroundN[indexAt(i,j)] = 80;
			mChartBackgroundN[indexAt(i,j)+1] = 80;
			mChartBackgroundN[indexAt(i,j)+2] = 80;
			mChartBackgroundN[indexAt(i,j)+3] = 255;
		}
	}

	std::vector<std::string> lResourceGroups = 
		Ogre::ResourceGroupManager::getSingleton().getResourceGroups();

	bool lFound = false;

	for (size_t l = 0; l < lResourceGroups.size(); l++)
		if (lResourceGroups[l].find("ChartGroup") != -1)
			lFound = true;

	if (!lFound)
		Ogre::ResourceGroupManager::getSingleton().createResourceGroup("ChartGroup");

	mTexture = Ogre::TextureManager::getSingleton().createManual(
    pName, "ChartGroup", Ogre::TEX_TYPE_2D, mPo2Width, pPo2Height, 1, 1,
    Ogre::PF_BYTE_BGRA, Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

	repaint();

	REGISTER_CALLBACK(eMonthPassed, Chart::monthPassed);
	REGISTER_CALLBACK(eYearPassed, Chart::yearPassed);
}

/*-----------------------------------------------------------------------------------------------*/

size_t Chart::indexAt(size_t pX, size_t pY)
{
	return ((pY * mPo2Width * 4) + (pX * 4));
}

/*-----------------------------------------------------------------------------------------------*/

void Chart::setColourAt(size_t pX, size_t pY, Ogre::ColourValue pColour)
{
	mChartN[indexAt(pX, pY)] = pColour.b * 254;
	mChartN[indexAt(pX, pY) + 1] = pColour.g * 254;
	mChartN[indexAt(pX, pY) + 2] = pColour.r * 254;
	mChartN[indexAt(pX, pY) + 3] = pColour.a * 254;
}

/*-----------------------------------------------------------------------------------------------*/

void Chart::monthPassed(EventData* pData)
{
	if (!mYearLine)
		if (mValueData.size() > 0)
			mMonthLine[mValueData.size() - 1] = true;
}

/*-----------------------------------------------------------------------------------------------*/

void Chart::yearPassed(EventData* pData)
{
	if (mYearLine)
		if (mValueData.size() > 0)
			mMonthLine[mValueData.size() - 1] = true;
}

/*-----------------------------------------------------------------------------------------------*/

void Chart::addValueSet(std::vector<int> pValues)
{
	mValueData.push_back(pValues);
	mMonthLine.push_back(false);

	if ((int)mValueData.size() > (mWidth - 20)) {
		mValueData.erase(mValueData.begin());
		mMonthLine.erase(mMonthLine.begin());
	}

	draw();
	repaint();
}

/*-----------------------------------------------------------------------------------------------*/

void Chart::draw(void)
{
	memcpy(mChartN, mChartBackgroundN, mBufferSize);

	if(!mValueData.empty()) {
		float lMax = 0.0;
		float lMin = 0.0;

		std::vector<int> lMaxSet;
		std::vector<int> lMinSet;

		if (mCommonScale) {
			for (size_t h = 0; h < mValueData.size(); h++) {
				for (size_t c = 0; c < mValueData[h].size(); c++) {
					if (mVisible[c])
						if (lMax < mValueData[h][c])
							lMax = mValueData[h][c];
				}
				for (size_t d = 0; d < mValueData[h].size(); d++) {
					if (mVisible[d])
						if (lMin > mValueData[h][d])
							lMin = mValueData[h][d];
				}
			}
		} else {
			for (size_t z = 0; z < mValueData[0].size(); z++) {
				lMaxSet.push_back(0);
				lMinSet.push_back(0);
			}
			for (unsigned int h = 0; h < mValueData.size(); h++) {
				for (size_t c = 0; c < mValueData[h].size(); c++) {
					if (mVisible[c])
						if (lMaxSet[c] < mValueData[h][c])
							lMaxSet[c] = mValueData[h][c];
				}
				for (size_t d = 0; d < mValueData[h].size(); d++) {
					if (mVisible[d])
						if (lMinSet[d] > mValueData[h][d])
							lMinSet[d] = mValueData[h][d];
				}
			}
		}
		for (size_t i = 0; i < mValueData.size(); i++) {
			if (mMonthLine[i]) {
				for (size_t l = 0; l < mHeight; l++) {
					setColourAt(i, l, Ogre::ColourValue(0.4,0.4,0.4,0.4));
				}
			}
			for (size_t j = 0; j < mValueData[i].size(); j++) {
				if (mVisible[j]) {
					if (!mCommonScale) {
						lMax = lMaxSet[j];
						lMin = lMinSet[j];
					}

					float lRange = (lMax - lMin);

					if(lRange == 0)
						lRange = 1;

					size_t lX = i;

					int lMinDist = lMin*-1;

					size_t lY;

					if (lMin < 0) {
						lY =   ( (mHeight-1) )
							- ( (0.1*mHeight-1) )
							- ( ((float)(mValueData[i][j]+lMinDist)/lRange)*(0.8*mHeight-1) );
					} else {
						lY =   ( (mHeight-1) )
							- ( (0.1*mHeight-1) )
							- ( ((float)mValueData[i][j]/lRange)*(0.8*mHeight-1) );
					}

					int lDistanceToLast = 0;

					if (i > 0) {
						int lYLast;

						if (lMin < 0) {
							lYLast =   ( (mHeight-1) )
								- ( (0.1*mHeight-1) )
								- ( ((float)(mValueData[i-1][j]+lMinDist)/lRange)*(0.8*mHeight-1) );
						} else {
							lYLast =   ( (mHeight-1) )
								- ( (0.1*mHeight-1) )
								- ( ((float)mValueData[i-1][j]/lRange)*(0.8*mHeight-1) );
						}

						lDistanceToLast = lY - lYLast;

						if (lDistanceToLast > 0) {
							for (int k = 0; k < lDistanceToLast; k++) {
								if (((lY - k) >= 0) && ((lY - k) < mHeight))
									setColourAt(lX, lY - k, mColourSet[j]);
							}
						} else {
							for (int k = 0; k > lDistanceToLast; k--) {
								if (((lY - k) >= 0) && ((lY - k) < mHeight))
									setColourAt(lX, lY - k, mColourSet[j]);
							}
						}
					}
					if ((lY >= 0) && (lY < mHeight))
						setColourAt(lX, lY, mColourSet[j]);
				}
			}
		}

		if (mNullLine) {
			for (size_t f = 0; f < mWidth; f++) {
				int y;

				if (lMin < 0) {
					y =   ( (mHeight-1) )
						- ( (0.1*mHeight-1) )
						- ( ((float)-1*lMin/(lMax - lMin))*(0.8*mHeight-1) );
				} else {
					y = (mHeight - 1) - (0.1 * mHeight - 1);
				}

				setColourAt(f, y, Ogre::ColourValue(0.4,0.4,0.4));
			}
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Chart::setColourSet(std::vector<Ogre::ColourValue> pColours)
{
	mColourSet = pColours;
}

/*-----------------------------------------------------------------------------------------------*/

void Chart::repaint(void)
{
	Ogre::HardwarePixelBufferSharedPtr lPixelBuffer = mTexture->getBuffer();

	lPixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
	const Ogre::PixelBox& lPixelBox = lPixelBuffer->getCurrentLock();

	Ogre::uint8* lDest = static_cast<Ogre::uint8*>(lPixelBox.data);

	memcpy(lDest, mChartN, mBufferSize);

	lPixelBuffer->unlock();
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::TexturePtr Chart::getTexture()
{
	return mTexture;
}

/*-----------------------------------------------------------------------------------------------*/

void Chart::setValuesVisible(std::vector<bool> pValues)
{
	mVisible = pValues;
}

/*-----------------------------------------------------------------------------------------------*/

void Chart::forceRepaint(void)
{
	draw();
	repaint();
}

/*-----------------------------------------------------------------------------------------------*/

void Chart::serializeIntoXMLElement(TiXmlElement* pParentElement)
{
	for (size_t i = 0; i < mValueData.size(); ++i) {
		TiXmlElement* lValueSet = new TiXmlElement("value_set");

		for (size_t j = 0; j < mValueData[0].size(); ++j) {
			TiXmlElement* lValue = new TiXmlElement("value");
			lValue->SetAttribute("data", mValueData[i][j]);
			lValueSet->LinkEndChild(lValue);
		}

		lValueSet->SetAttribute("month_line", mMonthLine[i] ? "true" : "false");

		pParentElement->LinkEndChild(lValueSet);
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Chart::deserializeFromXMLElement(TiXmlNode* pParentNode)
{
	TiXmlNode* lValueSet = pParentNode->FirstChildElement("value_set");

	while (lValueSet) {
		std::string lMonthLine = lValueSet->ToElement()->Attribute("month_line");

		if (lMonthLine == "false")
			mMonthLine.push_back(false);
		else
			mMonthLine.push_back(true);

		TiXmlNode* lValue = lValueSet->FirstChildElement("value");

		std::vector<int> lValues;

		while (lValue) {
			lValues.push_back(toNumber<int>(lValue->ToElement()->Attribute("data")));
			lValue = lValue->NextSiblingElement("value");
		}

		mValueData.push_back(lValues);

		lValueSet = lValueSet->NextSiblingElement("value_set");
	}
}
