#include "tinyxml.h"
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <cmath>
#include <sstream>
#include <fstream>
#ifdef WIN32
  #include <windows.h>
  #include <windowsx.h>
#else
  #include <sys/types.h>
  #include <dirent.h>
  #include <errno.h>
#endif
#include "boost/shared_ptr.hpp"
#include "boost/function.hpp"
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/dijkstra_shortest_paths.hpp"
#include "boost/graph/connected_components.hpp"
#include "OGRE/Ogre.h"
#include "OGRE/OgreConfigFile.h"
#include "OGRE/OgreStringConverter.h"
#include "OGRE/OgreException.h"
#include "MyGUI/MyGUI.h"
#include "irrKlang.h"
#ifdef WIN32
  #define OIS_DYNAMIC_LIB
  #include <OIS/OIS.h>
#else
  #include "OIS/OIS.h"
#endif
#include "Shared.h"
#include "StringsLoc.h"
#include "GameConfig.h"