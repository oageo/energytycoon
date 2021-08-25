#include "StdAfx.h"
#include "Main.h"
#include "StringsLoc.h"
#include "Event.h"
#include "GameObject.h"
#include "Timer.h"
#include "InstancedGeometryManager.h"
#include "StaticGameObject.h"
#include "Audio.h"
#include "resource.h"

/*-----------------------------------------------------------------------------------------------*/

// Static variable definitions
size_t GameObject::mIDCounter = 0;
std::vector<TimerInstance> GameTimer::mTimerInstances;
boost::shared_ptr<StrLoc> StrLoc::mInstance;
std::multimap<const eEventType, boost::function<void (EventData*)> > EventHandler::mCallbacks;
irrklang::ISoundEngine* Audio::mEngine;
boost::shared_ptr<InstancedGeometryManager> StaticGameObject::mTree1Batch;
boost::shared_ptr<InstancedGeometryManager> StaticGameObject::mTree2Batch;
boost::shared_ptr<InstancedGeometryManager> StaticGameObject::mTree1FloorBatch;
boost::shared_ptr<InstancedGeometryManager> StaticGameObject::mTree2FloorBatch;
std::map<std::string, std::string> GameConfig::mData;
std::map<int, std::string> GameConfig::mConversionMap;

/*-----------------------------------------------------------------------------------------------*/

Application::Application(void) :
	mRoot(0), mWindow(0), mSceneManager(0), mRestart(false), mDebug(false)
{
	REGISTER_CALLBACK(eRestartGame, Application::restartGame);
}

/*-----------------------------------------------------------------------------------------------*/

Application::~Application(void)
{
  if(mRoot)
    OGRE_DELETE mRoot;
}

/*-----------------------------------------------------------------------------------------------*/

void Application::go(void)
{
  setup();
	startRendering();
}

/*-----------------------------------------------------------------------------------------------*/

