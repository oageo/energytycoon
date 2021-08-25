#include "StdAfx.h"
#include "GUI.h"
#include "StringsLoc.h"
#include "Event.h"
#include "PanCamera.h"
#include "Map.h"
#include "Timer.h"
#include "Game.h"
#include "GameObject.h"
#include "City.h"
#include "Powerplant.h"
#include "Company.h"
#include "ResourceBuilding.h"
#include "CompanyBuilding.h"

/*-----------------------------------------------------------------------------------------------*/

const std::string cCityInfoPic[] = { "city_small.png", "city_medium.png", "city_large.png" };

/*-----------------------------------------------------------------------------------------------*/

GUI::GUI(Ogre::RenderWindow *pWindow, Ogre::RenderSystem* pRenderSystem, Ogre::Root* pRoot,
         bool pRestart)
: mTickerIDCounter(0),
  mCurrentTerrainHeight(0),
  mCurrentTerrainWidth(0),
	mCurrentLayout(0),
  mRenderSystem(pRenderSystem),
  mGameRunning(false),
  mRepairPP(false),
	mLastPanVisible(false),
  mLastTooltipVisible(false),
  mResearchIndex(0),
  mRepairCost(0),
	mLastTooltipText(L""),
  mShowBuy(true),
  mShowSell(true),
  mShowGas(true),
  mButtonDownTime(0.0),
	mShowCoal(true),
  mShowUranium(true),
  mOffersInvalidated(false),
  mObligationsInvalidated(false),
	mRoot(pRoot), mLoading(false),
  mSelectMessage(-1),
  mAskForRestart(true),
  mTickerArchiveLoaded(false),
	mMainMenuLock(-1),
  mOldGraphicsQuality("High"),
  mTutorialStep(0),
  mWeatherInCounter(-1.0),
	mWeatherOutCounter(-1.0),
  mWeatherStayCounter(-1.0),
  mTutorialClosed(false),
  mCompanyInfoLock(false),
  mOptionsChanged(false),
  mDemo(false),
  mShowDemoTime(true)
{
	mGUI.reset(new MyGUI::Gui());

	mGUI->initialise(pWindow);

	MyGUI::LanguageManager::getInstance().setCurrentLanguage(StrLoc::get()->MyGUILanguage());

	mISOToFormattedMapname["de"] = StrLoc::get()->Germany();
	mISOToFormattedMapname["us"] = StrLoc::get()->USA();

	mFormattedToISOMapname[StrLoc::get()->Germany()] = "de";
	mFormattedToISOMapname[StrLoc::get()->USA()] = "us";

	if (pRestart) {
		MyGUI::LayoutManager::getInstance().unloadLayout(mCurrentLayout);
		mCurrentLayout = MyGUI::LayoutManager::getInstance().load("loading.layout");
		mGUI->findWidget<MyGUI::StaticImage>("Background")
      ->setSize(mGUI->getViewWidth(), mGUI->getViewHeight());
		mGUI->findWidget<MyGUI::StaticImage>("Background")
      ->setImageTexture(cBackgrounds[getCurrentAspectRatio()]);
		MyGUI::PointerManager::getInstance().setVisible(false);
		mLoading = true;
	} else {
		mCurrentLayout = MyGUI::LayoutManager::getInstance().load("game.layout");
		setupGamePanels();
	}

	REGISTER_CALLBACK(eUpdateMinimap, GUI::updateMinimap);
	REGISTER_CALLBACK(eMinimapModeChange, GUI::updateMinimap);
	REGISTER_CALLBACK(eTickerMessage, GUI::tickerMessage);
	REGISTER_CALLBACK(eMessageBoxOK, GUI::messageBoxOK);
	REGISTER_CALLBACK(eUpdatePowerNetInfo, GUI::updatePowerNetWindow);
	REGISTER_CALLBACK(eUpdateTradeWindow, GUI::updateTradeWindow);
	REGISTER_CALLBACK(eUpdateObligations, GUI::updateObligations);
	REGISTER_CALLBACK(eUpdateFinanceWindow, GUI::updateFinances);
	REGISTER_CALLBACK(eUpdateLoans, GUI::updateLoans);
	REGISTER_CALLBACK(eUpdateResearchableButtons, GUI::updateResearchableButtons);
	REGISTER_CALLBACK(eUpdateCOBuildingButtons, GUI::updateCOBuildingButtons);
	REGISTER_CALLBACK(eShowNewspaper, GUI::showNewspaper);
	REGISTER_CALLBACK(eSetLoadingProgress, GUI::setLoadingProgress);
	REGISTER_CALLBACK(eSetLoadingStatus, GUI::setLoadingStatus);
	REGISTER_CALLBACK(eBankruptcyWarning, GUI::showBankruptcyWarning);
	REGISTER_CALLBACK(eBankruptcy, GUI::showBankruptcy);
  REGISTER_CALLBACK(eDemoTimeUp, GUI::showDemoTimeUp);
	REGISTER_CALLBACK(eMissionDone, GUI::showMissionDone);
	REGISTER_CALLBACK(eUpdateSpecialActions, GUI::updateSpecialActions);
	REGISTER_CALLBACK(eUpdateMissionGoals, GUI::updateMissionGoals);
	REGISTER_CALLBACK(eUpdateResourceTrend, GUI::updateResourceTrend);
	REGISTER_CALLBACK(eResetSpeedIndicator, GUI::resetSpeedIndicator);
	REGISTER_CALLBACK(eUpdateGUICompanyData, GUI::updateCompanyData);
	REGISTER_CALLBACK(eNewWeather, GUI::newWeather);

	mRepayment = 0;
	mYears = 4;
	mAmount = 500;

	preloadTextures();
}

/*-----------------------------------------------------------------------------------------------*/

