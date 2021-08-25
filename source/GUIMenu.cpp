#include "StdAfx.h"
#include "GUI.h"
#include "Game.h"

/*-----------------------------------------------------------------------------------------------*/

void GUI::menuResumeGamePressed(MyGUI::WidgetPtr _widget)
{
	EventHandler::raiseEvent(eGamestateChangeView);

	MyGUI::LayoutManager::getInstance().unloadLayout(mCurrentLayout);
	mCurrentLayout = MyGUI::LayoutManager::getInstance().load("game.layout");

	setupGamePanels();
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::menuExitPressed(MyGUI::WidgetPtr _widget)
{
	MyGUI::MessagePtr lQuestionExit = MyGUI::Message::createMessageBox("Message",
		StrLoc::get()->GameTitle(), StrLoc::get()->WantToQuit(),
		MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No |
		MyGUI::MessageBoxStyle::IconQuest);
	lQuestionExit->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::questionExit);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::questionExit(MyGUI::MessagePtr _sender, MyGUI::MessageBoxStyle _style)
{
	if(_style == MyGUI::MessageBoxStyle::Yes)
		exit(0);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::menuBackToMainPressed(MyGUI::WidgetPtr _widget)
{
	switchToMainMenu();
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::menuCreditsPressed(MyGUI::WidgetPtr _widget)
{
  // fade out hack
  unsigned long lStartTime = mRoot->getTimer()->getMilliseconds();
  unsigned long lPassedTime = 0;

  while (lPassedTime < 100) {
    mGUI->findWidget<MyGUI::Widget>("MainMenuWidget")->setAlpha(1.0 - (float)lPassedTime/100.0);
    mRoot->renderOneFrame();
    lPassedTime = mRoot->getTimer()->getMilliseconds() - lStartTime;
  }

	MyGUI::LayoutManager::getInstance().unloadLayout(mCurrentLayout);

	mCurrentLayout = MyGUI::LayoutManager::getInstance().load("credits_" + StrLoc::get()->LanguageCode() + ".layout");


	mGUI->findWidget<MyGUI::Widget>("CreditsWidget")->setPosition(
		(mGUI->getViewWidth()/2)
    - (mGUI->findWidget<MyGUI::Widget>("CreditsWidget")->getWidth()/2),
		mGUI->getViewHeight()/2
    - mGUI->findWidget<MyGUI::Widget>("CreditsWidget")->getHeight()/2);

	MyGUI::StaticImagePtr lBackground = mGUI->findWidget<MyGUI::StaticImage>("Background");
	lBackground->setImageTexture(cBackgrounds[getCurrentAspectRatio()]);
	lBackground->setSize(mGUI->getViewWidth(), mGUI->getViewHeight());

	mGUI->findWidget<MyGUI::StaticText>("CreditsText")->setCaption(StrLoc::get()->CreditsText());

	mGUI->findWidget<MyGUI::Button>("Back")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::menuBackToMainPressed);

	mGUI->findWidget<MyGUI::Button>("Back")->setCaption(StrLoc::get()->MainMenuBack());

  // fade in hack
  lStartTime = mRoot->getTimer()->getMilliseconds();
  lPassedTime = 0;

  while (lPassedTime < 100) {
    mGUI->findWidget<MyGUI::Widget>("CreditsWidget")->setAlpha((float)lPassedTime/100.0);
    mRoot->renderOneFrame();
    lPassedTime = mRoot->getTimer()->getMilliseconds() - lStartTime;
  }

  mGUI->findWidget<MyGUI::Widget>("CreditsWidget")->setAlpha(1.0);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::menuOptionsPressed(MyGUI::WidgetPtr _widget)
{
  // fade out hack
  unsigned long lStartTime = mRoot->getTimer()->getMilliseconds();
  unsigned long lPassedTime = 0;

  while (lPassedTime < 100) {
    mGUI->findWidget<MyGUI::Widget>("MainMenuWidget")->setAlpha(1.0 - (float)lPassedTime/100.0);
    mRoot->renderOneFrame();
    lPassedTime = mRoot->getTimer()->getMilliseconds() - lStartTime;
  }

	MyGUI::LayoutManager::getInstance().unloadLayout(mCurrentLayout);

	mCurrentLayout = MyGUI::LayoutManager::getInstance().load("options_" + StrLoc::get()->LanguageCode() + ".layout");

	mGUI->findWidget<MyGUI::Widget>("OptionsWidget")->setPosition(
		(mGUI->getViewWidth()/2) 
    - (mGUI->findWidget<MyGUI::Widget>("OptionsWidget")->getWidth()/2),
		mGUI->getViewHeight()/2 
    - mGUI->findWidget<MyGUI::Widget>("OptionsWidget")->getHeight()/2);

	MyGUI::StaticImagePtr lBackground = mGUI->findWidget<MyGUI::StaticImage>("Background");
	lBackground->setImageTexture(cBackgrounds[getCurrentAspectRatio()]);
	lBackground->setSize(mGUI->getViewWidth(), mGUI->getViewHeight());

	mGUI->findWidget<MyGUI::Button>("Back")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::menuBackToMainPressed);

	Ogre::ConfigOptionMap lConfigOptions = mRenderSystem->getConfigOptions();

	Ogre::ConfigOptionMap::const_iterator start = lConfigOptions.begin();
	Ogre::ConfigOptionMap::const_iterator end = lConfigOptions.end();

	while (start != end) {
		std::string lOptionName = start->first;
		std::string lCurrentValue = start->second.currentValue;
		std::vector<std::string> lPossibleValues = start->second.possibleValues;

		if (lOptionName == "Full Screen") {
			mGUI->findWidget<MyGUI::Button>("FullscreenCheck")->
				setStateCheck(lCurrentValue == "Yes");
		}
		else if (lOptionName == "Colour Depth") {
			mGUI->findWidget<MyGUI::Button>("32BitRadio")->
				setStateCheck(lCurrentValue == "32");
			mGUI->findWidget<MyGUI::Button>("16BitRadio")->
				setStateCheck(lCurrentValue == "16");
		}
		else if (lOptionName == "Video Mode") {
			size_t c = 0;
			while (c < lPossibleValues.size()) {
				if (toNumber<int>(lPossibleValues.at(c).substr(
					0, lPossibleValues.at(c).find("x")-1)) > 1023) {
					mGUI->findWidget<MyGUI::ComboBox>("ResolutionCombo")->
						addItem(lPossibleValues.at(c));
				}
				c++;
			}
			mGUI->findWidget<MyGUI::ComboBox>("ResolutionCombo")->setCaption(lCurrentValue);
		}
		start++;
	}

	mGUI->findWidget<MyGUI::ComboBox>("ResolutionCombo")->setEditStatic(true);

	mGUI->findWidget<MyGUI::Button>("OKButton")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::optionsButtonPressed);

	mGUI->findWidget<MyGUI::Button>("16BitRadio")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::optionsButtonPressed);

	mGUI->findWidget<MyGUI::Button>("32BitRadio")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::optionsButtonPressed);

	mGUI->findWidget<MyGUI::Button>("HighRadio")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::optionsButtonPressed);

	mGUI->findWidget<MyGUI::Button>("NormalRadio")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::optionsButtonPressed);

	mGUI->findWidget<MyGUI::Button>("FullscreenCheck")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::optionsButtonPressed);

	mGUI->findWidget<MyGUI::Button>("FullscreenCheck")->
		setCaption(StrLoc::get()->OptionsMenuFullscreen());
	mGUI->findWidget<MyGUI::Button>("HighRadio")->
		setCaption(StrLoc::get()->OptionsMenuHigh());
	mGUI->findWidget<MyGUI::Button>("NormalRadio")->
		setCaption(StrLoc::get()->OptionsMenuNormal());
	mGUI->findWidget<MyGUI::Button>("Back")->
		setCaption(StrLoc::get()->MainMenuBack());
	mGUI->findWidget<MyGUI::StaticText>("GraphicsQualityText")->
		setCaption(StrLoc::get()->OptionsMenuGraphicsQuality());
	mGUI->findWidget<MyGUI::StaticText>("DisplayModeText")->
		setCaption(StrLoc::get()->OptionsMenuDisplayMode());

	std::ifstream lFile("config.cfg");
	std::string lBuffer;

	while(getline(lFile, lBuffer)) {
		std::stringstream lTemp;

		if(lBuffer.find("Quality=") != -1) {
			if(lBuffer.find("High") != -1) {
				mGUI->findWidget<MyGUI::Button>("HighRadio")->setStateCheck(true);
				mGUI->findWidget<MyGUI::Button>("NormalRadio")->setStateCheck(false);
				mOldGraphicsQuality = "High";
			} else {
				mGUI->findWidget<MyGUI::Button>("HighRadio")->setStateCheck(false);
				mGUI->findWidget<MyGUI::Button>("NormalRadio")->setStateCheck(true);
				mOldGraphicsQuality = "Normal";
			}
		}
	}

	lFile.close();

  // fade in hack
  lStartTime = mRoot->getTimer()->getMilliseconds();
  lPassedTime = 0;

  while (lPassedTime < 100) {
    mGUI->findWidget<MyGUI::Widget>("OptionsWidget")->setAlpha((float)lPassedTime/100.0);
    mRoot->renderOneFrame();
    lPassedTime = mRoot->getTimer()->getMilliseconds() - lStartTime;
  }

  mGUI->findWidget<MyGUI::Widget>("OptionsWidget")->setAlpha(1.0);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::optionsButtonPressed(MyGUI::WidgetPtr _widget)
{
  mOptionsChanged = false;
	if(_widget->getName() == "OKButton") {
		std::string mNewResolution = mGUI->findWidget<MyGUI::ComboBox>("ResolutionCombo")->
			getCaption();
		std::string mNewBPP = mGUI->findWidget<MyGUI::Button>("32BitRadio")->
			getStateCheck() ? "32" : "16";
		std::string mNewFullscreen = mGUI->findWidget<MyGUI::Button>("FullscreenCheck")->
			getStateCheck() ? "Yes" : "No";
		std::string mNewQuality = mGUI->findWidget<MyGUI::Button>("HighRadio")->
			getStateCheck() ? "High" : "Normal";

		Ogre::ConfigOptionMap lConfigOptions = mRenderSystem->getConfigOptions();

		if(   (lConfigOptions["Video Mode"].currentValue != mNewResolution) 
			|| (lConfigOptions["Full Screen"].currentValue != mNewFullscreen)
			|| (mNewQuality != mOldGraphicsQuality)) {
			// store if user comes back to settings menu
			mRenderSystem->setConfigOption("Video Mode", mNewResolution);
			mRenderSystem->setConfigOption("Full Screen", mNewFullscreen);
			//mRenderSystem->setConfigOption("Colour Depth", mNewBPP);

			std::ofstream lConfigFile;
			lConfigFile.open ("config.cfg");
			lConfigFile << "Video Mode=" + mNewResolution << std::endl;
			lConfigFile << "Full Screen=" + mNewFullscreen << std::endl;
			lConfigFile << "Quality=" + mNewQuality << std::endl;
			lConfigFile.close();

			int lWidth = toNumber<int>(mNewResolution.substr(0, mNewResolution.find("x") - 1));
			int lHeight = toNumber<int>(mNewResolution.substr(mNewResolution.find("x") + 1, 
				mNewResolution.find("@")-mNewResolution.find("x")));
			bool lFullscreen = (mNewFullscreen == "Yes") ? true : false;

			if(lFullscreen) {
				mGUI->getRenderWindow()->setFullscreen(true, lWidth, lHeight);
			} else {
				mGUI->getRenderWindow()->setFullscreen(false, lWidth-8, lHeight-28);
#ifdef WIN32
				int lDesktopWidth = GetSystemMetrics(SM_CXSCREEN);
				int lDesktopHeight = GetSystemMetrics(SM_CYSCREEN);

				mGUI->getRenderWindow()->reposition((lDesktopWidth-lWidth)/2,(lDesktopHeight-lHeight)/2);
#else
        mGUI->getRenderWindow()->reposition(0,0);
#endif
			}

			EventHandler::raiseEvent(eGraphicOptionsChange);
		}

    mOptionsChanged = true;
		switchToMainMenu();
		mMainMenuLock = 5;
	}
	else if(_widget->getName() == "16BitRadio") {
		mGUI->findWidget<MyGUI::Button>("32BitRadio")->setStateCheck(false);
		mGUI->findWidget<MyGUI::Button>("16BitRadio")->setStateCheck(true);
	}
	else if(_widget->getName() == "32BitRadio") {
		mGUI->findWidget<MyGUI::Button>("16BitRadio")->setStateCheck(false);
		mGUI->findWidget<MyGUI::Button>("32BitRadio")->setStateCheck(true);
	}
	else if(_widget->getName() == "FullscreenCheck") {
		mGUI->findWidget<MyGUI::Button>("FullscreenCheck")->setStateCheck(
			!mGUI->findWidget<MyGUI::Button>("FullscreenCheck")->getStateCheck());
	}
	else if(_widget->getName() == "HighRadio") {
		mGUI->findWidget<MyGUI::Button>("HighRadio")->setStateCheck(true);
		mGUI->findWidget<MyGUI::Button>("NormalRadio")->setStateCheck(false);
	}
	else if(_widget->getName() == "NormalRadio") {
		mGUI->findWidget<MyGUI::Button>("NormalRadio")->setStateCheck(true);
		mGUI->findWidget<MyGUI::Button>("HighRadio")->setStateCheck(false);
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::menuLoadPressed(MyGUI::WidgetPtr _widget)
{
  // fade out hack
  unsigned long lStartTime = mRoot->getTimer()->getMilliseconds();
  unsigned long lPassedTime = 0;

  while (lPassedTime < 100) {
    mGUI->findWidget<MyGUI::Widget>("MainMenuWidget")->setAlpha(1.0 - (float)lPassedTime/100.0);
    mRoot->renderOneFrame();
    lPassedTime = mRoot->getTimer()->getMilliseconds() - lStartTime;
  }

	MyGUI::LayoutManager::getInstance().unloadLayout(mCurrentLayout);

	mCurrentLayout = MyGUI::LayoutManager::getInstance().load("load_" + StrLoc::get()->LanguageCode() + ".layout");

	mGUI->findWidget<MyGUI::Widget>("LoadWidget")->setPosition(
		(mGUI->getViewWidth()/2) - 
		(mGUI->findWidget<MyGUI::Widget>("LoadWidget")->getWidth()/2),
		mGUI->getViewHeight()/2 - 
		mGUI->findWidget<MyGUI::Widget>("LoadWidget")->getHeight()/2);

	MyGUI::StaticImagePtr lBackground = mGUI->findWidget<MyGUI::StaticImage>("Background");
	lBackground->setImageTexture(cBackgrounds[getCurrentAspectRatio()]);
	lBackground->setSize(mGUI->getViewWidth(), mGUI->getViewHeight());

	mGUI->findWidget<MyGUI::Button>("Back")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::menuBackToMainPressed);

std::string lUserHomeDirectory(getenv("HOME"));
 std::string lSaveGameDirectory = lUserHomeDirectory + "/.energytycoon/";
 
	std::vector<std::string> lSavedGames = findFilesInDirectory(lSaveGameDirectory + "saved_games");

	for(unsigned int i = 0; i < lSavedGames.size(); i++) {
		if (lSavedGames[i] != "mission_data" && lSavedGames[i] != "")
			mGUI->findWidget<MyGUI::List>("LoadList")->addItem(lSavedGames[i]);
	}

	mGUI->findWidget<MyGUI::Button>("LoadButton")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::loadButtonPressed);
	mGUI->findWidget<MyGUI::List>("LoadList")->eventListChangePosition = 
		MyGUI::newDelegate(this, &GUI::loadListPressed);

	mGUI->findWidget<MyGUI::Button>("LoadButton")
    ->setCaption(StrLoc::get()->LoadMenuLoad());
	mGUI->findWidget<MyGUI::Button>("Back")
    ->setCaption(StrLoc::get()->MainMenuBack());
	mGUI->findWidget<MyGUI::StaticText>("SaveGameInfo")
    ->setCaption(StrLoc::get()->NoSavegameSelected());

  // fade in hack
  lStartTime = mRoot->getTimer()->getMilliseconds();
  lPassedTime = 0;

  while (lPassedTime < 100) {
    mGUI->findWidget<MyGUI::Widget>("LoadWidget")->setAlpha((float)lPassedTime/100.0);
    mRoot->renderOneFrame();
    lPassedTime = mRoot->getTimer()->getMilliseconds() - lStartTime;
  }

  mGUI->findWidget<MyGUI::Widget>("LoadWidget")->setAlpha(1.0);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::loadButtonPressed(MyGUI::WidgetPtr _widget)
{
	if (mGUI->findWidget<MyGUI::List>("LoadList")->getIndexSelected() != MyGUI::ITEM_NONE) {
		if (!mGameRunning) {
			StartData lStartData;
			lStartData.mSerialized = true;
			lStartData.mNew = false;
			lStartData.mSerializedFile = 
				mGUI->findWidget<MyGUI::List>("LoadList")->getItemNameAt(
				mGUI->findWidget<MyGUI::List>("LoadList")->getIndexSelected());

			MyGUI::LayoutManager::getInstance().unloadLayout(mCurrentLayout);
			mCurrentLayout = MyGUI::LayoutManager::getInstance().load("loading.layout");
			mGUI->findWidget<MyGUI::StaticImage>("Background")
        ->setSize(mGUI->getViewWidth(), mGUI->getViewHeight());
			mGUI->findWidget<MyGUI::StaticImage>("Background")
        ->setImageTexture(cBackgrounds[getCurrentAspectRatio()]);

			MyGUI::PointerManager::getInstance().setVisible(false);
			mLoading = true;

			EventHandler::raiseEvent(eGamestateChangeGUI);
			EventHandler::raiseEvent(eLoadGame, new EventArg<StartData>(lStartData));
		} else {
			if(mAskForRestart) {
				MyGUI::MessagePtr lQuestionExit = MyGUI::Message::createMessageBox(
					"Message", StrLoc::get()->GameTitle(),
					StrLoc::get()->LoadMenuQuestion(),
					MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No 
					| MyGUI::MessageBoxStyle::IconQuest);
				lQuestionExit->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::unloadAndLoad);
			} else {
				unloadAndLoad(0, MyGUI::MessageBoxStyle::Yes);
			}
		}
	} else {
		MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
			StrLoc::get()->LoadMenuSelect());
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::loadListPressed(MyGUI::ListPtr _widget, unsigned int _index)
{
	if (mGUI->findWidget<MyGUI::List>("LoadList")->getIndexSelected() != MyGUI::ITEM_NONE) {
		std::string lFilename = mGUI->findWidget<MyGUI::List>("LoadList")->getItemNameAt(
			mGUI->findWidget<MyGUI::List>("LoadList")->getIndexSelected());

std::string lUserHomeDirectory(getenv("HOME"));
 std::string lSaveGameDirectory = lUserHomeDirectory + "/.energytycoon/";
 
		boost::shared_ptr<TiXmlDocument> lSavegame(new TiXmlDocument((lSaveGameDirectory 
			+ "saved_games/" + lFilename + ".xml").c_str()));

		lSavegame->LoadFile(TIXML_ENCODING_UTF8);

		TiXmlNode* rootNode;

		rootNode =  lSavegame->FirstChildElement("savegame");

		std::string lMapPrefix = rootNode->ToElement()->Attribute("map");
		std::string lMission = rootNode->ToElement()->Attribute("mission");

		lMission = getStartDataFromMission(lMission, StrLoc::get()->MyGUILanguage())[0];

                std::string lDateTime = "N/A";

#ifdef WIN32
		WIN32_FILE_ATTRIBUTE_DATA  lFileAttributes;
		GetFileAttributesEx((cDataDirPre + "saved_games\\" + lFilename + ".xml").c_str(),
			GetFileExInfoStandard, &lFileAttributes);

		SYSTEMTIME lTimeUTC, lTimeLocal;
		// convert the last-write time to local time.
		FileTimeToSystemTime(&lFileAttributes.ftLastWriteTime, &lTimeUTC);
		SystemTimeToTzSpecificLocalTime(NULL, &lTimeUTC, &lTimeLocal);

		std::string lHour = toString(lTimeLocal.wHour);
		if(lHour.length() == 1)
			lHour = "0" + lHour;

		std::string lMinutes = toString(lTimeLocal.wMinute);
		if(lMinutes.length() == 1)
			lMinutes = "0" + lMinutes;

		lDateTime = toString(lTimeLocal.wDay) + "." + toString(lTimeLocal.wMonth)
			+ "." + toString(lTimeLocal.wYear) + "  " + lHour + ":" + lMinutes;
#endif

		mGUI->findWidget<MyGUI::StaticText>("SaveGameInfo")->
			setCaption(StrLoc::get()->LoadMenuMap() + mISOToFormattedMapname[lMapPrefix] + "\n"
			+ "Mission: " + lMission + "\n" + StrLoc::get()->LoadMenuSaved() + lDateTime);
	} else 	{
		mGUI->findWidget<MyGUI::StaticText>("SaveGameInfo")->setCaption("");
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::menuSavePressed(MyGUI::WidgetPtr _widget)
{
  if(mDemo) {
		MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(), StrLoc::get()->DemoNoSave());
    return;
  }

  // fade out hack
  unsigned long lStartTime = mRoot->getTimer()->getMilliseconds();
  unsigned long lPassedTime = 0;

  while (lPassedTime < 100) {
    mGUI->findWidget<MyGUI::Widget>("MainMenuWidget")->setAlpha(1.0 - (float)lPassedTime/100.0);
    mRoot->renderOneFrame();
    lPassedTime = mRoot->getTimer()->getMilliseconds() - lStartTime;
  }

	MyGUI::LayoutManager::getInstance().unloadLayout(mCurrentLayout);

	mCurrentLayout = MyGUI::LayoutManager::getInstance().load("save_" + StrLoc::get()->LanguageCode() + ".layout");

	mGUI->findWidget<MyGUI::Widget>("SaveWidget")->setPosition(
		(mGUI->getViewWidth()/2) - 
		(mGUI->findWidget<MyGUI::Widget>("SaveWidget")->getWidth()/2),
		mGUI->getViewHeight()/2 - 
		mGUI->findWidget<MyGUI::Widget>("SaveWidget")->getHeight()/2);

	MyGUI::StaticImagePtr lBackground = mGUI->findWidget<MyGUI::StaticImage>("Background");
	lBackground->setImageTexture(cBackgrounds[getCurrentAspectRatio()]);
	lBackground->setSize(mGUI->getViewWidth(), mGUI->getViewHeight());

	mGUI->findWidget<MyGUI::Button>("Back")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::menuBackToMainPressed);

std::string lUserHomeDirectory(getenv("HOME"));
 std::string lSaveGameDirectory = lUserHomeDirectory + "/.energytycoon/";
 
	std::vector<std::string> lSavedGames = findFilesInDirectory(lSaveGameDirectory + "saved_games");

	for (unsigned int i = 0; i < lSavedGames.size(); i++) {
		if (lSavedGames[i] != "mission_data" && lSavedGames[i] != "")
			mGUI->findWidget<MyGUI::List>("SaveList")->addItem(lSavedGames[i]);
	}

	mGUI->findWidget<MyGUI::Button>("SaveButton")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::saveButtonPressed);
	mGUI->findWidget<MyGUI::List>("SaveList")->eventListChangePosition = 
		MyGUI::newDelegate(this, &GUI::saveListPressed);

	mGUI->findWidget<MyGUI::Button>("SaveButton")->setCaption(StrLoc::get()->SaveMenuSave());
	mGUI->findWidget<MyGUI::Button>("Back")->setCaption(StrLoc::get()->MainMenuBack());
	mGUI->findWidget<MyGUI::StaticText>("SaveGameInfo")
    ->setCaption(StrLoc::get()->NoSavegameSelected());

  // fade in hack
  lStartTime = mRoot->getTimer()->getMilliseconds();
  lPassedTime = 0;

  while (lPassedTime < 100) {
    mGUI->findWidget<MyGUI::Widget>("SaveWidget")->setAlpha((float)lPassedTime/100.0);
    mRoot->renderOneFrame();
    lPassedTime = mRoot->getTimer()->getMilliseconds() - lStartTime;
  }

  mGUI->findWidget<MyGUI::Widget>("SaveWidget")->setAlpha(1.0);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::saveButtonPressed(MyGUI::WidgetPtr _widget)
{
	if (mGUI->findWidget<MyGUI::Edit>("SaveName")->getCaption() != "") {
		bool lNameFound = false;

std::string lUserHomeDirectory(getenv("HOME"));
 std::string lSaveGameDirectory = lUserHomeDirectory + "/.energytycoon/";
 
		std::vector<std::string> lSavedGames = findFilesInDirectory(lSaveGameDirectory + "saved_games");

		for(unsigned int i = 0; i < lSavedGames.size(); i++)
			if(mGUI->findWidget<MyGUI::Edit>("SaveName")->getCaption() == lSavedGames[i])
				lNameFound = true;

		if (!lNameFound) {
			EventHandler::raiseEvent(eSerializeGame, new EventArg<std::string>(
				mGUI->findWidget<MyGUI::Edit>("SaveName")->getCaption()));
			mGUI->findWidget<MyGUI::List>("SaveList")->addItem(
				mGUI->findWidget<MyGUI::Edit>("SaveName")->getCaption());

			MyGUI::Message::createMessageBox("Message",
				StrLoc::get()->GameTitle(), StrLoc::get()->SaveMenuSaved());
		} else {
			MyGUI::MessagePtr lQuestionExit = MyGUI::Message::createMessageBox("Message",
				StrLoc::get()->GameTitle(), StrLoc::get()->SaveMenuOverwrite(),
				MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No 
			    | MyGUI::MessageBoxStyle::IconQuest);
			lQuestionExit->eventMessageBoxResult = MyGUI::newDelegate(this, &GUI::overwrite);
		}
	} else {
		MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
			StrLoc::get()->SaveMenuName());
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::overwrite(MyGUI::MessagePtr _sender, MyGUI::MessageBoxStyle _style)
{
	if (_style == MyGUI::MessageBoxStyle::Yes) {
		EventHandler::raiseEvent(eSerializeGame, new EventArg< std::string >(
			mGUI->findWidget<MyGUI::Edit>("SaveName")->getCaption()));
		saveListPressed(0, 0);
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::saveListPressed(MyGUI::ListPtr _widget, unsigned int _index)
{
	if (mGUI->findWidget< MyGUI::List >("SaveList")->getIndexSelected() != MyGUI::ITEM_NONE) {
		mGUI->findWidget< MyGUI::Edit >("SaveName")->setCaption(
			mGUI->findWidget< MyGUI::List >("SaveList")->getItemNameAt(
			mGUI->findWidget< MyGUI::List >("SaveList")->getIndexSelected()));

		std::string lFilename = mGUI->findWidget< MyGUI::List >("SaveList")->getItemNameAt(
			mGUI->findWidget< MyGUI::List >("SaveList")->getIndexSelected());

std::string lUserHomeDirectory(getenv("HOME"));
 std::string lSaveGameDirectory = lUserHomeDirectory + "/.energytycoon/";
 
		boost::shared_ptr<TiXmlDocument> lSavegame(new TiXmlDocument((lSaveGameDirectory 
			+ "saved_games/" + lFilename + ".xml").c_str()));

		lSavegame->LoadFile(TIXML_ENCODING_UTF8);

		TiXmlNode* rootNode;

		rootNode =  lSavegame->FirstChildElement("savegame");

		std::string lMapPrefix = rootNode->ToElement()->Attribute("map");
		std::string lMission = rootNode->ToElement()->Attribute("mission");

		lMission = getStartDataFromMission(lMission, StrLoc::get()->MyGUILanguage())[0];

                std::string lDateTime = "N/A";
#ifdef WIN32
		WIN32_FILE_ATTRIBUTE_DATA  lFileAttributes;
		GetFileAttributesEx((cDataDirPre + "SavedGames\\" + lFilename + ".xml").c_str(),
			GetFileExInfoStandard, &lFileAttributes);

		SYSTEMTIME lTimeUTC, lTimeLocal;
		// convert the last-write time to local time.
		FileTimeToSystemTime(&lFileAttributes.ftLastWriteTime, &lTimeUTC);
		SystemTimeToTzSpecificLocalTime(NULL, &lTimeUTC, &lTimeLocal);

		std::string lHour = toString(lTimeLocal.wHour);
		if(lHour.length() == 1)
			lHour = "0" + lHour;

		std::string lMinutes = toString(lTimeLocal.wMinute);
		if(lMinutes.length() == 1)
			lMinutes = "0" + lMinutes;

		lDateTime = toString(lTimeLocal.wDay) + "." + toString(lTimeLocal.wMonth) + "."
			+ toString(lTimeLocal.wYear) + "  " + lHour + ":" + lMinutes;
#endif
		mGUI->findWidget< MyGUI::StaticText >("SaveGameInfo")->
			setCaption(StrLoc::get()->LoadMenuMap() + mISOToFormattedMapname[lMapPrefix] + "\n"
			+ "Mission: " + lMission + "\n" + StrLoc::get()->LoadMenuSaved() + lDateTime);
	} else {
		mGUI->findWidget< MyGUI::StaticText >("SaveGameInfo")->setCaption("N/A");
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::menuNewPressed(MyGUI::WidgetPtr _widget)
{
  // fade out hack
  unsigned long lStartTime = mRoot->getTimer()->getMilliseconds();
  unsigned long lPassedTime = 0;

  while (lPassedTime < 100) {
    mGUI->findWidget<MyGUI::Widget>("MainMenuWidget")->setAlpha(1.0 - (float)lPassedTime/100.0);
    mRoot->renderOneFrame();
    lPassedTime = mRoot->getTimer()->getMilliseconds() - lStartTime;
  }

	MyGUI::LayoutManager::getInstance().unloadLayout(mCurrentLayout);

	mCurrentLayout = MyGUI::LayoutManager::getInstance().load("newgame_" + StrLoc::get()->LanguageCode() + ".layout");

	mGUI->findWidget< MyGUI::Widget >("NewGameWidget")->setPosition(
		(mGUI->getViewWidth()/2) - 
		(mGUI->findWidget< MyGUI::Widget >("NewGameWidget")->getWidth()/2),
		mGUI->getViewHeight()/2 - 
		mGUI->findWidget< MyGUI::Widget >("NewGameWidget")->getHeight()/2);

	MyGUI::StaticImagePtr lBackground = mGUI->findWidget< MyGUI::StaticImage >("Background");
	lBackground->setImageTexture(cBackgrounds[getCurrentAspectRatio()]);
	lBackground->setSize(mGUI->getViewWidth(), mGUI->getViewHeight());

	mGUI->findWidget< MyGUI::Button >("Back")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::menuBackToMainPressed);
	mGUI->findWidget< MyGUI::Button >("StartGameButton")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::newButtonPressed);
	mGUI->findWidget< MyGUI::Button >("EasyRadio")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::newButtonPressed);
	mGUI->findWidget< MyGUI::Button >("MediumRadio")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::newButtonPressed);
	mGUI->findWidget< MyGUI::Button >("HardRadio")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::newButtonPressed);
	mGUI->findWidget< MyGUI::Button >("Sandbox")->eventMouseButtonClick = 
		MyGUI::newDelegate(this, &GUI::newButtonPressed);
	mGUI->findWidget< MyGUI::List >("MapsList")->eventListChangePosition = 
		MyGUI::newDelegate(this, &GUI::newListPressed);
	mGUI->findWidget< MyGUI::List >("MissionsList")->eventListChangePosition = 
		MyGUI::newDelegate(this, &GUI::newListPressed);

	mGUI->findWidget< MyGUI::Button >("Back")->setCaption(StrLoc::get()->MainMenuBack());
	mGUI->findWidget< MyGUI::Button >("StartGameButton")->setCaption(StrLoc::get()->NewMenuStart());
	mGUI->findWidget< MyGUI::Button >("EasyRadio")->setCaption(StrLoc::get()->NewMenuEasy());
	mGUI->findWidget< MyGUI::Button >("MediumRadio")->setCaption(StrLoc::get()->NewMenuMedium());
	mGUI->findWidget< MyGUI::Button >("HardRadio")->setCaption(StrLoc::get()->NewMenuHard());
	mGUI->findWidget< MyGUI::StaticText >("MissionsText")->setCaption(StrLoc::get()->NewMenuMissions());
	mGUI->findWidget< MyGUI::StaticText >("DifficultyText")->setCaption(StrLoc::get()->NewMenuDifficulty());
	mGUI->findWidget< MyGUI::StaticText >("MapsText")->setCaption(StrLoc::get()->NewMenuMap());
	mGUI->findWidget< MyGUI::StaticText >("MissionGoalsText")->setCaption(StrLoc::get()->NewMenuMissionGoals());

	std::vector< std::string > lMissions = findFilesInDirectory(cDataDirPre + "missions", false);
	std::vector< std::string > lMapsNonUnique;
	std::vector< std::string > lMaps;

  for (unsigned int i = 0; i < lMissions.size(); i++) {
    if (!lMissions[i].empty() && lMissions[i] != "." && lMissions[i] != "..") {
		  lMapsNonUnique.push_back(getStartDataFromMission(lMissions[i],
        StrLoc::get()->MyGUILanguage())[1]);
    }
  }

	sort(lMapsNonUnique.begin(), lMapsNonUnique.end());

  std::unique_copy( lMapsNonUnique.begin(), lMapsNonUnique.end(), std::back_inserter( lMaps ) );

	for (unsigned int j = 0; j < lMaps.size(); j++)
		mGUI->findWidget<MyGUI::List>("MapsList")->addItem(mISOToFormattedMapname[lMaps[j]]);

	mGUI->findWidget<MyGUI::Button>("MediumRadio")->setStateCheck(true);

  // fade in hack
  lStartTime = mRoot->getTimer()->getMilliseconds();
  lPassedTime = 0;

  while (lPassedTime < 100) {
    mGUI->findWidget<MyGUI::Widget>("NewGameWidget")->setAlpha((float)lPassedTime/100.0);
    mRoot->renderOneFrame();
    lPassedTime = mRoot->getTimer()->getMilliseconds() - lStartTime;
  }

  mGUI->findWidget<MyGUI::Widget>("NewGameWidget")->setAlpha(1.0);
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::newButtonPressed(MyGUI::WidgetPtr _widget)
{
	if(_widget->getName() == "StartGameButton")
	{
		if ((mGUI->findWidget<MyGUI::List>("MapsList")->getIndexSelected() 
			!= MyGUI::ITEM_NONE)
			&& (mGUI->findWidget<MyGUI::List>("MissionsList")->getIndexSelected() 
			!= MyGUI::ITEM_NONE)) {
			if (mGameRunning == true) {
				if (mAskForRestart) {
					MyGUI::MessagePtr lQuestionExit = MyGUI::Message::createMessageBox("Message",
						StrLoc::get()->GameTitle(), StrLoc::get()->NewMenuQuestion(),
						MyGUI::MessageBoxStyle::Yes | MyGUI::MessageBoxStyle::No |
						MyGUI::MessageBoxStyle::IconQuest);
					lQuestionExit->eventMessageBoxResult = MyGUI::newDelegate(this,
						&GUI::unloadAndStartNew);
				} else {
					unloadAndStartNew(0, MyGUI::MessageBoxStyle::Yes);
				}
			} else {
				std::string lMission = mGUI->findWidget <MyGUI::List >("MissionsList")->
					getItemDataAt<std::string>(
					mGUI->findWidget<MyGUI::List>("MissionsList")->getIndexSelected())->c_str();

				bool lSandbox =  mGUI->findWidget< MyGUI::Button >("Sandbox")->getStateCheck();

				MyGUI::LayoutManager::getInstance().unloadLayout(mCurrentLayout);
				mCurrentLayout = MyGUI::LayoutManager::getInstance().load("loading.layout");

				mGUI->findWidget< MyGUI::StaticImage >("Background")
          ->setSize(mGUI->getViewWidth(), mGUI->getViewHeight());
				mGUI->findWidget< MyGUI::StaticImage >("Background")
          ->setImageTexture(cBackgrounds[getCurrentAspectRatio()]);

				StartData lData;
				lData.mMission = lMission;
				lData.mSandbox = lSandbox;
				lData.mNew = true;

				MyGUI::PointerManager::getInstance().setVisible(false);
				mLoading = true;

				EventHandler::raiseEvent(eGamestateChangeGUI);
				EventHandler::raiseEvent(eLoadGame, new EventArg<StartData>(lData));
			}
		} else {
			MyGUI::Message::createMessageBox("Message", StrLoc::get()->GameTitle(),
				StrLoc::get()->NewMenuSelect());
		}
	}
	else if (_widget->getName() == "EasyRadio") {
		mGUI->findWidget< MyGUI::Button >("Sandbox")->setStateCheck(false);
		mGUI->findWidget< MyGUI::Button >("EasyRadio")->setStateCheck(true);
		mGUI->findWidget< MyGUI::Button >("MediumRadio")->setStateCheck(false);
		mGUI->findWidget< MyGUI::Button >("HardRadio")->setStateCheck(false);
	}
	else if (_widget->getName() == "MediumRadio") {
		mGUI->findWidget< MyGUI::Button >("Sandbox")->setStateCheck(false);
		mGUI->findWidget< MyGUI::Button >("EasyRadio")->setStateCheck(false);
		mGUI->findWidget< MyGUI::Button >("MediumRadio")->setStateCheck(true);
		mGUI->findWidget< MyGUI::Button >("HardRadio")->setStateCheck(false);
	}
	else if (_widget->getName() == "HardRadio") {
		mGUI->findWidget< MyGUI::Button >("Sandbox")->setStateCheck(false);
		mGUI->findWidget< MyGUI::Button >("EasyRadio")->setStateCheck(false);
		mGUI->findWidget< MyGUI::Button >("MediumRadio")->setStateCheck(false);
		mGUI->findWidget< MyGUI::Button >("HardRadio")->setStateCheck(true);
	}
	else if (_widget->getName() == "Sandbox") {
		mGUI->findWidget< MyGUI::Button >("Sandbox")->setStateCheck(true);
		mGUI->findWidget< MyGUI::Button >("EasyRadio")->setStateCheck(false);
		mGUI->findWidget< MyGUI::Button >("MediumRadio")->setStateCheck(false);
		mGUI->findWidget< MyGUI::Button >("HardRadio")->setStateCheck(false);
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::unloadAndLoad(MyGUI::MessagePtr _sender, MyGUI::MessageBoxStyle _style)
{
	if (_style == MyGUI::MessageBoxStyle::Yes) {
		StartData lStartData;
		lStartData.mSerialized = true;
		lStartData.mNew = false;
		lStartData.mSerializedFile = mGUI->findWidget< MyGUI::List >("LoadList")->getItemNameAt(
			mGUI->findWidget<MyGUI::List>("LoadList")->getIndexSelected());

		EventHandler::raiseEvent(eDismantleGame, new EventArg<StartData>(lStartData));
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::unloadAndStartNew(MyGUI::MessagePtr _sender, MyGUI::MessageBoxStyle _style)
{
	if (_style == MyGUI::MessageBoxStyle::Yes) {
		std::string lMission = mGUI->findWidget< MyGUI::List >("MissionsList")
      ->getItemDataAt< std::string >(
			mGUI->findWidget< MyGUI::List >("MissionsList")->getIndexSelected())->c_str();

		StartData lStartData;
		lStartData.mMission = lMission;
		lStartData.mSandbox = mGUI->findWidget<MyGUI::Button>("Sandbox")->getStateCheck();
		lStartData.mNew = true;

		EventHandler::raiseEvent(eDismantleGame, new EventArg< StartData >(lStartData));
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GUI::newListPressed(MyGUI::ListPtr _widget, unsigned int _index)
{
	if (_widget->getName() == "MapsList") {
		mGUI->findWidget< MyGUI::List >("MissionsList")->removeAllItems();

		if (mGUI->findWidget< MyGUI::List >("MapsList")->getIndexSelected() != MyGUI::ITEM_NONE) {
			std::vector< std::string > lMissions 
        = findFilesInDirectory(cDataDirPre + "missions", false);
			std::vector< std::string > lMissionsMap;

			for (unsigned int i = 0; i < lMissions.size(); i++) {
				if (lMissions[i] == "." || lMissions[i] == "..")
					continue;
				std::string lMissionMap = getStartDataFromMission(lMissions[i],
					StrLoc::get()->MyGUILanguage())[1];
				std::string lMissionSelected = mGUI->findWidget<MyGUI::List>("MapsList")->
					getItemNameAt(
					mGUI->findWidget<MyGUI::List>("MapsList")->getIndexSelected());
				if (lMissionMap == mFormattedToISOMapname[lMissionSelected]) {
					mGUI->findWidget<MyGUI::List>("MissionsList")->addItem(
						getStartDataFromMission(lMissions[i], StrLoc::get()
            ->MyGUILanguage())[0], lMissions[i]);
				}
			}
		}
	}
	else if(_widget->getName() == "MissionsList") {
		if(mGUI->findWidget< MyGUI::List >("MissionsList")->getIndexSelected() != MyGUI::ITEM_NONE) {
			std::vector< std::string > lMissionGoals;

			lMissionGoals = getMissionGoalsFromMission(
				mGUI->findWidget<MyGUI::List>("MissionsList")->getItemDataAt<std::string>(
					mGUI->findWidget<MyGUI::List>("MissionsList")->getIndexSelected())->c_str(),
					StrLoc::get()->MyGUILanguage(), StrLoc::get()->GoalNone());

			std::string lCaption;

			if (lMissionGoals[0] == StrLoc::get()->GoalNone().asUTF8()) {
				lCaption = StrLoc::get()->GoalNone();
			} else {
				for (size_t j = 0; j < lMissionGoals.size(); j++)
					lCaption += "- " + lMissionGoals[j] + "\n";
			}

			mGUI->findWidget< MyGUI::StaticText >("MissionGoals")->setCaption(lCaption);
		}
	}
}