void Application::startRendering(void)
{
	mRoot->startRendering();

	if (mRestart) {
		mRestart = false;

		mRoot->removeFrameListener(mGame.get());
		mSceneManager->clearScene();
		mSceneManager->destroyAllCameras();
		mWindow->removeAllViewports();
		mRoot->destroySceneManager(mSceneManager);
		mGame->windowClosed(mWindow);
		EventHandler::clear();
		
		REGISTER_CALLBACK(eRestartGame, Application::restartGame);
		GameObject::resetIDCounter();

		std::vector<std::string> lLoadedResourceGroups;

		Ogre::ParticleSystemManager::getSingleton().removeAllTemplates();
		Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup("MinimapGroup");
		Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup("ChartGroup");

		lLoadedResourceGroups = Ogre::ResourceGroupManager::getSingleton().getResourceGroups();

		for (size_t i = 0; i < lLoadedResourceGroups.size(); i++) {
			if (lLoadedResourceGroups[i] != Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME) {
				Ogre::ResourceGroupManager::getSingleton().
					destroyResourceGroup(lLoadedResourceGroups[i]);
				Ogre::ResourceGroupManager::getSingleton().
					createResourceGroup(lLoadedResourceGroups[i]);
			}
		}

		setupResources();

		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
		Ogre::MaterialManager::getSingleton().initialise();
		Ogre::ResourceGroupManager::getSingleton().
			unlinkWorldGeometryFromResourceGroup(
			Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);

		mSceneManager = mRoot->createSceneManager(Ogre::ST_EXTERIOR_CLOSE, "GameSceneManager");

    // save demo time
    float lDemoTime = mGame->getDemoTime();

		mGame.reset(new Game(mWindow, mSceneManager, mRoot, true));

    mGame->setDemoTime(lDemoTime);

		EventHandler::raiseEvent(eLoadGame, new EventArg<StartData>(mStartData));
		EventHandler::raiseEvent(eUpdateMinimap);

		mRoot->addFrameListener(mGame.get());

		startRendering();
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Application::restartGame(EventData* pData)
{
	mRoot->queueEndRendering();
	mRestart = true;
	mStartData = static_cast<EventArg<StartData>*>(pData)->mData1;
}

/*-----------------------------------------------------------------------------------------------*/

void Application::setup(void)
{
#ifdef WIN32
  mRoot = OGRE_NEW Ogre::Root("plugins.cfg", "", "ogre.log");
#else
  mRoot = OGRE_NEW Ogre::Root("/usr/share/games/energytycoon/plugins.cfg", "", "");
#endif
  setupResources();
	parseSettings();

	Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_BOREME);

	Ogre::NameValuePairList lOptions;

	lOptions["title"] = StrLoc::get()->GameTitle();

	Ogre::RenderSystem* lRenderSystem;

#ifdef WIN32
	if (!mDebug)
		lRenderSystem = mRoot->getRenderSystemByName("Direct3D9 Rendering Subsystem");
	else
		lRenderSystem = mRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
#else
	lRenderSystem = mRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
#endif

  if((mLastWindowWidth == -1) && (mLastWindowHeight == -1))
  {
#ifdef WIN32
   RECT lDesktopRect;
   const HWND lDesktopHandle = GetDesktopWindow();

   GetWindowRect(lDesktopHandle, &lDesktopRect);

   mLastWindowWidth = lDesktopRect.right;
   mLastWindowHeight = lDesktopRect.bottom;
#else
    // todo: retrieve linux desktop size
#endif
  }

	lRenderSystem->setConfigOption("Full Screen", mLastFullscreen ? "Yes" : "No");
	lRenderSystem->setConfigOption("Video Mode", toString(mLastWindowWidth) + " x "
		+ toString(mLastWindowHeight));

	mRoot->setRenderSystem(lRenderSystem);
	mRoot->initialise(false);

	mWindow = mRoot->createRenderWindow("GameRenderWindow", mLastWindowWidth,
		mLastWindowHeight, mLastFullscreen, &lOptions);

	mSceneManager = mRoot->createSceneManager(Ogre::ST_EXTERIOR_CLOSE, "GameSceneManager");

    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(3);
	Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_BILINEAR);

	loadResources();

	mGame.reset(new Game(mWindow, mSceneManager, mRoot));
	mRoot->addFrameListener(mGame.get());
}

/*-----------------------------------------------------------------------------------------------*/

void Application::setupResources(void)
{
	Ogre::ConfigFile lConfigFile;
#ifdef WIN32
	lConfigFile.load("resources.cfg");
#else
	lConfigFile.load("/usr/share/games/energytycoon/resources.cfg");
#endif

    Ogre::ConfigFile::SectionIterator lSectionIterator =
				lConfigFile.getSectionIterator();

    Ogre::String lSectionName, lTypeName, lArchName;

    while (lSectionIterator.hasMoreElements())  {
        lSectionName = lSectionIterator.peekNextKey();

        Ogre::ConfigFile::SettingsMultiMap *lSettings = lSectionIterator.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;

        for(i = lSettings->begin(); i != lSettings->end(); ++i)  {
            lTypeName = i->first;
            lArchName = i->second;

            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                lArchName, lTypeName, lSectionName);
        }
    }
}

/*-----------------------------------------------------------------------------------------------*/