GUI::~GUI(void)
{
	mGUI->shutdown();
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::setGameRunning(bool pRunning)
{
	mGameRunning = pRunning;
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::setTutorial(bool pEnabled)
{
	mTutorial = pEnabled;
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::updateWeather(double pTime)
{
	if (mWeatherOutCounter > 0.0) {
		mGUI->findWidget<MyGUI::Widget>("WeatherWidget")->setPosition(
			mGUI->getRenderWindow()->getWidth() - (1.0 - mWeatherOutCounter)*170,
			mGUI->getRenderWindow()->getHeight() - 300);

		mWeatherOutCounter -= pTime;

		if(mWeatherOutCounter < 0.0)
			mWeatherStayCounter = 4.0;
	}
	else if (mWeatherStayCounter > 0.0) {
		mWeatherStayCounter -= pTime;

		if (mWeatherStayCounter < 0.0)
			mWeatherInCounter = 1.0;
	}
	else if (mWeatherInCounter > 0.0) {
		mWeatherInCounter -= pTime;

		mGUI->findWidget<MyGUI::Widget>("WeatherWidget")->setPosition(
			mGUI->getRenderWindow()->getWidth() - (mWeatherInCounter)*170,
			mGUI->getRenderWindow()->getHeight() - 300);

		if (mWeatherInCounter < 0.0)
			mGUI->findWidget<MyGUI::Widget>("WeatherWidget")->setVisible(false);
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::newWeather(EventData* pData)
{
	mCurrentWeather = static_cast<EventArg<Weather>*>(pData)->mData1;

	mGUI->findWidget<MyGUI::Widget>("WeatherWidget")->setVisible(true);
	mGUI->findWidget<MyGUI::StaticText>("WeatherText")
    ->setCaption(StrLoc::get()->ForecastForThisMonth());

	if (mCurrentWeather.mSunshineDuration == eVeryLong) {
		mGUI->findWidget<MyGUI::StaticText>("SunText")->setCaption(StrLoc::get()->PlentySunshine());
		mGUI->findWidget<MyGUI::StaticImage>("SunIcon")->setImageTexture("heavy_sun.png");
	}
	else if (mCurrentWeather.mSunshineDuration == eAverage) {
		mGUI->findWidget<MyGUI::StaticText>("SunText")->setCaption(StrLoc::get()->NormalSunshine());
		mGUI->findWidget<MyGUI::StaticImage>("SunIcon")->setImageTexture("normal_sun.png");
	}
	else if (mCurrentWeather.mSunshineDuration == eLow) {
		mGUI->findWidget<MyGUI::StaticText>("SunText")->setCaption(StrLoc::get()->HardlySunshine());
		mGUI->findWidget<MyGUI::StaticImage>("SunIcon")->setImageTexture("no_sun.png");
	}

	if (mCurrentWeather.mWindSpeed == eStrongSpeed) {
		mGUI->findWidget<MyGUI::StaticText>("WindText")->setCaption(StrLoc::get()->StrongWind());
		mGUI->findWidget<MyGUI::StaticImage>("WindIcon")->setImageTexture("heavy_wind.png");
	}
	else if (mCurrentWeather.mWindSpeed == eNormalSpeed) {
		mGUI->findWidget<MyGUI::StaticText>("WindText")->setCaption(StrLoc::get()->NormalWind());
		mGUI->findWidget<MyGUI::StaticImage>("WindIcon")->setImageTexture("normal_wind.png");
	}
	else if (mCurrentWeather.mWindSpeed == eNoSpeed) {
		mGUI->findWidget<MyGUI::StaticText>("WindText")->setCaption(StrLoc::get()->LittleWind());
		mGUI->findWidget<MyGUI::StaticImage>("WindIcon")->setImageTexture("no_wind.png");
	}

	mWeatherOutCounter = 1.0;
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::delaySwitchToMainMenu(void)
{
	if (mMainMenuLock < 0)
		return;

	if (mMainMenuLock > 0)
		--mMainMenuLock;

	if (mMainMenuLock == 0) {
		switchToMainMenu();
		mMainMenuLock = -1;
	}
}

/*-----------------------------------------------------------------------------------------------*/

size_t GUI::getCurrentAspectRatio(void)
{
	double lAspectRatio = ((double)mGUI->getRenderWindow()->getWidth())
		/((double)mGUI->getRenderWindow()->getHeight());

  size_t languageShift = 0;

  if(StrLoc::get()->LanguageCode() == "en")
    languageShift = 4;

	if (abs(lAspectRatio - cAspectRatios[e16_9]) < 0.01)
		return 3 + languageShift;
	else if (abs(lAspectRatio - cAspectRatios[e16_10]) < 0.01)
		return 2 + languageShift;
	else if (abs(lAspectRatio - cAspectRatios[e5_4]) < 0.01)
		return 0 + languageShift;
	else
		return 1 + languageShift;
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::preloadTextures(void)
{ // prevents crashes with some opengl drivers
	MyGUI::StaticImagePtr lDummy = mGUI->createWidget<MyGUI::StaticImage>("StaticImage",
		0, 0, 0, 0, MyGUI::Align::Default, "Main");
	lDummy->setImageTexture("city_large.png");
	lDummy->setImageTexture("city_medium.png");
	lDummy->setImageTexture("city_small.png");
	lDummy->setImageTexture("co_headquarters.png");
	lDummy->setImageTexture("co_public_relations.png");
	lDummy->setImageTexture("co_research.png");
	lDummy->setImageTexture("pp_bio.png");
	lDummy->setImageTexture("pp_coal_large.png");
	lDummy->setImageTexture("pp_coal_small.png");
	lDummy->setImageTexture("pp_gas_large.png");
	lDummy->setImageTexture("pp_gas_small.png");
	lDummy->setImageTexture("pp_nuclear_fusion.png");
	lDummy->setImageTexture("pp_nuclear_large.png");
	lDummy->setImageTexture("pp_nuclear_small.png");
	lDummy->setImageTexture("pp_solar_large.png");
	lDummy->setImageTexture("pp_solar_small.png");
	lDummy->setImageTexture("pp_solar_updraft.png");
	lDummy->setImageTexture("pp_wind_large.png");
	lDummy->setImageTexture("pp_wind_small.png");
	lDummy->setImageTexture("pp_wind_offshore.png");
	lDummy->setImageTexture("re_coal.png");
	lDummy->setImageTexture("re_gas.png");
	lDummy->setImageTexture("re_uranium.png");
	lDummy->setImageTexture("city.png");
	lDummy->setImageTexture("co.png");
	lDummy->setImageTexture("connected.png");
	lDummy->setImageTexture("info_small_co.png");
	lDummy->setImageTexture("info_small_net.png");
	lDummy->setImageTexture("info_small_net_a.png");
	lDummy->setImageTexture("info_small_pp.png");
	lDummy->setImageTexture("info_small_re.png");
	lDummy->setImageTexture("not_connected.png");
	lDummy->setImageTexture("pp.png");
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::tickerMessage(EventData *pData) {
	TickerMessage lMessage;
	lMessage = static_cast<EventArg<TickerMessage>*>(pData)->mData1;
	lMessage.mID = mTickerIDCounter;
	lMessage.mDateTime = mCurrentTime;

	mTickerArchive.insert(mTickerArchive.begin(), lMessage);

	mGUI->findWidget<MyGUI::List>("TickerList")->removeAllItems();

	for (size_t i = 0; i < mTickerArchive.size(); ++i)
		mGUI->findWidget<MyGUI::List>("TickerList")->addItem(mTickerArchive[i].mDateTime 
    + ": " + mTickerArchive[i].mMessage);

	mTickerIDCounter++;
}

/*-----------------------------------------------------------------------------------------------*/

bool GUI::isMouseOverGUI(void)
{
	if (!MyGUI::InputManager::getInstance().getMouseFocusWidget())
		return false;

	return true;
}

/*-----------------------------------------------------------------------------------------------*/

MyGUI::Gui* GUI::getMyGUI(void)
{
	return mGUI.get();
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::showTooltip(bool pShow, std::wstring pText)
{
	mGUI->findWidget<MyGUI::Widget>("Tooltip")->setVisible(pShow);
	mGUI->findWidget<MyGUI::StaticText>("TooltipText")->setCaption(pText);

	MyGUI::ISubWidgetText* lText = 
    mGUI->findWidget<MyGUI::StaticText>("TooltipText")->getSubWidgetText();
	MyGUI::IntSize lSize = lText ? lText->getTextSize() : MyGUI::IntSize();

	lSize.width = lSize.width + 14;
	lSize.height = lSize.height + 11;
	mGUI->findWidget<MyGUI::Widget>("Tooltip")->setSize(lSize);

	if ((mLastTooltipVisible != pShow) || (mLastTooltipText != pText)) {
		MyGUI::IntPoint lPos = MyGUI::InputManager::getInstance().getMousePosition();
		lPos.top -= 30;
		lPos.left += 10;

		if((lPos.top - mGUI->findWidget<MyGUI::Widget>("Tooltip")->getHeight()) < 0)
			lPos.top += 60;

		if((lPos.left + mGUI->findWidget<MyGUI::Widget>("Tooltip")->getWidth()) 
            > (int)mGUI->getRenderWindow()->getWidth())
			lPos.left -= (20 + mGUI->findWidget<MyGUI::Widget>("Tooltip")->getWidth());

		mGUI->findWidget<MyGUI::Widget>("Tooltip")->setPosition(lPos);
	}

	mLastTooltipVisible = pShow;
	mLastTooltipText = pText;
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::updateTooltip(void)
{
	if (!mGUI->findWidget<MyGUI::Widget>("Tooltip", false))
		return;

	if (MyGUI::InputManager::getInstance().getMouseFocusWidget()) {
		std::string lName = MyGUI::InputManager::getInstance().getMouseFocusWidget()->getName();

		if (lName == "BuildPowerplant")
			showTooltip(true, StrLoc::get()->TooltipBuildPowerplant());
		else if (lName == "PPRepair")
			showTooltip(true, StrLoc::get()->TooltipRepair());
		else if (lName == "RERepair")
			showTooltip(true, StrLoc::get()->TooltipRepair());
		else if (lName == "SpInactive")
			showTooltip(true, StrLoc::get()->TooltipSpImgInactive());
		else if (lName == "Sp1Img")
			showTooltip(true, StrLoc::get()->TooltipSp1Img());
		else if (lName == "Sp2Img")
			showTooltip(true, StrLoc::get()->TooltipSp2Img());
		else if (lName == "Sp3Img")
			showTooltip(true, StrLoc::get()->TooltipSp3ImgA() 
      + toString(mCompany->getTaxFreeTradesLeft()) + StrLoc::get()->TooltipSp3ImgB());
		else if (lName == "Sp4Img")
			showTooltip(true, StrLoc::get()->TooltipSp4Img());
		else if (lName == "Sp5Img")
			showTooltip(true, StrLoc::get()->TooltipSp5Img());
		else if (lName == "Sp6Img")
			showTooltip(true, StrLoc::get()->TooltipSp6Img());
		else if (lName == "BuildDeployment")
			showTooltip(true, StrLoc::get()->TooltipBuildDistribution());
		else if (lName == "BuildResource")
			showTooltip(true, StrLoc::get()->TooltipBuildResource());
		else if (lName == "BuildMisc")
			showTooltip(true, StrLoc::get()->TooltipBuildMisc());
		else if (lName == "Demolish")
			showTooltip(true, StrLoc::get()->TooltipDemolish());
		else if (lName == "Menu")
			showTooltip(true, StrLoc::get()->TooltipMenu());
		else if (lName == "Mission")
			showTooltip(true, StrLoc::get()->TooltipMission());
		else if (lName == "FinancesMain")
			showTooltip(true, StrLoc::get()->TooltipFinances());
		else if (lName == "Trade")
			showTooltip(true, StrLoc::get()->TooltipTrade());
		else if (lName == "CEO")
			showTooltip(true, StrLoc::get()->TooltipCEO());
		else if (lName == "Research")
			showTooltip(true, StrLoc::get()->TooltipResearch());
		else if (lName == "ZoomIn")
			showTooltip(true, StrLoc::get()->TooltipZoomIn());
		else if (lName == "RotateLeft")
			showTooltip(true, StrLoc::get()->TooltipRotateRight());
		else if (lName == "RotateRight")
			showTooltip(true, StrLoc::get()->TooltipRotateLeft());
		else if (lName == "ZoomOut")
			showTooltip(true, StrLoc::get()->TooltipZoomOut());
		else if (lName == "SpeedPause")
			showTooltip(true, StrLoc::get()->TooltipPause());
		else if (lName == "SpeedSlower")
			showTooltip(true, StrLoc::get()->TooltipSlower());
		else if (lName == "SpeedFaster")
			showTooltip(true, StrLoc::get()->TooltipFaster());
		else if (lName == "MinimapShowSupply")
			showTooltip(true, StrLoc::get()->TooltipMMSupply());
		else if (lName == "MinimapShowNets")
			showTooltip(true, StrLoc::get()->TooltipMMCapacity());
		else if (lName == "MinimapShowResources")
			showTooltip(true, StrLoc::get()->TooltipMMResources());
		else if (lName == "MinimapShowDefault")
			showTooltip(true, StrLoc::get()->TooltipMMDefault());
		else if (lName == "Finances")
			showTooltip(true, StrLoc::get()->TooltipFinances());
		else if (lName == "ShowPowerNets")
			showTooltip(true, StrLoc::get()->TooltipPowerNets());
		else if (lName == "TickerArchive")
			showTooltip(true, StrLoc::get()->TooltipMessageArchive());
		else if (lName == toString(ePPNuclearLarge)) {
			if (mCompany->getResearchSet().mResearched[18])
				showTooltip(true, StrLoc::get()->TooltipNuclearLarge() 
        + "\n" + StrLoc::get()->TooltipCost() 
				+ toString(GameConfig::getInt(GameConfig::cCOTS(ePPNuclearLarge),cCOVCost)/1000)
				+ L" k€\n" + StrLoc::get()->TooltipPower() 
        + GameConfig::getString(GameConfig::cCOTS(ePPNuclearLarge), cCOVPower) + "MW\n"
				+ StrLoc::get()->TooltipImage() 
				+ GameConfig::getString(GameConfig::cCOTS(ePPNuclearLarge),cCOVImage));
		}
		else if (lName == toString(ePPNuclearSmall)) {
			if (mCompany->getResearchSet().mResearched[8])
				showTooltip(true, StrLoc::get()->TooltipNuclearSmall() 
        + "\n" + StrLoc::get()->TooltipCost() 
				+ toString(GameConfig::getInt(GameConfig::cCOTS(ePPNuclearSmall),cCOVCost)/1000)
				+ L" k€\n" + StrLoc::get()->TooltipPower() 
        + GameConfig::getString(GameConfig::cCOTS(ePPNuclearSmall), cCOVPower) + "MW\n"
				+ StrLoc::get()->TooltipImage() 
				+ GameConfig::getString(GameConfig::cCOTS(ePPNuclearSmall),cCOVImage));
		}
		else if (lName == toString(ePPCoalLarge)) {
			if (mCompany->getResearchSet().mResearched[0])
				showTooltip(true, StrLoc::get()->TooltipCoalLarge() 
        + "\n" + StrLoc::get()->TooltipCost() 
				+ toString(GameConfig::getInt(GameConfig::cCOTS(ePPCoalLarge),cCOVCost)/1000)
				+ L" k€\n" + StrLoc::get()->TooltipPower() 
        + GameConfig::getString(GameConfig::cCOTS(ePPCoalLarge), cCOVPower) + "MW\n"
				+ StrLoc::get()->TooltipImage() 
				+ GameConfig::getString(GameConfig::cCOTS(ePPCoalLarge),cCOVImage));
		}
		else if (lName == toString(ePPGasLarge)) {
			if (mCompany->getResearchSet().mResearched[9])
				showTooltip(true, StrLoc::get()->TooltipGasLarge() 
        + "\n" + StrLoc::get()->TooltipCost() 
				+ toString(GameConfig::getInt(GameConfig::cCOTS(ePPGasLarge),cCOVCost)/1000)
				+ L" k€\n" + StrLoc::get()->TooltipPower() 
        + GameConfig::getString(GameConfig::cCOTS(ePPGasLarge), cCOVPower) + "MW\n"
				+ StrLoc::get()->TooltipImage() 
				+ GameConfig::getString(GameConfig::cCOTS(ePPGasLarge),cCOVImage));
		}
		else if (lName == toString(ePPWindLarge)) {
			if (mCompany->getResearchSet().mResearched[1])
				showTooltip(true, StrLoc::get()->TooltipWindLarge() 
        + "\n" + StrLoc::get()->TooltipCost() 
				+ toString(GameConfig::getInt(GameConfig::cCOTS(ePPWindLarge),cCOVCost)/1000)
				+ L" k€\n" + StrLoc::get()->TooltipPower() 
        + GameConfig::getString(GameConfig::cCOTS(ePPWindLarge), cCOVPower) + "MW\n"
				+ StrLoc::get()->TooltipImage() 
				+ GameConfig::getString(GameConfig::cCOTS(ePPWindLarge),cCOVImage));
		}
		else if (lName == toString(ePPWindOffshore)) {
			if (mCompany->getResearchSet().mResearched[6])
				showTooltip(true, StrLoc::get()->TooltipWindOffshore() 
        + "\n" + StrLoc::get()->TooltipCost() 
				+ toString(GameConfig::getInt(GameConfig::cCOTS(ePPWindOffshore),cCOVCost)/1000)
				+ L" k€\n" + StrLoc::get()->TooltipPower() 
        + GameConfig::getString(GameConfig::cCOTS(ePPWindOffshore), cCOVPower) + "MW\n"
				+ StrLoc::get()->TooltipImage() 
				+ GameConfig::getString(GameConfig::cCOTS(ePPWindOffshore),cCOVImage));
		}
		else if (lName == toString(ePPBio)) {
			if (mCompany->getResearchSet().mResearched[3])
				showTooltip(true, StrLoc::get()->TooltipBio() 
        + "\n" + StrLoc::get()->TooltipCost() 
				+ toString(GameConfig::getInt(GameConfig::cCOTS(ePPBio),cCOVCost)/1000)
				+ L" k€\n" + StrLoc::get()->TooltipPower() 
        + GameConfig::getString(GameConfig::cCOTS(ePPBio), cCOVPower) + "MW\n"
				+ StrLoc::get()->TooltipImage() 
				+ GameConfig::getString(GameConfig::cCOTS(ePPBio),cCOVImage));
		}
		else if (lName == toString(ePPSolarLarge)) {
			if (mCompany->getResearchSet().mResearched[15])
				showTooltip(true, StrLoc::get()->TooltipSolarLarge() 
        + "\n" + StrLoc::get()->TooltipCost() 
				+ toString(GameConfig::getInt(GameConfig::cCOTS(ePPSolarLarge),cCOVCost)/1000)
				+ L" k€\n" + StrLoc::get()->TooltipPower() 
        + GameConfig::getString(GameConfig::cCOTS(ePPSolarLarge), cCOVPower) + "MW\n"
				+ StrLoc::get()->TooltipImage() 
				+ GameConfig::getString(GameConfig::cCOTS(ePPSolarLarge),cCOVImage));
		}
		else if (lName == toString(ePPSolarUpdraft)) {
			if (mCompany->getResearchSet().mResearched[16])
				showTooltip(true, StrLoc::get()->TooltipSolarUpdraft() 
        + "\n" + StrLoc::get()->TooltipCost() 
				+ toString(GameConfig::getInt(GameConfig::cCOTS(ePPSolarUpdraft),cCOVCost)/1000)
				+ L" k€\n" + StrLoc::get()->TooltipPower() 
        + GameConfig::getString(GameConfig::cCOTS(ePPSolarUpdraft), cCOVPower) + "MW\n"
				+ StrLoc::get()->TooltipImage() 
				+ GameConfig::getString(GameConfig::cCOTS(ePPSolarUpdraft),cCOVImage));
		}
		else if (lName == toString(ePPNuclearFusion)) {
			if (mCompany->getResearchSet().mResearched[20])
				showTooltip(true, StrLoc::get()->TooltipNuclearFusion() 
        + "\n" + StrLoc::get()->TooltipCost() 
				+ toString(GameConfig::getInt(GameConfig::cCOTS(ePPNuclearFusion),cCOVCost)/1000)
				+ L" k€\n" + StrLoc::get()->TooltipPower() 
        + GameConfig::getString(GameConfig::cCOTS(ePPNuclearFusion), cCOVPower) + "MW\n"
				+ StrLoc::get()->TooltipImage() 
				+ GameConfig::getString(GameConfig::cCOTS(ePPNuclearFusion),cCOVImage));
		}
		else if (lName == toString(eRECoalLarge)) {
			if (mCompany->getResearchSet().mResearched[10])
				showTooltip(true, StrLoc::get()->TooltipCoalRLarge() 
        + "\n" + StrLoc::get()->TooltipCost() 
				+ toString(GameConfig::getInt(GameConfig::cCOTS(eRECoalLarge),cCOVCost)/1000)
				+ L" k€\n" + StrLoc::get()->TooltipOutput() 
        + GameConfig::getString(GameConfig::cCOTS(eRECoalLarge), cCOVRate)
        + " " + StrLoc::get()->TooltipKTons() 
				+ StrLoc::get()->TooltipPerWeek() + "\n" + StrLoc::get()->TooltipImage() 
				+ GameConfig::getString(GameConfig::cCOTS(eRECoalLarge),cCOVImage));
		}
		else if (lName == toString(eREGasLarge)) {
			if (mCompany->getResearchSet().mResearched[12])
				showTooltip(true, StrLoc::get()->TooltipGasRLarge() 
        + "\n" + StrLoc::get()->TooltipCost() 
				+ toString(GameConfig::getInt(GameConfig::cCOTS(eREGasLarge),cCOVCost)/1000)
				+ L" k€\n" + StrLoc::get()->TooltipOutput() 
        + GameConfig::getString(GameConfig::cCOTS(eREGasLarge), cCOVRate) 
        + " " + StrLoc::get()->TooltipKCubicFeet() 
				+ StrLoc::get()->TooltipPerWeek() + "\n" + StrLoc::get()->TooltipImage() 
				+ GameConfig::getString(GameConfig::cCOTS(eREGasLarge),cCOVImage));
		}
		else if (lName == toString(eREUraniumLarge)) {
			if (mCompany->getResearchSet().mResearched[19])
				showTooltip(true, StrLoc::get()->TooltipUraniumRLarge() 
        + "\n" + StrLoc::get()->TooltipCost() 
				+ toString(GameConfig::getInt(GameConfig::cCOTS(eREUraniumLarge),cCOVCost)/1000)
				+ L" k€\n" + StrLoc::get()->TooltipOutput() 
        + GameConfig::getString(GameConfig::cCOTS(eREUraniumLarge), cCOVRate) 
        + " " + StrLoc::get()->TooltipRods() 
				+ StrLoc::get()->TooltipPerWeek() + "\n" + StrLoc::get()->TooltipImage() 
				+ GameConfig::getString(GameConfig::cCOTS(eREUraniumLarge),cCOVImage));
		}
		else if (lName == toString(eDIPoleLarge))
			showTooltip(true, StrLoc::get()->TooltipLineLarge() 
      + "\n" + StrLoc::get()->TooltipCost() 
			+ toString(GameConfig::getInt(GameConfig::cCOTS(eDIPoleLarge),cCOVCost)/1000)
			+ L" k€\n" + StrLoc::get()->TooltipCapacity() 
      + GameConfig::getString("PoleLargeCapacity") + "MW\n" + StrLoc::get()->TooltipImage() 
			+ GameConfig::getString(GameConfig::cCOTS(eDIPoleLarge),cCOVImage));
		else if (lName == toString(eDIPoleSmall))
			showTooltip(true, StrLoc::get()->TooltipLineSmall() 
      + "\n" + StrLoc::get()->TooltipCost() 
			+ toString(GameConfig::getInt(GameConfig::cCOTS(eDIPoleSmall),cCOVCost)/1000)
			+ L" k€\n" + StrLoc::get()->TooltipCapacity()
      + GameConfig::getString("PoleSmallCapacity") + "MW\n" + StrLoc::get()->TooltipImage() 
			+ GameConfig::getString(GameConfig::cCOTS(eDIPoleSmall),cCOVImage));
		else if (lName == toString(ePPWindSmall))
			showTooltip(true, StrLoc::get()->TooltipWindSmall()
      + "\n" + StrLoc::get()->TooltipCost() 
			+ toString(GameConfig::getInt(GameConfig::cCOTS(ePPWindSmall),cCOVCost)/1000)
			+ L" k€\n" + StrLoc::get()->TooltipPower() 
      + GameConfig::getString(GameConfig::cCOTS(ePPWindSmall), cCOVPower) + "MW\n"
			+ StrLoc::get()->TooltipImage() 
			+ GameConfig::getString(GameConfig::cCOTS(ePPWindSmall),cCOVImage));
		else if (lName == toString(ePPCoalSmall))
			showTooltip(true, StrLoc::get()->TooltipCoalSmall()
      + "\n" + StrLoc::get()->TooltipCost() 
			+ toString(GameConfig::getInt(GameConfig::cCOTS(ePPCoalSmall),cCOVCost)/1000)
			+ L" k€\n" + StrLoc::get()->TooltipPower() 
      + GameConfig::getString(GameConfig::cCOTS(ePPNuclearLarge), cCOVPower) + "MW\n"
			+ StrLoc::get()->TooltipImage() 
			+ GameConfig::getString(GameConfig::cCOTS(ePPCoalSmall),cCOVImage));
		else if (lName == toString(ePPGasSmall))
			showTooltip(true, StrLoc::get()->TooltipGasSmall() 
      + "\n" + StrLoc::get()->TooltipCost() 
			+ toString(GameConfig::getInt(GameConfig::cCOTS(ePPGasSmall),cCOVCost)/1000)
			+ L" k€\n" + StrLoc::get()->TooltipPower() 
      + GameConfig::getString(GameConfig::cCOTS(ePPGasSmall), cCOVPower) + "MW\n"
			+ StrLoc::get()->TooltipImage() 
			+ GameConfig::getString(GameConfig::cCOTS(ePPGasSmall),cCOVImage));
		else if (lName == toString(ePPSolarSmall))
			showTooltip(true, StrLoc::get()->TooltipSolarSmall() 
      + "\n" + StrLoc::get()->TooltipCost() 
			+ toString(GameConfig::getInt(GameConfig::cCOTS(ePPSolarSmall),cCOVCost)/1000)
			+ L" k€\n" + StrLoc::get()->TooltipPower() 
      + GameConfig::getString(GameConfig::cCOTS(ePPSolarSmall), cCOVPower) + "MW\n"
			+ StrLoc::get()->TooltipImage() 
			+ GameConfig::getString(GameConfig::cCOTS(ePPSolarSmall),cCOVImage));
		else if (lName == toString(eRECoalSmall))
			showTooltip(true, StrLoc::get()->TooltipCoalRSmall() 
      + "\n" + StrLoc::get()->TooltipCost() 
			+ toString(GameConfig::getInt(GameConfig::cCOTS(eRECoalSmall),cCOVCost)/1000)
			+ L" k€\n" + StrLoc::get()->TooltipOutput() 
      + GameConfig::getString(GameConfig::cCOTS(eRECoalSmall), cCOVRate) 
      + " " + StrLoc::get()->TooltipKTons() 
			+ StrLoc::get()->TooltipPerWeek() + "\n" + StrLoc::get()->TooltipImage() 
			+ GameConfig::getString(GameConfig::cCOTS(eRECoalSmall),cCOVImage));
		else if (lName == toString(eREGasSmall))
			showTooltip(true, StrLoc::get()->TooltipGasRSmall() 
      + "\n" + StrLoc::get()->TooltipCost() 
			+ toString(GameConfig::getInt(GameConfig::cCOTS(eREGasSmall),cCOVCost)/1000)
			+ L" k€\n" + StrLoc::get()->TooltipOutput() 
      + GameConfig::getString(GameConfig::cCOTS(eREGasSmall), cCOVRate) 
      + " " + StrLoc::get()->TooltipKCubicFeet() 
			+ StrLoc::get()->TooltipPerWeek() 
      + "\n" + StrLoc::get()->TooltipImage() 
			+ GameConfig::getString(GameConfig::cCOTS(eREGasSmall),cCOVImage));
		else if (lName == toString(eREUraniumSmall))
			showTooltip(true, StrLoc::get()->TooltipUraniumRSmall() 
      + "\n" + StrLoc::get()->TooltipCost() 
			+ toString(GameConfig::getInt(GameConfig::cCOTS(eREUraniumSmall),cCOVCost)/1000)
			+ L" k€\n" + StrLoc::get()->TooltipOutput() 
      + GameConfig::getString(GameConfig::cCOTS(eREUraniumSmall), cCOVRate) 
      + " " + StrLoc::get()->TooltipRods() 
			+ StrLoc::get()->TooltipPerWeek() + "\n" 
      + StrLoc::get()->TooltipImage() 
			+ GameConfig::getString(GameConfig::cCOTS(eREUraniumSmall),cCOVImage));
		else if (lName == toString(eCOHeadquarters))
			showTooltip(true, StrLoc::get()->TooltipHeadquarters() 
      + "\n" + StrLoc::get()->TooltipCost() 
			+ toString(GameConfig::getInt(GameConfig::cCOTS(eCOHeadquarters),cCOVCost)/1000)
			+ L" k€\n" + StrLoc::get()->TooltipImage() 
      + GameConfig::getString(GameConfig::cCOTS(eCOHeadquarters),cCOVImage));
		else if (lName == toString(eCOPublicRelations))
			showTooltip(true, StrLoc::get()->TooltipPR() 
      + "\n" + StrLoc::get()->TooltipCost() 
			+ toString(GameConfig::getInt(GameConfig::cCOTS(eCOPublicRelations),cCOVCost)/1000)
			+ L" k€\n" + StrLoc::get()->TooltipImage() 
      + GameConfig::getString(GameConfig::cCOTS(eCOPublicRelations),cCOVImage));
		else if (lName == toString(eCOResearch))
			showTooltip(true, StrLoc::get()->TooltipResearchBuilding() 
      + "\n" + StrLoc::get()->TooltipCost() 
			+ toString(GameConfig::getInt(GameConfig::cCOTS(eCOResearch),cCOVCost)/1000)
			+ L" k€\n" + StrLoc::get()->TooltipImage() 
      + GameConfig::getString(GameConfig::cCOTS(eCOResearch),cCOVImage));
		else if (lName == "Re1")
			showTooltip(true, StrLoc::get()->TooltipRe1() + L" (" 
      + toString(mCompany->getResearchSet().mPrice[0]) + L"k€)");
		else if (lName == "Re2")
			showTooltip(true, StrLoc::get()->TooltipRe2() + L" (" 
      + toString(mCompany->getResearchSet().mPrice[1]) + L"k€)");
		else if (lName == "Re3")
			showTooltip(true, StrLoc::get()->TooltipRe3() + L" (" 
      + toString(mCompany->getResearchSet().mPrice[2]) + L"k€)");
		else if (lName == "Re4")
			showTooltip(true, StrLoc::get()->TooltipRe4() + L" (" 
      + toString(mCompany->getResearchSet().mPrice[3]) + L"k€)");
		else if (lName == "Re5")
			showTooltip(true, StrLoc::get()->TooltipRe5() + L" (" 
      + toString(mCompany->getResearchSet().mPrice[4]) + L"k€)");
		else if (lName == "Re6")
			showTooltip(true, StrLoc::get()->TooltipRe6() + L" (" 
      + toString(mCompany->getResearchSet().mPrice[5]) + L"k€)");
		else if (lName == "Re7")
			showTooltip(true, StrLoc::get()->TooltipRe7() + L" (" 
      + toString(mCompany->getResearchSet().mPrice[6]) + L"k€)");
		else if (lName == "Re8")
			showTooltip(true, StrLoc::get()->TooltipRe8() + L" (" 
      + toString(mCompany->getResearchSet().mPrice[7]) + L"k€)");
		else if (lName == "Re9")
			showTooltip(true, StrLoc::get()->TooltipRe9() + L" (" 
      + toString(mCompany->getResearchSet().mPrice[8]) + L"k€)");
		else if (lName == "Re10")
			showTooltip(true, StrLoc::get()->TooltipRe10() + L" (" 
      + toString(mCompany->getResearchSet().mPrice[9]) + L"k€)");
		else if (lName == "Re11") {
			if (!mCompany->isResearchBuilt())
				showTooltip(true, StrLoc::get()->TooltipReNotAvailable());
			else
				showTooltip(true, StrLoc::get()->TooltipRe11() + L" ("
        + toString(mCompany->getResearchSet().mPrice[10]) + L"k€)");
		}
		else if (lName == "Re12") {
			if (!mCompany->isResearchBuilt())
				showTooltip(true, StrLoc::get()->TooltipReNotAvailable());
			else
				showTooltip(true, StrLoc::get()->TooltipRe12() + L" (" 
        + toString(mCompany->getResearchSet().mPrice[11]) + L"k€)");
		}
		else if (lName == "Re13") {
			if (!mCompany->isResearchBuilt())
				showTooltip(true, StrLoc::get()->TooltipReNotAvailable());
			else
				showTooltip(true, StrLoc::get()->TooltipRe13() + L" (" 
        + toString(mCompany->getResearchSet().mPrice[12]) + L"k€)");
		}
		else if (lName == "Re14") {
			if (!mCompany->isResearchBuilt())
				showTooltip(true, StrLoc::get()->TooltipReNotAvailable());
			else
				showTooltip(true, StrLoc::get()->TooltipRe14() + L" (" 
        + toString(mCompany->getResearchSet().mPrice[13]) + L"k€)");
		}
		else if (lName == "Re15") {
			if (!mCompany->isResearchBuilt())
				showTooltip(true, StrLoc::get()->TooltipReNotAvailable());
			else
				showTooltip(true, StrLoc::get()->TooltipRe15() + L" (" 
        + toString(mCompany->getResearchSet().mPrice[14]) + L"k€)");
		}
		else if (lName == "Re16") {
			if (!mCompany->isResearchBuilt())
				showTooltip(true, StrLoc::get()->TooltipReNotAvailable());
			else
				showTooltip(true, StrLoc::get()->TooltipRe16() + L" (" 
        + toString(mCompany->getResearchSet().mPrice[15]) + L"k€)");
		}
		else if (lName == "Re17") {
			if (!mCompany->isResearchBuilt())
				showTooltip(true, StrLoc::get()->TooltipReNotAvailable());
			else
				showTooltip(true, StrLoc::get()->TooltipRe17() + L" (" 
        + toString(mCompany->getResearchSet().mPrice[16]) + L"k€)");
		}
		else if (lName == "Re18") {
			if (!mCompany->isResearchBuilt())
				showTooltip(true, StrLoc::get()->TooltipReNotAvailable());
			else
				showTooltip(true, StrLoc::get()->TooltipRe18() + L" (" 
        + toString(mCompany->getResearchSet().mPrice[17]) + L"k€)");
		}
		else if (lName == "Re19") {
			if (!mCompany->isResearchBuilt())
				showTooltip(true, StrLoc::get()->TooltipReNotAvailable());
			else
				showTooltip(true, StrLoc::get()->TooltipRe19() + L" (" 
        + toString(mCompany->getResearchSet().mPrice[18]) + L"k€)");
		}
		else if (lName == "Re20") {
			if (!mCompany->isResearchBuilt())
				showTooltip(true, StrLoc::get()->TooltipReNotAvailable());
			else
				showTooltip(true, StrLoc::get()->TooltipRe20() + L" (" 
        + toString(mCompany->getResearchSet().mPrice[19]) + L"k€)");
		}
		else if (lName == "Re21") {
			if (!mCompany->isResearchBuilt())
				showTooltip(true, StrLoc::get()->TooltipReNotAvailable());
			else
				showTooltip(true, StrLoc::get()->TooltipRe21() + L" (" 
        + toString(mCompany->getResearchSet().mPrice[20]) + L"k€)");
		}
		else if (lName == "Sp1")
			showTooltip(true, StrLoc::get()->TooltipSp1());
		else if (lName == "Sp2")
			showTooltip(true, StrLoc::get()->TooltipSp2());
		else if (lName == "Sp3")
			showTooltip(true, StrLoc::get()->TooltipSp3());
		else if (lName == "Sp4") {
			if (!mCompany->isHeadquartersBuilt())
				showTooltip(true, StrLoc::get()->TooltipSpNotAvailable());
			else
				showTooltip(true, StrLoc::get()->TooltipSp4());
		}
		else if (lName == "Sp5") {
			if (!mCompany->isHeadquartersBuilt())
				showTooltip(true, StrLoc::get()->TooltipSpNotAvailable());
			else
				showTooltip(true, StrLoc::get()->TooltipSp5());
		}
		else if (lName == "Sp6") {
			if (!mCompany->isHeadquartersBuilt())
				showTooltip(true, StrLoc::get()->TooltipSpNotAvailable());
			else
				showTooltip(true, StrLoc::get()->TooltipSp6());
		}
		else if (lName == "Ad1")
			showTooltip(true, StrLoc::get()->TooltipAd1());
		else if (lName == "Ad2")
			showTooltip(true, StrLoc::get()->TooltipAd2());
		else if (lName == "Ad3") {
			if (!mCompany->isPRBuilt())
				showTooltip(true, StrLoc::get()->TooltipAdNotAvailable());
			else
				showTooltip(true, StrLoc::get()->TooltipAd3());
		}
		else if (lName == "Ad4") {
			if (!mCompany->isPRBuilt())
				showTooltip(true, StrLoc::get()->TooltipAdNotAvailable());
			else
				showTooltip(true, StrLoc::get()->TooltipAd4());
		} else {
			showTooltip(false, L"");
		}
	} else {
		showTooltip(false, L"");
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::updateGameTime(Ogre::UTFString pTime, eSpeed pSpeed)
{
	if (!mGUI->findWidget<MyGUI::StaticText>("DateTime", false))
		return;

	mCurrentSpeed = pSpeed;
	mCurrentTime = pTime;

	mGUI->findWidget<MyGUI::StaticText>("DateTime")->setCaption(pTime);
	mGUI->findWidget<MyGUI::StaticText>("DateTimeShadow")->setCaption(pTime);

	MyGUI::WidgetPtr lTickerPanel = mGUI->findWidget<MyGUI::Widget>("TickerPanel");

	if (mTickerArchive.size() > 0) {
		if (mTickerArchive.size() == 1) {
			mGUI->findWidget<MyGUI::StaticText>("TickerLine1")
        ->setCaption("");
			mGUI->findWidget<MyGUI::StaticText>("TickerLine2")
        ->setCaption(mTickerArchive[0].mMessage);

			mGUI->findWidget<MyGUI::StaticText>("TickerLine1Shadow")
        ->setCaption("");
			mGUI->findWidget<MyGUI::StaticText>("TickerLine2Shadow")
        ->setCaption(mTickerArchive[0].mMessage);
		}
		else if (mTickerArchive.size() >= 2) {
			mGUI->findWidget<MyGUI::StaticText>("TickerLine1")
				->setCaption(mTickerArchive[1].mMessage);

			mGUI->findWidget<MyGUI::StaticText>("TickerLine2")
				->setCaption(mTickerArchive[0].mMessage);

			mGUI->findWidget<MyGUI::StaticText>("TickerLine1Shadow")
				->setCaption(mTickerArchive[1].mMessage);

			mGUI->findWidget<MyGUI::StaticText>("TickerLine2Shadow")
				->setCaption(mTickerArchive[0].mMessage);
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::updatePowerNetWindow(EventData* pData)
{
	if (mLoading)
		return;

	int lTabSelected = MyGUI::ITEM_NONE;

	int lCitySelected = MyGUI::ITEM_NONE;
	int lCityScroll = 0;
	int lPowerplantSelected = MyGUI::ITEM_NONE;
	int lPowerplantScroll = 0;

	mGUI->findWidget<MyGUI::Tab>("PowerNetTab")->setSmoothShow(false);

	if (mGUI->findWidget<MyGUI::Tab>("PowerNetTab")->getItemCount() > 0)
		lTabSelected = mGUI->findWidget<MyGUI::Tab>("PowerNetTab")->getIndexSelected();

	if (lTabSelected != MyGUI::ITEM_NONE) {
		if (mGUI->findWidget<MyGUI::List>("CitiesList" + toString(lTabSelected), false)) {
			lCitySelected = mGUI->findWidget<MyGUI::List>("CitiesList" 
        + toString(lTabSelected))->getIndexSelected();
			lCityScroll = mGUI->findWidget<MyGUI::List>("CitiesList" 
        + toString(lTabSelected))->getScrollPosition();
		}

		if (mGUI->findWidget<MyGUI::List>("PowerplantsList" + toString(lTabSelected), false)) {
			lPowerplantSelected = mGUI->findWidget<MyGUI::List>("PowerplantsList" 
        + toString(lTabSelected))->getIndexSelected();
			lPowerplantScroll = mGUI->findWidget<MyGUI::List>("PowerplantsList" 
        + toString(lTabSelected))->getScrollPosition();
		}
	}

	if (mCompany) {
		mGUI->findWidget<MyGUI::Tab>("PowerNetTab")->removeAllItems();

		if (mCompany->getPowerNet()->getSubnetCount() > 0) {
			for (size_t j = 0; j < mCompany->getPowerNet()->getSubnetCount(); j++) {
				if (mCompany->getPowerNet()->getSubnetPowerplants(j).size() != 0) {
					createPowerNetTab(j);
				}
			}
		}
	}

	if (((int)mGUI->findWidget<MyGUI::Tab>("PowerNetTab")->getItemCount() 
      > lTabSelected) && (lTabSelected != MyGUI::ITEM_NONE)) {
		mGUI->findWidget<MyGUI::Tab>("PowerNetTab")->setIndexSelected(lTabSelected);

		if (mGUI->findWidget<MyGUI::List>("CitiesList" + toString(lTabSelected), false)) {
			if (((int)mGUI->findWidget<MyGUI::List>("CitiesList" + toString(lTabSelected))
        ->getItemCount() > lCitySelected) && (lCitySelected != MyGUI::ITEM_NONE)) {
				mGUI->findWidget<MyGUI::List>("CitiesList" + toString(lTabSelected))
          ->setIndexSelected(lCitySelected);
				mGUI->findWidget<MyGUI::List>("CitiesList" + toString(lTabSelected))
          ->setScrollPosition(lCityScroll);
				powerNetListPressed(mGUI->findWidget<MyGUI::List>("CitiesList" 
          + toString(lTabSelected)), lCitySelected);
			}
		}

		if (mGUI->findWidget<MyGUI::List>("PowerplantsList" + toString(lTabSelected), false)) {
			if (((int)mGUI->findWidget<MyGUI::List>("PowerplantsList" + toString(lTabSelected))
        ->getItemCount() > lPowerplantSelected) && (lPowerplantSelected != MyGUI::ITEM_NONE)) {
				mGUI->findWidget<MyGUI::List>("PowerplantsList" + toString(lTabSelected))
          ->setIndexSelected(lPowerplantSelected);
				mGUI->findWidget<MyGUI::List>("PowerplantsList" + toString(lTabSelected))
          ->setScrollPosition(lPowerplantScroll);
				powerNetListPressed(mGUI->findWidget<MyGUI::List>("PowerplantsList" 
          + toString(lTabSelected)), lPowerplantSelected);
			}
		}
	}

	if(mCompany)
		if(mCompany->getPowerNet()->getSubnetCount() > 0)
			mGUI->findWidget<MyGUI::Tab>("PowerNetTab")->setVisible(true);

	mGUI->findWidget<MyGUI::Tab>("PowerNetTab")->setSmoothShow(true);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::createPowerNetTab(int pIndex)
{
	MyGUI::TabPtr lPowerNetTab = mGUI->findWidget<MyGUI::Tab>("PowerNetTab");

	MyGUI::TabItemPtr lTabItem = lPowerNetTab->createWidget<MyGUI::TabItem>("Default",
	4, 26, 635, 447, MyGUI::Align::Default, "Net " + toString(pIndex));
	
	lTabItem->setCaption(StrLoc::get()->CONet() + " " + toString(pIndex+1));

	MyGUI::StaticTextPtr lText = lTabItem->createWidget<MyGUI::StaticText>("StaticText",
		12, 6, 72, 16, MyGUI::Align::Default);

	lText->setCaption(StrLoc::get()->COCities());

	lText = lTabItem->createWidget<MyGUI::StaticText>("StaticText",
		220, 6, 100, 16, MyGUI::Align::Default);

	lText->setCaption(StrLoc::get()->COPowerplants());

	lText = lTabItem->createWidget<MyGUI::StaticText>("StaticText",
		437, 4, 80, 16, MyGUI::Align::Default);

	lText->setCaption(StrLoc::get()->CONet() + ":");

	MyGUI::ListPtr lList = lTabItem->createWidget<MyGUI::List>("List",
		13, 28, 195, 204, MyGUI::Align::Default, "CitiesList" + toString(pIndex));

	std::vector<boost::shared_ptr<City> > lCities = mCompany
    ->getPowerNet()->getSubnetCities(pIndex);

	int lPowerNeeded = 0;

	for (size_t i = 0; i < lCities.size(); i++) {
		lList->addItem(lCities[i]->getName(), lCities[i]);
		lPowerNeeded += lCities[i]->getConsumption();
	}

	lList->eventListChangePosition = MyGUI::newDelegate(this, &GUI::powerNetListPressed);

	lList = lTabItem->createWidget<MyGUI::List>("List",
		221, 28, 195, 204, MyGUI::Align::Default, "PowerplantsList" + toString(pIndex));

	std::vector<boost::shared_ptr<Powerplant> > lPowerplants = mCompany
    ->getPowerNet()->getSubnetPowerplants(pIndex);

	int lPowerAvailable = 0;

	for (size_t i = 0; i < lPowerplants.size(); i++) {
		lList->addItem(lPowerplants[i]->getName(), lPowerplants[i]);
		lPowerAvailable += lPowerplants[i]->getPowerInMW();
	}

	lList->eventListChangePosition = MyGUI::newDelegate(this, &GUI::powerNetListPressed);

	MyGUI::WidgetPtr lPanel = lTabItem->createWidget<MyGUI::Widget>("Panel",
		13, 240, 195, 192, MyGUI::Align::Default, "CitiesPanel" + toString(pIndex));

	lText = lPanel->createWidget<MyGUI::StaticText>("StaticText",
		4, 4, 187, 184, MyGUI::Align::Default, "CityPanelText" + toString(pIndex));

	lText->setFontHeight(14);
	lText->setCaption(StrLoc::get()->COSelectCity());

	lPanel = lTabItem->createWidget<MyGUI::Widget>("Panel",
		221, 240, 195, 192, MyGUI::Align::Default, "PowerplantsPanel" + toString(pIndex));

	lText = lPanel->createWidget<MyGUI::StaticText>("StaticText",
		4, 4, 187, 184, MyGUI::Align::Default, "PowerplantPanelText" + toString(pIndex));

	lText->setFontHeight(14);
	lText->setCaption(StrLoc::get()->COSelectPP());

	lPanel = lTabItem->createWidget<MyGUI::Widget>("Panel",
		429, 28, 195, 404, MyGUI::Align::Default, "NetPanel" + toString(pIndex));

	lText = lPanel->createWidget<MyGUI::StaticText>("StaticText",
		4, 4, 187, 396, MyGUI::Align::Default, "NetPanelText" + toString(pIndex));

	lText->setFontHeight(14);

	std::string lEfficiency;

	if (mCompany->getPowerNet()->getSubnetEfficiency(pIndex) == -1)
		lEfficiency = "N/A";
	else
		lEfficiency = toString(mCompany->getPowerNet()->getSubnetEfficiency(pIndex)) + "%";

	lText->setCaption(StrLoc::get()->CONetEfficiency() + lEfficiency + "\n" 
    + StrLoc::get()->CONetAvailable()
		+ toString(lPowerAvailable) + "MW\n" + StrLoc::get()->CONetNeeded() 
    + toString(lPowerNeeded) + "MW");
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::powerNetListPressed(MyGUI::ListPtr _widget, size_t _index)
{
	int lPowerNetIndex = toNumber<int>(_widget->getName().substr(_widget->getName().size()-1, 1));

	if (_widget->getName().find("CitiesList") != -1) {
		if (_index != MyGUI::ITEM_NONE) {
			boost::shared_ptr<City>* lCity = _widget->getItemDataAt<boost::shared_ptr<City> >(_index);

			mGUI->findWidget<MyGUI::StaticText>("CityPanelText" 
        + toString(lPowerNetIndex))->setCaption(
				StrLoc::get()->Residents() + toString(lCity->get()->getResidentCount()) 
        + "\n" + StrLoc::get()->PowerNeeded() + "\n"
				+ toString(lCity->get()->getConsumption()) + "MW"
				+ "\n" + StrLoc::get()->PowerSuppliedByYou() + "\n" 
        + toString(lCity->get()->getConsumption()-lCity->get()->getPowerNeeded()) + "MW");
		} else {
			mGUI->findWidget<MyGUI::StaticText>("CityPanelText" 
        + toString(lPowerNetIndex))->setCaption(StrLoc::get()->COSelectCity());
		}
	}
	else if (_widget->getName().find("PowerplantsList") != -1) {
		if (_index != MyGUI::ITEM_NONE) {
			boost::shared_ptr<Powerplant>* lPowerplant 
        = _widget->getItemDataAt<boost::shared_ptr<Powerplant> >(_index);

			mGUI->findWidget<MyGUI::StaticText>("PowerplantPanelText" 
        + toString(lPowerNetIndex))->setCaption(
				StrLoc::get()->TooltipPower() + toString(lPowerplant->get()->getPowerInMW()) 
        + "MW\n" + StrLoc::get()->CONetNeeded() 
				+ toString(lPowerplant->get()->getPowerInMW() 
        - lPowerplant->get()->getPowerLeft()) + "MW");
		} else {
			mGUI->findWidget<MyGUI::StaticText>("PowerplantPanelText" 
        + toString(lPowerNetIndex))->setCaption(StrLoc::get()->COSelectPP());
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::updateCompanyData(EventData* pData)
{
	std::string lEfficiency = "";

	if (mCompany->getPowerNet()->getEfficiency() == -1) {
		lEfficiency = "N/A";
		mGUI->findWidget<MyGUI::StaticText>("Efficiency")
      ->setTextColour(MyGUI::Colour(0.95, 0.95, 0.95));
	} else {
		lEfficiency = toString(mCompany->getPowerNet()->getEfficiency()) + "%";

		if(mCompany->getPowerNet()->getEfficiency() < 50)
			mGUI->findWidget<MyGUI::StaticText>("Efficiency")
      ->setTextColour(MyGUI::Colour(0.98,0.0,0.0));
		else if((mCompany->getPowerNet()->getEfficiency() >= 50)
            && (mCompany->getPowerNet()->getEfficiency() < 80))
			mGUI->findWidget<MyGUI::StaticText>("Efficiency")
      ->setTextColour(MyGUI::Colour(0.98,0.98,0.0));
		else
			mGUI->findWidget<MyGUI::StaticText>("Efficiency")
      ->setTextColour(MyGUI::Colour(0.0,0.98,0.0));
	}

	mGUI->findWidget<MyGUI::StaticText>("MoneyShadow")
    ->setCaption(toStringW(mCompany->getMoney()/1000) + L"k€");
	mGUI->findWidget<MyGUI::StaticText>("Money")
    ->setCaption(toStringW(mCompany->getMoney()/1000) + L"k€");
	mGUI->findWidget<MyGUI::StaticText>("Efficiency")->setCaption(lEfficiency);
	mGUI->findWidget<MyGUI::StaticText>("EfficiencyShadow")->setCaption(lEfficiency);

	changeShadowTextCaption("CoalText", StrLoc::get()->COCoal()
		+ toString(mCompany->getCoal()) + " " + StrLoc::get()->TooltipKTons());

	changeShadowTextCaption("GasText", StrLoc::get()->COGas()
		+ toString(mCompany->getGas()) + " " + StrLoc::get()->TooltipKCubicFeet());

	changeShadowTextCaption("UraniumText", StrLoc::get()->COUranium()
		+ toString(mCompany->getUranium()) + " " + StrLoc::get()->TooltipRods());

	std::vector<MissionGoal> lGoals = mCompany->getMissionGoals();

	for (size_t i = 0; i < lGoals.size(); i++) {
		mGUI->findWidget<MyGUI::StaticText>("MissionGoal" 
      + toString(i+1))->setVisible(true);
		mGUI->findWidget<MyGUI::StaticText>("MissionGoal" 
      + toString(i+1))->setCaption(lGoals[i].mText);
		mGUI->findWidget<MyGUI::StaticImage>("MissionMarker" 
      + toString(i+1))->setImageTexture(lGoals[i].mDone ? "done.png" : "not_done.png");
		mGUI->findWidget<MyGUI::StaticImage>("MissionMarker" 
      + toString(i+1))->setVisible(true);
	}

	mGUI->findWidget<MyGUI::StaticText>("CoalPrice")->setCaption(StrLoc::get()
    ->COCoal() + "\n" + toString(mCompany->getCoalPrice()) 
    + L" k€/" + StrLoc::get()->COKTonShort());
	mGUI->findWidget<MyGUI::StaticText>("GasPrice")->setCaption(StrLoc::get()
    ->COGas() + "\n" + toString(mCompany->getGasPrice()) 
    + L" k€/" + StrLoc::get()->COKCubicShort());
	mGUI->findWidget<MyGUI::StaticText>("UraniumPrice")->setCaption(StrLoc::get()
    ->COUranium() + "\n" + toString(mCompany->getUraniumPrice()) 
    + L" k€/" + StrLoc::get()->CORodShort());

	mGUI->findWidget<MyGUI::Edit>("Price")
    ->setCaption(toString(mCompany->getPrice()));
	mGUI->findWidget<MyGUI::Edit>("Advertising")
    ->setCaption(toString(mCompany->getAdvertising()));
	mGUI->findWidget<MyGUI::Edit>("Maintenance"
    )->setCaption(toString(mCompany->getMaintenance()));

	mGUI->findWidget<MyGUI::Edit>("Image")
    ->setCaption(toString(mCompany->getImage()));
	mGUI->findWidget<MyGUI::Edit>("Customers"
    )->setCaption(toString(mCompany->getCustomers()));

	mGUI->findWidget<MyGUI::Edit>("EnvSupport")
    ->setCaption(toString(mCompany->getEnvSupport()));
	mGUI->findWidget<MyGUI::Edit>("SupplyMaint")
    ->setCaption(toString(mCompany->getSupplyMaintenance()));
	mGUI->findWidget<MyGUI::Edit>("EnvSupport")
    ->setCaption(toString(mCompany->getEnvSupport()));

	if (mGUI->findWidget<MyGUI::StaticImage>("CityPanel")->isVisible())
		showCityInfo(mCityInfo);

	if (mGUI->findWidget<MyGUI::StaticImage>("PowerplantPanel")->isVisible())
		showPowerPlantInfo(mPowerplantInfo);

	if (mGUI->findWidget<MyGUI::StaticImage>("CompanyPanel")->isVisible())
		showCompanyInfo(mCompanyInfo);

	if (mGUI->findWidget<MyGUI::StaticImage>("ResourceInfoPanel")->isVisible())
		showResourceInfo(mResourceInfo);


	int lResearchProgress = 0;

	mGUI->findWidget<MyGUI::Edit>("CurrentResearch")
    ->setCaption(mCompany->getCurrentResearch(lResearchProgress));
	mGUI->findWidget<MyGUI::Progress>("ResearchState")
    ->setProgressPosition(lResearchProgress);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::updateFinanceRadios(void)
{
	mGUI->findWidget<MyGUI::Button>("EmployeeNoneRadio")->setStateCheck(false);
	mGUI->findWidget<MyGUI::Button>("EmployeeBasicRadio")->setStateCheck(false);
	mGUI->findWidget<MyGUI::Button>("EmployeeExtensiveRadio")->setStateCheck(false);
	mGUI->findWidget<MyGUI::Button>("SupportPoorRadio")->setStateCheck(false);
	mGUI->findWidget<MyGUI::Button>("SupportNormalRadio")->setStateCheck(false);
	mGUI->findWidget<MyGUI::Button>("SupportExtensiveRadio")->setStateCheck(false);
	mGUI->findWidget<MyGUI::Button>("SafetyNoneRadio")->setStateCheck(false);
	mGUI->findWidget<MyGUI::Button>("SafetyNormalRadio")->setStateCheck(false);
	mGUI->findWidget<MyGUI::Button>("SafetyHeavyRadio")->setStateCheck(false);
	mGUI->findWidget<MyGUI::Button>("TermNoneRadio")->setStateCheck(false);
	mGUI->findWidget<MyGUI::Button>("Term12Radio")->setStateCheck(false);
	mGUI->findWidget<MyGUI::Button>("Term24Radio")->setStateCheck(false);

	if (mCompany->getTrainingLevel() == eNoTraining)
		mGUI->findWidget<MyGUI::Button>("EmployeeNoneRadio")->setStateCheck(true);
	else if (mCompany->getTrainingLevel() == eNormalTraining)
		mGUI->findWidget<MyGUI::Button>("EmployeeBasicRadio")->setStateCheck(true);
	else if (mCompany->getTrainingLevel() == eExtensiveTraining)
		mGUI->findWidget<MyGUI::Button>("EmployeeExtensiveRadio")->setStateCheck(true);

	if (mCompany->getSupportLevel() == eNoSupport)
		mGUI->findWidget<MyGUI::Button>("SupportPoorRadio")->setStateCheck(true);
	else if (mCompany->getSupportLevel() == eNormalSupport)
		mGUI->findWidget<MyGUI::Button>("SupportNormalRadio")->setStateCheck(true);
	else if (mCompany->getSupportLevel() == eExtensiveSupport)
		mGUI->findWidget<MyGUI::Button>("SupportExtensiveRadio")->setStateCheck(true);

	if (mCompany->getSafetyLevel() == eNoSafety)
		mGUI->findWidget<MyGUI::Button>("SafetyNoneRadio")->setStateCheck(true);
	else if (mCompany->getSafetyLevel() == eNormalSafety)
		mGUI->findWidget<MyGUI::Button>("SafetyNormalRadio")->setStateCheck(true);
	else if (mCompany->getSafetyLevel() == eExtensiveSafety)
		mGUI->findWidget<MyGUI::Button>("SafetyHeavyRadio")->setStateCheck(true);

	if (mCompany->getContractTime() == eNoTime)
		mGUI->findWidget<MyGUI::Button>("TermNoneRadio")->setStateCheck(true);
	else if (mCompany->getContractTime() == e12)
		mGUI->findWidget<MyGUI::Button>("Term12Radio")->setStateCheck(true);
	else if (mCompany->getContractTime() == e24)
		mGUI->findWidget<MyGUI::Button>("Term24Radio")->setStateCheck(true);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::fitInfoPanel(MyGUI::StaticImagePtr pInfoPanel, MyGUI::WidgetPtr pButtonPanel,
                       MyGUI::WidgetPtr pMinimapPanel)
{
	pInfoPanel->setPosition(pButtonPanel->getWidth() - 9 , mGUI->getViewHeight() 
    - pInfoPanel->getHeight() + 116);
	pInfoPanel->setSize(mGUI->getViewWidth() - pButtonPanel->getWidth() 
    - pMinimapPanel->getWidth() + 18, 
		pInfoPanel->getHeight());
	pInfoPanel->setVisible(false);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::setMinimap(std::string pFilename)
{
  mGUI->findWidget<MyGUI::StaticImage>("MinimapBackMap")->setImageTexture(pFilename);
}

void GUI::tutorialClosePressed(MyGUI::WidgetPtr _widget)
{
	MyGUI::MessagePtr lQuestion = 
    MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
    StrLoc::get()->TutorialReallyClose(),
		MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No | MyGUI::MessageBoxStyle::IconQuest);
	lQuestion->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::tutorialCloseQuestion);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::tutorialWindowClosePressed(MyGUI::WindowPtr _widget, const std::string& _string)
{
	MyGUI::MessagePtr lQuestion = 
    MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
		StrLoc::get()->TutorialReallyClose(),
		MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No | MyGUI::MessageBoxStyle::IconQuest);
	lQuestion->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::tutorialCloseQuestion);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::tutorialCloseQuestion(MyGUI::MessagePtr _sender, MyGUI::MessageBoxStyle _style)
{
	if (_style == MyGUI::MessageBoxStyle::Yes) {
		mGUI->findWidget<MyGUI::Window>("TutorialWindow")->setVisible(false);
		mTutorialClosed = true;
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::tutorialShutdownQuestion(MyGUI::MessagePtr _sender, MyGUI::MessageBoxStyle _style)
{
	if (_style == MyGUI::MessageBoxStyle::Yes) {
		EventHandler::raiseEvent(eShutdownGame);
		Audio::setMenu();
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::tutorialNextQuestion(MyGUI::MessagePtr _sender, MyGUI::MessageBoxStyle _style)
{
	if (_style == MyGUI::MessageBoxStyle::Yes) {
		std::string lNextMission;

		if (mMission == "de_tutorial1.xml")
			lNextMission = "de_tutorial2.xml";
		else if (mMission == "de_tutorial2.xml")
			lNextMission = "de_tutorial3.xml";

		StartData lStartData;
		lStartData.mMission = lNextMission;
		lStartData.mSandbox = false;
		lStartData.mNew = true;

		EventHandler::raiseEvent(eDismantleGame, new EventArg<StartData>(lStartData));
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::tutorialContinuePressed(MyGUI::WidgetPtr _widget)
{
	if (mTutorialStep < (StrLoc::get()->TutorialStep()[mTutorialIndex].size()-1)) {
		mTutorialStep++;
		mGUI->findWidget<MyGUI::Edit>("TutorialText")->setCaption(StrLoc::get()
      ->TutorialStep()[mTutorialIndex][mTutorialStep]);
		mGUI->findWidget<MyGUI::Window>("TutorialWindow")->setCaption(StrLoc::get()
      ->Tutorial() + " " + toString(mTutorialIndex+1) + " (" + StrLoc::get()->Step() + " " 
			+ toString(mTutorialStep+1) + "/" + toString(StrLoc::get()
      ->TutorialStep()[mTutorialIndex].size()) + ")");

		mGUI->findWidget<MyGUI::Button>("TutorialBack")->setVisible(true);

		if (mTutorialStep == (StrLoc::get()->TutorialStep()[mTutorialIndex].size()-1)) {
			if (mTutorialIndex == 2)
				mGUI->findWidget<MyGUI::Button>("TutorialContinue")
        ->setCaption(StrLoc::get()->EndTutorial());
			else
				mGUI->findWidget<MyGUI::Button>("TutorialContinue")
        ->setCaption(StrLoc::get()->NextTutorial());
		}
	} else {
		std::string lNextMission;
		if(mMission == "de_tutorial1.xml") {
			MyGUI::MessagePtr lQuestion = 
        MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
				StrLoc::get()->NextTutorialQuestion(),
				MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No | MyGUI::MessageBoxStyle::IconQuest);
			lQuestion->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::tutorialNextQuestion);
		}
		else if(mMission == "de_tutorial2.xml") {
			MyGUI::MessagePtr lQuestion = MyGUI::
        Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
				StrLoc::get()->NextTutorialQuestion(),
				MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No | MyGUI::MessageBoxStyle::IconQuest);
			lQuestion->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::tutorialNextQuestion);
		} else {
			MyGUI::MessagePtr lQuestion = 
        MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
				StrLoc::get()->TutorialReallyClose(),
				MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No | MyGUI::MessageBoxStyle::IconQuest);
			lQuestion->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::tutorialShutdownQuestion);
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::tutorialBackPressed(MyGUI::WidgetPtr _widget)
{
	if (mTutorialStep > 0) {
		mTutorialStep--;
		mGUI->findWidget<MyGUI::Edit>("TutorialText")
      ->setCaption(StrLoc::get()->TutorialStep()[mTutorialIndex][mTutorialStep]);
		mGUI->findWidget<MyGUI::Window>("TutorialWindow")
      ->setCaption(StrLoc::get()->Tutorial() + " " + toString(mTutorialIndex+1) 
      + " (" + StrLoc::get()->Step() + " " 
			+ toString(mTutorialStep+1) + "/" 
      + toString(StrLoc::get()->TutorialStep()[mTutorialIndex].size()) + ")");
	}

	if (mTutorialStep == 0)
		mGUI->findWidget<MyGUI::Button>("TutorialBack")->setVisible(false);

	mGUI->findWidget<MyGUI::Button>("TutorialContinue")->setCaption(StrLoc::get()->TutorialContinue());
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::askForCompanyName(void)
{
	mGUI->findWidget<MyGUI::Window>("CompanyNameWindow")->setVisible(true);
	mGUI->findWidget<MyGUI::Window>("CompanyNameWindow")->setShadow(true);
	mGUI->findWidget<MyGUI::Window>("CompanyNameWindow")->setCaption(StrLoc::get()->GameTitle());

	mGUI->findWidget<MyGUI::Window>("CompanyNameWindow")->setPosition(
		(mGUI->getRenderWindow()->getWidth()/2)
    -(mGUI->findWidget<MyGUI::Window>("CompanyNameWindow")->getWidth()/2),
		(mGUI->getRenderWindow()->getHeight()/2)
    -(mGUI->findWidget<MyGUI::Window>("CompanyNameWindow")->getHeight()/2));

	mGUI->findWidget<MyGUI::StaticText>("CompanyNameQuestion")
    ->setCaption(StrLoc::get()->QuestionCompanyName());
	mGUI->findWidget<MyGUI::Edit>("CompanyName")
    ->setCaption(StrLoc::get()->DefaultCompanyName());

	mGUI->findWidget<MyGUI::Button>("CompanyNameOK")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::companyNameOK);

	mGUI->findWidget<MyGUI::Window>("CompanyNameWindow")->eventWindowButtonPressed = 
		MyGUI::newDelegate(this, &GUI::companyNameWOK);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::companyNameOK(MyGUI::WidgetPtr _widget)
{
	mGUI->findWidget<MyGUI::Window>("CompanyNameWindow")->setVisible(false);
	mCompany->setName(mGUI->findWidget<MyGUI::Edit>("CompanyName")->getCaption());

	EventHandler::raiseEvent(eShowNewspaper, 
		new EventArg<std::string>("np_founded.png", StrLoc::get()->NPFounded(),
    StrLoc::get()->NPFoundedDetail()));
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::companyNameWOK(MyGUI::WindowPtr _widget, const std::string& _string)
{
	MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
    StrLoc::get()->PleaseChooseCompanyName());
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::setupGamePanels()
{
	Ogre::RenderWindow *lWindow = mGUI->getRenderWindow();

	mInfoPanelWidget = mGUI->createWidget<MyGUI::Widget>("Default",
		0, 0, mGUI->getRenderWindow()->getWidth(), mGUI->getRenderWindow()
    ->getHeight(), MyGUI::Align::Default, "Overlapped");
	mInfoPanelWidget->setNeedMouseFocus(false);

	if (mTutorial && !mTutorialClosed) {
		if (mMission == "de_tutorial1.xml")
			mTutorialIndex = 0;
		else if (mMission == "de_tutorial2.xml")
			mTutorialIndex = 1;
		else
			mTutorialIndex = 2;
	
		mGUI->findWidget<MyGUI::Window>("TutorialWindow")->setVisible(true);
		mGUI->findWidget<MyGUI::Window>("TutorialWindow")->setShadow(true);
		mGUI->findWidget<MyGUI::Window>("TutorialWindow")->setPosition(lWindow->getWidth()-418, 60);
		mGUI->findWidget<MyGUI::Window>("TutorialWindow")
      ->setCaption(StrLoc::get()->Tutorial() 
      + " " + toString(mTutorialIndex+1) + " (" + StrLoc::get()->Step() + " " 
			+ toString(mTutorialStep+1) + "/" 
      + toString(StrLoc::get()->TutorialStep()[mTutorialIndex].size()) + ")");
		mGUI->findWidget<MyGUI::Button>("TutorialContinue")
      ->setCaption(StrLoc::get()->TutorialContinue());
		mGUI->findWidget<MyGUI::Button>("TutorialBack")
      ->setCaption(StrLoc::get()->TutorialBack());
		mGUI->findWidget<MyGUI::Button>("TutorialEnd")
      ->setCaption(StrLoc::get()->TutorialExit());
		mGUI->findWidget<MyGUI::Edit>("TutorialText")
      ->setCaption(StrLoc::get()->TutorialStep()[mTutorialIndex][mTutorialStep]);

		if(mTutorialStep == 0)
			mGUI->findWidget<MyGUI::Button>("TutorialBack")->setVisible(false);

		mGUI->findWidget<MyGUI::Button>("TutorialContinue")->eventMouseButtonClick = 
			MyGUI::newDelegate(this, &GUI::tutorialContinuePressed);
		mGUI->findWidget<MyGUI::Button>("TutorialEnd")->eventMouseButtonClick = 
			MyGUI::newDelegate(this, &GUI::tutorialClosePressed);
		mGUI->findWidget<MyGUI::Window>("TutorialWindow")->eventWindowButtonPressed = 
			MyGUI::newDelegate(this, &GUI::tutorialWindowClosePressed);
		mGUI->findWidget<MyGUI::Button>("TutorialBack")->eventMouseButtonClick = 
			MyGUI::newDelegate(this, &GUI::tutorialBackPressed);
	}

	MyGUI::WidgetPtr lButtonPanel = mGUI->findWidget<MyGUI::Widget>("ButtonPanel");
	MyGUI::WidgetPtr lDateTimePanel = mGUI->findWidget<MyGUI::Widget>("DateTimePanel");
	MyGUI::WidgetPtr lMinimapPanel = mGUI->findWidget<MyGUI::Widget>("MinimapPanel");
	MyGUI::WidgetPtr lResourcePanel = mGUI->findWidget<MyGUI::Widget>("ResourcePanel");
	MyGUI::WidgetPtr lTickerPanel = mGUI->findWidget<MyGUI::Widget>("TickerPanel");
	MyGUI::StaticImagePtr lBuildPowerplantPanel = mGUI->findWidget<MyGUI::StaticImage>("BuildPowerplantPanel");
	MyGUI::StaticImagePtr lBuildResourcePanel = mGUI->findWidget<MyGUI::StaticImage>("BuildResourcePanel");
	MyGUI::StaticImagePtr lBuildDistributionPanel = mGUI->findWidget<MyGUI::StaticImage>("BuildDistributionPanel");
	MyGUI::StaticImagePtr lBuildMiscPanel = mGUI->findWidget<MyGUI::StaticImage>("BuildMiscPanel");
	MyGUI::StaticImagePtr lPowerplantPanel = mGUI->findWidget<MyGUI::StaticImage>("PowerplantPanel");
	MyGUI::StaticImagePtr lCityPanel = mGUI->findWidget<MyGUI::StaticImage>("CityPanel");
	MyGUI::StaticImagePtr lCompanyPanel = mGUI->findWidget<MyGUI::StaticImage>("CompanyPanel");
	MyGUI::StaticImagePtr lResourceInfoPanel = mGUI->findWidget<MyGUI::StaticImage>("ResourceInfoPanel");
	MyGUI::StaticImagePtr lInfoPanel = mGUI->findWidget<MyGUI::StaticImage>("InfoPanel");

	lButtonPanel->setPosition(0, lWindow->getHeight() - lButtonPanel->getHeight());
	lDateTimePanel->setPosition(lWindow->getWidth() - lDateTimePanel->getWidth() - 2,2);
	lMinimapPanel->setPosition(lWindow->getWidth() - lMinimapPanel->getWidth(),
							   lWindow->getHeight() - lMinimapPanel->getHeight());
	lResourcePanel->setPosition(2, 2);

	lTickerPanel->setPosition(lResourcePanel->getWidth() + 2, 2);
	lTickerPanel->setSize(lWindow->getWidth() - lResourcePanel->getWidth()
    - lDateTimePanel->getWidth() - 4,
							lTickerPanel->getHeight());

	mGUI->findWidget<MyGUI::StaticImage>("MiddleTicker")->setSize(lTickerPanel->getWidth()-96, 64);
	mGUI->findWidget<MyGUI::StaticImage>("RightTicker")->setPosition(lTickerPanel->getWidth()-32,0);

	fitInfoPanel(lBuildPowerplantPanel, lButtonPanel, lMinimapPanel);
	fitInfoPanel(lBuildResourcePanel, lButtonPanel, lMinimapPanel);
	fitInfoPanel(lBuildDistributionPanel, lButtonPanel, lMinimapPanel);
	fitInfoPanel(lBuildMiscPanel, lButtonPanel, lMinimapPanel);
	fitInfoPanel(lPowerplantPanel, lButtonPanel, lMinimapPanel);
	fitInfoPanel(lCityPanel, lButtonPanel, lMinimapPanel);
	fitInfoPanel(lCompanyPanel, lButtonPanel, lMinimapPanel);
	fitInfoPanel(lResourceInfoPanel, lButtonPanel, lMinimapPanel);
	fitInfoPanel(lInfoPanel, lButtonPanel, lMinimapPanel);
	fitInfoPanel(lBuildPowerplantPanel, lButtonPanel, lMinimapPanel);

	mGUI->findWidget<MyGUI::Button>("SpeedFaster")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::gameSpeedFasterPressed); 
	mGUI->findWidget<MyGUI::Button>("SpeedSlower")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::gameSpeedSlowerPressed); 
	mGUI->findWidget<MyGUI::Button>("SpeedPause")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::gameSpeedPausePressed); 
	mGUI->findWidget<MyGUI::Button>("BuildPowerplant")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::buildPowerPlantPressed); 
	mGUI->findWidget<MyGUI::Button>("BuildResource")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::buildResourcePlantPressed); 
	mGUI->findWidget<MyGUI::Button>("BuildDeployment")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::buildDistributionPressed); 
	mGUI->findWidget<MyGUI::Button>("BuildMisc")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::buildMiscPressed); 
	mGUI->findWidget<MyGUI::Button>("Demolish")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::demolishPressed); 
	mGUI->findWidget<MyGUI::StaticImage>("Minimap")->eventMouseButtonPressed = 
		MyGUI::newDelegate(this, &GUI::minimapPressed);
	mGUI->findWidget<MyGUI::StaticImage>("Minimap")->eventMouseDrag = 
		MyGUI::newDelegate(this, &GUI::minimapDragged);
	mGUI->findWidget<MyGUI::Button>("ShowHideResource")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::showHideResourcesPressed);
	mGUI->findWidget<MyGUI::Window>("PowerNetsWindow")->eventWindowButtonPressed = 
		MyGUI::newDelegate(this, &GUI::closeWindowPressed);
	mGUI->findWidget<MyGUI::Button>("ShowPowerNets")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::showPowerNetsWindowPressed);
	mGUI->findWidget<MyGUI::Button>("TickerArchive")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::showTickerArchiveWindowPressed);
	mGUI->findWidget<MyGUI::Window>("TickerArchiveWindow")->eventWindowButtonPressed = 
		MyGUI::newDelegate(this, &GUI::closeWindowPressed);
	mGUI->findWidget<MyGUI::StaticText>("TickerLine1")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::tickerLinePressed);
	mGUI->findWidget<MyGUI::StaticText>("TickerLine2")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::tickerLinePressed);
	mGUI->findWidget<MyGUI::StaticText>("TickerLine1")->eventMouseSetFocus = 
		MyGUI::newDelegate(this, &GUI::tickerLineMouseOver);
	mGUI->findWidget<MyGUI::StaticText>("TickerLine2")->eventMouseSetFocus = 
		MyGUI::newDelegate(this, &GUI::tickerLineMouseOver);
	mGUI->findWidget<MyGUI::StaticText>("TickerLine1")->eventMouseLostFocus = 
		MyGUI::newDelegate(this, &GUI::tickerLineMouseOut);
	mGUI->findWidget<MyGUI::StaticText>("TickerLine2")->eventMouseLostFocus = 
		MyGUI::newDelegate(this, &GUI::tickerLineMouseOut);
	mGUI->findWidget<MyGUI::Button>("Menu")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::menuPressed); 
	mGUI->findWidget<MyGUI::Button>("Mission")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::missionPressed);
	mGUI->findWidget<MyGUI::Button>("FinancesMain")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesPressed);
	mGUI->findWidget<MyGUI::Button>("Finances")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesPressed);
	mGUI->findWidget<MyGUI::Button>("Trade")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::tradePressed);
	mGUI->findWidget<MyGUI::Button>("CEO")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::ceoPressed);
	mGUI->findWidget<MyGUI::Button>("Research")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::researchPressed);
	mGUI->findWidget<MyGUI::Button>("ZoomIn")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::zoomRotatePressed);
	mGUI->findWidget<MyGUI::Button>("RotateLeft")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::zoomRotatePressed);
	mGUI->findWidget<MyGUI::Button>("ZoomOut")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::zoomRotatePressed);
	mGUI->findWidget<MyGUI::Button>("RotateRight")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::zoomRotatePressed);
	mGUI->findWidget<MyGUI::Window>("TradeWindow")->eventWindowButtonPressed = 
		MyGUI::newDelegate(this, &GUI::closeWindowPressed);
	mGUI->findWidget<MyGUI::Window>("MissionWindow")->eventWindowButtonPressed = 
		MyGUI::newDelegate(this, &GUI::closeWindowPressed);
	mGUI->findWidget<MyGUI::Window>("ResearchWindow")->eventWindowButtonPressed = 
		MyGUI::newDelegate(this, &GUI::closeWindowPressed);
	mGUI->findWidget<MyGUI::Window>("FinancesWindow")->eventWindowButtonPressed = 
		MyGUI::newDelegate(this, &GUI::closeWindowPressed);
	mGUI->findWidget<MyGUI::Window>("CEOWindow")->eventWindowButtonPressed = 
		MyGUI::newDelegate(this, &GUI::closeWindowPressed);
	mGUI->findWidget<MyGUI::Button>("MinimapShowSupply")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::minimapButtonPressed);
	mGUI->findWidget<MyGUI::Button>("MinimapShowNets")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::minimapButtonPressed);
	mGUI->findWidget<MyGUI::Button>("MinimapShowResources")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::minimapButtonPressed);
	mGUI->findWidget<MyGUI::Button>("MinimapShowDefault")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::minimapButtonPressed);
	mGUI->findWidget<MyGUI::Button>("PPRepair")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::repairButtonPressed);
	mGUI->findWidget<MyGUI::Button>("RERepair")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::repairButtonPressed);

	int lNextButtonX = 10;
	int lNextButtonY = 10;

	createButton(lBuildPowerplantPanel, "?", toString(ePPNuclearLarge), lNextButtonX, lNextButtonY);
	computeNextButtonPlacement(lBuildPowerplantPanel, lNextButtonX, lNextButtonY);
	createButton(lBuildPowerplantPanel, "?", toString(ePPNuclearSmall), lNextButtonX, lNextButtonY);
	computeNextButtonPlacement(lBuildPowerplantPanel, lNextButtonX, lNextButtonY);
	createButton(lBuildPowerplantPanel, "CoaS", toString(ePPCoalSmall), lNextButtonX, lNextButtonY);
	computeNextButtonPlacement(lBuildPowerplantPanel, lNextButtonX, lNextButtonY);
	createButton(lBuildPowerplantPanel, "?", toString(ePPCoalLarge), lNextButtonX, lNextButtonY);
	computeNextButtonPlacement(lBuildPowerplantPanel, lNextButtonX, lNextButtonY);
	createButton(lBuildPowerplantPanel, "GasS", toString(ePPGasSmall), lNextButtonX, lNextButtonY);
	computeNextButtonPlacement(lBuildPowerplantPanel, lNextButtonX, lNextButtonY);
	createButton(lBuildPowerplantPanel, "?", toString(ePPGasLarge), lNextButtonX, lNextButtonY);
	computeNextButtonPlacement(lBuildPowerplantPanel, lNextButtonX, lNextButtonY);
	createButton(lBuildPowerplantPanel, "WinS", toString(ePPWindSmall), lNextButtonX, lNextButtonY);
	computeNextButtonPlacement(lBuildPowerplantPanel, lNextButtonX, lNextButtonY);
	createButton(lBuildPowerplantPanel, "?", toString(ePPWindLarge), lNextButtonX, lNextButtonY);
	computeNextButtonPlacement(lBuildPowerplantPanel, lNextButtonX, lNextButtonY);
	createButton(lBuildPowerplantPanel, "?", toString(ePPWindOffshore), lNextButtonX, lNextButtonY);
	computeNextButtonPlacement(lBuildPowerplantPanel, lNextButtonX, lNextButtonY);
	createButton(lBuildPowerplantPanel, "?", toString(ePPBio), lNextButtonX, lNextButtonY);
	computeNextButtonPlacement(lBuildPowerplantPanel, lNextButtonX, lNextButtonY);
	createButton(lBuildPowerplantPanel, "?", toString(ePPSolarLarge), lNextButtonX, lNextButtonY);
	computeNextButtonPlacement(lBuildPowerplantPanel, lNextButtonX, lNextButtonY);

	createButton(lBuildPowerplantPanel, "SolS", toString(ePPSolarSmall), lNextButtonX, lNextButtonY);
	computeNextButtonPlacement(lBuildPowerplantPanel, lNextButtonX, lNextButtonY);
	createButton(lBuildPowerplantPanel, "?", toString(ePPSolarUpdraft), lNextButtonX, lNextButtonY);
	computeNextButtonPlacement(lBuildPowerplantPanel, lNextButtonX, lNextButtonY);
	createButton(lBuildPowerplantPanel, "?", toString(ePPNuclearFusion), lNextButtonX, lNextButtonY);

	lNextButtonX = 10;
	lNextButtonY = 10;

	createButton(lBuildResourcePanel, "RGaS", toString(eREGasSmall), lNextButtonX, lNextButtonY);
	computeNextButtonPlacement(lBuildResourcePanel, lNextButtonX, lNextButtonY);
	createButton(lBuildResourcePanel, "?", toString(eREGasLarge), lNextButtonX, lNextButtonY);
	computeNextButtonPlacement(lBuildResourcePanel, lNextButtonX, lNextButtonY);
	createButton(lBuildResourcePanel, "RCoS", toString(eRECoalSmall), lNextButtonX, lNextButtonY);
	computeNextButtonPlacement(lBuildResourcePanel, lNextButtonX, lNextButtonY);
	createButton(lBuildResourcePanel, "?", toString(eRECoalLarge), lNextButtonX, lNextButtonY);
	computeNextButtonPlacement(lBuildResourcePanel, lNextButtonX, lNextButtonY);
	createButton(lBuildResourcePanel, "RUrS", toString(eREUraniumSmall), lNextButtonX, lNextButtonY);
	computeNextButtonPlacement(lBuildResourcePanel, lNextButtonX, lNextButtonY);
	createButton(lBuildResourcePanel, "?", toString(eREUraniumLarge), lNextButtonX, lNextButtonY);

	lNextButtonX = 10;
	lNextButtonY = 10;

	createButton(lBuildDistributionPanel, "Large", toString(eDIPoleLarge), lNextButtonX, lNextButtonY);
	computeNextButtonPlacement(lBuildDistributionPanel, lNextButtonX, lNextButtonY);
	createButton(lBuildDistributionPanel, "Small", toString(eDIPoleSmall), lNextButtonX, lNextButtonY);

	lNextButtonX = 10;
	lNextButtonY = 10;

	createButton(lBuildMiscPanel, "Head", toString(eCOHeadquarters), lNextButtonX, lNextButtonY);
	computeNextButtonPlacement(lBuildMiscPanel, lNextButtonX, lNextButtonY);
	createButton(lBuildMiscPanel, "Rese", toString(eCOResearch), lNextButtonX, lNextButtonY);
	computeNextButtonPlacement(lBuildMiscPanel, lNextButtonX, lNextButtonY);
	createButton(lBuildMiscPanel, "Publ", toString(eCOPublicRelations), lNextButtonX, lNextButtonY);

	if (mCompany) {
		updateResearchableButtons(0);
		updateCOBuildingButtons(0);

		if (mCurrentSpeed == ePause)
			mGUI->findWidget<MyGUI::StaticImage>("SpeedIndicator")->setImageTexture("pause.png");
		else if (mCurrentSpeed == eSlow)
			mGUI->findWidget<MyGUI::StaticImage>("SpeedIndicator")->setImageTexture("slow.png");
		else if (mCurrentSpeed == eNormal)
			mGUI->findWidget<MyGUI::StaticImage>("SpeedIndicator")->setImageTexture("normal.png");
		else if (mCurrentSpeed == eFast)
			mGUI->findWidget<MyGUI::StaticImage>("SpeedIndicator")->setImageTexture("fast.png");
	}

	if (mTickerArchiveLoaded) {
		for (size_t i = 0; i < mTickerArchive.size(); ++i)
			mGUI->findWidget<MyGUI::List>("TickerList")->addItem(mTickerArchive[i].mDateTime 
      + ": " + mTickerArchive[i].mMessage);

		mTickerArchiveLoaded = false;
	}

	mGUI->findWidget<MyGUI::StaticImage>("NewspaperImage")
    ->setImageTexture(StrLoc::get()->NewspaperImage());
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::computeNextButtonPlacement(MyGUI::StaticImagePtr pPanel, int &oNextX, int &oNextY)
{
	int lPanelWidth = pPanel->getWidth();

	if ((oNextX + 2*cPanelButtonSize + cPanelButtonGap) > (lPanelWidth - 4)) {
		oNextX = 10;
		oNextY = oNextY + cPanelButtonSize + cPanelButtonGap;
	} else {
		oNextX = oNextX + cPanelButtonSize + cPanelButtonGap;
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::createButton(MyGUI::StaticImagePtr pPanel, std::string pCaption, std::string pName,
                       int pX, int pY)
{
	MyGUI::ButtonPtr lButton = 
		pPanel->createWidget<MyGUI::Button>("Build" + pName, 
		pX, pY, cPanelButtonSize, cPanelButtonSize, MyGUI::Align::Default, pName);
	lButton->setCaption(pCaption);
	lButton->eventMouseButtonClick = MyGUI::newDelegate(this, &GUI::buildPressed);

	if (pCaption == "?")
		lButton->setEnabled(false);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::switchToMainMenu(void)
{
	while(mInfoPanelWidget->getChildCount() > 0)
		mGUI->destroyWidget(mInfoPanelWidget->getChildAt(mInfoPanelWidget->getChildCount()-1));

	MyGUI::LayoutManager::getInstance().unloadLayout(mCurrentLayout);

  mCurrentLayout = MyGUI::LayoutManager::getInstance().load("main_menu_" + StrLoc::get()->LanguageCode() + ".layout");

	mGUI->findWidget<MyGUI::Widget>("MainMenuWidget")->setPosition(
		(mGUI->getRenderWindow()->getWidth()) 
    - (mGUI->findWidget<MyGUI::Widget>("MainMenuWidget")->getWidth()),
		mGUI->getRenderWindow()->getHeight()
    - mGUI->findWidget<MyGUI::Widget>("MainMenuWidget")->getHeight());

	MyGUI::StaticImagePtr lBackground = mGUI->findWidget<MyGUI::StaticImage>("Background");
	lBackground->setImageTexture(cBackgrounds[getCurrentAspectRatio()]);
	lBackground->setSize(mGUI->getViewWidth(), mGUI->getViewHeight());

	if (!mGameRunning || !mAskForRestart) {
		mGUI->findWidget<MyGUI::Button>("Resume")->setVisible(false);
		mGUI->findWidget<MyGUI::Button>("SaveGame")->setVisible(false);
	}

	mGUI->findWidget<MyGUI::Button>("Resume")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::menuResumeGamePressed);
	mGUI->findWidget<MyGUI::Button>("Exit")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::menuExitPressed);
	mGUI->findWidget<MyGUI::Button>("LoadGame")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::menuLoadPressed);
	mGUI->findWidget<MyGUI::Button>("SaveGame")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::menuSavePressed);
	mGUI->findWidget<MyGUI::Button>("Credits")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::menuCreditsPressed);
	mGUI->findWidget<MyGUI::Button>("Options")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::menuOptionsPressed);
	mGUI->findWidget<MyGUI::Button>("NewGame")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::menuNewPressed);

	mGUI->findWidget<MyGUI::Button>("Resume")->setCaption(StrLoc::get()->MainMenuResumeGame());
	mGUI->findWidget<MyGUI::Button>("Exit")->setCaption(StrLoc::get()->MainMenuClose());
	mGUI->findWidget<MyGUI::Button>("LoadGame")->setCaption(StrLoc::get()->MainMenuLoadGame());
	mGUI->findWidget<MyGUI::Button>("SaveGame")->setCaption(StrLoc::get()->MainMenuSaveGame());
	mGUI->findWidget<MyGUI::Button>("Credits")->setCaption(StrLoc::get()->MainMenuCredits());
	mGUI->findWidget<MyGUI::Button>("Options")->setCaption(StrLoc::get()->MainMenuOptions());
	mGUI->findWidget<MyGUI::Button>("NewGame")->setCaption(StrLoc::get()->MainMenuNewGame());

  // fade in hack
  unsigned long lStartTime = mRoot->getTimer()->getMilliseconds();
  unsigned long lPassedTime = 0;

  if (!mOptionsChanged) {
    while (lPassedTime < 100) {
      mGUI->findWidget<MyGUI::Widget>("MainMenuWidget")->setAlpha((float)lPassedTime/100.0);
      mRoot->renderOneFrame();
      lPassedTime = mRoot->getTimer()->getMilliseconds() - lStartTime;
    }
    mGUI->findWidget<MyGUI::Widget>("MainMenuWidget")->setAlpha(1.0);
  }
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::showInfo(boost::shared_ptr<GameObject> pObject)
{
	switch (pObject->getType()) {
	case eCity:
		showCityInfo(boost::dynamic_pointer_cast<City>(pObject));
		break;
	case ePowerplant:
		showPowerPlantInfo(boost::dynamic_pointer_cast<Powerplant>(pObject));
		break;
	case eCompany:
		showCompanyInfo(boost::dynamic_pointer_cast<CompanyBuilding>(pObject));
		break;
	case eResource:
		showResourceInfo(boost::dynamic_pointer_cast<ResourceBuilding>(pObject));
		break;
	default:
		break;
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::showCityInfo(boost::shared_ptr<City> pCity)
{
	mCityInfo = pCity;
	unshowInfo();
	mGUI->findWidget<MyGUI::StaticImage>("CityPanel")->setVisible(true);

	mGUI->findWidget<MyGUI::StaticText>("SuppliedByText")->setCaption(StrLoc::get()->SuppliedBy());
	mGUI->findWidget<MyGUI::StaticText>("PriceText")->setCaption(StrLoc::get()->Price());
	mGUI->findWidget<MyGUI::Button>("CityGlobalCheck")->setCaption(StrLoc::get()->UseGlobalSettings());
	mGUI->findWidget<MyGUI::StaticText>("AdsText")->setCaption(StrLoc::get()->AdsBudget());
	mGUI->findWidget<MyGUI::StaticText>("PriceUnitText")->setCaption(L"k€/\n" + StrLoc::get()->Week());
	mGUI->findWidget<MyGUI::StaticImage>("CityInfoPic")->setImageTexture(cCityInfoPic[pCity->getSize()]);
	mGUI->findWidget<MyGUI::StaticText>("CityName")->setCaption(pCity->getName());

	std::string lConnectThisCity = "";

	if(pCity->getCustomers() == 0)
		lConnectThisCity = StrLoc::get()->ConnectThisCity();

	mGUI->findWidget<MyGUI::StaticText>("CityInfo")->setCaption(StrLoc::get()->Residents() 
    + toString(pCity->getResidentCount())
		+ "\n" + StrLoc::get()->Customers() + toString(pCity->getCustomers())  
    + "\n" + StrLoc::get()->PowerNeeded() + "\n" + toString(pCity->getConsumption())
		+ "MW\n" + StrLoc::get()->PowerSuppliedByYou() + "\n" +
		toString(pCity->getConsumption() - pCity->getPowerNeeded()) + "MW" + "\n" + lConnectThisCity);

	mGUI->findWidget<MyGUI::StaticText>("CityPPs")->setCaption(pCity->getSuppliers());

	mGUI->findWidget<MyGUI::Button>("CityShowNet")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::cityButtonPressed);
	mGUI->findWidget<MyGUI::Button>("CityShowSales")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::cityButtonPressed);

	mGUI->findWidget<MyGUI::Button>("CityGlobalCheck")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::cityButtonPressed);

	if (mCityInfo->getInheritSettings()) {
		mCityInfo->setPrice(mCompany->getPrice());
		mCityInfo->setAdvertising(mCompany->getAdvertising());
	}

	mGUI->findWidget<MyGUI::Button>("CityGlobalCheck")->setStateCheck(mCityInfo->getInheritSettings());
	mGUI->findWidget<MyGUI::Button>("CityPriceUp")->setEnabled(!mCityInfo->getInheritSettings());
	mGUI->findWidget<MyGUI::Button>("CityPriceDown")->setEnabled(!mCityInfo->getInheritSettings());
	mGUI->findWidget<MyGUI::Button>("CityAdsUp")->setEnabled(!mCityInfo->getInheritSettings());
	mGUI->findWidget<MyGUI::Button>("CityAdsDown")->setEnabled(!mCityInfo->getInheritSettings());
	mGUI->findWidget<MyGUI::Edit>("CityPrice")->setEnabled(!mCityInfo->getInheritSettings());
	mGUI->findWidget<MyGUI::Edit>("CityPrice")->setCaption(toString(mCityInfo->getPrice()));
	mGUI->findWidget<MyGUI::Edit>("CityAds")->setEnabled(!mCityInfo->getInheritSettings());
	mGUI->findWidget<MyGUI::Edit>("CityAds")->setCaption(toString(mCityInfo->getAdvertising()));

	mGUI->findWidget<MyGUI::Button>("CityPriceUp")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::cityButtonPressed);
	mGUI->findWidget<MyGUI::Button>("CityPriceDown")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::cityButtonPressed);
	mGUI->findWidget<MyGUI::Button>("CityAdsUp")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::cityButtonPressed);
	mGUI->findWidget<MyGUI::Button>("CityAdsDown")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::cityButtonPressed);

	mGUI->findWidget<MyGUI::Button>("CityPriceUp")->eventMouseButtonPressed = 
		MyGUI::newDelegate(this, &GUI::buttonDownPressed);
	mGUI->findWidget<MyGUI::Button>("CityPriceDown")->eventMouseButtonPressed = 
		MyGUI::newDelegate(this, &GUI::buttonDownPressed);
	mGUI->findWidget<MyGUI::Button>("CityAdsUp")->eventMouseButtonPressed = 
		MyGUI::newDelegate(this, &GUI::buttonDownPressed);
	mGUI->findWidget<MyGUI::Button>("CityAdsDown")->eventMouseButtonPressed = 
		MyGUI::newDelegate(this, &GUI::buttonDownPressed);

	mGUI->findWidget<MyGUI::Button>("CityPriceUp")->eventMouseButtonReleased = 
		MyGUI::newDelegate(this, &GUI::buttonDownReleased);
	mGUI->findWidget<MyGUI::Button>("CityPriceDown")->eventMouseButtonReleased = 
		MyGUI::newDelegate(this, &GUI::buttonDownReleased);
	mGUI->findWidget<MyGUI::Button>("CityAdsUp")->eventMouseButtonReleased = 
		MyGUI::newDelegate(this, &GUI::buttonDownReleased);
	mGUI->findWidget<MyGUI::Button>("CityAdsDown")->eventMouseButtonReleased = 
		MyGUI::newDelegate(this, &GUI::buttonDownReleased);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::cityButtonPressed(MyGUI::WidgetPtr _widget)
{
	if (_widget->getName().find("CityGlobalCheck") != -1) {
		mGUI->findWidget<MyGUI::Button>("CityGlobalCheck")
      ->setStateCheck(!mGUI->findWidget<MyGUI::Button>("CityGlobalCheck")->getStateCheck());

		mCityInfo->setInheritSettings(mGUI->findWidget<MyGUI::Button>("CityGlobalCheck")
      ->getStateCheck());

		if (mGUI->findWidget<MyGUI::Button>("CityGlobalCheck")->getStateCheck()) {
			mCityInfo->setPrice(mCompany->getPrice());
			mCityInfo->setAdvertising(mCompany->getAdvertising());
		}

		mGUI->findWidget<MyGUI::Button>("CityPriceUp")->setEnabled(!mCityInfo->getInheritSettings());
		mGUI->findWidget<MyGUI::Button>("CityPriceDown")->setEnabled(!mCityInfo->getInheritSettings());
		mGUI->findWidget<MyGUI::Button>("CityAdsUp")->setEnabled(!mCityInfo->getInheritSettings());
		mGUI->findWidget<MyGUI::Button>("CityAdsDown")->setEnabled(!mCityInfo->getInheritSettings());
		mGUI->findWidget<MyGUI::Edit>("CityPrice")->setEnabled(!mCityInfo->getInheritSettings());
		mGUI->findWidget<MyGUI::Edit>("CityPrice")->setCaption(toString(mCityInfo->getPrice()));
		mGUI->findWidget<MyGUI::Edit>("CityAds")->setEnabled(!mCityInfo->getInheritSettings());
		mGUI->findWidget<MyGUI::Edit>("CityAds")->setCaption(toString(mCityInfo->getAdvertising()));
	}
	else if (_widget->getName().find("CityShowNet") != -1) {
		showPowerNetsWindowPressed(0);
	}
	else if (_widget->getName().find("CityShowSales") != -1) {
		financesPressed(0);
	}
	else if (_widget->getName().find("CityPriceUp") != -1) {
		if(mCityInfo->getPrice() < 100)
			mCityInfo->setPrice(mCityInfo->getPrice()+1);
		showCityInfo(mCityInfo);
	}
	else if (_widget->getName().find("CityPriceDown") != -1) {
		if(mCityInfo->getPrice() > 0)
			mCityInfo->setPrice(mCityInfo->getPrice()-1);
		showCityInfo(mCityInfo);
	}
	else if (_widget->getName().find("CityAdsUp") != -1) {
		if(mCityInfo->getAdvertising() < 100)
			mCityInfo->setAdvertising(mCityInfo->getAdvertising()+1);
		showCityInfo(mCityInfo);
	}
	else if (_widget->getName().find("CityAdsDown") != -1) {
		if(mCityInfo->getAdvertising() > 0)
			mCityInfo->setAdvertising(mCityInfo->getAdvertising()-1);
		showCityInfo(mCityInfo);
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::showPowerPlantInfo(boost::shared_ptr<Powerplant> pPowerplant)
{
	if (!pPowerplant)
		return;

	mPowerplantInfo = pPowerplant;
	unshowInfo();
	mGUI->findWidget<MyGUI::StaticImage>("PowerplantPanel")->setVisible(true);

	mGUI->findWidget<MyGUI::StaticText>("PPMaintenanceText")
    ->setCaption(StrLoc::get()->PPMaintenance());
	mGUI->findWidget<MyGUI::Button>("PPGlobalCheck")
    ->setCaption(StrLoc::get()->UseGlobalSettings());
	mGUI->findWidget<MyGUI::StaticText>("PPConnectedText")
    ->setCaption(StrLoc::get()->PPConnected());

	if (mPowerplantInfo->getSubtype() == ePPNuclearSmall)
		mGUI->findWidget<MyGUI::StaticImage>("PPInfoPic")->setImageTexture("pp_nuclear_small.png");
	else if (mPowerplantInfo->getSubtype() == ePPNuclearLarge)
		mGUI->findWidget<MyGUI::StaticImage>("PPInfoPic")->setImageTexture("pp_nuclear_large.png");
	else if (mPowerplantInfo->getSubtype() == ePPCoalSmall)
		mGUI->findWidget<MyGUI::StaticImage>("PPInfoPic")->setImageTexture("pp_coal_small.png");
	else if (mPowerplantInfo->getSubtype() == ePPCoalLarge)
		mGUI->findWidget<MyGUI::StaticImage>("PPInfoPic")->setImageTexture("pp_coal_large.png");
	else if (mPowerplantInfo->getSubtype() == ePPWindSmall)
		mGUI->findWidget<MyGUI::StaticImage>("PPInfoPic")->setImageTexture("pp_wind_small.png");
	else if (mPowerplantInfo->getSubtype() == ePPWindLarge)
		mGUI->findWidget<MyGUI::StaticImage>("PPInfoPic")->setImageTexture("pp_wind_large.png");
	else if (mPowerplantInfo->getSubtype() == ePPWindOffshore)
		mGUI->findWidget<MyGUI::StaticImage>("PPInfoPic")->setImageTexture("pp_wind_offshore.png");
	else if (mPowerplantInfo->getSubtype() == ePPWater)
		mGUI->findWidget<MyGUI::StaticImage>("PPInfoPic")->setImageTexture("pp_water.png");
	else if (mPowerplantInfo->getSubtype() == ePPBio)
		mGUI->findWidget<MyGUI::StaticImage>("PPInfoPic")->setImageTexture("pp_bio.png");
	else if (mPowerplantInfo->getSubtype() == ePPSolarSmall)
		mGUI->findWidget<MyGUI::StaticImage>("PPInfoPic")->setImageTexture("pp_solar_small.png");
	else if (mPowerplantInfo->getSubtype() == ePPSolarLarge)
		mGUI->findWidget<MyGUI::StaticImage>("PPInfoPic")->setImageTexture("pp_solar_large.png");
	else if (mPowerplantInfo->getSubtype() == ePPSolarUpdraft)
		mGUI->findWidget<MyGUI::StaticImage>("PPInfoPic")->setImageTexture("pp_solar_updraft.png");
	else if (mPowerplantInfo->getSubtype() == ePPNuclearFusion)
		mGUI->findWidget<MyGUI::StaticImage>("PPInfoPic")->setImageTexture("pp_nuclear_fusion.png");
	else if (mPowerplantInfo->getSubtype() == ePPGasLarge)
		mGUI->findWidget<MyGUI::StaticImage>("PPInfoPic")->setImageTexture("pp_gas_large.png");
	else if (mPowerplantInfo->getSubtype() == ePPGasSmall)
		mGUI->findWidget<MyGUI::StaticImage>("PPInfoPic")->setImageTexture("pp_gas_small.png");

	std::string lRepairString = "";

	if (mPowerplantInfo->getDamaged() && !mPowerplantInfo->isBeingRepaired()) {
		mGUI->findWidget<MyGUI::StaticText>("PPRepair")->setVisible(true);
		lRepairString = "#FF0000" + StrLoc::get()->DamagedNeedsRepair();
	}
	else if (mPowerplantInfo->isBeingRepaired()) {
		mGUI->findWidget<MyGUI::StaticText>("PPRepair")->setVisible(false);
		lRepairString = "#FF0000" + StrLoc::get()->IsBeingRepaired();
	} else {
		mGUI->findWidget<MyGUI::StaticText>("PPRepair")->setVisible(false);
	}

	mGUI->findWidget<MyGUI::StaticText>("PPName")->setCaption(pPowerplant->getName());

	mGUI->findWidget<MyGUI::StaticText>("PPInfo")->setCaption(StrLoc::get()->PPOutput()
    + toString(pPowerplant->getPowerInMW())+"MW\n" + StrLoc::get()->PPUsed()
		+ toString(pPowerplant->getPowerInMW() - pPowerplant->getPowerLeft()) + "MW\n"
		+ StrLoc::get()->Condition() + toString(pPowerplant->getCondition()) + "%\n" 
    + StrLoc::get()->BuiltYear() + toString(pPowerplant->getConstructionYear())
		+ "\n" + StrLoc::get()->WeeklyCost() + toString(pPowerplant->getOperatingCosts()/1000)
    + L"k€\n" 
		+ lRepairString);

	mGUI->findWidget<MyGUI::StaticText>("PPCities")->setCaption(pPowerplant->getReceivers());

	if (mPowerplantInfo->getInheritSettings())
		mPowerplantInfo->setMaintenance(mCompany->getMaintenance());

	mGUI->findWidget<MyGUI::Button>("PPGlobalCheck")->setStateCheck(pPowerplant->getInheritSettings());
	mGUI->findWidget<MyGUI::Button>("PPMaintUp")->setEnabled(!pPowerplant->getInheritSettings());
	mGUI->findWidget<MyGUI::Button>("PPMaintDown")->setEnabled(!pPowerplant->getInheritSettings());
	mGUI->findWidget<MyGUI::Edit>("PPMaintenance")->setEnabled(!pPowerplant->getInheritSettings());
	mGUI->findWidget<MyGUI::Edit>("PPMaintenance")->setCaption(toString(pPowerplant->getMaintenance()));

	mGUI->findWidget<MyGUI::Button>("PPGlobalCheck")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::powerplantButtonPressed);

	mGUI->findWidget<MyGUI::Button>("PPShowNet")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::powerplantButtonPressed);
	mGUI->findWidget<MyGUI::Button>("PPShowSales")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::powerplantButtonPressed);

	mGUI->findWidget<MyGUI::Button>("PPMaintUp")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::powerplantButtonPressed);
	mGUI->findWidget<MyGUI::Button>("PPMaintDown")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::powerplantButtonPressed);

	mGUI->findWidget<MyGUI::Button>("PPMaintUp")->eventMouseButtonPressed = 
		MyGUI::newDelegate(this, &GUI::buttonDownPressed);
	mGUI->findWidget<MyGUI::Button>("PPMaintDown")->eventMouseButtonPressed = 
		MyGUI::newDelegate(this, &GUI::buttonDownPressed);

	mGUI->findWidget<MyGUI::Button>("PPMaintUp")->eventMouseButtonReleased = 
		MyGUI::newDelegate(this, &GUI::buttonDownReleased);
	mGUI->findWidget<MyGUI::Button>("PPMaintDown")->eventMouseButtonReleased = 
		MyGUI::newDelegate(this, &GUI::buttonDownReleased);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::powerplantButtonPressed(MyGUI::WidgetPtr _widget)
{
	if (_widget->getName().find("PPGlobalCheck") != -1) {
		mGUI->findWidget<MyGUI::Button>("PPGlobalCheck")
      ->setStateCheck(!mGUI->findWidget<MyGUI::Button>("PPGlobalCheck")->getStateCheck());

		mPowerplantInfo->setInheritSettings(mGUI
      ->findWidget<MyGUI::Button>("PPGlobalCheck")->getStateCheck());

		if (mGUI->findWidget<MyGUI::Button>("PPGlobalCheck")->getStateCheck())
			mPowerplantInfo->setMaintenance(mCompany->getMaintenance());

		mGUI->findWidget<MyGUI::Button>("PPMaintUp")
      ->setEnabled(!mPowerplantInfo->getInheritSettings());
		mGUI->findWidget<MyGUI::Button>("PPMaintDown")
      ->setEnabled(!mPowerplantInfo->getInheritSettings());
		mGUI->findWidget<MyGUI::Edit>("PPMaintenance")
      ->setEnabled(!mPowerplantInfo->getInheritSettings());
		mGUI->findWidget<MyGUI::Edit>("PPMaintenance")
      ->setCaption(toString(mPowerplantInfo->getMaintenance()));
	}
	else if (_widget->getName().find("PPShowNet") != -1) {
		showPowerNetsWindowPressed(0);
	}
	else if (_widget->getName().find("PPShowSales") != -1) {
		financesPressed(0);
	}
	else if(_widget->getName().find("PPRepair") != -1) {
	}
	else if (_widget->getName().find("PPMaintUp") != -1) {
		if (mPowerplantInfo->getMaintenance() < 100)
			mPowerplantInfo->setMaintenance(mPowerplantInfo->getMaintenance()+1);
		showPowerPlantInfo(mPowerplantInfo);
	}
	else if (_widget->getName().find("PPMaintDown") != -1) {
		if (mPowerplantInfo->getMaintenance() > 0)
			mPowerplantInfo->setMaintenance(mPowerplantInfo->getMaintenance()-1);
		showPowerPlantInfo(mPowerplantInfo);
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::showResourceInfo(boost::shared_ptr<ResourceBuilding> pResourceBuilding)
{
	mResourceInfo = pResourceBuilding;

	unshowInfo();

	mGUI->findWidget<MyGUI::Button>("REGlobalCheck")
    ->setCaption(StrLoc::get()->UseGlobalSettings());
	mGUI->findWidget<MyGUI::StaticText>("REMaintenanceText")
    ->setCaption(StrLoc::get()->PPMaintenance());

	if (mResourceInfo->getSubtype() == eREGasSmall) {
		mGUI->findWidget<MyGUI::StaticImage>("REInfoPic")->setImageTexture("re_gas.png");
		mGUI->findWidget<MyGUI::StaticText>("REInfo")->setCaption(StrLoc::get()->TooltipOutput() 
      + GameConfig::getString("REGasSmall_Rate") + " " + StrLoc::get()->TooltipKCubicFeet() 
      + StrLoc::get()->TooltipPerWeek() + "\n" 
      + StrLoc::get()->Condition()+toString(mResourceInfo->getCondition())+"%"
			+ "\n" + StrLoc::get()->WeeklyCost() + toString(mResourceInfo->getOperatingCosts()/1000)
      + L"k€");
	}
	else if (mResourceInfo->getSubtype() == eREGasLarge) {
		mGUI->findWidget<MyGUI::StaticImage>("REInfoPic")->setImageTexture("re_gas_large.png");
		mGUI->findWidget<MyGUI::StaticText>("REInfo")->setCaption(StrLoc::get()->TooltipOutput() 
      + GameConfig::getString("REGasLarge_Rate") + " " + StrLoc::get()->TooltipKCubicFeet() 
      + StrLoc::get()->TooltipPerWeek() + "\n" 
      + StrLoc::get()->Condition()+toString(mResourceInfo->getCondition())+"%"
			+ "\n" + StrLoc::get()->WeeklyCost() + toString(mResourceInfo->getOperatingCosts()/1000)
      + L"k€");
	}
	else if (mResourceInfo->getSubtype() == eREUraniumSmall) {
		mGUI->findWidget<MyGUI::StaticImage>("REInfoPic")->setImageTexture("re_uranium.png");
		mGUI->findWidget<MyGUI::StaticText>("REInfo")->setCaption(StrLoc::get()->TooltipOutput() 
      + GameConfig::getString("REUraniumSmall_Rate") + " " + StrLoc::get()->TooltipRods() 
      + StrLoc::get()->TooltipPerWeek() + "\n" 
      + StrLoc::get()->Condition()+toString(mResourceInfo->getCondition())+"%"
			+ "\n" + StrLoc::get()->WeeklyCost() + toString(mResourceInfo->getOperatingCosts()/1000)
      + L"k€");
	}
	else if (mResourceInfo->getSubtype() == eREUraniumLarge) {
		mGUI->findWidget<MyGUI::StaticImage>("REInfoPic")->setImageTexture("re_uranium_large.png");
		mGUI->findWidget<MyGUI::StaticText>("REInfo")->setCaption(StrLoc::get()->TooltipOutput() 
      + GameConfig::getString("REUraniumLarge_Rate") + " " + StrLoc::get()->TooltipRods() 
      + StrLoc::get()->TooltipPerWeek() 
      + "\n" + StrLoc::get()->Condition()+toString(mResourceInfo->getCondition())+"%"
			+ "\n" + StrLoc::get()->WeeklyCost() + toString(mResourceInfo->getOperatingCosts()/1000)
      + L"k€");
	}
	else if (mResourceInfo->getSubtype() == eRECoalSmall) {
		mGUI->findWidget<MyGUI::StaticImage>("REInfoPic")->setImageTexture("re_coal.png");
		mGUI->findWidget<MyGUI::StaticText>("REInfo")->setCaption(StrLoc::get()->TooltipOutput() 
      + GameConfig::getString("RECoalSmall_Rate") + " " + StrLoc::get()->TooltipKTons() 
      + StrLoc::get()->TooltipPerWeek() + "\n" 
      + StrLoc::get()->Condition()+toString(mResourceInfo->getCondition())+"%"
			+ "\n" + StrLoc::get()->WeeklyCost() + toString(mResourceInfo->getOperatingCosts()/1000)
      + L"k€");
	}
	else if (mResourceInfo->getSubtype() == eRECoalLarge) {
		mGUI->findWidget<MyGUI::StaticImage>("REInfoPic")->setImageTexture("re_coal_large.png");
		mGUI->findWidget<MyGUI::StaticText>("REInfo")->setCaption(StrLoc::get()->TooltipOutput() 
      + GameConfig::getString("RECoalLarge_Rate") + " " 
      + StrLoc::get()->TooltipKTons() + StrLoc::get()->TooltipPerWeek() + "\n" 
      + StrLoc::get()->Condition()+toString(mResourceInfo->getCondition())+"%"
			+ "\n" + StrLoc::get()->WeeklyCost() + toString(mResourceInfo->getOperatingCosts()/1000)
      + L"k€");
	}

	if (mResourceInfo->getDamaged() && !mResourceInfo->isBeingRepaired()) {
		mGUI->findWidget<MyGUI::StaticText>("REInfo")->setCaption(StrLoc::get()->TooltipOutput() 
      + StrLoc::get()->Nothing() + "\n" + StrLoc::get()->Condition()
      + toString(mResourceInfo->getCondition())+"%"
			+ "\n" + StrLoc::get()->WeeklyCost() + toString(mResourceInfo->getOperatingCosts()/1000) 
      + L"k€\n#FF0000" + StrLoc::get()->DamagedNeedsRepair());
	}
	else if (mResourceInfo->isBeingRepaired()) {
		mGUI->findWidget<MyGUI::StaticText>("REInfo")->setCaption(StrLoc::get()->TooltipOutput() 
      + StrLoc::get()->Nothing() + "\n" + StrLoc::get()->Condition()
      + toString(mResourceInfo->getCondition())+"%"
			+ "\n" + StrLoc::get()->WeeklyCost() + toString(mResourceInfo->getOperatingCosts()/1000) 
      + L"k€\n#FF0000" + StrLoc::get()->IsBeingRepaired());
	}

	mGUI->findWidget<MyGUI::StaticText>("REName")->setCaption(pResourceBuilding->getName());

	mGUI->findWidget<MyGUI::StaticImage>("ResourceInfoPanel")->setVisible(true);

	mGUI->findWidget<MyGUI::Button>("REGlobalCheck")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::resourceButtonPressed);

	if (mResourceInfo->getInheritSettings())
		mResourceInfo->setMaintenance(mCompany->getMaintenance());

	mGUI->findWidget<MyGUI::Button>("REGlobalCheck")
    ->setStateCheck(mResourceInfo->getInheritSettings());
	mGUI->findWidget<MyGUI::Button>("REMaintUp")
    ->setEnabled(!mResourceInfo->getInheritSettings());
	mGUI->findWidget<MyGUI::Button>("REMaintDown")
    ->setEnabled(!mResourceInfo->getInheritSettings());
	mGUI->findWidget<MyGUI::Edit>("REMaintenance")
    ->setEnabled(!mResourceInfo->getInheritSettings());
	mGUI->findWidget<MyGUI::Edit>("REMaintenance")
    ->setCaption(toString(mResourceInfo->getMaintenance()));

	if (mResourceInfo->getDamaged() && !mResourceInfo->isBeingRepaired())
		mGUI->findWidget<MyGUI::Button>("RERepair")->setVisible(true);
	else
		mGUI->findWidget<MyGUI::Button>("RERepair")->setVisible(false);

	mGUI->findWidget<MyGUI::Button>("REMaintUp")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::resourceButtonPressed);
	mGUI->findWidget<MyGUI::Button>("REMaintDown")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::resourceButtonPressed);

	mGUI->findWidget<MyGUI::Button>("REMaintUp")->eventMouseButtonPressed = 
		MyGUI::newDelegate(this, &GUI::buttonDownPressed);
	mGUI->findWidget<MyGUI::Button>("REMaintDown")->eventMouseButtonPressed = 
		MyGUI::newDelegate(this, &GUI::buttonDownPressed);

	mGUI->findWidget<MyGUI::Button>("REMaintUp")->eventMouseButtonReleased = 
		MyGUI::newDelegate(this, &GUI::buttonDownReleased);
	mGUI->findWidget<MyGUI::Button>("REMaintDown")->eventMouseButtonReleased = 
		MyGUI::newDelegate(this, &GUI::buttonDownReleased);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::resourceButtonPressed(MyGUI::WidgetPtr _widget)
{	
	if (_widget->getName().find("REGlobalCheck") != -1) {
		mGUI->findWidget<MyGUI::Button>("REGlobalCheck")
      ->setStateCheck(!mGUI->findWidget<MyGUI::Button>("REGlobalCheck")->getStateCheck());

		mResourceInfo->setInheritSettings(mGUI
      ->findWidget<MyGUI::Button>("REGlobalCheck")->getStateCheck());

		if (mGUI->findWidget<MyGUI::Button>("REGlobalCheck")->getStateCheck())
			mResourceInfo->setMaintenance(mCompany->getMaintenance());

		mGUI->findWidget<MyGUI::Button>("REMaintUp")
      ->setEnabled(!mResourceInfo->getInheritSettings());
		mGUI->findWidget<MyGUI::Button>("REMaintDown")
      ->setEnabled(!mResourceInfo->getInheritSettings());
		mGUI->findWidget<MyGUI::Edit>("REMaintenance")
      ->setEnabled(!mResourceInfo->getInheritSettings());
		mGUI->findWidget<MyGUI::Edit>("REMaintenance")
      ->setCaption(toString(mResourceInfo->getMaintenance()));
	}
	else if (_widget->getName().find("RERepair") != -1) {
	}
	else if (_widget->getName().find("REMaintUp") != -1) {
		if(mResourceInfo->getMaintenance() < 100)
			mResourceInfo->setMaintenance(mResourceInfo->getMaintenance()+1);
		showResourceInfo(mResourceInfo);
	}
	else if(_widget->getName().find("REMaintDown") != -1) {
		if(mResourceInfo->getMaintenance() > 0)
			mResourceInfo->setMaintenance(mResourceInfo->getMaintenance()-1);
		showResourceInfo(mResourceInfo);
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::showCompanyInfo(boost::shared_ptr<CompanyBuilding> pCompanyBuilding)
{
	if (mCompanyInfo)
		if (mCompanyInfo->getID() != pCompanyBuilding->getID())
			mCompanyInfoLock = false;

	mCompanyInfo = pCompanyBuilding;

	unshowInfo();

	mGUI->findWidget<MyGUI::StaticImage>("CompanyPanel")->setVisible(true);

	mGUI->findWidget<MyGUI::Button>("COShowWindow")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::companyButtonPressed);

	if (!mCompanyInfoLock) {
		mCompanyInfoLock = true;

		if (mCompanyInfo->getSubtype() == eCOHeadquarters) {
			mGUI->findWidget<MyGUI::StaticImage>("COInfoPic")->setImageTexture("co_headquarters.png");
			mGUI->findWidget<MyGUI::Button>("COShowWindow")->changeWidgetSkin("ButtonCoHead");
			mGUI->findWidget<MyGUI::StaticText>("COName")
        ->setCaption(StrLoc::get()->TooltipHeadquarters());
		}
		else if (mCompanyInfo->getSubtype() == eCOResearch) {
			mGUI->findWidget<MyGUI::StaticImage>("COInfoPic")->setImageTexture("co_research.png");
			mGUI->findWidget<MyGUI::Button>("COShowWindow")->changeWidgetSkin("ButtonCoRes");
			mGUI->findWidget<MyGUI::StaticText>("COName")
        ->setCaption(StrLoc::get()->TooltipResearchBuilding());
		}
		else if (mCompanyInfo->getSubtype() == eCOPublicRelations) {
			mGUI->findWidget<MyGUI::StaticImage>("COInfoPic")
        ->setImageTexture("co_public_relations.png");
			mGUI->findWidget<MyGUI::Button>("COShowWindow")->changeWidgetSkin("ButtonCoHead");
			mGUI->findWidget<MyGUI::StaticText>("COName")->setCaption(StrLoc::get()->TooltipPR());
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::companyButtonPressed(MyGUI::WidgetPtr _widget)
{
	if (_widget->getName() == "COShowWindow") {
		if(mCompanyInfo->getSubtype() == eCOResearch)
			researchPressed(0);
		else if (mCompanyInfo->getSubtype() == eCOHeadquarters)
			ceoPressed(0);
		else if (mCompanyInfo->getSubtype() == eCOPublicRelations)
			ceoPressed(0);
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::showInfo(std::string pInfo)
{
	unshowInfo();

	mGUI->findWidget<MyGUI::StaticImage>("InfoPanel")->setVisible(true);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::unshowInfo(void)
{
	mGUI->findWidget<MyGUI::StaticImage>("BuildResourcePanel")->setVisible(false);
	mGUI->findWidget<MyGUI::StaticImage>("BuildDistributionPanel")->setVisible(false);
	mGUI->findWidget<MyGUI::StaticImage>("BuildMiscPanel")->setVisible(false);
	mGUI->findWidget<MyGUI::StaticImage>("BuildPowerplantPanel")->setVisible(false);

	mGUI->findWidget<MyGUI::StaticImage>("PowerplantPanel")->setVisible(false);
	mGUI->findWidget<MyGUI::StaticImage>("CityPanel")->setVisible(false);
	mGUI->findWidget<MyGUI::StaticImage>("ResourceInfoPanel")->setVisible(false);
	mGUI->findWidget<MyGUI::StaticImage>("CompanyPanel")->setVisible(false);
	mGUI->findWidget<MyGUI::StaticImage>("InfoPanel")->setVisible(false);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::updateMinimapWindow(float pX, float pY, float pHeight, float pTerrainWidth,
                              float pTerrainHeight)
{
	if (mLoading)
		return;

	MyGUI::StaticImagePtr lMinimapWindow = mGUI->findWidget<MyGUI::StaticImage>("MinimapWindow");

	lMinimapWindow->setSize(cMinimapWindowWidth*(pHeight/GameConfig::getDouble("MaxZoomOut"))*0.45,
    cMinimapWindowHeight*(pHeight/GameConfig::getDouble("MaxZoomOut"))*0.45);

	if(lMinimapWindow->getSize().width < 27)
		lMinimapWindow->setImageTexture("minimap_window_small.png");
	else
		lMinimapWindow->setImageTexture("minimap_window.png");

	int lOffsetX = 0;
	int lOffsetY = 0;
	int lLargerSize = 0;

	if (pTerrainWidth > pTerrainHeight) {
		lOffsetY = (pTerrainWidth - pTerrainHeight)/2;
		lLargerSize = pTerrainWidth;
	}
	else if (pTerrainWidth < pTerrainHeight) {
		lOffsetX = (pTerrainHeight - pTerrainWidth)/2;
		lLargerSize = pTerrainHeight;
	} else {
		lLargerSize = pTerrainHeight;
	}

	lMinimapWindow->setPosition(
	((((pX+lOffsetX)/lLargerSize)*cMinimapRealWidth)-(lMinimapWindow->getSize().width/2)),
	((((pY+lOffsetY)/lLargerSize)*cMinimapRealHeight)-(lMinimapWindow->getSize().height/2)));

	mCurrentTerrainWidth = pTerrainWidth;
	mCurrentTerrainHeight = pTerrainHeight;
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::updateMinimap(EventData* pData)
{
	if (mLoading)
		return;

	mGUI->findWidget<MyGUI::StaticImage>("Minimap")->setImageTexture("minimap");
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::setPanIcon(bool pVisible, int pX, int pY, bool pRotate, bool pLastPosOnly)
{
	if (!pLastPosOnly) {
		if (pRotate)
			mGUI->findWidget<MyGUI::StaticImage>("PanIcon")->setImageTexture("rotate.png");
		else
			mGUI->findWidget<MyGUI::StaticImage>("PanIcon")->setImageTexture("pan.png");

		mGUI->findWidget<MyGUI::StaticImage>("PanIcon")->setVisible(pVisible);

		if(mLastPanVisible == false) {
			mGUI->findWidget<MyGUI::StaticImage>("PanIcon")->setPosition(MyGUI::IntPoint(pX-16, pY-16));
			mLastPanPosition.x = pX;
			mLastPanPosition.y = pY;
		}

		mLastPanVisible = pVisible;
	} else {
		mLastPanPosition.x = pX;
		mLastPanPosition.y = pY;
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::messageBoxOK(EventData* pData)
{
	MyGUI::Message::createMessageBox("Message", static_cast<EventArg<std::string>*>(pData)->mData1,
		static_cast<EventArg<std::string>*>(pData)->mData2);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::changeShadowTextCaption(std::string pTextName, std::string pCaption)
{
	mGUI->findWidget<MyGUI::StaticText>(pTextName)->setCaption(pCaption);
	mGUI->findWidget<MyGUI::StaticText>(pTextName + "Shadow")->setCaption(pCaption);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::changeShadowTextCaptionW(std::string pTextName, std::wstring pCaption)
{
	mGUI->findWidget<MyGUI::StaticText>(pTextName)->setCaption(pCaption);
	mGUI->findWidget<MyGUI::StaticText>(pTextName + "Shadow")->setCaption(pCaption);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::menuPressed(MyGUI::WidgetPtr _widget)
{
	EventHandler::raiseEvent(eGamestateChangeGUI);
	switchToMainMenu();
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::missionPressed(MyGUI::WidgetPtr _widget)
{
	EventHandler::raiseEvent(eGamestateChangeView);

	mGUI->findWidget<MyGUI::Window>("MissionWindow")->setCaption(StrLoc::get()->MissionCaption());

	mGUI->findWidget<MyGUI::Window>("MissionWindow")->setVisible(true);
	mGUI->findWidget<MyGUI::Window>("MissionWindow")->setPosition(
		(mGUI->getRenderWindow()->getWidth()/2)
    -(mGUI->findWidget<MyGUI::Window>("MissionWindow")->getWidth()/2),
		(mGUI->getRenderWindow()->getHeight()/2)
    -(mGUI->findWidget<MyGUI::Window>("MissionWindow")->getHeight()/2));

	mGUI->findWidget<MyGUI::Window>("MissionWindow")->setShadow(true);

	MyGUI::LayerManager::getInstance().upLayerItem(mGUI->findWidget<MyGUI::Window>("MissionWindow"));
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::updateFinances(EventData* pData)
{
	if (mLoading)
		return;

	std::vector<FinancialArchive> lFinancesData = mCompany->getFinancialData();

	MyGUI::ListPtr lFinanceList[] = { mGUI->findWidget<MyGUI::List>("CurrentMonth"),
		mGUI->findWidget<MyGUI::List>("LastMonth"),
		mGUI->findWidget<MyGUI::List>("CurrentYear"),
		mGUI->findWidget<MyGUI::List>("LastYear") };

	MyGUI::ListPtr lFinanceTotal[] = { mGUI->findWidget<MyGUI::List>("CurrentMonthTotal"),
		mGUI->findWidget<MyGUI::List>("LastMonthTotal"),
		mGUI->findWidget<MyGUI::List>("CurrentYearTotal"),
		mGUI->findWidget<MyGUI::List>("LastYearTotal") };

	for (int i = 0; i < 4; i++) {
		lFinanceList[i]->removeAllItems();
		lFinanceTotal[i]->removeAllItems();

		lFinanceList[i]->addItem(((lFinancesData[i].mLoanPayments == 0) ? "#000000" 
      : ((lFinancesData[i].mLoanPayments < 0) ? "#FF0000" : "#00FF00")) 
      + toString(lFinancesData[i].mLoanPayments));
		lFinanceList[i]->addItem(((lFinancesData[i].mTrainingSafety == 0) ? "#000000" 
      : ((lFinancesData[i].mTrainingSafety < 0) ? "#FF0000" : "#00FF00")) 
      + toString(lFinancesData[i].mTrainingSafety));
		lFinanceList[i]->addItem(((lFinancesData[i].mMaintenance == 0) ? "#000000" 
      : ((lFinancesData[i].mMaintenance < 0) ? "#FF0000" : "#00FF00")) 
      + toString(lFinancesData[i].mMaintenance));
		lFinanceList[i]->addItem(((lFinancesData[i].mAdvertising == 0) ? "#000000" 
      : ((lFinancesData[i].mAdvertising < 0) ? "#FF0000" : "#00FF00")) 
      + toString(lFinancesData[i].mAdvertising));
		lFinanceList[i]->addItem(((lFinancesData[i].mOperation == 0) ? "#000000" 
      : ((lFinancesData[i].mOperation < 0) ? "#FF0000" : "#00FF00")) 
      + toString(lFinancesData[i].mOperation));
		lFinanceList[i]->addItem(((lFinancesData[i].mEnvironmental == 0) ? "#000000" 
      : ((lFinancesData[i].mEnvironmental < 0) ? "#FF0000" : "#00FF00")) 
      + toString(lFinancesData[i].mEnvironmental));
		lFinanceList[i]->addItem(((lFinancesData[i].mResearch == 0) ? "#000000" 
      : ((lFinancesData[i].mResearch < 0) ? "#FF0000" : "#00FF00")) 
      + toString(lFinancesData[i].mResearch));
		lFinanceList[i]->addItem(((lFinancesData[i].mBuilding == 0) ? "#000000" 
      : ((lFinancesData[i].mBuilding < 0) ? "#FF0000" : "#00FF00")) 
      + toString(lFinancesData[i].mBuilding));
		lFinanceList[i]->addItem(((lFinancesData[i].mSpecialExpenses == 0) ? "#000000" 
      : ((lFinancesData[i].mSpecialExpenses < 0) ? "#FF0000" : "#00FF00")) 
      + toString(lFinancesData[i].mSpecialExpenses));
		lFinanceList[i]->addItem(((lFinancesData[i].mTrade == 0) ? "#000000" 
      : ((lFinancesData[i].mTrade < 0) ? "#FF0000" : "#00FF00")) 
      + toString(lFinancesData[i].mTrade));
		lFinanceList[i]->addItem(((lFinancesData[i].mCustomerIncome == 0) ? "#000000" 
      : ((lFinancesData[i].mCustomerIncome < 0) ? "#FF0000" : "#00FF00")) 
      + toString(lFinancesData[i].mCustomerIncome));

		lFinanceTotal[i]->setScrollVisible(false);
		lFinanceTotal[i]->addItem(((lFinancesData[i].mTotal == 0) ? "#000000" 
      : ((lFinancesData[i].mTotal < 0) ? "#FF0000" : "#00FF00")) 
      + toString(lFinancesData[i].mTotal));
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::financesPressed(MyGUI::WidgetPtr _widget)
{
	EventHandler::raiseEvent(eGamestateChangeView);

	mGUI->findWidget<MyGUI::Window>("FinancesWindow")->setVisible(true);
	mGUI->findWidget<MyGUI::Window>("FinancesWindow")->setPosition(
		(mGUI->getRenderWindow()->getWidth()/2)
    -(mGUI->findWidget<MyGUI::Window>("FinancesWindow")->getWidth()/2),
		(mGUI->getRenderWindow()->getHeight()/2)
    -(mGUI->findWidget<MyGUI::Window>("FinancesWindow")->getHeight()/2));

	mGUI->findWidget<MyGUI::Window>("FinancesWindow")->setShadow(true);

	MyGUI::LayerManager::getInstance().upLayerItem(mGUI->findWidget<MyGUI::Window>("FinancesWindow"));

	mGUI->findWidget<MyGUI::Window>("FinancesWindow")->setCaption(StrLoc::get()->FinancesCaption());
	mGUI->findWidget<MyGUI::StaticText>("FinancesPriceText")->setCaption(StrLoc::get()->PriceText());
	mGUI->findWidget<MyGUI::StaticText>("FinancesAdsText")->setCaption(StrLoc::get()->AdsText());
	mGUI->findWidget<MyGUI::StaticText>("MaintenanceText")->setCaption(StrLoc::get()->MaintenanceText());
	mGUI->findWidget<MyGUI::TabItem>("SalesTab")->setCaption(StrLoc::get()->SalesCaption());
	mGUI->findWidget<MyGUI::StaticText>("PriceUnit")->setCaption(StrLoc::get()->PriceUnit());
	mGUI->findWidget<MyGUI::StaticText>("AdsUnit")->setCaption(StrLoc::get()->AdsUnit());
	mGUI->findWidget<MyGUI::StaticText>("EnvText")->setCaption(StrLoc::get()->EnvText());
	mGUI->findWidget<MyGUI::StaticText>("ContractText")->setCaption(StrLoc::get()->ContractText());
	mGUI->findWidget<MyGUI::StaticText>("SupportText")->setCaption(StrLoc::get()->SupportText());
	mGUI->findWidget<MyGUI::StaticText>("SupplyText")->setCaption(StrLoc::get()->SupplyText());
	mGUI->findWidget<MyGUI::StaticText>("SafetyText")->setCaption(StrLoc::get()->SafetyText());
	mGUI->findWidget<MyGUI::Button>("TermNoneRadio")->setCaption(StrLoc::get()->TermNone());
	mGUI->findWidget<MyGUI::Button>("Term12Radio")->setCaption(StrLoc::get()->Term12());
	mGUI->findWidget<MyGUI::Button>("Term24Radio")->setCaption(StrLoc::get()->Term24());
	mGUI->findWidget<MyGUI::Button>("SupportPoorRadio")->setCaption(StrLoc::get()->SupportPoor());
	mGUI->findWidget<MyGUI::Button>("SupportNormalRadio")->setCaption(StrLoc::get()->SupportNormal());
	mGUI->findWidget<MyGUI::Button>("SupportExtensiveRadio")->setCaption(StrLoc::get()->SupportExtensive());
	mGUI->findWidget<MyGUI::Button>("SafetyNoneRadio")->setCaption(StrLoc::get()->SafetyNone());
	mGUI->findWidget<MyGUI::Button>("SafetyNormalRadio")->setCaption(StrLoc::get()->SafetyNormal());
	mGUI->findWidget<MyGUI::Button>("SafetyHeavyRadio")->setCaption(StrLoc::get()->SafetyHeavy());
	mGUI->findWidget<MyGUI::Button>("EmployeeNoneRadio")->setCaption(StrLoc::get()->EmployeeNone());
	mGUI->findWidget<MyGUI::Button>("EmployeeBasicRadio")->setCaption(StrLoc::get()->EmployeeBasic());
	mGUI->findWidget<MyGUI::Button>("EmployeeExtensiveRadio")->setCaption(StrLoc::get()->EmployeeExtensive());
	mGUI->findWidget<MyGUI::StaticText>("EnvUnit")->setCaption(StrLoc::get()->EnvUnit());
	mGUI->findWidget<MyGUI::StaticText>("CurrentMonthText")->setCaption(StrLoc::get()->CurrentMonthText());
	mGUI->findWidget<MyGUI::StaticText>("LastMonthText")->setCaption(StrLoc::get()->LastMonthText());
	mGUI->findWidget<MyGUI::StaticText>("CurrentYearText")->setCaption(StrLoc::get()->CurrentYearText());
	mGUI->findWidget<MyGUI::StaticText>("LastYearText")->setCaption(StrLoc::get()->LastYearText());
	mGUI->findWidget<MyGUI::StaticText>("TotalText")->setCaption(StrLoc::get()->TotalText());
	mGUI->findWidget<MyGUI::StaticText>("FLoan")->setCaption(StrLoc::get()->FLoan());
	mGUI->findWidget<MyGUI::StaticText>("FSafety")->setCaption(StrLoc::get()->FSafety());
	mGUI->findWidget<MyGUI::StaticText>("FAdvertising")->setCaption(StrLoc::get()->FAdvertising());
	mGUI->findWidget<MyGUI::StaticText>("FMaintenance")->setCaption(StrLoc::get()->FMaintenance());
	mGUI->findWidget<MyGUI::StaticText>("FOperation")->setCaption(StrLoc::get()->FOperation());
	mGUI->findWidget<MyGUI::StaticText>("FEnvironment")->setCaption(StrLoc::get()->FEnvironment());
	mGUI->findWidget<MyGUI::StaticText>("FResearch")->setCaption(StrLoc::get()->FResearch());
	mGUI->findWidget<MyGUI::StaticText>("FBuilding")->setCaption(StrLoc::get()->FBuilding());
	mGUI->findWidget<MyGUI::StaticText>("FIncome")->setCaption(StrLoc::get()->FIncome());
	mGUI->findWidget<MyGUI::StaticText>("FSpecial")->setCaption(StrLoc::get()->FSpecial());
	mGUI->findWidget<MyGUI::StaticText>("FTrade")->setCaption(StrLoc::get()->FTrade());
	mGUI->findWidget<MyGUI::TabItem>("ChartTab")->setCaption(StrLoc::get()->ChartTab());
	mGUI->findWidget<MyGUI::Button>("ExpensesCheck")->setCaption(StrLoc::get()->ExpensesCheck());
	mGUI->findWidget<MyGUI::Button>("IncomeCheck")->setCaption(StrLoc::get()->IncomeCheck());
	mGUI->findWidget<MyGUI::Button>("FundsCheck")->setCaption(StrLoc::get()->FundsCheck());
	mGUI->findWidget<MyGUI::Button>("NetWorthCheck")->setCaption(StrLoc::get()->NetWorthCheck());
	mGUI->findWidget<MyGUI::TabItem>("LoanTab")->setCaption(StrLoc::get()->LoanTab());
	mGUI->findWidget<MyGUI::Button>("TakeOut")->setCaption(StrLoc::get()->TakeOutButton());
	mGUI->findWidget<MyGUI::Button>("Repayment")->setCaption(StrLoc::get()->RepaymentButton());
	mGUI->findWidget<MyGUI::StaticText>("NoLoans")->setCaption(StrLoc::get()->NoLoansText());
	mGUI->findWidget<MyGUI::StaticText>("PeriodText")->setCaption(StrLoc::get()->PeriodText());
	mGUI->findWidget<MyGUI::StaticText>("AmountText")->setCaption(StrLoc::get()->AmountText());
	mGUI->findWidget<MyGUI::StaticText>("AmountText2")->setCaption(StrLoc::get()->AmountText());
	mGUI->findWidget<MyGUI::StaticText>("PeriodUnit")->setCaption(StrLoc::get()->PeriodUnit());
	mGUI->findWidget<MyGUI::StaticText>("NewCharge")->setCaption(StrLoc::get()->NewMonthlyCharge());
	mGUI->findWidget<MyGUI::TabItem>("IncomeTab")->setCaption(StrLoc::get()->IncomeCaption());
	mGUI->findWidget<MyGUI::StaticText>("CurrentLoansText")->setCaption(StrLoc::get()->CurrentLoansText());
	mGUI->findWidget<MyGUI::StaticText>("EmployeeText")->setCaption(StrLoc::get()->EmployeeText());

	mGUI->findWidget<MyGUI::Button>("PriceUp")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("PriceDown")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("MaintenanceUp")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("MaintenanceDown")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("AdvertisingUp")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("AdvertisingDown")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("RepayUp")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("RepayDown")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("AmountUp")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("AmountDown")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("YearsUp")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("YearsDown")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);

	mGUI->findWidget<MyGUI::Button>("SafetyNoneRadio")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("SafetyNormalRadio")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("SafetyHeavyRadio")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("EmployeeNoneRadio")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("EmployeeBasicRadio")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("EmployeeExtensiveRadio")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("TermNoneRadio")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Term12Radio")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Term24Radio")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("SupportPoorRadio")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("SupportNormalRadio")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("SupportExtensiveRadio")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);

	mGUI->findWidget<MyGUI::Button>("EnvSupportUp")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("EnvSupportDown")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("SupplyMaintUp")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("SupplyMaintDown")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);

	mGUI->findWidget<MyGUI::Button>("PriceUp")->eventMouseButtonPressed = 
		MyGUI::newDelegate(this, &GUI::buttonDownPressed);
	mGUI->findWidget<MyGUI::Button>("PriceDown")->eventMouseButtonPressed = 
		MyGUI::newDelegate(this, &GUI::buttonDownPressed);
	mGUI->findWidget<MyGUI::Button>("MaintenanceUp")->eventMouseButtonPressed = 
		MyGUI::newDelegate(this, &GUI::buttonDownPressed);
	mGUI->findWidget<MyGUI::Button>("MaintenanceDown")->eventMouseButtonPressed = 
		MyGUI::newDelegate(this, &GUI::buttonDownPressed);
	mGUI->findWidget<MyGUI::Button>("AdvertisingUp")->eventMouseButtonPressed = 
		MyGUI::newDelegate(this, &GUI::buttonDownPressed);
	mGUI->findWidget<MyGUI::Button>("AdvertisingDown")->eventMouseButtonPressed = 
		MyGUI::newDelegate(this, &GUI::buttonDownPressed);
	mGUI->findWidget<MyGUI::Button>("RepayUp")->eventMouseButtonPressed = 
		MyGUI::newDelegate(this, &GUI::buttonDownPressed);
	mGUI->findWidget<MyGUI::Button>("RepayDown")->eventMouseButtonPressed = 
		MyGUI::newDelegate(this, &GUI::buttonDownPressed);
	mGUI->findWidget<MyGUI::Button>("AmountUp")->eventMouseButtonPressed = 
		MyGUI::newDelegate(this, &GUI::buttonDownPressed);
	mGUI->findWidget<MyGUI::Button>("AmountDown")->eventMouseButtonPressed = 
		MyGUI::newDelegate(this, &GUI::buttonDownPressed);
	mGUI->findWidget<MyGUI::Button>("YearsUp")->eventMouseButtonPressed = 
		MyGUI::newDelegate(this, &GUI::buttonDownPressed);
	mGUI->findWidget<MyGUI::Button>("YearsDown")->eventMouseButtonPressed = 
		MyGUI::newDelegate(this, &GUI::buttonDownPressed);

	mGUI->findWidget<MyGUI::Button>("EnvSupportUp")->eventMouseButtonPressed = 
		MyGUI::newDelegate(this, &GUI::buttonDownPressed);
	mGUI->findWidget<MyGUI::Button>("EnvSupportDown")->eventMouseButtonPressed = 
		MyGUI::newDelegate(this, &GUI::buttonDownPressed);
	mGUI->findWidget<MyGUI::Button>("SupplyMaintUp")->eventMouseButtonPressed = 
		MyGUI::newDelegate(this, &GUI::buttonDownPressed);
	mGUI->findWidget<MyGUI::Button>("SupplyMaintDown")->eventMouseButtonPressed = 
		MyGUI::newDelegate(this, &GUI::buttonDownPressed);

	mGUI->findWidget<MyGUI::Button>("PriceUp")->eventMouseButtonReleased = 
		MyGUI::newDelegate(this, &GUI::buttonDownReleased);
	mGUI->findWidget<MyGUI::Button>("PriceDown")->eventMouseButtonReleased = 
		MyGUI::newDelegate(this, &GUI::buttonDownReleased);
	mGUI->findWidget<MyGUI::Button>("MaintenanceUp")->eventMouseButtonReleased = 
		MyGUI::newDelegate(this, &GUI::buttonDownReleased);
	mGUI->findWidget<MyGUI::Button>("MaintenanceDown")->eventMouseButtonReleased = 
		MyGUI::newDelegate(this, &GUI::buttonDownReleased);
	mGUI->findWidget<MyGUI::Button>("AdvertisingUp")->eventMouseButtonReleased = 
		MyGUI::newDelegate(this, &GUI::buttonDownReleased);
	mGUI->findWidget<MyGUI::Button>("AdvertisingDown")->eventMouseButtonReleased = 
		MyGUI::newDelegate(this, &GUI::buttonDownReleased);
	mGUI->findWidget<MyGUI::Button>("RepayUp")->eventMouseButtonReleased = 
		MyGUI::newDelegate(this, &GUI::buttonDownReleased);
	mGUI->findWidget<MyGUI::Button>("RepayDown")->eventMouseButtonReleased = 
		MyGUI::newDelegate(this, &GUI::buttonDownReleased);
	mGUI->findWidget<MyGUI::Button>("AmountUp")->eventMouseButtonReleased = 
		MyGUI::newDelegate(this, &GUI::buttonDownReleased);
	mGUI->findWidget<MyGUI::Button>("AmountDown")->eventMouseButtonReleased = 
		MyGUI::newDelegate(this, &GUI::buttonDownReleased);
	mGUI->findWidget<MyGUI::Button>("YearsUp")->eventMouseButtonReleased = 
		MyGUI::newDelegate(this, &GUI::buttonDownReleased);
	mGUI->findWidget<MyGUI::Button>("YearsDown")->eventMouseButtonReleased = 
		MyGUI::newDelegate(this, &GUI::buttonDownReleased);

	mGUI->findWidget<MyGUI::Button>("EnvSupportUp")->eventMouseButtonReleased = 
		MyGUI::newDelegate(this, &GUI::buttonDownReleased);
	mGUI->findWidget<MyGUI::Button>("EnvSupportDown")->eventMouseButtonReleased = 
		MyGUI::newDelegate(this, &GUI::buttonDownReleased);
	mGUI->findWidget<MyGUI::Button>("SupplyMaintUp")->eventMouseButtonReleased = 
		MyGUI::newDelegate(this, &GUI::buttonDownReleased);
	mGUI->findWidget<MyGUI::Button>("SupplyMaintDown")->eventMouseButtonReleased = 
		MyGUI::newDelegate(this, &GUI::buttonDownReleased);

	mGUI->findWidget<MyGUI::Button>("ExpensesCheck")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("IncomeCheck")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("FundsCheck")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("NetWorthCheck")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);

	mGUI->findWidget<MyGUI::Button>("TakeOut")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Repayment")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::financesButtonPressed);

	updateFinanceRadios();

	mGUI->findWidget<MyGUI::Button>("ExpensesCheck")->setStateCheck(true);
	mGUI->findWidget<MyGUI::Button>("IncomeCheck")->setStateCheck(true);
	mGUI->findWidget<MyGUI::Button>("FundsCheck")->setStateCheck(true);
	mGUI->findWidget<MyGUI::Button>("NetWorthCheck")->setStateCheck(true);

	FinanceChartSet lSet;

	lSet.mExpenses = true;
	lSet.mIncome = true;
	lSet.mFunds = true;
	lSet.mNetWorth = true;

	EventHandler::raiseEvent(eUpdateFinanceChart, new EventArg<FinanceChartSet>(lSet));

	updateFinances(0);
	updateLoans(0);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::financesButtonPressed(MyGUI::WidgetPtr _widget)
{
	if (_widget->getName().find("PriceUp") != -1) {
		if (mCompany->getPrice() < 100)
			mCompany->setPrice(mCompany->getPrice()+1);
	}
	else if (_widget->getName().find("PriceDown") != -1) {
		if (mCompany->getPrice() > 0)
			mCompany->setPrice(mCompany->getPrice()-1);
	}
	else if (_widget->getName().find("MaintenanceUp") != -1) {
		if (mCompany->getMaintenance() < 100)
			mCompany->setMaintenance(mCompany->getMaintenance()+1);
	}
	else if (_widget->getName().find("MaintenanceDown") != -1) {
		if (mCompany->getMaintenance() > 0)
			mCompany->setMaintenance(mCompany->getMaintenance()-1);
	}
	else if (_widget->getName().find("AdvertisingUp") != -1) {
		if(mCompany->getAdvertising() < 100)
			mCompany->setAdvertising(mCompany->getAdvertising()+1);
	}
	else if (_widget->getName().find("AdvertisingDown") != -1) {
		if(mCompany->getAdvertising() > 0)
			mCompany->setAdvertising(mCompany->getAdvertising()-1);
	}
	else if (_widget->getName().find("Check") != -1) {
		static_cast<MyGUI::ButtonPtr>(_widget)->setStateCheck(
      !static_cast<MyGUI::ButtonPtr>(_widget)->getStateCheck());
	
		FinanceChartSet lSet;

		lSet.mExpenses  = mGUI->findWidget<MyGUI::Button>("ExpensesCheck")->getStateCheck();
		lSet.mIncome = mGUI->findWidget<MyGUI::Button>("IncomeCheck")->getStateCheck();
		lSet.mFunds = mGUI->findWidget<MyGUI::Button>("FundsCheck")->getStateCheck();
		lSet.mNetWorth = mGUI->findWidget<MyGUI::Button>("NetWorthCheck")->getStateCheck();

		EventHandler::raiseEvent(eUpdateFinanceChart, new EventArg<FinanceChartSet>(lSet));
	}
	else if (_widget->getName().find("TakeOut") != -1) {
		std::string lWhyNot;
		if (mCompany->isCreditworthy(mAmount*1000, lWhyNot)) {
			MyGUI::MessagePtr lQuestionBuy = MyGUI::Message::createMessageBox("Message",
        StrLoc::get()->GameTitle(),
				StrLoc::get()->TakeoutA() + toString(mAmount) + StrLoc::get()->TakeoutB() 
        + toString(mYears) + StrLoc::get()->TakeoutC(),
				MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No 
        | MyGUI::MessageBoxStyle::IconQuest);
			lQuestionBuy->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::question);
			lQuestionBuy->setUserString("Name", "LoanQuestion");
		} else {
			MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(), lWhyNot);
		}
	}
	else if (_widget->getName().find("Repayment") != -1) {
		if (mGUI->findWidget<MyGUI::List>("LoanList")->getIndexSelected() != MyGUI::ITEM_NONE) {
			if (mRepayment > 0) {
				MyGUI::MessagePtr lQuestionBuy = MyGUI::Message::createMessageBox("Message",
          StrLoc::get()->GameTitle(),
					StrLoc::get()->RepayA() + toString(mRepayment) + StrLoc::get()->RepayB(),
					MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No 
          | MyGUI::MessageBoxStyle::IconQuest);
				lQuestionBuy->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::question);
				lQuestionBuy->setUserString("Name", "RepayQuestion");
			} else {
				MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
          StrLoc::get()->ChooseSum());
			}
		} else {
			MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
        StrLoc::get()->SelectLoan());
		}
	}
	else if (_widget->getName().find("RepayUp") != -1) {
		if (mGUI->findWidget<MyGUI::List>("LoanList")->getIndexSelected() != MyGUI::ITEM_NONE) {
			if (mRepayment <= ((mCompany->getLoans()[mGUI->findWidget<MyGUI::List>("LoanList")
        ->getIndexSelected()].mAmountLeft/1000)-100)) {
				mRepayment += 100;
				recalcRate();
			} else {
				mRepayment = mCompany->getLoans()[mGUI->findWidget<MyGUI::List>("LoanList")
          ->getIndexSelected()].mAmountLeft/1000;
				recalcRate();
			}
		}

		mGUI->findWidget<MyGUI::Edit>("RepayAmount")->setCaption(toString(mRepayment));
	}
	else if (_widget->getName().find("RepayDown") != -1) {
		if (mGUI->findWidget<MyGUI::List>("LoanList")->getIndexSelected() != MyGUI::ITEM_NONE) {
			if (mRepayment > 99) {
				mRepayment -= 100;
				recalcRate();
			} else {
				mRepayment = 0;
				recalcRate();
			}
		}

		mGUI->findWidget<MyGUI::Edit>("RepayAmount")->setCaption(toString(mRepayment));
	}
	else if (_widget->getName().find("AmountUp") != -1) {
		if (mAmount < 5000) {
			mAmount += 100;
			recalcCreditData();
		}

		mGUI->findWidget<MyGUI::Edit>("TakeoutAmount")->setCaption(toString(mAmount));
	}
	else if (_widget->getName().find("AmountDown") != -1) {
		if (mAmount > 100) {
			mAmount -= 100;
			recalcCreditData();
		}

		mGUI->findWidget<MyGUI::Edit>("TakeoutAmount")->setCaption(toString(mAmount));
	}
	else if (_widget->getName().find("YearsUp") != -1) {
		if (mYears < 30) {
			mYears += 1;
			recalcCreditData();
		}

		mGUI->findWidget<MyGUI::Edit>("Years")->setCaption(toString(mYears));
	}
	else if (_widget->getName().find("YearsDown") != -1) {
		if (mYears > 1) {
			mYears -= 1;
			recalcCreditData();
		}

		mGUI->findWidget<MyGUI::Edit>("Years")->setCaption(toString(mYears));
	}
	else if (_widget->getName().find("SafetyNoneRadio") != -1) {
		mCompany->setSafetyLevel(eNoSafety);
		updateFinanceRadios();
	}
	else if (_widget->getName().find("SafetyNormalRadio") != -1) {
		mCompany->setSafetyLevel(eNormalSafety);
		updateFinanceRadios();
	}
	else if (_widget->getName().find("SafetyHeavyRadio") != -1) {
		mCompany->setSafetyLevel(eExtensiveSafety);
		updateFinanceRadios();
	}
	else if (_widget->getName().find("EmployeeNoneRadio") != -1) {
		mCompany->setTrainingLevel(eNoTraining);
		updateFinanceRadios();
	}
	else if (_widget->getName().find("EmployeeBasicRadio") != -1) {
		mCompany->setTrainingLevel(eNormalTraining);
		updateFinanceRadios();
	}
	else if (_widget->getName().find("EmployeeExtensiveRadio") != -1) {
		mCompany->setTrainingLevel(eExtensiveTraining);
		updateFinanceRadios();
	}
	else if (_widget->getName().find("TermNoneRadio") != -1) {
		mCompany->setContractTime(eNoTime);
		updateFinanceRadios();
	}
	else if (_widget->getName().find("Term12Radio") != -1) {
		mCompany->setContractTime(e12);
		updateFinanceRadios();
	}
	else if (_widget->getName().find("Term24Radio") != -1) {
		mCompany->setContractTime(e24);
		updateFinanceRadios();
	}
	else if (_widget->getName().find("SupportPoorRadio") != -1) {
		mCompany->setSupportLevel(eNoSupport);
		updateFinanceRadios();
	}
	else if (_widget->getName().find("SupportNormalRadio") != -1) {
		mCompany->setSupportLevel(eNormalSupport);
		updateFinanceRadios();
	}
	else if (_widget->getName().find("SupportExtensiveRadio") != -1) {
		mCompany->setSupportLevel(eExtensiveSupport);
		updateFinanceRadios();
	}
	else if (_widget->getName().find("EnvSupportUp") != -1) {
		mCompany->setEnvSupport(mCompany->getEnvSupport()+1);
	}
	else if (_widget->getName().find("EnvSupportDown") != -1) {
		if (mCompany->getEnvSupport() > 0)
			mCompany->setEnvSupport(mCompany->getEnvSupport()-1);
	}
	else if (_widget->getName().find("SupplyMaintUp") != -1) {
		if (mCompany->getSupplyMaintenance() < 100)
			mCompany->setSupplyMaintenance(mCompany->getSupplyMaintenance()+1);
	}
	else if (_widget->getName().find("SupplyMaintDown") != -1) {
		if (mCompany->getSupplyMaintenance() > 0)
			mCompany->setSupplyMaintenance(mCompany->getSupplyMaintenance()-1);
	}

	updateCompanyData(0);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::tradePressed(MyGUI::WidgetPtr _widget)
{
	EventHandler::raiseEvent(eGamestateChangeView);

	mGUI->findWidget<MyGUI::Window>("TradeWindow")->setVisible(true);
	mGUI->findWidget<MyGUI::Window>("TradeWindow")->setPosition(
		(mGUI->getRenderWindow()->getWidth()/2)
    -(mGUI->findWidget<MyGUI::Window>("TradeWindow")->getWidth()/2),
		(mGUI->getRenderWindow()->getHeight()/2)
    -(mGUI->findWidget<MyGUI::Window>("TradeWindow")->getHeight()/2));

	mGUI->findWidget<MyGUI::Window>("TradeWindow")->setShadow(true);

	MyGUI::LayerManager::getInstance().upLayerItem(mGUI->findWidget<MyGUI::Window>("TradeWindow"));

	mGUI->findWidget<MyGUI::Window>("TradeWindow")->setCaption(StrLoc::get()->TradeCaption());
	mGUI->findWidget<MyGUI::StaticText>("TradingPartnersText")->setCaption(StrLoc::get()->TradingPartnersText());
	mGUI->findWidget<MyGUI::StaticText>("NoTradingPartners")->setCaption(StrLoc::get()->NoTradingPartners());
	mGUI->findWidget<MyGUI::StaticText>("WorldMarketText")->setCaption(StrLoc::get()->WorldMarketText());
	mGUI->findWidget<MyGUI::Button>("Sign")->setCaption(StrLoc::get()->SignButton());
	mGUI->findWidget<MyGUI::StaticText>("NoTrade")->setCaption(StrLoc::get()->NoTradeOffers());
	mGUI->findWidget<MyGUI::StaticText>("OffersText")->setCaption(StrLoc::get()->OffersText());
	mGUI->findWidget<MyGUI::Button>("GasOfferCheck")->setCaption(StrLoc::get()->Gas());
	mGUI->findWidget<MyGUI::Button>("CoalOfferCheck")->setCaption(StrLoc::get()->Coal());
	mGUI->findWidget<MyGUI::Button>("UraniumOfferCheck")->setCaption(StrLoc::get()->Uranium());
	mGUI->findWidget<MyGUI::Button>("BuyCheck")->setCaption(StrLoc::get()->Buy());
	mGUI->findWidget<MyGUI::Button>("SellCheck")->setCaption(StrLoc::get()->Sell());
	mGUI->findWidget<MyGUI::Button>("AcceptOffer")->setCaption(StrLoc::get()->AcceptOfferButton());
	mGUI->findWidget<MyGUI::StaticText>("NoObligations")->setCaption(StrLoc::get()->NoObligations());
	mGUI->findWidget<MyGUI::Button>("CancelOffer")->setCaption(StrLoc::get()->CancelOfferButton());
	mGUI->findWidget<MyGUI::StaticText>("CurrentObligationsText")->setCaption(StrLoc::get()->CurrentObligationsText());

	updateTradeData();
	
	mGUI->findWidget<MyGUI::Button>("Sign")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::tradeButtonPressed);
	mGUI->findWidget<MyGUI::Button>("AcceptOffer")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::tradeButtonPressed);
	mGUI->findWidget<MyGUI::Button>("GasOfferCheck")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::tradeButtonPressed);
	mGUI->findWidget<MyGUI::Button>("CoalOfferCheck")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::tradeButtonPressed);
	mGUI->findWidget<MyGUI::Button>("UraniumOfferCheck")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::tradeButtonPressed);
	mGUI->findWidget<MyGUI::Button>("BuyCheck")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::tradeButtonPressed);
	mGUI->findWidget<MyGUI::Button>("SellCheck")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::tradeButtonPressed);
	mGUI->findWidget<MyGUI::Button>("CancelOffer")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::tradeButtonPressed);
	mGUI->findWidget<MyGUI::List>("TradingPartners")->eventListChangePosition = 
		MyGUI::newDelegate(this, &GUI::tradeListPressed);

	mGUI->findWidget<MyGUI::Button>("GasOfferCheck")->setStateCheck(mShowGas);
	mGUI->findWidget<MyGUI::Button>("CoalOfferCheck")->setStateCheck(mShowCoal);
	mGUI->findWidget<MyGUI::Button>("UraniumOfferCheck")->setStateCheck(mShowUranium);
	mGUI->findWidget<MyGUI::Button>("BuyCheck")->setStateCheck(mShowBuy);
	mGUI->findWidget<MyGUI::Button>("SellCheck")->setStateCheck(mShowSell);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::updateTradeData(void)
{
	if (mCompany) {
		int lObligationSelected = mGUI->findWidget<MyGUI::List>("Obligations")->getIndexSelected();
		int lPartnerSelected = mGUI->findWidget<MyGUI::List>("TradingPartners")->getIndexSelected();

		int lObligationScroll = mGUI->findWidget<MyGUI::List>("Obligations")->getScrollPosition();
		int lPartnerScroll = mGUI->findWidget<MyGUI::List>("TradingPartners")->getScrollPosition();

		std::vector<TradingOffer> lOffers  = mCompany->getTradingOffers();
		std::vector<TradingOffer> lObligations = mCompany->getTradingObligations();
		std::vector<TradingPartner> lPartners = mCompany->getTradingPartners();

		mGUI->findWidget<MyGUI::List>("TradingPartners")->removeAllItems();
		mGUI->findWidget<MyGUI::List>("OffersList")->removeAllItems();
		mGUI->findWidget<MyGUI::List>("Obligations")->removeAllItems();

		mGUI->findWidget<MyGUI::StaticText>("NoTrade")->setVisible(true);
		mGUI->findWidget<MyGUI::StaticText>("NoObligations")->setVisible(true);
		mGUI->findWidget<MyGUI::StaticText>("NoTradingPartners")->setVisible(true);

		for (size_t i = 0; i < lPartners.size(); i++) {
			if (!lPartners[i].mAvailable) {
				mGUI->findWidget<MyGUI::StaticText>("NoTradingPartners")->setVisible(false);

				mGUI->findWidget<MyGUI::List>("TradingPartners")->addItem(
					lPartners[i].mName + L" (" + toString(lPartners[i].mPrice) + L"k€)");
			}
		}

		for (size_t j = 0; j < lOffers.size(); j++) {
			bool lShow = false;

			if ((lOffers[j].mResource == eCoal) && (mShowCoal))
				lShow = true;
			else if ((lOffers[j].mResource == eGas) && (mShowGas))
				lShow = true;
			else if ((lOffers[j].mResource == eUranium) && (mShowUranium))
				lShow = true;

			if (lShow) {
				if (((lOffers[j].formatted().find(StrLoc::get()->Sell()) != -1) && (mShowSell))
					|| ((lOffers[j].formatted().find(StrLoc::get()->Buy()) != -1) && (mShowBuy)))
					lShow = true;
				else
					lShow = false;
			}

			if (lShow) {
				mGUI->findWidget<MyGUI::StaticText>("NoTrade")->setVisible(false);
				mGUI->findWidget<MyGUI::List>("OffersList")->addItem(lOffers[j].formatted());
			}
		}

		for (size_t k = 0; k < lObligations.size(); k++) {
			if ((lObligations[k].mPermanent) || (!lObligations[k].mUnique)) {
				mGUI->findWidget<MyGUI::StaticText>("NoObligations")->setVisible(false);
				mGUI->findWidget<MyGUI::List>("Obligations")->addItem(lObligations[k].formattedCurrent());
			}
		}

		if (lObligationSelected != MyGUI::ITEM_NONE) {
			if ((int)mGUI->findWidget<MyGUI::List>("Obligations")->getItemCount() > lObligationSelected) {
				mGUI->findWidget<MyGUI::List>("Obligations")->setIndexSelected(lObligationSelected);
				mGUI->findWidget<MyGUI::List>("Obligations")->setScrollPosition(lObligationScroll);
			}
		}

		if(lPartnerSelected != MyGUI::ITEM_NONE) {
			if((int)mGUI->findWidget<MyGUI::List>("TradingPartners")->getItemCount() > lPartnerSelected) {
				mGUI->findWidget<MyGUI::List>("TradingPartners")->setIndexSelected(lPartnerSelected);
				mGUI->findWidget<MyGUI::List>("TradingPartners")->setScrollPosition(lPartnerScroll);
			}
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::updateTradeWindow(EventData* pData)
{
	if (mLoading)
		return;

	updateTradeData();
	mOffersInvalidated = true;
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::updateObligations(EventData* pData)
{
	if (mLoading)
		return;

	if (mCompany) {
		int lObligationSelected = mGUI->findWidget<MyGUI::List>("Obligations")->getIndexSelected();
		int lObligationScroll = mGUI->findWidget<MyGUI::List>("Obligations")->getScrollPosition();

		std::vector<TradingOffer> lObligations = mCompany->getTradingObligations();
		mGUI->findWidget<MyGUI::List>("Obligations")->removeAllItems();
		mGUI->findWidget<MyGUI::StaticText>("NoObligations")->setVisible(true);

		for (size_t k = 0; k < lObligations.size(); k++) {
			if ((lObligations[k].mPermanent) || (!lObligations[k].mUnique)) {
				mGUI->findWidget<MyGUI::StaticText>("NoObligations")->setVisible(false);
				mGUI->findWidget<MyGUI::List>("Obligations")->addItem(lObligations[k].formattedCurrent());
			}
		}

		if (lObligationSelected != MyGUI::ITEM_NONE) {
			if ((int)mGUI->findWidget<MyGUI::List>("Obligations")->getItemCount() > lObligationSelected) {
				mGUI->findWidget<MyGUI::List>("Obligations")->setIndexSelected(lObligationSelected);
				mGUI->findWidget<MyGUI::List>("Obligations")->setScrollPosition(lObligationScroll);
			}
		}
	}

	mObligationsInvalidated = true;
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::tradeButtonPressed(MyGUI::WidgetPtr _widget)
{
	size_t lSelectedPartner = mGUI->findWidget<MyGUI::List>("TradingPartners")->getIndexSelected();
	size_t lSelectedOffer = mGUI->findWidget<MyGUI::List>("OffersList")->getIndexSelected();
	size_t lSelectedObligations = mGUI->findWidget<MyGUI::List>("Obligations")->getIndexSelected();

	if (_widget->getName().find("Check") != -1) {
		static_cast<MyGUI::ButtonPtr>(_widget)
      ->setStateCheck(!static_cast<MyGUI::ButtonPtr>(_widget)->getStateCheck());

		if (_widget->getName().find("Sell") != -1)
			mShowSell = !mShowSell;
		if (_widget->getName().find("Buy") != -1)
			mShowBuy = !mShowBuy;
		if (_widget->getName().find("Coal") != -1)
			mShowCoal = !mShowCoal;
		if (_widget->getName().find("Gas") != -1)
			mShowGas = !mShowGas;
		if (_widget->getName().find("Uranium") != -1)
			mShowUranium = !mShowUranium;

		updateTradeData();
	}

	if (_widget->getName().find("Sign") != -1) {
		if (lSelectedPartner != MyGUI::ITEM_NONE) {
			std::string lItem = mGUI->findWidget<MyGUI::List>("TradingPartners")
        ->getItemNameAt(lSelectedPartner);
			std::string lCountry = lItem.substr(0, lItem.find("(") - 1);
			std::string lCost = lItem.substr(lItem.find("(") + 1);
			lCost.resize(lCost.size()-1);

			MyGUI::MessagePtr lQuestionBuy = MyGUI::Message::createMessageBox("Message",
        StrLoc::get()->GameTitle(),
				StrLoc::get()->SignTradeA() + lCountry + StrLoc::get()->SignTradeB() 
        + lCost + StrLoc::get()->SignTradeC(),
				MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No 
        | MyGUI::MessageBoxStyle::IconQuest);
			lQuestionBuy->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::question);
			lQuestionBuy->setUserString("Name", "TradingPartnerQuestion");
		} else {
			MyGUI::MessagePtr lMessage = 
				MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
        StrLoc::get()->SelectPartner());
		}
	}
	else if(_widget->getName().find("AcceptOffer") != -1) {
		if(lSelectedOffer != MyGUI::ITEM_NONE) {
			mOffersInvalidated = false;
			MyGUI::MessagePtr lQuestionOffer = MyGUI::Message::createMessageBox("Message"
        , StrLoc::get()->GameTitle(),
				 mGUI->findWidget<MyGUI::List>("OffersList")->getItemNameAt(lSelectedOffer) + "?",
				MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No
        | MyGUI::MessageBoxStyle::IconQuest);
			lQuestionOffer->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::question);
			lQuestionOffer->setUserString("Name", "OfferQuestion");
		} else {
			MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
        StrLoc::get()->SelectOffer());
		}
	}
	else if(_widget->getName().find("CancelOffer") != -1) {
		if(lSelectedObligations != MyGUI::ITEM_NONE) {
			MyGUI::MessagePtr lQuestionCancel = MyGUI::Message::createMessageBox("Message",
        StrLoc::get()->GameTitle(),
				StrLoc::get()->CurrentlyResearching(),
				MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No 
        | MyGUI::MessageBoxStyle::IconQuest);
			lQuestionCancel->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::question);
			lQuestionCancel->setUserString("Name", "CancelQuestion");
		} else {
			MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
        StrLoc::get()->SelectObligation());
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::question(MyGUI::MessagePtr _sender, MyGUI::MessageBoxStyle _style)
{
	if (_sender->getUserString("Name").find("TradingPartnerQuestion") != -1) {
		if (_style == MyGUI::MessageBoxStyle::Yes) {
			size_t lSelectedPartner = mGUI->findWidget<MyGUI::List>("TradingPartners")
        ->getIndexSelected();
			std::string lItem = mGUI->findWidget<MyGUI::List>("TradingPartners")
        ->getItemNameAt(lSelectedPartner);

			if (!mCompany->buyTradingPartner(lItem.substr(0, lItem.find("(") - 1)))
				MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
        StrLoc::get()->NoMoney());
			else
				updateTradeData();
		}
	}
	else if (_sender->getUserString("Name").find("CancelQuestion") != -1) {
		if (_style == MyGUI::MessageBoxStyle::Yes) {
			size_t lSelectedObligation = mGUI->findWidget<MyGUI::List>("Obligations")
        ->getIndexSelected();
			if (lSelectedObligation != MyGUI::ITEM_NONE) {
				mCompany->cancelTradingObligation(lSelectedObligation);
				updateTradeData();
			} else {
				MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
          StrLoc::get()->ObligationNA());
			}
		}
	}
	else if (_sender->getUserString("Name").find("OfferQuestion") != -1) {
		if (_style == MyGUI::MessageBoxStyle::Yes) {
			if (!mOffersInvalidated) {
				size_t lSelectedOffers = mGUI->findWidget<MyGUI::List>("OffersList")
          ->getIndexSelected();
				if(mCompany->acceptTradingOffer(mGUI->findWidget<MyGUI::List>("OffersList")
          ->getItemNameAt(lSelectedOffers))) {
					updateTradeData();
				} else {
					if(mGUI->findWidget<MyGUI::List>("OffersList")
            ->getItemNameAt(lSelectedOffers).find("Sell ") != std::string::npos)
						MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
            StrLoc::get()->NoResources());
					else
						MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
            StrLoc::get()->NoMoney());
				}
			} else {
				MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
          StrLoc::get()->OfferNA());
			}
		}
	}
	else if (_sender->getUserString("Name").find("LoanQuestion") != -1) {
		if (_style == MyGUI::MessageBoxStyle::Yes) {
			Loan lLoan;
			lLoan.mAmountLeft = mAmount*1000;
			lLoan.mMonthLeft = mYears*12;
			lLoan.mMonthlyRate = mMonthlyRate*1000;

			mCompany->takeOutLoan(lLoan);

			updateLoans(0);
		}
	}
	else if (_sender->getUserString("Name").find("RepayQuestion") != -1) {
		if (_style == MyGUI::MessageBoxStyle::Yes) {
			if (!mCompany->unscheduledRepayment(mGUI->findWidget<MyGUI::List>("LoanList")
        ->getIndexSelected(), mRepayment*1000))
				MyGUI::Message::createMessageBox("Message", StrLoc::get()
        ->GameTitle(), StrLoc::get()->NoMoney());
			else
				updateLoans(0);
		}
	}
	else if (_sender->getUserString("Name") == "Ad1Question") {
		if (_style == MyGUI::MessageBoxStyle::Yes) {
			if (!mCompany->runAdCampaign(eInternet)) {
				MyGUI::Message::createMessageBox("Message",
          StrLoc::get()->GameTitle(), StrLoc::get()->NoMoney());
			} else {
				TickerMessage lMessage;
				lMessage.mMessage = StrLoc::get()->Ad1Started();
				lMessage.mPointOfInterest = Ogre::Vector2(-1,-1);
				lMessage.mUrgent = false;
				lMessage.mDetail = StrLoc::get()->Ad1Detail();

				EventHandler::raiseEvent(eTickerMessage, new EventArg<TickerMessage>(lMessage));
			}
		}
	}
	else if (_sender->getUserString("Name") == "Ad2Question") {
		if (_style == MyGUI::MessageBoxStyle::Yes) {
			if (!mCompany->runAdCampaign(eNewspaper)) {
				MyGUI::Message::createMessageBox("Message",
          StrLoc::get()->GameTitle(), StrLoc::get()->NoMoney());
			} else {
				TickerMessage lMessage;
				lMessage.mMessage = StrLoc::get()->Ad2Started();
				lMessage.mPointOfInterest = Ogre::Vector2(-1,-1);
				lMessage.mUrgent = false;
				lMessage.mDetail = StrLoc::get()->Ad2Detail();

				EventHandler::raiseEvent(eTickerMessage, new EventArg<TickerMessage>(lMessage));
			}
		}
	}
	else if (_sender->getUserString("Name") == "Ad3Question") {
		if (_style == MyGUI::MessageBoxStyle::Yes) {
			if (!mCompany->runAdCampaign(eRadio)) {
				MyGUI::Message::createMessageBox("Message",
          StrLoc::get()->GameTitle(), StrLoc::get()->NoMoney());
			} else {
				TickerMessage lMessage;
				lMessage.mMessage = StrLoc::get()->Ad3Started();
				lMessage.mPointOfInterest = Ogre::Vector2(-1,-1);
				lMessage.mUrgent = false;
				lMessage.mDetail = StrLoc::get()->Ad3Detail();

				EventHandler::raiseEvent(eTickerMessage, new EventArg<TickerMessage>(lMessage));
			}
		}
	}
	else if (_sender->getUserString("Name") == "Ad4Question") {
		if (_style == MyGUI::MessageBoxStyle::Yes) {
			if (!mCompany->runAdCampaign(eTV)) {
				MyGUI::Message::createMessageBox("Message",
          StrLoc::get()->GameTitle(), StrLoc::get()->NoMoney());
			} else {
				TickerMessage lMessage;
				lMessage.mMessage = StrLoc::get()->Ad4Started();
				lMessage.mPointOfInterest = Ogre::Vector2(-1,-1);
				lMessage.mUrgent = false;
				lMessage.mDetail = StrLoc::get()->Ad4Detail();

				EventHandler::raiseEvent(eTickerMessage, new EventArg<TickerMessage>(lMessage));
			}
		}
	}
	else if (_sender->getUserString("Name") == "Sp2Question") {
		if (_style == MyGUI::MessageBoxStyle::Yes) {
			if (!mCompany->runSpecialAction(eEmployee)) {
				MyGUI::Message::createMessageBox("Message",
          StrLoc::get()->GameTitle(), StrLoc::get()->NoMoney());
			} else {
				TickerMessage lMessage;
				lMessage.mMessage = StrLoc::get()->Sp2Started();
				lMessage.mDetail = StrLoc::get()->Sp2Detail();
				lMessage.mPointOfInterest = Ogre::Vector2(-1,-1);
				lMessage.mUrgent = false;

				EventHandler::raiseEvent(eTickerMessage, new EventArg<TickerMessage>(lMessage));
				EventHandler::raiseEvent(eUpdateSpecialActions);
			}
		}
	}
	else if (_sender->getUserString("Name") == "Sp1Question") {
		if (_style == MyGUI::MessageBoxStyle::Yes) {
			if (!mCompany->runSpecialAction(eWaste)) {
				MyGUI::Message::createMessageBox("Message",
          StrLoc::get()->GameTitle(), StrLoc::get()->NoMoney());
			} else {
				TickerMessage lMessage;
				lMessage.mMessage = StrLoc::get()->Sp1Started();
				lMessage.mDetail = StrLoc::get()->Sp1Detail();
				lMessage.mPointOfInterest = Ogre::Vector2(-1,-1);
				lMessage.mUrgent = false;

				EventHandler::raiseEvent(eTickerMessage, new EventArg<TickerMessage>(lMessage));
				EventHandler::raiseEvent(eUpdateSpecialActions);
			}
		}
	}
	else if (_sender->getUserString("Name") == "Sp3Question") {
		if (_style == MyGUI::MessageBoxStyle::Yes) {
			if (!mCompany->runSpecialAction(eTaxfraud)) {
				MyGUI::Message::createMessageBox("Message",
          StrLoc::get()->GameTitle(), StrLoc::get()->NoMoney());
			} else {
				TickerMessage lMessage;
				lMessage.mMessage = StrLoc::get()->Sp3Started();
				lMessage.mDetail = StrLoc::get()->Sp3Detail();
				lMessage.mPointOfInterest = Ogre::Vector2(-1,-1);
				lMessage.mUrgent = false;

				EventHandler::raiseEvent(eTickerMessage, new EventArg<TickerMessage>(lMessage));
				EventHandler::raiseEvent(eUpdateSpecialActions);
			}
		}
	}
	else if(_sender->getUserString("Name") == "Sp4Question") {
		if (_style == MyGUI::MessageBoxStyle::Yes) {
			if (!mCompany->runSpecialAction(eBribe)) {
				MyGUI::Message::createMessageBox("Message",
          StrLoc::get()->GameTitle(), StrLoc::get()->NoMoney());
			} else {
				TickerMessage lMessage;
				lMessage.mMessage = StrLoc::get()->Sp4Started();
				lMessage.mDetail = StrLoc::get()->Sp4Detail();
				lMessage.mPointOfInterest = Ogre::Vector2(-1,-1);
				lMessage.mUrgent = false;

				EventHandler::raiseEvent(eTickerMessage, new EventArg<TickerMessage>(lMessage));
				EventHandler::raiseEvent(eUpdateSpecialActions);
			}
		}
	}
	else if (_sender->getUserString("Name") == "Sp5Question") {
		if (_style == MyGUI::MessageBoxStyle::Yes) {
			if (!mCompany->runSpecialAction(eWar)) {
				MyGUI::Message::createMessageBox("Message",
          StrLoc::get()->GameTitle(), StrLoc::get()->NoMoney());
			} else {
				TickerMessage lMessage;
				lMessage.mMessage = StrLoc::get()->Sp5Started();
				lMessage.mDetail = StrLoc::get()->Sp5Detail();
				lMessage.mPointOfInterest = Ogre::Vector2(-1,-1);
				lMessage.mUrgent = false;

				EventHandler::raiseEvent(eTickerMessage, new EventArg<TickerMessage>(lMessage));
				EventHandler::raiseEvent(eUpdateSpecialActions);
			}
		}
	}
	else if (_sender->getUserString("Name") == "Sp6Question") {
		if (_style == MyGUI::MessageBoxStyle::Yes) {
			if (!mCompany->runSpecialAction(ePrice)) {
				MyGUI::Message::createMessageBox("Message",
          StrLoc::get()->GameTitle(), StrLoc::get()->NoMoney());
			} else {
				TickerMessage lMessage;
				lMessage.mMessage = StrLoc::get()->Sp6Started();
				lMessage.mDetail = StrLoc::get()->Sp6Detail();
				lMessage.mPointOfInterest = Ogre::Vector2(-1,-1);
				lMessage.mUrgent = false;

				EventHandler::raiseEvent(eTickerMessage, new EventArg<TickerMessage>(lMessage));
				EventHandler::raiseEvent(eUpdateSpecialActions);
			}
		}
	}
	else if(_sender->getUserString("Name") == "ResearchQuestion") {
		if(_style == MyGUI::MessageBoxStyle::Yes) {
			if(!mCompany->startResearch(mResearchIndex-1))
				MyGUI::Message::createMessageBox("Message",
        StrLoc::get()->GameTitle(), StrLoc::get()->NoMoney());
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::tradeListPressed(MyGUI::ListPtr _widget, size_t _index)
{
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::tradeQuestion(MyGUI::MessagePtr _sender, MyGUI::MessageBoxStyle _style)
{
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::researchPressed(MyGUI::WidgetPtr _widget)
{
	EventHandler::raiseEvent(eGamestateChangeView);

	mGUI->findWidget<MyGUI::Window>("ResearchWindow")->setShadow(true);

	mGUI->findWidget<MyGUI::Window>("ResearchWindow")->setVisible(true);
	mGUI->findWidget<MyGUI::Window>("ResearchWindow")->setPosition(
		(mGUI->getRenderWindow()->getWidth()/2)
    -(mGUI->findWidget<MyGUI::Window>("ResearchWindow")->getWidth()/2),
		(mGUI->getRenderWindow()->getHeight()/2)
    -(mGUI->findWidget<MyGUI::Window>("ResearchWindow")->getHeight()/2));

	MyGUI::LayerManager::getInstance().upLayerItem(
    mGUI->findWidget<MyGUI::Window>("ResearchWindow"));

	mGUI->findWidget<MyGUI::Window>("ResearchWindow")
    ->setCaption(StrLoc::get()->ResearchCaption());
	mGUI->findWidget<MyGUI::StaticText>("CurrentResearchText")
    ->setCaption(StrLoc::get()->CurrentlyResearching());

	mGUI->findWidget<MyGUI::Button>("Re1")->eventMouseButtonClick
    = MyGUI::newDelegate(this, &GUI::researchButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Re2")->eventMouseButtonClick
    = MyGUI::newDelegate(this, &GUI::researchButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Re3")->eventMouseButtonClick
    = MyGUI::newDelegate(this, &GUI::researchButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Re4")->eventMouseButtonClick
    = MyGUI::newDelegate(this, &GUI::researchButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Re5")->eventMouseButtonClick
    = MyGUI::newDelegate(this, &GUI::researchButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Re6")->eventMouseButtonClick
    = MyGUI::newDelegate(this, &GUI::researchButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Re7")->eventMouseButtonClick
    = MyGUI::newDelegate(this, &GUI::researchButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Re8")->eventMouseButtonClick
    = MyGUI::newDelegate(this, &GUI::researchButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Re9")->eventMouseButtonClick
    = MyGUI::newDelegate(this, &GUI::researchButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Re10")->eventMouseButtonClick
    = MyGUI::newDelegate(this, &GUI::researchButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Re11")->eventMouseButtonClick
    = MyGUI::newDelegate(this, &GUI::researchButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Re12")->eventMouseButtonClick
    = MyGUI::newDelegate(this, &GUI::researchButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Re13")->eventMouseButtonClick
    = MyGUI::newDelegate(this, &GUI::researchButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Re14")->eventMouseButtonClick
    = MyGUI::newDelegate(this, &GUI::researchButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Re15")->eventMouseButtonClick
    = MyGUI::newDelegate(this, &GUI::researchButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Re16")->eventMouseButtonClick
    = MyGUI::newDelegate(this, &GUI::researchButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Re17")->eventMouseButtonClick
    = MyGUI::newDelegate(this, &GUI::researchButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Re18")->eventMouseButtonClick
    = MyGUI::newDelegate(this, &GUI::researchButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Re19")->eventMouseButtonClick
    = MyGUI::newDelegate(this, &GUI::researchButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Re20")->eventMouseButtonClick
    = MyGUI::newDelegate(this, &GUI::researchButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Re21")->eventMouseButtonClick
    = MyGUI::newDelegate(this, &GUI::researchButtonPressed);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::researchButtonPressed(MyGUI::WidgetPtr _widget)
{
	if (_widget->getName().find("Re") != -1) {
		mResearchIndex = toNumber<int>(_widget->getName().substr(2));
		int lTemp;

		if ((mResearchIndex > 10) && (!mCompany->isResearchBuilt())) {
			MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
        StrLoc::get()->ResearchNoAccess());
		}
		else if (!mCompany->isAvailableInTree(mResearchIndex-1)) {
			MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
        StrLoc::get()->ResearchNoAccessTree());
		}
		else if (Ogre::UTFString(mCompany->getCurrentResearch(lTemp)) != StrLoc::get()->Nothing()) {
			MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
        StrLoc::get()->ResearchOne());
		}
		else if (mCompany->getResearchSet().mResearched[mResearchIndex-1]) {
			MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
        StrLoc::get()->ResearchDone());
		} else {
			MyGUI::MessagePtr lQuestionResearch = MyGUI::Message::createMessageBox("Message",
        StrLoc::get()->GameTitle(),
				StrLoc::get()->StartResearchA() + mCompany->getResearchSet().mName[mResearchIndex-1]
        + StrLoc::get()->StartResearchB()
				+ toString(mCompany->getResearchSet().mPrice[mResearchIndex-1]) + L"k€)",
				MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No
        | MyGUI::MessageBoxStyle::IconQuest);
			lQuestionResearch->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::question);
			lQuestionResearch->setUserString("Name", "ResearchQuestion");
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::ceoPressed(MyGUI::WidgetPtr _widget)
{
	EventHandler::raiseEvent(eGamestateChangeView);

	mGUI->findWidget<MyGUI::Window>("CEOWindow")->setVisible(true);
	mGUI->findWidget<MyGUI::Window>("CEOWindow")->setPosition(
		(mGUI->getRenderWindow()->getWidth()/2)
    -(mGUI->findWidget<MyGUI::Window>("CEOWindow")->getWidth()/2),
		(mGUI->getRenderWindow()->getHeight()/2)
    -(mGUI->findWidget<MyGUI::Window>("CEOWindow")->getHeight()/2));

	mGUI->findWidget<MyGUI::Window>("CEOWindow")->setShadow(true);

	mGUI->findWidget<MyGUI::Window>("CEOWindow")->setCaption(StrLoc::get()->CEOCaption());
	mGUI->findWidget<MyGUI::TabItem>("OfficeTabText")->setCaption(StrLoc::get()->CEOCaption());
	mGUI->findWidget<MyGUI::TabItem>("CustomersTabText")->setCaption(StrLoc::get()->CEOCustomers());
	mGUI->findWidget<MyGUI::StaticText>("CustomerOpinionsText")->setCaption(StrLoc::get()->CustomersOpinionsFrom());
	mGUI->findWidget<MyGUI::StaticText>("NoOpinions")->setCaption(StrLoc::get()->NoOpinions());
	mGUI->findWidget<MyGUI::Button>("AllCitiesCheck")->setCaption(StrLoc::get()->AllCities());
	mGUI->findWidget<MyGUI::StaticText>("CEOCustomersText")->setCaption(StrLoc::get()->Customers());

	MyGUI::LayerManager::getInstance().upLayerItem(mGUI->findWidget<MyGUI::Window>("CEOWindow"));

	mGUI->findWidget<MyGUI::Button>("AllCitiesCheck")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::ceoButtonPressed);
	mGUI->findWidget<MyGUI::ComboBox>("CitiesCombo")->eventComboChangePosition = 
		MyGUI::newDelegate(this, &GUI::ceoComboPressed);


	mGUI->findWidget<MyGUI::Button>("Ad1")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::ceoButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Ad2")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::ceoButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Ad3")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::ceoButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Ad4")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::ceoButtonPressed);

	mGUI->findWidget<MyGUI::Button>("Sp1")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::ceoButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Sp2")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::ceoButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Sp3")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::ceoButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Sp4")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::ceoButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Sp5")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::ceoButtonPressed);
	mGUI->findWidget<MyGUI::Button>("Sp6")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::ceoButtonPressed);

	mGUI->findWidget<MyGUI::Tab>("CEOTab")->eventTabChangeSelect = 
		MyGUI::newDelegate(this, &GUI::ceoTabPressed);

	std::vector<boost::shared_ptr<City> > lConnectedCities = mCompany->getConnectedCities(true);

	mGUI->findWidget<MyGUI::ComboBox>("CitiesCombo")->removeAllItems();
	mGUI->findWidget<MyGUI::ComboBox>("CitiesCombo")->setComboModeDrop(true);

	for (size_t i = 0; i < lConnectedCities.size(); i++)
		mGUI->findWidget<MyGUI::ComboBox>("CitiesCombo")->addItem(lConnectedCities[i]->getName());

	if (!lConnectedCities.empty())
		mGUI->findWidget<MyGUI::ComboBox>("CitiesCombo")->setIndexSelected(0);

	mGUI->findWidget<MyGUI::List>("OpinionsList")->removeAllItems();
	mGUI->findWidget<MyGUI::StaticText>("NoOpinions")->setVisible(true);

	std::vector<std::string> lOpinions = mCompany->getCustomerOpinions(true);

	for (size_t j = 0; j < lOpinions.size(); j++) {
		mGUI->findWidget<MyGUI::StaticText>("NoOpinions")->setVisible(false);
		mGUI->findWidget<MyGUI::List>("OpinionsList")->addItem(lOpinions[j]);
	}

	mGUI->findWidget<MyGUI::Button>("AllCitiesCheck")->setStateCheck(true);
	mGUI->findWidget<MyGUI::ComboBox>("CitiesCombo")->setEnabled(false);
	mGUI->findWidget<MyGUI::ComboBox>("CitiesCombo")
    ->setTextColour(Ogre::ColourValue(0.3, 0.3, 0.3));
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::ceoButtonPressed(MyGUI::WidgetPtr _widget)
{
	if (_widget->getName().find("AllCitiesCheck") != -1) {
		mGUI->findWidget<MyGUI::Button>("AllCitiesCheck")->setStateCheck(
			!mGUI->findWidget<MyGUI::Button>("AllCitiesCheck")->getStateCheck());

		if (mGUI->findWidget<MyGUI::Button>("AllCitiesCheck")->getStateCheck()) {
			mGUI->findWidget<MyGUI::ComboBox>("CitiesCombo")->setEnabled(false);
			mGUI->findWidget<MyGUI::ComboBox>("CitiesCombo")
        ->setTextColour(Ogre::ColourValue(0.3, 0.3, 0.3));

			mGUI->findWidget<MyGUI::List>("OpinionsList")->removeAllItems();

			std::vector<std::string> lOpinions = mCompany->getCustomerOpinions(true);

			mGUI->findWidget<MyGUI::StaticText>("NoOpinions")->setVisible(true);

			for (size_t j = 0; j < lOpinions.size(); j++) {
				mGUI->findWidget<MyGUI::StaticText>("NoOpinions")->setVisible(false);
				mGUI->findWidget<MyGUI::List>("OpinionsList")->addItem(lOpinions[j]);
			}
		} else {
			mGUI->findWidget<MyGUI::ComboBox>("CitiesCombo")->setEnabled(true);
			mGUI->findWidget<MyGUI::ComboBox>("CitiesCombo")
        ->setTextColour(Ogre::ColourValue(0.0, 0.0, 0.0));
		}
	}
	else if (_widget->getName() == "Ad1") {
		MyGUI::MessagePtr lQuestionBuy = MyGUI::Message::createMessageBox("Message", 
      StrLoc::get()->GameTitle(),
			StrLoc::get()->Ad1Question(),
			MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No 
      | MyGUI::MessageBoxStyle::IconQuest);
		lQuestionBuy->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::question);
		lQuestionBuy->setUserString("Name", "Ad1Question");
	}
	else if (_widget->getName() == "Ad2") {
		MyGUI::MessagePtr lQuestionBuy = MyGUI::Message::createMessageBox("Message", 
      StrLoc::get()->GameTitle(),
			StrLoc::get()->Ad2Question(),
			MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No 
      | MyGUI::MessageBoxStyle::IconQuest);
		lQuestionBuy->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::question);
		lQuestionBuy->setUserString("Name", "Ad2Question");
	}
	else if (_widget->getName() == "Ad3") {
		if (mCompany->isPRBuilt()) {
			MyGUI::MessagePtr lQuestionBuy = MyGUI::Message::createMessageBox("Message",
        StrLoc::get()->GameTitle(),
				StrLoc::get()->Ad3Question(),
				MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No 
        | MyGUI::MessageBoxStyle::IconQuest);
			lQuestionBuy->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::question);
			lQuestionBuy->setUserString("Name", "Ad3Question");
		} else {
			MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
        StrLoc::get()->AdNoAccess());
		}
	}
	else if (_widget->getName() == "Ad4") {
		if (mCompany->isPRBuilt()) {
			MyGUI::MessagePtr lQuestionBuy = MyGUI::Message::createMessageBox("Message",
        StrLoc::get()->GameTitle(),
				StrLoc::get()->Ad4Question(),
				MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No 
        | MyGUI::MessageBoxStyle::IconQuest);
			lQuestionBuy->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::question);
			lQuestionBuy->setUserString("Name", "Ad4Question");
		} else {
			MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
        StrLoc::get()->AdNoAccess());
		}
	}
	else if (_widget->getName() == "Sp1") {
		if (!mCompany->isSpecialActionRunning(eEmployee)) {
			MyGUI::MessagePtr lQuestionBuy = MyGUI::Message::createMessageBox("Message", 
        StrLoc::get()->GameTitle(),
				StrLoc::get()->Sp1Question(),
				MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No 
        | MyGUI::MessageBoxStyle::IconQuest);
			lQuestionBuy->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::question);
			lQuestionBuy->setUserString("Name", "Sp2Question");
		} else {
			MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
        StrLoc::get()->SpStarted());
		}
	}
	else if (_widget->getName() == "Sp2") {
		if (!mCompany->isSpecialActionRunning(eWaste)) {
			MyGUI::MessagePtr lQuestionBuy = MyGUI::Message::createMessageBox("Message",
        StrLoc::get()->GameTitle(),
				StrLoc::get()->Sp2Question(),
				MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No 
        | MyGUI::MessageBoxStyle::IconQuest);
			lQuestionBuy->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::question);
			lQuestionBuy->setUserString("Name", "Sp1Question");
		} else {
			MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
        StrLoc::get()->SpStarted());
		}
	}
	else if (_widget->getName() == "Sp3") {
		if (!mCompany->isSpecialActionRunning(eTaxfraud)) {
			MyGUI::MessagePtr lQuestionBuy = MyGUI::Message::createMessageBox("Message",
        StrLoc::get()->GameTitle(),
				StrLoc::get()->Sp3Question(),
				MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No | MyGUI::MessageBoxStyle::IconQuest);
			lQuestionBuy->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::question);
			lQuestionBuy->setUserString("Name", "Sp3Question");
		} else {
			MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
        StrLoc::get()->SpStarted());
		}
	}
	else if (_widget->getName() == "Sp4") {
		if (mCompany->isHeadquartersBuilt()) {
			if (!mCompany->isSpecialActionRunning(eBribe)) {
				MyGUI::MessagePtr lQuestionBuy = MyGUI::Message::createMessageBox("Message",
          StrLoc::get()->GameTitle(),
					StrLoc::get()->Sp4Question(),
					MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No 
          | MyGUI::MessageBoxStyle::IconQuest);
				lQuestionBuy->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::question);
				lQuestionBuy->setUserString("Name", "Sp4Question");
			} else {
				MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
          StrLoc::get()->SpStarted());
			}
		} else {
			MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
        StrLoc::get()->SpNoAccess());
		}
	}
	else if (_widget->getName() == "Sp5") {
		if (mCompany->isHeadquartersBuilt()) {
			if (!mCompany->isSpecialActionRunning(eWar)) {
				MyGUI::MessagePtr lQuestionBuy = MyGUI::Message::createMessageBox("Message",
          StrLoc::get()->GameTitle(),
					StrLoc::get()->Sp5Question(),
					MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No |
          MyGUI::MessageBoxStyle::IconQuest);
				lQuestionBuy->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::question);
				lQuestionBuy->setUserString("Name", "Sp5Question");
			} else {
				MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
          StrLoc::get()->SpStarted());
			}
		} else {
			MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
        StrLoc::get()->SpNoAccess());
		}
	}
	else if (_widget->getName() == "Sp6") {
		if (mCompany->isHeadquartersBuilt()) {
			if (!mCompany->isSpecialActionRunning(ePrice)) {
				MyGUI::MessagePtr lQuestionBuy = MyGUI::Message::createMessageBox("Message",
          StrLoc::get()->GameTitle(),
					StrLoc::get()->Sp6Question(),
					MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No |
          MyGUI::MessageBoxStyle::IconQuest);
				lQuestionBuy->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::question);
				lQuestionBuy->setUserString("Name", "Sp6Question");
			} else {
				MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle()
          , StrLoc::get()->SpStarted());
			}
		} else {
			MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
        StrLoc::get()->SpNoAccess());
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::ceoTabPressed(MyGUI::TabPtr _widget, size_t _index)
{
	EventHandler::raiseEvent(eUpdateCOBuildingButtons);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::ceoComboPressed(MyGUI::ComboBoxPtr _widget, size_t _index)
{
	mGUI->findWidget<MyGUI::List>("OpinionsList")->removeAllItems();

	std::vector<std::string> lOpinions = mCompany->getCustomerOpinions(false);

	mGUI->findWidget<MyGUI::StaticText>("NoOpinions")->setVisible(true);

	for (size_t j = 0; j < lOpinions.size(); j++) {
		mGUI->findWidget<MyGUI::StaticText>("NoOpinions")->setVisible(false);
		mGUI->findWidget<MyGUI::List>("OpinionsList")->addItem(lOpinions[j]);
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::zoomRotatePressed(MyGUI::WidgetPtr _widget)
{
	EventHandler::raiseEvent(eGamestateChangeView);

	if (_widget->getName() == "ZoomIn")
		EventHandler::raiseEvent(eCameraZoom, new EventArg<int>(-1));
	else if (_widget->getName() == "ZoomOut")
		EventHandler::raiseEvent(eCameraZoom, new EventArg<int>(1));
	else if (_widget->getName() == "RotateLeft")
		EventHandler::raiseEvent(eCameraRotate, new EventArg<int>(1));
	else if (_widget->getName() == "RotateRight")
		EventHandler::raiseEvent(eCameraRotate, new EventArg<int>(-1));
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::minimapButtonPressed(MyGUI::WidgetPtr _widget)
{
	EventHandler::raiseEvent(eGamestateChangeView);

	if (_widget->getName() == "MinimapShowSupply")
		EventHandler::raiseEvent(eMinimapModeChange, new EventArg<int>((int)eSupply));
	else if (_widget->getName() == "MinimapShowNets")
		EventHandler::raiseEvent(eMinimapModeChange, new EventArg<int>((int)eLines));
	else if (_widget->getName() == "MinimapShowResources")
		EventHandler::raiseEvent(eMinimapModeChange, new EventArg<int>((int)eResources));
	else if (_widget->getName() == "MinimapShowDefault")
		EventHandler::raiseEvent(eMinimapModeChange, new EventArg<int>((int)eDefault));
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::setHoverText(int pX, int pY, std::string pText, MyGUI::Colour pColour)
{
	mGUI->findWidget<MyGUI::StaticText>("HoverText")->setPosition(pX, pY);
	mGUI->findWidget<MyGUI::StaticText>("HoverText")->setCaption(pText);
	mGUI->findWidget<MyGUI::StaticText>("HoverText")->setTextColour(pColour);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::setHoverTextVisible(bool pVisible)
{
	mGUI->findWidget<MyGUI::StaticText>("HoverText")->setVisible(pVisible);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::gameSpeedFasterPressed(MyGUI::WidgetPtr _widget)
{
	if (mCurrentSpeed == ePause)
		mGUI->findWidget<MyGUI::StaticImage>("SpeedIndicator")->setImageTexture("slow.png");
	else if (mCurrentSpeed == eSlow)
		mGUI->findWidget<MyGUI::StaticImage>("SpeedIndicator")->setImageTexture("normal.png");
	else if (mCurrentSpeed == eNormal)
		mGUI->findWidget<MyGUI::StaticImage>("SpeedIndicator")->setImageTexture("fast.png");

	EventHandler::raiseEvent(eGameSpeedFaster);
	EventHandler::raiseEvent(eGamestateChangeView);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::gameSpeedSlowerPressed(MyGUI::WidgetPtr _widget)
{
	if (mCurrentSpeed == eSlow)
		mGUI->findWidget<MyGUI::StaticImage>("SpeedIndicator")->setImageTexture("pause.png");
	else if (mCurrentSpeed == eNormal)
		mGUI->findWidget<MyGUI::StaticImage>("SpeedIndicator")->setImageTexture("slow.png");
	else if (mCurrentSpeed == eFast)
		mGUI->findWidget<MyGUI::StaticImage>("SpeedIndicator")->setImageTexture("normal.png");

	EventHandler::raiseEvent(eGameSpeedSlower);
	EventHandler::raiseEvent(eGamestateChangeView);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::gameSpeedPausePressed(MyGUI::WidgetPtr _widget)
{
	mGUI->findWidget<MyGUI::StaticImage>("SpeedIndicator")->setImageTexture("pause.png");

	EventHandler::raiseEvent(eGameSpeedPause);
	EventHandler::raiseEvent(eGamestateChangeView);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::buildPressed(MyGUI::WidgetPtr _widget)
{
	if ((toNumber<int>(_widget->getName()) == eCOHeadquarters) && (mCompany->isHeadquartersBuilt())) {
		MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
      StrLoc::get()->UniqueHeadquarters());
		unshowInfo();
		return;
	}
	else if ((toNumber<int>(_widget->getName()) == eCOResearch) && (mCompany->isResearchBuilt())) {
		MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
      StrLoc::get()->UniqueResearch());
		unshowInfo();
		return;
	}
	else if ((toNumber<int>(_widget->getName()) == eCOPublicRelations) && (mCompany->isPRBuilt())) {
		MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
      StrLoc::get()->UniquePR());
		unshowInfo();
		return;
	}

	bool lNotResearched = false;
	ResearchSet lResearchSet = mCompany->getResearchSet();

	if ((toNumber<int>(_widget->getName()) == ePPCoalLarge) && (!lResearchSet.mResearched[0]))
		lNotResearched = true;
	else if ((toNumber<int>(_widget->getName()) == ePPWindLarge) && (!lResearchSet.mResearched[1]))
		lNotResearched = true;
	else if ((toNumber<int>(_widget->getName()) == ePPBio) && (!lResearchSet.mResearched[3]))
		lNotResearched = true;
	else if ((toNumber<int>(_widget->getName()) == ePPWindOffshore) && (!lResearchSet.mResearched[6]))
		lNotResearched = true;
	else if ((toNumber<int>(_widget->getName()) == ePPNuclearSmall) && (!lResearchSet.mResearched[8]))
		lNotResearched = true;
	else if ((toNumber<int>(_widget->getName()) == ePPGasLarge) && (!lResearchSet.mResearched[9]))
		lNotResearched = true;
	else if ((toNumber<int>(_widget->getName()) == eRECoalLarge) && (!lResearchSet.mResearched[10]))
		lNotResearched = true;
	else if ((toNumber<int>(_widget->getName()) == eREGasLarge) && (!lResearchSet.mResearched[12]))
		lNotResearched = true;
	else if ((toNumber<int>(_widget->getName()) == ePPSolarLarge) && (!lResearchSet.mResearched[15]))
		lNotResearched = true;
	else if ((toNumber<int>(_widget->getName()) == ePPSolarUpdraft) && (!lResearchSet.mResearched[16]))
		lNotResearched = true;
	else if ((toNumber<int>(_widget->getName()) == ePPNuclearLarge) && (!lResearchSet.mResearched[18]))
		lNotResearched = true;
	else if ((toNumber<int>(_widget->getName()) == eREUraniumLarge) && (!lResearchSet.mResearched[19]))
		lNotResearched = true;
	else if ((toNumber<int>(_widget->getName()) == ePPNuclearFusion) && (!lResearchSet.mResearched[20]))
		lNotResearched = true;

	if (lNotResearched) {
		MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
      StrLoc::get()->BuildingNoAccess());
		return;
	}

	EventHandler::raiseEvent(eGamestateChangeBuild, 
		new EventArg<int>(toNumber<int>(_widget->getName())));

	unshowInfo();
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::buildPowerPlantPressed(MyGUI::WidgetPtr _widget)
{
	unshowInfo();
	EventHandler::raiseEvent(eHideSelector);
	EventHandler::raiseEvent(eGamestateChangeView);
	mGUI->findWidget<MyGUI::StaticImage>("BuildPowerplantPanel")->setVisible(true);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::buildResourcePlantPressed(MyGUI::WidgetPtr _widget)
{
	unshowInfo();
	EventHandler::raiseEvent(eHideSelector);
	EventHandler::raiseEvent(eGamestateChangeView);
	mGUI->findWidget<MyGUI::StaticImage>("BuildResourcePanel")->setVisible(true);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::buildDistributionPressed(MyGUI::WidgetPtr _widget)
{
	unshowInfo();
	EventHandler::raiseEvent(eHideSelector);
	EventHandler::raiseEvent(eGamestateChangeView);
	mGUI->findWidget<MyGUI::StaticImage>("BuildDistributionPanel")->setVisible(true);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::buildMiscPressed(MyGUI::WidgetPtr _widget)
{
	unshowInfo();
	EventHandler::raiseEvent(eHideSelector);
	EventHandler::raiseEvent(eGamestateChangeView);
	mGUI->findWidget<MyGUI::StaticImage>("BuildMiscPanel")->setVisible(true);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::demolishPressed(MyGUI::WidgetPtr _widget)
{
	unshowInfo();
	EventHandler::raiseEvent(eHideSelector);
	MyGUI::PointerManager::getInstance().setPointer("demolish", _widget);
	EventHandler::raiseEvent(eGamestateChangeDemolish);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::minimapPressed(MyGUI::WidgetPtr _widget, int _left, int _top, MyGUI::MouseButton _button)
{
	EventHandler::raiseEvent(eGamestateChangeView);

	float lXRelative = _left - _widget->getAbsolutePosition().left;
	float lYRelative = _top - _widget->getAbsolutePosition().top;

	int lOffsetX = 0;
	int lOffsetY = 0;
	int lLargerSize = 0;

	if (mCurrentTerrainWidth > mCurrentTerrainHeight) {
		lOffsetY = (mCurrentTerrainWidth - mCurrentTerrainHeight)/2;
		lLargerSize = mCurrentTerrainWidth;
	}
	else if (mCurrentTerrainWidth < mCurrentTerrainHeight) {
		lOffsetX = (mCurrentTerrainHeight - mCurrentTerrainWidth)/2;
		lLargerSize = mCurrentTerrainHeight;
	} else {
		lLargerSize = mCurrentTerrainHeight;
	}

	int lCameraX = (float)((float)lXRelative 
		/ (float)cMinimapRealWidth) * lLargerSize;

	int lCameraY = (float)((float)lYRelative 
		/ (float)cMinimapRealHeight) * lLargerSize;

	if ( (lCameraX > lOffsetX)
		&& (lCameraX < (lLargerSize - lOffsetX))
		&& (lCameraY > lOffsetY)
		&& (lCameraY < (lLargerSize - lOffsetY)) ) {
		EventHandler::raiseEvent(eMoveCamera, new EventArg<int>(lCameraX - lOffsetX,
			lCameraY - lOffsetY));
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::minimapDragged(MyGUI::WidgetPtr _widget, int _left, int _top)
{
	EventHandler::raiseEvent(eGamestateChangeView);
	minimapPressed(mGUI->findWidget<MyGUI::StaticImage>("Minimap"), _left, _top,
    MyGUI::MouseButton::None);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::showHideResourcesPressed(MyGUI::WidgetPtr _widget)
{
	EventHandler::raiseEvent(eGamestateChangeView);

	if (mGUI->findWidget<MyGUI::Widget>("ResourceOverviewPanel")->isVisible()) {	
		mGUI->findWidget<MyGUI::Widget>("CoalPanel")->setVisible(true);
		mGUI->findWidget<MyGUI::Widget>("GasPanel")->setVisible(true);
		mGUI->findWidget<MyGUI::Widget>("UraniumPanel")->setVisible(true);
		mGUI->findWidget<MyGUI::StaticImage>("LeftRP")->setImageTexture("linksobendetail.png");
		mGUI->findWidget<MyGUI::StaticImage>("LeftRP")->setSize(256,256);
		mGUI->findWidget<MyGUI::Button>("ShowHideResource")->setCaption("^");
		mGUI->findWidget<MyGUI::Widget>("ResourceOverviewPanel")->setVisible(false);
	} else {
		mGUI->findWidget<MyGUI::StaticImage>("LeftRP")->setImageTexture("linksoben.png");
		mGUI->findWidget<MyGUI::StaticImage>("LeftRP")->setSize(256,128);
		mGUI->findWidget<MyGUI::Button>("ShowHideResource")->setCaption("v");
		mGUI->findWidget<MyGUI::Widget>("ResourceOverviewPanel")->setVisible(true);
		mGUI->findWidget<MyGUI::Widget>("CoalPanel")->setVisible(false);
		mGUI->findWidget<MyGUI::Widget>("GasPanel")->setVisible(false);
		mGUI->findWidget<MyGUI::Widget>("UraniumPanel")->setVisible(false);
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::showPowerNetsWindowPressed(MyGUI::WidgetPtr _widget)
{
	EventHandler::raiseEvent(eGamestateChangeView);

	if (mCompany->getPowerNet()->getSubnetCount() != 0) {
		bool lSaneNetFound = false;

		for (size_t j = 0; j < mCompany->getPowerNet()->getSubnetCount(); j++)
			if(mCompany->getPowerNet()->getSubnetPowerplants(j).size() != 0)
				lSaneNetFound = true;

		if (lSaneNetFound) {
			updatePowerNetWindow(0);

			mGUI->findWidget<MyGUI::Window>("PowerNetsWindow")->setVisible(true);
			mGUI->findWidget<MyGUI::Window>("PowerNetsWindow")
        ->setCaption(StrLoc::get()->PowerNetCaption());
			mGUI->findWidget<MyGUI::Window>("PowerNetsWindow")->setPosition(
				(mGUI->getRenderWindow()->getWidth()/2)
        -(mGUI->findWidget<MyGUI::Window>("PowerNetsWindow")->getWidth()/2),
				(mGUI->getRenderWindow()->getHeight()/2)
        -(mGUI->findWidget<MyGUI::Window>("PowerNetsWindow")->getHeight()/2));

			mGUI->findWidget<MyGUI::Window>("PowerNetsWindow")->setShadow(true);

			MyGUI::LayerManager::getInstance().upLayerItem(mGUI
        ->findWidget<MyGUI::Window>("PowerNetsWindow"));
		} else {
			MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle()
        , StrLoc::get()->NoPowerNet());
		}
	} else {
		MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
      StrLoc::get()->NoPowerNet());
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::closeWindowPressed(MyGUI::WindowPtr _widget, const std::string& _string)
{
	_widget->setVisible(false);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::showTickerArchiveWindowPressed(MyGUI::WidgetPtr _widget)
{
	EventHandler::raiseEvent(eGamestateChangeView);

	if (mTickerArchive.size() != 0) {
    if(mGUI->findWidget<MyGUI::List>("TickerList")->getItemCount() == 0) {
      // List was cleared (main menu and back), refill
	    for (size_t i = 0; i < mTickerArchive.size(); ++i)
		    mGUI->findWidget<MyGUI::List>("TickerList")->addItem(mTickerArchive[i].mDateTime 
        + ": " + mTickerArchive[i].mMessage);
    }

		mGUI->findWidget<MyGUI::Window>("TickerArchiveWindow")
      ->setCaption(StrLoc::get()->TickerCaption());
		mGUI->findWidget<MyGUI::Edit>("MessageDetail")
      ->setCaption("Details: " + StrLoc::get()->GoalNone());

		mGUI->findWidget<MyGUI::Window>("TickerArchiveWindow")->setVisible(true);
		mGUI->findWidget<MyGUI::Window>("TickerArchiveWindow")->setPosition(
			(mGUI->getRenderWindow()->getWidth()/2)
      -(mGUI->findWidget<MyGUI::Window>("TickerArchiveWindow")->getWidth()/2),
			(mGUI->getRenderWindow()->getHeight()/2
      )-(mGUI->findWidget<MyGUI::Window>("TickerArchiveWindow")->getHeight()/2));

		mGUI->findWidget<MyGUI::Window>("TickerArchiveWindow")->setShadow(true);

		MyGUI::LayerManager::getInstance().upLayerItem(mGUI
      ->findWidget<MyGUI::Window>("TickerArchiveWindow"));

		mGUI->findWidget<MyGUI::List>("TickerList")->eventListChangePosition = 
			MyGUI::newDelegate(this, &GUI::tickerListPressed);

		if (mSelectMessage != -1) {
			mGUI->findWidget<MyGUI::List>("TickerList")->setIndexSelected(mSelectMessage);
			mGUI->findWidget<MyGUI::Edit>("MessageDetail")
        ->setCaption("Details: " + mTickerArchive[mSelectMessage].mDetail);

			mSelectMessage = -1;
		}
	} else {
		MyGUI::Message::createMessageBox("Message", StrLoc::get()
      ->GameTitle(), StrLoc::get()->TickerEmpty());
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::tickerListPressed(MyGUI::ListPtr _widget, size_t _index)
{
	if (_index != MyGUI::ITEM_NONE)
		mGUI->findWidget<MyGUI::Edit>("MessageDetail")
    ->setCaption("Details: " + mTickerArchive[_index].mDetail);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::tickerLinePressed(MyGUI::WidgetPtr _widget)
{
	EventHandler::raiseEvent(eGamestateChangeView);
	if (_widget->getName() == "TickerLine1") {
		if (mTickerArchive.size() > 1) {
			EventHandler::raiseEvent(eMoveCamera, new EventArg<int>(
				mTickerArchive[1].mPointOfInterest.x,
				mTickerArchive[1].mPointOfInterest.y));

      if (Ogre::UTFString(mTickerArchive[1].mDetail) != StrLoc::get()->GoalNone()) {
				mSelectMessage = 1;
				showTickerArchiveWindowPressed(0);
			}
		}
	} else {
		if (mTickerArchive.size() > 0) {
			EventHandler::raiseEvent(eMoveCamera, new EventArg<int>(
				mTickerArchive[0].mPointOfInterest.x,
				mTickerArchive[0].mPointOfInterest.y));

      if (Ogre::UTFString(mTickerArchive[0].mDetail) != StrLoc::get()->GoalNone()) {
				mSelectMessage = 0;
				showTickerArchiveWindowPressed(0);
			}
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::tickerLineMouseOver(MyGUI::WidgetPtr _widget, MyGUI::WidgetPtr _old)
{
	if (_widget->getName() == "TickerLine1")
		mGUI->findWidget<MyGUI::StaticText>("TickerLine1")
    ->setTextColour(Ogre::ColourValue(0.95,0.95,0.0));
	else
		mGUI->findWidget<MyGUI::StaticText>("TickerLine2")
    ->setTextColour(Ogre::ColourValue(0.95,0.95,0.0));
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::tickerLineMouseOut(MyGUI::WidgetPtr _widget, MyGUI::WidgetPtr _old)
{
	if (_widget->getName() == "TickerLine1")
		mGUI->findWidget<MyGUI::StaticText>("TickerLine1")
    ->setTextColour(Ogre::ColourValue(0.95,0.95,0.95));
	else
		mGUI->findWidget<MyGUI::StaticText>("TickerLine2")
    ->setTextColour(Ogre::ColourValue(0.95,0.95,0.95));
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::buttonDownPressed(MyGUI::WidgetPtr _widget, int, int, MyGUI::MouseButton)
{
	if (_widget->getName() == "RepayUp")
		mBD.mAmountRepayUp = true;
	else if (_widget->getName() == "RepayDown")
		mBD.mAmountRepayDown = true;
	else if (_widget->getName() == "AmountUp")
		mBD.mAmountTakeoutUp = true;
	else if (_widget->getName() == "AmountDown")
		mBD.mAmountTakeoutDown = true;
	else if (_widget->getName() == "YearsUp")
		mBD.mLoanYearsUp = true;
	else if (_widget->getName() == "YearsDown")
		mBD.mLoanYearsDown = true;
	else if (_widget->getName() == "PriceUp")
		mBD.mPriceUp = true;
	else if (_widget->getName() == "PriceDown")
		mBD.mPriceDown = true;
	else if (_widget->getName() == "MaintenanceDown")
		mBD.mMaintenanceDown = true;
	else if (_widget->getName() == "MaintenanceUp")
		mBD.mMaintenanceUp = true;
	else if (_widget->getName() == "AdvertisingUp")
		mBD.mAdsUp = true;
	else if (_widget->getName() == "AdvertisingDown")
		mBD.mAdsDown = true;
	else if (_widget->getName() == "EnvSupportUp")
		mBD.mEnvSupportUp = true;
	else if (_widget->getName() == "EnvSupportDown")
		mBD.mEnvSupportDown = true;
	else if (_widget->getName() == "SupplyMaintUp")
		mBD.mMaintenanceSupplyUp = true;
	else if (_widget->getName() == "SupplyMaintDown")
		mBD.mMaintenanceSupplyDown = true;
	else if (_widget->getName() == "PPMaintUp")
		mBD.mPPMaintUp = true;
	else if (_widget->getName() == "PPMaintDown")
		mBD.mPPMaintDown = true;
	else if (_widget->getName() == "REMaintUp")
		mBD.mREMaintUp = true;
	else if (_widget->getName() == "REMaintDown")
		mBD.mREMaintDown = true;
	else if (_widget->getName() == "CityPriceUp")
		mBD.mCityPriceUp = true;
	else if (_widget->getName() == "CityPriceDown")
		mBD.mCityPriceDown = true;
	else if (_widget->getName() == "CityAdsUp")
		mBD.mCityAdsUp = true;
	else if (_widget->getName() == "CityAdsDown")
		mBD.mCityAdsDown = true;
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::buttonDownReleased(MyGUI::WidgetPtr _widget, int, int, MyGUI::MouseButton)
{
	mButtonDownTime = 0.0;

	if (_widget->getName() == "RepayUp")
		mBD.mAmountRepayUp = false;
	else if (_widget->getName() == "RepayDown")
		mBD.mAmountRepayDown = false;
	else if (_widget->getName() == "AmountUp")
		mBD.mAmountTakeoutUp = false;
	else if (_widget->getName() == "AmountDown")
		mBD.mAmountTakeoutDown = false;
	else if (_widget->getName() == "YearsUp")
		mBD.mLoanYearsUp = false;
	else if (_widget->getName() == "YearsDown")
		mBD.mLoanYearsDown = false;
	else if (_widget->getName() == "PriceUp")
		mBD.mPriceUp = false;
	else if (_widget->getName() == "PriceDown")
		mBD.mPriceDown = false;
	else if (_widget->getName() == "MaintenanceDown")
		mBD.mMaintenanceDown = false;
	else if (_widget->getName() == "MaintenanceUp")
		mBD.mMaintenanceUp = false;
	else if (_widget->getName() == "AdvertisingUp")
		mBD.mAdsUp = false;
	else if (_widget->getName() == "AdvertisingDown")
		mBD.mAdsDown = false;
	else if (_widget->getName() == "EnvSupportUp")
		mBD.mEnvSupportUp = false;
	else if (_widget->getName() == "EnvSupportDown")
		mBD.mEnvSupportDown = false;
	else if (_widget->getName() == "SupplyMaintUp")
		mBD.mMaintenanceSupplyUp = false;
	else if (_widget->getName() == "SupplyMaintDown")
		mBD.mMaintenanceSupplyDown = false;
	else if (_widget->getName() == "PPMaintUp")
		mBD.mPPMaintUp = false;
	else if (_widget->getName() == "PPMaintDown")
		mBD.mPPMaintDown = false;
	else if (_widget->getName() == "REMaintUp")
		mBD.mREMaintUp = false;
	else if (_widget->getName() == "REMaintDown")
		mBD.mREMaintDown = false;
	else if (_widget->getName() == "CityPriceUp")
		mBD.mCityPriceUp = false;
	else if (_widget->getName() == "CityPriceDown")
		mBD.mCityPriceDown = false;
	else if (_widget->getName() == "CityAdsUp")
		mBD.mCityAdsUp = false;
	else if (_widget->getName() == "CityAdsDown")
		mBD.mCityAdsDown = false;
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::updateButtonsDown(float pElapsedTime)
{
	bool lChange = false;

	if (mBD.mAmountRepayUp||mBD.mAmountRepayDown||mBD.mAmountTakeoutUp||mBD.mAmountTakeoutDown||
		mBD.mLoanYearsUp||mBD.mLoanYearsDown||mBD.mPriceUp||mBD.mPriceDown||mBD.mMaintenanceDown||
		mBD.mMaintenanceUp||mBD.mAdsUp||mBD.mAdsDown||mBD.mMaintenanceSupplyUp||
    mBD.mMaintenanceSupplyDown||mBD.mEnvSupportUp||mBD.mEnvSupportDown||mBD.mPPMaintUp||
    mBD.mPPMaintDown||mBD.mCityPriceUp||mBD.mCityPriceDown||mBD.mCityAdsUp||mBD.mCityAdsDown||
    mBD.mREMaintUp||mBD.mREMaintDown) {
		mButtonDownTime += pElapsedTime;
	}

	if (mButtonDownTime > 0.11) {
		mButtonDownTime = 0;
		lChange = true;
	}

	if (mBD.mPriceUp && lChange) {
		if (mCompany->getPrice() < 100)
			mCompany->setPrice(mCompany->getPrice()+1);
		updateCompanyData(0);
	}
	else if (mBD.mPriceDown && lChange) {
		if (mCompany->getPrice() > 0)
			mCompany->setPrice(mCompany->getPrice()-1);
		updateCompanyData(0);
	}
	else if (mBD.mMaintenanceUp && lChange) {
		if (mCompany->getMaintenance() < 100)
			mCompany->setMaintenance(mCompany->getMaintenance()+1);
		updateCompanyData(0);
	}
	else if (mBD.mMaintenanceDown && lChange) {
		if (mCompany->getMaintenance() > 0)
			mCompany->setMaintenance(mCompany->getMaintenance()-1);
		updateCompanyData(0);
	}
	else if (mBD.mAdsUp && lChange) {
		if (mCompany->getAdvertising() < 100)
			mCompany->setAdvertising(mCompany->getAdvertising()+1);
		updateCompanyData(0);
	}
	else if (mBD.mAdsDown && lChange) {
		if (mCompany->getAdvertising() > 0)
			mCompany->setAdvertising(mCompany->getAdvertising()-1);
		updateCompanyData(0);
	}
	else if (mBD.mAmountRepayUp && lChange) {
		if (mGUI->findWidget<MyGUI::List>("LoanList")->getIndexSelected() != MyGUI::ITEM_NONE) {
			if (mRepayment <= ((mCompany->getLoans()[mGUI->findWidget<MyGUI::List>("LoanList")
        ->getIndexSelected()].mAmountLeft/1000)-100)) {
				mRepayment += 100;
				recalcRate();
			} else {
				mRepayment = mCompany->getLoans()[mGUI->findWidget<MyGUI::List>("LoanList")
          ->getIndexSelected()].mAmountLeft/1000;
				recalcRate();
			}
		}

		mGUI->findWidget<MyGUI::Edit>("RepayAmount")->setCaption(toString(mRepayment));
	}
	else if (mBD.mAmountRepayDown && lChange) {
		if (mGUI->findWidget<MyGUI::List>("LoanList")->getIndexSelected() != MyGUI::ITEM_NONE) {
			if (mRepayment > 99) {
				mRepayment -= 100;
				recalcRate();
			} else {
				mRepayment = 0;
				recalcRate();
			}
		}

		mGUI->findWidget<MyGUI::Edit>("RepayAmount")->setCaption(toString(mRepayment));
	}
	else if (mBD.mAmountTakeoutUp && lChange) {
		if (mAmount < 5000) {
			mAmount += 100;

			mGUI->findWidget<MyGUI::Edit>("TakeoutAmount")->setCaption(toString(mAmount));
			recalcCreditData();
		}
	}
	else if (mBD.mAmountTakeoutDown && lChange) {
		if (mAmount > 100) {
			mAmount -= 100;

			mGUI->findWidget<MyGUI::Edit>("TakeoutAmount")->setCaption(toString(mAmount));
			recalcCreditData();
		}
	}
	else if (mBD.mLoanYearsUp && lChange) {
		if (mYears < 30) {
			mYears += 1;

			mGUI->findWidget<MyGUI::Edit>("Years")->setCaption(toString(mYears));
			recalcCreditData();
		}
	}
	else if (mBD.mLoanYearsDown && lChange) {
		if (mYears > 1) {
			mYears -= 1;

			mGUI->findWidget<MyGUI::Edit>("Years")->setCaption(toString(mYears));
			recalcCreditData();
		}
	}
	else if (mBD.mEnvSupportUp && lChange) {
		mCompany->setEnvSupport(mCompany->getEnvSupport()+1);
		updateCompanyData(0);
	}
	else if (mBD.mEnvSupportDown && lChange) {
		if(mCompany->getEnvSupport() > 0)
			mCompany->setEnvSupport(mCompany->getEnvSupport()-1);
		updateCompanyData(0);
	}
	else if (mBD.mMaintenanceSupplyUp && lChange) {
		if(mCompany->getSupplyMaintenance() < 100)
			mCompany->setSupplyMaintenance(mCompany->getSupplyMaintenance()+1);
		updateCompanyData(0);
	}
	else if (mBD.mMaintenanceSupplyDown && lChange) {
		if(mCompany->getSupplyMaintenance() > 0)
			mCompany->setSupplyMaintenance(mCompany->getSupplyMaintenance()-1);
		updateCompanyData(0);
	}
	else if (mBD.mCityAdsUp && lChange) {
		mCityInfo->setAdvertising(mCityInfo->getAdvertising()+1);
		showCityInfo(mCityInfo);
	}
	else if (mBD.mCityAdsDown && lChange) {
		if (mCityInfo->getAdvertising() > 0)
			mCityInfo->setAdvertising(mCityInfo->getAdvertising()-1);
		showCityInfo(mCityInfo);
	}
	else if (mBD.mCityPriceUp && lChange) {
		if (mCityInfo->getPrice() < 100)
			mCityInfo->setPrice(mCityInfo->getPrice()+1);
		showCityInfo(mCityInfo);
	}
	else if (mBD.mCityPriceDown && lChange) {
		if (mCityInfo->getPrice() > 0)
			mCityInfo->setPrice(mCityInfo->getPrice()-1);
		showCityInfo(mCityInfo);
	}
	else if (mBD.mPPMaintUp && lChange) {
		if (mPowerplantInfo->getMaintenance() < 100)
			mPowerplantInfo->setMaintenance(mPowerplantInfo->getMaintenance()+1);
		showPowerPlantInfo(mPowerplantInfo);
	}
	else if (mBD.mPPMaintDown && lChange) {
		if (mPowerplantInfo->getMaintenance() > 0)
			mPowerplantInfo->setMaintenance(mPowerplantInfo->getMaintenance()-1);
		showPowerPlantInfo(mPowerplantInfo);
	}
	else if (mBD.mREMaintUp && lChange) {
		if (mResourceInfo->getMaintenance() < 100)
			mResourceInfo->setMaintenance(mResourceInfo->getMaintenance()+1);
		showResourceInfo(mResourceInfo);
	}
	else if (mBD.mREMaintDown && lChange) {
		if (mResourceInfo->getMaintenance() > 0)
			mResourceInfo->setMaintenance(mResourceInfo->getMaintenance()-1);
		showResourceInfo(mResourceInfo);
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::recalcCreditData(void)
{
	mInterestRate = (mYears*0.4) + ((float)(rand() % 10) / 10.0);
	mMonthlyRate = (mAmount + (mAmount * (mInterestRate/10.0))) / (mYears*12.0);

	mGUI->findWidget<MyGUI::StaticText>("LoanInfo")->setCaption(StrLoc::get()->InterestRate()
    + toString(mInterestRate) + "%\n" + StrLoc::get()->MonthlyCharge()
		+ toString(mMonthlyRate) + L"k€");
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::recalcRate(void)
{
	if (mGUI->findWidget<MyGUI::List>("LoanList")->getIndexSelected() != MyGUI::ITEM_NONE) {
		Loan lSelectedLoan = mCompany->getLoans()[mGUI->findWidget<MyGUI::List>("LoanList"
      )->getIndexSelected()];
		mNewMonthlyRate = ((lSelectedLoan.mAmountLeft-(mRepayment*1000))
      /lSelectedLoan.mMonthLeft)/1000;
		mGUI->findWidget<MyGUI::StaticText>("NewCharge")->setCaption(StrLoc::get()->NewMonthlyCharge() + toString(mNewMonthlyRate) + L"k€");
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::updateLoans(EventData* pData)
{
	if (mLoading)
		return;

	mGUI->findWidget<MyGUI::StaticText>("NoLoans")->setVisible(true);

	int lLoanSelected = mGUI->findWidget<MyGUI::List>("LoanList")->getIndexSelected();
	int lLoanScroll = mGUI->findWidget<MyGUI::List>("LoanList")->getScrollPosition();

	std::vector<Loan> lLoans = mCompany->getLoans();

	mGUI->findWidget<MyGUI::List>("LoanList")->removeAllItems();

	for (size_t i = 0; i < lLoans.size(); i++) {
		mGUI->findWidget<MyGUI::StaticText>("NoLoans")->setVisible(false);
		mGUI->findWidget<MyGUI::List>("LoanList")->addItem(lLoans[i].formatted());
	}

	if ((lLoanSelected != MyGUI::ITEM_NONE) 
    && ((int)mGUI->findWidget<MyGUI::List>("LoanList")->getItemCount() > lLoanSelected)) {
		mGUI->findWidget<MyGUI::List>("LoanList")->setIndexSelected(lLoanSelected);
		mGUI->findWidget<MyGUI::List>("LoanList")->setScrollPosition(lLoanScroll);

		if (mRepayment > mCompany->getLoans()[lLoanSelected].mAmountLeft/1000) {
			mRepayment = mCompany->getLoans()[lLoanSelected].mAmountLeft/1000;
			mGUI->findWidget<MyGUI::Edit>("RepayAmount")->setCaption(toString(mRepayment));
			recalcRate();
		}
	} else {
		mRepayment = 0;
	}

	mGUI->findWidget<MyGUI::Edit>("RepayAmount")->setCaption(toString(mRepayment));
	mGUI->findWidget<MyGUI::Edit>("Years")->setCaption(toString(mYears));
	mGUI->findWidget<MyGUI::Edit>("TakeoutAmount")->setCaption(toString(mAmount));

	recalcCreditData();
	recalcRate();
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::updateInfoPanels(std::vector<InfoPanel> pPanels)
{
	while (mInfoPanelWidget->getChildCount() > 0)
		mGUI->destroyWidget(mInfoPanelWidget->getChildAt(mInfoPanelWidget->getChildCount()-1));

	if (mInfoPanelWidget)
		mGUI->destroyChildWidget(mInfoPanelWidget);

	mInfoPanelWidget = mGUI->createWidget<MyGUI::Widget>("Default",
		0, 0, mGUI->getRenderWindow()->getWidth(), mGUI->getRenderWindow()->getHeight(),
    MyGUI::Align::Default, "Back");
	mInfoPanelWidget->setNeedMouseFocus(false);

	for (size_t i = 0; i < pPanels.size(); i++) {
		if (pPanels[i].mType == eCity) {
			MyGUI::StaticImagePtr lPanel = mInfoPanelWidget
        ->createWidget<MyGUI::StaticImage>("StaticImage",
				pPanels[i].mPosition.x, pPanels[i].mPosition.y, 256, 64, MyGUI::Align::Default,
        "infopanels" + toString(i));
			lPanel->setNeedMouseFocus(false);
			lPanel->setImageTexture("info_small_net.png");

			MyGUI::StaticTextPtr lText = lPanel->createWidget<MyGUI::StaticText>("StaticText",
				47, 8, 137, 19, MyGUI::Align::Default,  "infopanelstext" + toString(i));
			lText->setNeedMouseFocus(false);
			lText->setCaption(pPanels[i].mName);
			lText->setTextAlign(MyGUI::Align::Center);
			lText->setTextColour(Ogre::ColourValue(0.2,0.2,0.3));

			MyGUI::StaticImagePtr lIcon = lPanel->createWidget<MyGUI::StaticImage>("StaticImage",
				8, 8, 32, 32, MyGUI::Align::Default,  "infopaneltypeicon" + toString(i));
			lIcon->setNeedMouseFocus(false);
			lIcon->setImageTexture("city.png");

			if (pPanels[i].mConnected) {
				MyGUI::StaticImagePtr lIconCon = lPanel->createWidget<MyGUI::StaticImage>("StaticImage",
					28, 6, 32, 32, MyGUI::Align::Default,  "infopanelconnectionicon" + toString(i));
				lIconCon->setNeedMouseFocus(false);
				lIconCon->setImageTexture("connected.png");
			} else {
				MyGUI::StaticImagePtr lIconCon = lPanel->createWidget<MyGUI::StaticImage>("StaticImage",
					28, 6, 32, 32, MyGUI::Align::Default,  "infopanelconnectionicon" + toString(i));
				lIconCon->setNeedMouseFocus(false);
				lIconCon->setImageTexture("not_connected.png");
			}
		}
		else if(pPanels[i].mType == ePowerplant) {
			MyGUI::StaticImagePtr lPanel = mInfoPanelWidget
        ->createWidget<MyGUI::StaticImage>("StaticImage",
				pPanels[i].mPosition.x, pPanels[i].mPosition.y, 256, 64, MyGUI::Align::Default,
        "infopanels" + toString(i));
			lPanel->setNeedMouseFocus(false);
			lPanel->setImageTexture("info_small_pp.png");

			MyGUI::StaticTextPtr lText = lPanel->createWidget<MyGUI::StaticText>("StaticText",
				28, 8, 137, 19, MyGUI::Align::Default,  "infopanelstext" + toString(i));
			lText->setNeedMouseFocus(false);
			lText->setCaption(pPanels[i].mName);
			lText->setTextAlign(MyGUI::Align::Center);
			lText->setTextColour(Ogre::ColourValue(0.2,0.2,0.3));

			MyGUI::StaticImagePtr lIcon = lPanel->createWidget<MyGUI::StaticImage>("StaticImage",
				8, 8, 32, 32, MyGUI::Align::Default,  "infopaneltypeicon" + toString(i));
			lIcon->setNeedMouseFocus(false);
			lIcon->setImageTexture("pp.png");
		}
		else if(pPanels[i].mType == eCompany) {
			MyGUI::StaticImagePtr lPanel = mInfoPanelWidget
        ->createWidget<MyGUI::StaticImage>("StaticImage",
				pPanels[i].mPosition.x, pPanels[i].mPosition.y, 256, 64, MyGUI::Align::Default,
        "infopanels" + toString(i));
			lPanel->setNeedMouseFocus(false);
			lPanel->setImageTexture("info_small_co.png");

			MyGUI::StaticTextPtr lText = lPanel->createWidget<MyGUI::StaticText>("StaticText",
				28, 8, 137, 19, MyGUI::Align::Default,  "infopanelstext" + toString(i));
			lText->setNeedMouseFocus(false);
			lText->setCaption(pPanels[i].mName);
			lText->setTextAlign(MyGUI::Align::Center);
			lText->setTextColour(Ogre::ColourValue(0.2,0.2,0.3));

			MyGUI::StaticImagePtr lIcon = lPanel->createWidget<MyGUI::StaticImage>("StaticImage",
				8, 8, 32, 32, MyGUI::Align::Default,  "infopaneltypeicon" + toString(i));
			lIcon->setNeedMouseFocus(false);
			lIcon->setImageTexture("co.png");
		}
		else if(pPanels[i].mType == eResource) {
			MyGUI::StaticImagePtr lPanel = mInfoPanelWidget
        ->createWidget<MyGUI::StaticImage>("StaticImage",
				pPanels[i].mPosition.x, pPanels[i].mPosition.y, 256, 64, MyGUI::Align::Default
        ,  "infopanels" + toString(i));
			lPanel->setNeedMouseFocus(false);
			lPanel->setImageTexture("info_small_re.png");

			MyGUI::StaticTextPtr lText = lPanel->createWidget<MyGUI::StaticText>("StaticText",
				28, 8, 137, 19, MyGUI::Align::Default,  "infopanelstext" + toString(i));
			lText->setNeedMouseFocus(false);
			lText->setCaption(pPanels[i].mName);
			lText->setTextAlign(MyGUI::Align::Center);
			lText->setTextColour(Ogre::ColourValue(0.2,0.2,0.3));

			MyGUI::StaticImagePtr lIcon = lPanel->createWidget<MyGUI::StaticImage>("StaticImage",
				8, 8, 32, 32, MyGUI::Align::Default,  "infopaneltypeicon" + toString(i));
			lIcon->setNeedMouseFocus(false);
			lIcon->setImageTexture("pp.png");
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::updateResearchableButtons(EventData* pData)
{
	ResearchSet lResearchSet = mCompany->getResearchSet();

	if (lResearchSet.mResearched[0])
		mGUI->findWidget<MyGUI::Button>(toString(ePPCoalLarge))->setEnabled(true);
	if (lResearchSet.mResearched[1])
		mGUI->findWidget<MyGUI::Button>(toString(ePPWindLarge))->setEnabled(true);
	if (lResearchSet.mResearched[3])
		mGUI->findWidget<MyGUI::Button>(toString(ePPBio))->setEnabled(true);
	if (lResearchSet.mResearched[6])
		mGUI->findWidget<MyGUI::Button>(toString(ePPWindOffshore))->setEnabled(true);
	if (lResearchSet.mResearched[8])
		mGUI->findWidget<MyGUI::Button>(toString(ePPNuclearSmall))->setEnabled(true);
	if (lResearchSet.mResearched[9])
		mGUI->findWidget<MyGUI::Button>(toString(ePPGasLarge))->setEnabled(true);
	if (lResearchSet.mResearched[10])
		mGUI->findWidget<MyGUI::Button>(toString(eRECoalLarge))->setEnabled(true);
	if (lResearchSet.mResearched[12])
		mGUI->findWidget<MyGUI::Button>(toString(eREGasLarge))->setEnabled(true);
	if (lResearchSet.mResearched[15])
		mGUI->findWidget<MyGUI::Button>(toString(ePPSolarLarge))->setEnabled(true);
	if (lResearchSet.mResearched[16])
		mGUI->findWidget<MyGUI::Button>(toString(ePPSolarUpdraft))->setEnabled(true);
	if (lResearchSet.mResearched[18])
		mGUI->findWidget<MyGUI::Button>(toString(ePPNuclearLarge))->setEnabled(true);
	if (lResearchSet.mResearched[19])
		mGUI->findWidget<MyGUI::Button>(toString(eREUraniumLarge))->setEnabled(true);
	if (lResearchSet.mResearched[20])
		mGUI->findWidget<MyGUI::Button>(toString(ePPNuclearFusion))->setEnabled(true);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::updateCOBuildingButtons(EventData* pData)
{
	if (mCompany->isHeadquartersBuilt()) {
		mGUI->findWidget<MyGUI::Button>("Sp4")->setEnabled(true);
		mGUI->findWidget<MyGUI::Button>("Sp5")->setEnabled(true);
		mGUI->findWidget<MyGUI::Button>("Sp6")->setEnabled(true);
	} else {
		mGUI->findWidget<MyGUI::Button>("Sp4")->setEnabled(false);
		mGUI->findWidget<MyGUI::Button>("Sp5")->setEnabled(false);
		mGUI->findWidget<MyGUI::Button>("Sp6")->setEnabled(false);
	}

	for (int i = 0; i < 21; ++i) {
		if (mCompany->isAvailableInTree(i))
			mGUI->findWidget<MyGUI::Button>("Re" + toString(i+1))->setEnabled(true);
		else
			mGUI->findWidget<MyGUI::Button>("Re" + toString(i+1))->setEnabled(false);
	}

	if(mCompany->isPRBuilt()) {
		mGUI->findWidget<MyGUI::Button>("Ad3")->setEnabled(true);
		mGUI->findWidget<MyGUI::Button>("Ad4")->setEnabled(true);
	} else {
		mGUI->findWidget<MyGUI::Button>("Ad3")->setEnabled(false);
		mGUI->findWidget<MyGUI::Button>("Ad4")->setEnabled(false);
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::showNewspaper(EventData* pData)
{
	mGUI->findWidget<MyGUI::Widget>("Newspaper")->setPosition(
		(mGUI->getRenderWindow()->getWidth()/2)
    -(mGUI->findWidget<MyGUI::Widget>("Newspaper")->getWidth()/2),
		(mGUI->getRenderWindow()->getHeight()/2)
    -(mGUI->findWidget<MyGUI::Widget>("Newspaper")->getHeight()/2));
	mGUI->findWidget<MyGUI::Widget>("Newspaper")->setVisible(true);

	mGUI->findWidget<MyGUI::StaticImage>("NewspaperPhoto")
    ->setImageTexture(static_cast<EventArg<std::string>*>(pData)->mData1);
	mGUI->findWidget<MyGUI::StaticText>("Headline")
    ->setCaption(static_cast<EventArg<std::string>*>(pData)->mData2);
	mGUI->findWidget<MyGUI::StaticText>("Teaser")
    ->setCaption(static_cast<EventArg<std::string>*>(pData)->mData3);

	mGUI->findWidget<MyGUI::Button>("CloseNewspaper")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::closeNewspaperPressed);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::closeNewspaperPressed(MyGUI::WidgetPtr _widget)
{
	mGUI->findWidget<MyGUI::Widget>("Newspaper")->setVisible(false);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::finishLoading(void)
{
	EventHandler::raiseEvent(eGamestateChangeView);

	MyGUI::LayoutManager::getInstance().unloadLayout(mCurrentLayout);
	mCurrentLayout = MyGUI::LayoutManager::getInstance().load("game.layout");

	setupGamePanels();

	MyGUI::PointerManager::getInstance().setVisible(true);

	setGameRunning(true);

	mLoading = false;
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::setLoadingProgress(EventData *pData)
{
	mGUI->findWidget<MyGUI::Widget>("LoadingWidget")->setPosition(
	(mGUI->getRenderWindow()->getWidth()/2)
  -(mGUI->findWidget<MyGUI::Widget>("LoadingWidget")->getWidth()/2),
	(mGUI->getRenderWindow()->getHeight()/2)
  -(mGUI->findWidget<MyGUI::Widget>("LoadingWidget")->getHeight()/2));

	mGUI->findWidget<MyGUI::Progress>("LoadingProgress")
    ->setProgressPosition(static_cast<EventArg<int>*>(pData)->mData1);
		
	mRoot->renderOneFrame();
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::setLoadingStatus(EventData *pData)
{
	mGUI->findWidget<MyGUI::StaticText>("LoadingStatus")->setCaption(StrLoc::get()->Loading() 
    + " (" + static_cast<EventArg<std::string>*>(pData)->mData1 + ")");
	mRoot->renderOneFrame();
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::showBankruptcyWarning(EventData* pData)
{
	mGUI->findWidget<MyGUI::Window>("BankruptcyImminentWindow")->setVisible(true);
	mGUI->findWidget<MyGUI::Window>("BankruptcyImminentWindow")->setPosition(
		(mGUI->getRenderWindow()->getWidth()/2)
    -(mGUI->findWidget<MyGUI::Window>("BankruptcyImminentWindow")->getWidth()/2),
		(mGUI->getRenderWindow()->getHeight()/2)
    -(mGUI->findWidget<MyGUI::Window>("BankruptcyImminentWindow")->getHeight()/2));

	mGUI->findWidget<MyGUI::Window>("BankruptcyImminentWindow")->setShadow(true);

	MyGUI::LayerManager::getInstance().upLayerItem(mGUI
    ->findWidget<MyGUI::Window>("BankruptcyImminentWindow"));

	mGUI->findWidget<MyGUI::Window>("BankruptcyImminentWindow")
    ->setCaption(StrLoc::get()->BankruptcyImminentCaption());
	mGUI->findWidget<MyGUI::StaticText>("BankruptcyImminentText")
    ->setCaption(StrLoc::get()->BankruptcyImminentText());

	mGUI->findWidget<MyGUI::Button>("BIOK")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::missionButtonPressed);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::showBankruptcy(EventData* pData)
{

	mGUI->findWidget<MyGUI::Window>("BankruptcyWindow")->setVisible(true);
	mGUI->findWidget<MyGUI::Window>("BankruptcyWindow")->setPosition(
		(mGUI->getRenderWindow()->getWidth()/2)
    -(mGUI->findWidget<MyGUI::Window>("BankruptcyWindow")->getWidth()/2),
		(mGUI->getRenderWindow()->getHeight()/2)
    -(mGUI->findWidget<MyGUI::Window>("BankruptcyWindow")->getHeight()/2));

	mGUI->findWidget<MyGUI::Window>("BankruptcyWindow")->setShadow(true);

	MyGUI::LayerManager::getInstance().upLayerItem(mGUI
    ->findWidget<MyGUI::Window>("BankruptcyWindow"));

	mGUI->findWidget<MyGUI::Window>("BankruptcyWindow")
    ->setCaption(StrLoc::get()->BankruptcyCaption());
	mGUI->findWidget<MyGUI::Button>("QuitToMenuB")->setCaption(StrLoc::get()->ToMenu());
	mGUI->findWidget<MyGUI::StaticText>("BankruptcyText")
    ->setCaption(StrLoc::get()->BankruptcyText());

	mGUI->findWidget<MyGUI::Button>("QuitToMenuB")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::missionButtonPressed);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::showMissionDone(EventData* pData)
{
	mGUI->findWidget<MyGUI::Window>("MissionDoneWindow")->setVisible(true);
	mGUI->findWidget<MyGUI::Window>("MissionDoneWindow")->setPosition(
		(mGUI->getRenderWindow()->getWidth()/2)-
    (mGUI->findWidget<MyGUI::Window>("MissionDoneWindow")->getWidth()/2),
		(mGUI->getRenderWindow()->getHeight()/2)-
    (mGUI->findWidget<MyGUI::Window>("MissionDoneWindow")->getHeight()/2));

	mGUI->findWidget<MyGUI::Window>("MissionDoneWindow")->setShadow(true);

	MyGUI::LayerManager::getInstance().upLayerItem(mGUI
    ->findWidget<MyGUI::Window>("MissionDoneWindow"));

	mGUI->findWidget<MyGUI::Window>("MissionDoneWindow")
    ->setCaption(StrLoc::get()->MissionDoneCaption());
	mGUI->findWidget<MyGUI::Button>("KeepPlaying")->setCaption(StrLoc::get()->KeepPlaying());
	mGUI->findWidget<MyGUI::Button>("QuitToMenu")->setCaption(StrLoc::get()->ToMenu());
	mGUI->findWidget<MyGUI::StaticText>("MissionDoneText")
    ->setCaption(StrLoc::get()->MissionDoneText());

	mGUI->findWidget<MyGUI::Button>("KeepPlaying")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::missionButtonPressed);
	mGUI->findWidget<MyGUI::Button>("QuitToMenu")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::missionButtonPressed);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::showDemoTimeUp(EventData* pData)
{
  mShowDemoTime = false;
  updateDemoTime(0);

  showTooltip(false, std::wstring());

	mGUI->findWidget<MyGUI::Window>("MissionDoneWindow")->setVisible(false);
	mGUI->findWidget<MyGUI::Window>("BankruptcyImminentWindow")->setVisible(false);
	mGUI->findWidget<MyGUI::Window>("BankruptcyWindow")->setVisible(false);
	mGUI->findWidget<MyGUI::Window>("CEOWindow")->setVisible(false);
	mGUI->findWidget<MyGUI::Window>("ResearchWindow")->setVisible(false);
	mGUI->findWidget<MyGUI::Window>("MissionWindow")->setVisible(false);
	mGUI->findWidget<MyGUI::Window>("FinancesWindow")->setVisible(false);
	mGUI->findWidget<MyGUI::Window>("PowerNetsWindow")->setVisible(false);
	mGUI->findWidget<MyGUI::Window>("TradeWindow")->setVisible(false);
	mGUI->findWidget<MyGUI::Window>("TickerArchiveWindow")->setVisible(false);
	mGUI->findWidget<MyGUI::Widget>("Newspaper")->setVisible(false);
	mGUI->findWidget<MyGUI::Window>("TutorialWindow")->setVisible(false);
	mGUI->findWidget<MyGUI::Window>("CompanyNameWindow")->setVisible(false);

	mGUI->findWidget<MyGUI::StaticImage>("BackgroundDemo")
    ->setVisible(true);
	mGUI->findWidget<MyGUI::StaticImage>("BackgroundDemo")
    ->setSize(mGUI->getViewWidth(), mGUI->getViewHeight());
	mGUI->findWidget<MyGUI::StaticImage>("BackgroundDemo")
    ->setImageTexture(cBackgrounds[getCurrentAspectRatio()]);

  unsigned long lStartTime = mRoot->getTimer()->getMilliseconds();
  unsigned long lPassedTime = 0;
  while (lPassedTime < 150) {
    mGUI->findWidget<MyGUI::StaticImage>("BackgroundDemo")->setAlpha((float)lPassedTime/150.0);
    mRoot->renderOneFrame();
    lPassedTime = mRoot->getTimer()->getMilliseconds() - lStartTime;
  }
  mGUI->findWidget<MyGUI::StaticImage>("BackgroundDemo")->setAlpha(1.0);

  MyGUI::WindowPtr lWindow = mGUI->findWidget<MyGUI::Window>("DemoTimeUpWindow");

	lWindow->setVisible(true);
	lWindow->setPosition(
		(mGUI->getRenderWindow()->getWidth()/2) - (lWindow->getWidth()/2),
		(mGUI->getRenderWindow()->getHeight()/2) - (lWindow->getHeight()/2));

  lWindow->setShadow(true);

	MyGUI::LayerManager::getInstance().upLayerItem(lWindow);

	lWindow->setCaption(StrLoc::get()->DemoTimeUpCaption());
	mGUI->findWidget<MyGUI::Button>("Website")->setCaption("www.energietycoon.de");
	mGUI->findWidget<MyGUI::Button>("QuitDemo")->setCaption(StrLoc::get()->MainMenuClose());
	mGUI->findWidget<MyGUI::StaticText>("DemoTimeText")->setCaption(StrLoc::get()->DemoTimeUpText());

	mGUI->findWidget<MyGUI::Button>("Website")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::demoWebsitePressed);
	mGUI->findWidget<MyGUI::Button>("QuitDemo")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::demoQuitPressed);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::demoQuitPressed(MyGUI::WidgetPtr _widget)
{
  exit(0);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::demoWebsitePressed(MyGUI::WidgetPtr _widget)
{
  //ShellExecute(0, "open", "http://www.energietycoon.de", 0, 0, SW_SHOWNORMAL);
  exit(0);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::missionButtonPressed(MyGUI::WidgetPtr _widget)
{
	if (_widget->getName() == "BIOK") {
		mGUI->findWidget<MyGUI::Window>("BankruptcyImminentWindow")->setVisible(false);
	}
	else if ((_widget->getName() == "QuitToMenuB") || (_widget->getName() == "QuitToMenu")) {
		EventHandler::raiseEvent(eShutdownGame);
		Audio::setMenu();
	}
	else if (_widget->getName() == "KeepPlaying") {
		mGUI->findWidget<MyGUI::Window>("MissionDoneWindow")->setVisible(false);
		mCompany->setKeepPlaying();
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::updateSpecialActions(EventData* pData)
{
	mGUI->findWidget<MyGUI::StaticImage>("Sp1Img")->setVisible(false);
	mGUI->findWidget<MyGUI::StaticImage>("Sp2Img")->setVisible(false);
	mGUI->findWidget<MyGUI::StaticImage>("Sp3Img")->setVisible(false);
	mGUI->findWidget<MyGUI::StaticImage>("Sp4Img")->setVisible(false);
	mGUI->findWidget<MyGUI::StaticImage>("Sp5Img")->setVisible(false);
	mGUI->findWidget<MyGUI::StaticImage>("Sp6Img")->setVisible(false);

	if (mCompany->isSpecialActionRunning(eEmployee))
		mGUI->findWidget<MyGUI::StaticImage>("Sp1Img")->setVisible(true);

	if (mCompany->isSpecialActionRunning(eWaste))
		mGUI->findWidget<MyGUI::StaticImage>("Sp2Img")->setVisible(true);

	if (mCompany->isSpecialActionRunning(eTaxfraud))
		mGUI->findWidget<MyGUI::StaticImage>("Sp3Img")->setVisible(true);

	if (mCompany->isSpecialActionRunning(eBribe))
		mGUI->findWidget<MyGUI::StaticImage>("Sp4Img")->setVisible(true);

	if (mCompany->isSpecialActionRunning(eWar))
		mGUI->findWidget<MyGUI::StaticImage>("Sp5Img")->setVisible(true);

	if (mCompany->isSpecialActionRunning(ePrice))
		mGUI->findWidget<MyGUI::StaticImage>("Sp6Img")->setVisible(true);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::updateMissionGoals(EventData* pData)
{
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::updateResourceTrend(EventData* pData)
{
	std::vector<int> lResourceTrends = mCompany->getResourceTrends();

	if (lResourceTrends[0] == 1)
		mGUI->findWidget<MyGUI::StaticImage>("CoalTrend")->setImageTexture("resource_down.png");
	else if (lResourceTrends[0] == 0)
		mGUI->findWidget<MyGUI::StaticImage>("CoalTrend")->setImageTexture("resource_none.png");
	else
		mGUI->findWidget<MyGUI::StaticImage>("CoalTrend")->setImageTexture("resource_up.png");

	if (lResourceTrends[1] == 1)
		mGUI->findWidget<MyGUI::StaticImage>("GasTrend")->setImageTexture("resource_down.png");
	else if (lResourceTrends[1] == 0)
		mGUI->findWidget<MyGUI::StaticImage>("GasTrend")->setImageTexture("resource_none.png");
	else
		mGUI->findWidget<MyGUI::StaticImage>("GasTrend")->setImageTexture("resource_up.png");

	if (lResourceTrends[2] == 1)
		mGUI->findWidget<MyGUI::StaticImage>("UraniumTrend")->setImageTexture("resource_down.png");
	else if (lResourceTrends[2] == 0)
		mGUI->findWidget<MyGUI::StaticImage>("UraniumTrend")->setImageTexture("resource_none.png");
	else
		mGUI->findWidget<MyGUI::StaticImage>("UraniumTrend")->setImageTexture("resource_up.png");
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::setNoRunningGame(void)
{
	mAskForRestart = false;
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::resetSpeedIndicator(EventData* pData)
{
	int lSpeed = static_cast<EventArg<int>*>(pData)->mData1;

	if (lSpeed == 0)
		mGUI->findWidget<MyGUI::StaticImage>("SpeedIndicator")->setImageTexture("pause.png");
	else if (lSpeed == 1)
		mGUI->findWidget<MyGUI::StaticImage>("SpeedIndicator")->setImageTexture("slow.png");
	else if (lSpeed == 2)
		mGUI->findWidget<MyGUI::StaticImage>("SpeedIndicator")->setImageTexture("normal.png");
	else if (lSpeed == 3)
		mGUI->findWidget<MyGUI::StaticImage>("SpeedIndicator")->setImageTexture("fast.png");
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::repairButtonPressed(MyGUI::WidgetPtr _widget)
{
	if (_widget->getName() == "RERepair") {
		if (mCompany->getMoney() >= (mResourceInfo->getOperatingCosts()*10)) {
			MyGUI::MessagePtr lQuestionExit = MyGUI::Message::createMessageBox("Message",
        StrLoc::get()->GameTitle(), StrLoc::get()->RepairA()
				+ mResourceInfo->getName() + StrLoc::get()->RepairB() 
        + toString(mResourceInfo->getOperatingCosts()/100) + L"k€?" ,
				MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No 
        | MyGUI::MessageBoxStyle::IconQuest);
			lQuestionExit->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::repairQuestion);
			mRepairCost = mResourceInfo->getOperatingCosts()*10;
			mRepairPP = false;
		} else {
			MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
        StrLoc::get()->NoMoney());
		}
	} else {
		if (mCompany->getMoney() >= (mPowerplantInfo->getOperatingCosts()*10)) {
			MyGUI::MessagePtr lQuestionExit = MyGUI::Message::createMessageBox("Message",
        StrLoc::get()->GameTitle(), StrLoc::get()->RepairA()
				+ mPowerplantInfo->getName() + StrLoc::get()->RepairB() 
        + toString(mPowerplantInfo->getOperatingCosts()/100) + L"k€?" ,
				MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No 
        | MyGUI::MessageBoxStyle::IconQuest);
			lQuestionExit->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::repairQuestion);
			mRepairCost = mPowerplantInfo->getOperatingCosts()*10;
			mRepairPP = true;
		} else {
			MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
        StrLoc::get()->NoMoney());
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::repairQuestion(MyGUI::MessagePtr _sender, MyGUI::MessageBoxStyle _style)
{
	if (_style == MyGUI::MessageBoxStyle::Yes) {
		mCompany->setMoney(mCompany->getMoney() -  mRepairCost);

		int lRepairTime = (rand() % 4) + 1;

		MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
      StrLoc::get()->Repairs1() +  toString(lRepairTime) + StrLoc::get()->Repairs2());

		if (mRepairPP)
			mPowerplantInfo->repair(lRepairTime);
		else
			mResourceInfo->repair(lRepairTime);
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::popMessage(std::string pMessage, double pDuration, int pX, int pY)
{
	PopMessage lNewMessage;

	lNewMessage.mYOffset = 0.0;
	lNewMessage.mOriginalY = pY;
	lNewMessage.mDurationLeft = pDuration;
	lNewMessage.mWidget =  mGUI->createWidget<MyGUI::StaticText>("StaticText",
		pX, pY, 600, 600, MyGUI::Align::Default, "Statistic");
	lNewMessage.mWidget->setNeedMouseFocus(false);
	lNewMessage.mWidget->setCaption(pMessage);
	lNewMessage.mWidget->setFontHeight(23);
	mPopMessages.push_back(lNewMessage);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::updatePopMessages(double pElapsedTime)
{
	std::vector<PopMessage>::iterator it = mPopMessages.begin();

	while (it != mPopMessages.end()) {
		it->mDurationLeft -= pElapsedTime;

		if (it->mDurationLeft < 0.0) {
			mGUI->destroyChildWidget(it->mWidget);
			it = mPopMessages.erase(it);
		} else {
			if(it->mDurationLeft < 0.5)
				it->mWidget->setAlpha(it->mDurationLeft*2.0);

			it->mYOffset += pElapsedTime*70.0;
			it->mWidget->setPosition(it->mWidget->getPosition().left, it->mOriginalY - it->mYOffset);
			it++;
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::setDemo(bool pEnabled)
{
  mDemo = pEnabled;
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::updateDemoTime(float pDemoTime)
{
  if(!mGUI->findWidget<MyGUI::StaticText>("DemoTime", false))
    return;

  if(!mShowDemoTime) {
    mGUI->findWidget<MyGUI::StaticText>("DemoTime")->setVisible(false);
    return;
  }

  int lDemoSeconds = cDemoSeconds - pDemoTime;

  int lDemoMinutesLeft = lDemoSeconds / 60;
  int lDemoSecondsLeft = lDemoSeconds % 60;

  std::string lDemoMinutesString = toString<int>(lDemoMinutesLeft);
  std::string lDemoSecondsString = toString<int>(lDemoSecondsLeft);

  if(lDemoMinutesLeft < 10)
    lDemoMinutesString = "0" + lDemoMinutesString;

  if(lDemoMinutesLeft == 0)
    lDemoMinutesString = "00";

  if(lDemoSecondsLeft < 10)
    lDemoSecondsString = "0" + lDemoSecondsString;

  if(lDemoSeconds < 0) {
    mGUI->findWidget<MyGUI::StaticText>("DemoTime")->setVisible(false);
  } else {
    std::string lDemoLeftString = StrLoc::get()->DemoTime() + " " + lDemoMinutesString + ":" + lDemoSecondsString;
    mGUI->findWidget<MyGUI::StaticText>("DemoTime")->setCaption(lDemoLeftString);
  }
}

/*-----------------------------------------------------------------------------------------------*/