void Application::loadResources(void)
{
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

/*-----------------------------------------------------------------------------------------------*/

void Application::parseSettings(void)
{
#ifdef WIN32
	std::ifstream lFile("config.cfg");
#else
	std::ifstream lFile("/usr/share/games/energytycoon/config.cfg");
#endif
	std::string lBuffer;

	while (getline(lFile, lBuffer)) {
		std::stringstream lTemp;

		if (lBuffer.find("Full Screen=") != -1) {
			if (lBuffer.find("Yes") != -1)
				mLastFullscreen = true;
			else
				mLastFullscreen = false;
		}
		else if (lBuffer.find("Video Mode=") != -1) {
			mLastWindowWidth = toNumber<int>(lBuffer.substr(lBuffer.find("=")+1,
				lBuffer.find("x")-1));
			mLastWindowHeight = toNumber<int>(lBuffer.substr(lBuffer.find("x")+1));
		}
		else if (lBuffer.find("debug_gl") != -1) {
			mDebug = true;
		}
	}

	lFile.close();
}

/*-----------------------------------------------------------------------------------------------*/

#ifdef WIN32
LRESULT CALLBACK LangChooserProc(HWND pWindowHandle, UINT pMessage, WPARAM wParam,
								 LPARAM lParam)
{
	HWND lComboHandle = GetDlgItem(pWindowHandle, IDC_LANGUAGECOMBO);
	bool lQuit = false;

	switch (pMessage) {
	case WM_INITDIALOG:
		ComboBox_AddString(lComboHandle, "Deutsch");
		ComboBox_AddString(lComboHandle, "English");
		ComboBox_SetCurSel(lComboHandle, 0);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			lQuit = true;
			break;
		}
		break;
	case WM_CLOSE:
		lQuit = true;
		break;
	default:
		return false;
	}

	if (lQuit) {
		int lSelected = ComboBox_GetCurSel(lComboHandle);
		std::ofstream lFile;

		lFile.open("lang.cfg");

		if(lSelected == 0)
			lFile << "de";
		else
			lFile << "en";
		else
			lFile << "ja";

		lFile.close();

		EndDialog(pWindowHandle, 1);
	}

	return true;
}
#endif

/*-----------------------------------------------------------------------------------------------*/

#ifdef WIN32
int __stdcall WinMain(HINSTANCE pInstance, HINSTANCE, char* ,int)
#else
#include <sys/stat.h>
#include <sys/types.h>
int main(int, char**)
#endif
{
#ifdef WIN32
	GameConfig::load("game.cfg");
#else
	GameConfig::load("/usr/share/games/energytycoon/game.cfg");
#endif

#ifdef WIN32
	DWORD lFileAttributes;

	lFileAttributes = GetFileAttributes("/usr/share/games/energytycoon/lang.cfg");
	if (lFileAttributes == 0xFFFFFFFF) {
		DialogBox(pInstance, MAKEINTRESOURCE(IDD_LANGUAGECHOOSER),
			0, reinterpret_cast<DLGPROC>(LangChooserProc));
	}
#endif

#ifndef WIN32
	std::string lUserHomeDirectory(getenv("HOME"));
	std::string lProgramDirectory = lUserHomeDirectory + "/.energytycoon";
	std::string lSavedFilesDirectory = lUserHomeDirectory + "/.energytycoon/saved_games";
 
	int dir1 = mkdir(lProgramDirectory.c_str(), 0777);
	int dir2 = mkdir(lSavedFilesDirectory.c_str(), 0777);
 
	std::cout << "Creating savegame directory (" << lSavedFilesDirectory << ") :" << dir1 << "&" << dir2 << std::endl;
#endif

	std::ifstream lFile;

#ifdef WIN32
	lFile.open("lang.cfg");
#else
	lFile.open("/usr/share/games/energytycoon/lang.cfg");
#endif

	std::string lLanguage;
	lFile >> lLanguage;
	lFile.close();

	if(lLanguage == "de")
		StrLoc::setLanguage(new LangDE());
	else if(lLanguage == "en")
		StrLoc::setLanguage(new LangEN());
	else if(lLanguage == "ja")
		StrLoc::setLanguage(new LangJA());

	Audio::start();
	Application lApplication;

  try {
    lApplication.go();
  } catch(std::exception& pException) {
#ifdef WIN32
    MessageBox(NULL, pException.what(), "An exception has occurred!",
      MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
    std::cout << "An exception has occured! " << pException.what() << std::endl;
#endif
  } catch(...) {
#ifdef WIN32
    MessageBox(NULL, "Unhandled exception", "An unhandled exception has occurred!",
      MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
    std::cout << "An unhandled exception has occured! " << std::endl;
#endif
  }

	Audio::shutdown();
  return 0;
}
