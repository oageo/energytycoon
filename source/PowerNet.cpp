#include "StdAfx.h"
#include "PowerNet.h"
#include "Pole.h"
#include "Map.h"
#include "GUI.h"
#include "Event.h"
#include "PowerLine.h"
#include "City.h"
#include "Powerplant.h"

/*-----------------------------------------------------------------------------------------------*/

PowerNet::PowerNet(void)
{
	REGISTER_CALLBACK(eUpdatePowerNet, PowerNet::updatePowerNet);

	updateEfficiency();
}

/*-----------------------------------------------------------------------------------------------*/

void PowerNet::removeNode(boost::shared_ptr< Collidable > pNode)
{
	for (unsigned int k = 0; k < num_vertices(mGraph); k++) {
		if (mGraph[vertex(k,mGraph)].mObject->getID() == pNode->getID()) {	// found node

			boost::graph_traits< PowerNetGraph >::out_edge_iterator lEdgeI, lEdgeIEnd, lNext;
			tie(lEdgeI, lEdgeIEnd) = out_edges(vertex(k,mGraph), mGraph);

			for (lNext = lEdgeI; lEdgeI != lEdgeIEnd; lEdgeI = lNext) {
        // remove all out edges (power lines) 
				++lNext;
				
				mGraph[edge(source(*lEdgeI, mGraph), target(*lEdgeI, mGraph),
					mGraph).first].mObject->removeFromSceneManager();

				remove_edge(*lEdgeI, mGraph);
			}

			remove_vertex(vertex(k, mGraph), mGraph);	// remove node
		}
	}

	updateDistribution();
}

/*-----------------------------------------------------------------------------------------------*/

bool PowerNet::addBuildNodes(std::vector< boost::shared_ptr< Collidable > > pNodes,
							 std::vector< boost::shared_ptr< PowerLine > > pLines)
{
	bool lReturn = false;

	for (unsigned int i = 0; i < pNodes.size(); i++)	{
    // check if node is already in graph (i.e. 2 connections to city)
		bool lAlreadyInGraph = false;
		for (unsigned int k = 0; k < num_vertices(mGraph); k++) {
			if (mGraph[vertex(k,mGraph)].mObject->getID() == pNodes[i]->getID())
				lAlreadyInGraph = true;
		}

		if (!lAlreadyInGraph)
			add_vertex(VertexProperties(pNodes[i]), mGraph);	// add the nodes

		if (i > 0) {
			vertex_descriptor lV1, lV2;

			for(unsigned int k = 0; k < num_vertices(mGraph); k++) { 
        // find vertices to attach edge to
				if(mGraph[vertex(k, mGraph)].mObject->getID() == pNodes[i-1]->getID())
					lV1 = vertex(k, mGraph);
				if(mGraph[vertex(k, mGraph)].mObject->getID() == pNodes[i]->getID())
					lV2 = vertex(k, mGraph);
			}

			lReturn = isLineInGraph(pLines[i-1]);

			if(!lReturn)
				add_edge(lV1, lV2, EdgeProperties(pLines[i-1]), mGraph);
		}
	}

	updateDistribution();

	return lReturn;
}

/*-----------------------------------------------------------------------------------------------*/

bool PowerNet::isLineInGraph(boost::shared_ptr< PowerLine > pLine)
{
	boost::graph_traits< PowerNetGraph >::edge_iterator lEdgeI, lEdgeIEnd;
	boost::tie(lEdgeI, lEdgeIEnd) = edges(mGraph);

	for (boost::tie(lEdgeI, lEdgeIEnd) = edges(mGraph); lEdgeI != lEdgeIEnd; ++lEdgeI) {
		Ogre::Vector2 lExistingFrom = mGraph[edge(source(*lEdgeI, mGraph),
			target(*lEdgeI, mGraph), mGraph).first].mObject->getFrom();
		Ogre::Vector2 lExistingTo = mGraph[edge(source(*lEdgeI, mGraph),
			target(*lEdgeI, mGraph), mGraph).first].mObject->getTo();

		Ogre::Vector2 lRequestFrom = pLine->getFrom();
		Ogre::Vector2 lRequestTo = pLine->getTo();

		if (((lRequestFrom == lExistingFrom) && (lRequestTo == lExistingTo))
			||((lRequestFrom == lExistingTo) && (lRequestTo == lExistingFrom))) {
			return true;
		}
	}

	return false;
}

/*-----------------------------------------------------------------------------------------------*/

bool PowerNet::lineColliding(boost::shared_ptr< PowerLine > pLine)
{
	boost::graph_traits< PowerNetGraph >::edge_iterator lEdgeI, lEdgeIEnd;
	boost::tie(lEdgeI, lEdgeIEnd) = edges(mGraph);

	for (boost::tie(lEdgeI, lEdgeIEnd) = edges(mGraph); lEdgeI != lEdgeIEnd; ++lEdgeI) {
		if (intersectLines(	pLine->getFrom(),
							pLine->getSuggestionPosition(),
							mGraph[edge(source(*lEdgeI, mGraph),
							target(*lEdgeI, mGraph), mGraph).first].mObject->getFrom(),
							mGraph[edge(source(*lEdgeI, mGraph),
							target(*lEdgeI, mGraph), mGraph).first].mObject->getTo())) {
			return true;
		}
	}

	return false;
}

/*-----------------------------------------------------------------------------------------------*/

void PowerNet::alignPoles(void)
{
	for (unsigned int i = 0; i < num_vertices(mGraph); i++) {
		std::vector< boost::shared_ptr< GameObject > > lAdjacents;
		std::vector< boost::shared_ptr< PowerLine > > lLines;

		boost::graph_traits<PowerNetGraph>::out_edge_iterator lEdgeI, lEdgeIEnd, lNextEdge;
		tie(lEdgeI, lEdgeIEnd) = out_edges(vertex(i, mGraph), mGraph);

		for (lNextEdge = lEdgeI; lEdgeI != lEdgeIEnd; lEdgeI = lNextEdge) {
      // iterate through all out edges (power lines)
			++lNextEdge;

			lAdjacents.push_back(mGraph[target(*lEdgeI, mGraph)].mObject);
			lLines.push_back(mGraph[edge(source(*lEdgeI, mGraph),
				target(*lEdgeI, mGraph), mGraph).first].mObject);
		}

		boost::shared_ptr< GameObject > lSource = mGraph[vertex(i, mGraph)].mObject;

		if (lSource->getType() == ePole) {
			if (lAdjacents.size() == 1) {
				boost::dynamic_pointer_cast< Pole >(lSource)->removeIntersection();

				Ogre::Vector2 lRotateTo;
				
				if(lSource->getPosition().x > lAdjacents[0]->getPosition().x) {
					lRotateTo = (lAdjacents[0]->getPosition() 
						- lSource->getPosition()).perpendicular();
				} else {
					lRotateTo = (lSource->getPosition() 
						- lAdjacents[0]->getPosition()).perpendicular();
				}

				Ogre::Radian lRotate = Ogre::Math::ACos(
					lRotateTo.normalisedCopy().dotProduct(Ogre::Vector2(1.0,0.0)));

				boost::dynamic_pointer_cast< Pole >(lSource)->setRotation(lRotate);

				lLines[0]->adjust(boost::dynamic_pointer_cast< PowerNetVertex >(lSource), 
								  boost::dynamic_pointer_cast< PowerNetVertex >(lAdjacents[0]));
			}
			else if ((lAdjacents.size() == 2) 
				    || (lAdjacents.size() == 3) 
				    || (lAdjacents.size() == 4)) {
				boost::dynamic_pointer_cast< Pole >(lSource)->removeIntersection();

				Ogre::Vector2 lRotateTo;

				if (lAdjacents[0]->getPosition().x > lAdjacents[1]->getPosition().x) {
					lRotateTo = (lAdjacents[1]->getPosition() 
						- lAdjacents[0]->getPosition()).perpendicular();
				} else {
					lRotateTo = (lAdjacents[0]->getPosition() 
						- lAdjacents[1]->getPosition()).perpendicular();
				}

				Ogre::Radian lRotate = Ogre::Math::ACos(
					lRotateTo.normalisedCopy().dotProduct(Ogre::Vector2(1.0,0.0)));
				boost::dynamic_pointer_cast< Pole >(lSource)->setRotation(lRotate);

				lLines[0]->adjust(boost::dynamic_pointer_cast<PowerNetVertex>(lSource), 
					boost::dynamic_pointer_cast< PowerNetVertex >(lAdjacents[0]));

				lLines[1]->adjust(boost::dynamic_pointer_cast<PowerNetVertex>(lSource), 
					boost::dynamic_pointer_cast< PowerNetVertex >(lAdjacents[1]));

				for (unsigned int i = 2; i < lAdjacents.size(); i++) {
					boost::dynamic_pointer_cast< Pole >(lSource)->changeIntoIntersection();

					lLines[i]->adjust(boost::dynamic_pointer_cast< PowerNetVertex >(lSource), 
						boost::dynamic_pointer_cast< PowerNetVertex >(lAdjacents[i]));
				}
			}
		} else {
			for (unsigned int k = 0; k < lAdjacents.size(); k++) {
				lLines[k]->adjust(boost::dynamic_pointer_cast< PowerNetVertex >(lSource), 
					boost::dynamic_pointer_cast< PowerNetVertex >(lAdjacents[k]));
			}
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void PowerNet::redistribute(PowerNetGraph pGraph)
{
	std::vector< vertex_descriptor > lVerticesToRemoveNextIteration;

	for (unsigned int i = 0; i < num_vertices(pGraph); i++) {
		if (pGraph[vertex(i, pGraph)].mObject->getType() == ePowerplant) {	// for all powerplants ...
			std::vector< int > lDistances(num_vertices(pGraph));
			std::vector< vertex_descriptor > lPredecessor(num_vertices(pGraph));

			boost::dijkstra_shortest_paths(pGraph, vertex(i, pGraph),	// ... get shortest path to all vertices, ...
				weight_map(get(&EdgeProperties::mWeight, pGraph))
				.distance_map(make_iterator_property_map(lDistances.begin(), 
				get(boost::vertex_index, pGraph)))
				.predecessor_map(&lPredecessor[0]));

			std::vector< int > lDistancesMaxTemp = lDistances;
			std::vector< int > lDistancesMax = lDistances;
			std::sort(lDistancesMaxTemp.begin(), lDistancesMaxTemp.end());

			for (unsigned int l = 0; l < lDistancesMax.size(); l++)	// ... remove unreachable vertices ...
				if(lDistancesMaxTemp[l] > cMaxUniqueVertexDistance)
					lDistancesMax.assign(lDistancesMaxTemp.begin(),
					lDistancesMaxTemp.begin() + l);

			sort(lDistancesMax.begin(), lDistancesMax.end()); // ... and sort the rest according to distance

			for (int j = 1; j <= lDistancesMax[lDistancesMax.size() - 1]; j++) {
        // iterate through citys with closest first
				for (unsigned int k = 0; k < lDistances.size(); k++) {	
					// for all citys connected to powerplant ...
					if ((lDistances[k] == j) && (pGraph[vertex(k, pGraph)].mObject->getType() == eCity)) {
						int lDest = k;
						int lSource = i;

						std::vector< int > lPath;

						int lCurrentNode = lDest;
						int lPredecessorNode = 0;

						lPath.push_back(lDest);

						do {  // ... find path from city to powerplant
							lPredecessorNode = lPredecessor[lCurrentNode];

							if (lPredecessorNode == lCurrentNode) {
								// unreachable
								break;
							}
							else if (lPredecessorNode == lSource) {
								// next step found
								break;
							} else {
								// one step on the path
								lCurrentNode = lPredecessorNode;
								lPath.push_back(lCurrentNode);
							}
						}
						while (true);

						int lPowerGiven = 
							boost::dynamic_pointer_cast< City >(pGraph[vertex(lDest, pGraph)].mObject)
							->getPowerNeeded();

						int lPowerLeft = 
							boost::dynamic_pointer_cast< Powerplant >(pGraph[vertex(lSource, pGraph)].mObject)
							->getPowerLeft();

						if (lPowerLeft < lPowerGiven)
							lPowerGiven = lPowerLeft;

						if ((lPowerGiven > 0) && (lPowerLeft > 0)) {
							for (int m = lPath.size() - 1; m >= 0; m--) {
								if (pGraph[vertex(lPath[m], pGraph)].mObject->getType() == ePole) {
									boost::shared_ptr< Pole > lPole = 
										boost::dynamic_pointer_cast<Pole>(pGraph[vertex(lPath[m],
										pGraph)].mObject);

									if (lPole->getCapacityLeft() >= lPowerGiven) {
										lPole->addStrain(lPowerGiven);
									} else {
										lPowerGiven = lPole->getCapacityLeft();

										lPole->addStrain(lPowerGiven);

										lVerticesToRemoveNextIteration.push_back(
											vertex(lPath[m], pGraph));
									}
								}
								else if (pGraph[vertex(lPath[m], pGraph)].mObject->getType() == eCity) {
									boost::shared_ptr< City > lCity = boost::dynamic_pointer_cast< City> (
										pGraph[vertex(lPath[m], pGraph)].mObject);

									if (m == 0) {
										lCity->addPowerSupplied(lPowerGiven);

										if (lPowerGiven > 0) {
											lCity->addSupplier(boost::dynamic_pointer_cast<Powerplant>(
												pGraph[vertex(lSource, pGraph)].mObject)
												->getName() + ": " + toString(lPowerGiven) + "MW");
										}
									} else {
										continue; // not the city we want, assume unlimited transfer capacity
									}
								} else {
									continue; // assume powerplants have unlimited transfer capacity
								}
							}

							boost::dynamic_pointer_cast< Powerplant >(
								pGraph[vertex(lSource, pGraph)].mObject)
								->addPowerUsed(lPowerGiven);

							if (lPowerGiven > 0) {
								boost::dynamic_pointer_cast< Powerplant >(
									pGraph[vertex(lSource, pGraph)].mObject)
									->addReceiver(boost::dynamic_pointer_cast< City >(
									pGraph[vertex(lPath[0], pGraph)].mObject)
									->getName() +": "+ toString(lPowerGiven) + "MW");
							}
						}
					}
				}
			}
		}
	}

	// remove poles that have reached their full capacity
	if (lVerticesToRemoveNextIteration.size() > 0) {	// could contain equal items
		// sort for stl::unique
		std::sort(lVerticesToRemoveNextIteration.begin(),
			lVerticesToRemoveNextIteration.end(),
			std::greater<vertex_descriptor>());

		for (std::vector<vertex_descriptor>::iterator it = lVerticesToRemoveNextIteration.begin();
			it < unique(lVerticesToRemoveNextIteration.begin(),
			lVerticesToRemoveNextIteration.end());
			it++) { // iterate through all unique poles and remove them
			clear_vertex(*it, pGraph);
			remove_vertex(*it, pGraph);
		}

		redistribute(pGraph); // redistribute remaining power
	}
}

/*-----------------------------------------------------------------------------------------------*/

void PowerNet::updateDistribution(void)
{
	// reset all power distribution values
	for (unsigned int n = 0; n < num_vertices(mGraph); n++) {
		boost::dynamic_pointer_cast< PowerNetVertex >(mGraph[vertex(n, mGraph)].mObject)->reset();
	}

	redistribute(mGraph);
	alignPoles();
	updateEfficiency();

	EventHandler::raiseEvent(eMinimapModeChange, new EventArg< int >(-1));
	EventHandler::raiseEvent(eUpdatePowerNetInfo);
}

/*-----------------------------------------------------------------------------------------------*/

unsigned int PowerNet::getSubnetCount(void) {
	int lNumberSubnets = 0;
	std::vector< int > lSubnet(num_vertices(mGraph));

	if(lSubnet.size() > 0) {
		lNumberSubnets = connected_components(mGraph, &lSubnet[0]);
	}

	return lNumberSubnets;
}

/*-----------------------------------------------------------------------------------------------*/

std::vector< boost::shared_ptr< Powerplant > > PowerNet::getSubnetPowerplants(int pSubnetIndex) {
	std::vector< boost::shared_ptr< Powerplant > > lReturn;
	int lNumberSubnets = 0;
	std::vector< int > lSubnet(num_vertices(mGraph));

	if (lSubnet.size() > 0)
		lNumberSubnets = connected_components(mGraph, &lSubnet[0]);

	for (int i = 0; i != lSubnet.size(); ++i) {
		if (mGraph[vertex(i, mGraph)].mObject->getType() == ePowerplant) {
			if (lSubnet[i] == pSubnetIndex) {
				lReturn.push_back(boost::dynamic_pointer_cast<Powerplant>(
					mGraph[vertex(i, mGraph)].mObject));
			}
		}
	}

	return lReturn;
}

/*-----------------------------------------------------------------------------------------------*/

std::vector< boost::shared_ptr< City > > PowerNet::getSubnetCities(int pSubnetIndex)
{
	std::vector< boost::shared_ptr< City > > lReturn;
	int lNumberSubnets = 0;
	std::vector< int > lSubnet(num_vertices(mGraph));

	if(lSubnet.size() > 0)
		lNumberSubnets = connected_components(mGraph, &lSubnet[0]);

	for (int i = 0; i != lSubnet.size(); ++i) {
		if (mGraph[vertex(i, mGraph)].mObject->getType() == eCity) {
			if (lSubnet[i] == pSubnetIndex) {
				lReturn.push_back(boost::dynamic_pointer_cast<City>(
					mGraph[vertex(i, mGraph)].mObject));
			}
		}
	}

	return lReturn;
}

/*-----------------------------------------------------------------------------------------------*/

int PowerNet::getSubnetEfficiency(int pSubnetIndex)
{
	std::vector< boost::shared_ptr< City > > lCities = getSubnetCities(pSubnetIndex);
	std::vector <boost::shared_ptr< Powerplant > > lPowerplants
    = getSubnetPowerplants(pSubnetIndex);

	int lPowerConsumption = 0;
	int lPowerStillNeeded = 0;

	int lPowerAvailable = 0;
	int lPowerNotUsed = 0;

	for (unsigned int i = 0; i < lCities.size(); i++) {
		lPowerConsumption += lCities[i]->getConsumption();
		lPowerStillNeeded += lCities[i]->getPowerNeeded();
	}

	for (unsigned int j = 0; j < lPowerplants.size(); j++) {
		lPowerAvailable += lPowerplants[j]->getPowerInMW();
		lPowerNotUsed += lPowerplants[j]->getPowerLeft();
	}

	int lPowerGiven = lPowerAvailable - lPowerNotUsed;
	int lPowerConsumed = lPowerConsumption - lPowerStillNeeded;

	if ((lPowerAvailable == 0) || (lPowerConsumption == 0)) {
		return -1;
	}
	else if (lPowerAvailable > lPowerConsumption) {
		return ((((float)lPowerConsumption/(float)lPowerAvailable)
			- ((float)lPowerStillNeeded/(float)lPowerAvailable)) * 100);
	}
	else if (lPowerAvailable < lPowerConsumption) {
		return ((((float)lPowerAvailable/(float)lPowerConsumption)
			- ((float)lPowerNotUsed/(float)lPowerConsumption)) * 100);
	} else {
		return 100;
	}
}

/*-----------------------------------------------------------------------------------------------*/

int PowerNet::getEfficiency(void)
{
	return mEfficiency;
}

/*-----------------------------------------------------------------------------------------------*/

void PowerNet::updateEfficiency(void)
{
	int lPowerConsumption = 0;
	int lPowerStillNeeded = 0;

	int lPowerAvailable = 0;
	int lPowerNotUsed = 0;

	for (unsigned int n = 0; n < num_vertices(mGraph); n++) {
		if (mGraph[vertex(n, mGraph)].mObject->getType() == eCity) {
			lPowerConsumption += boost::dynamic_pointer_cast<City>(
				mGraph[vertex(n, mGraph)].mObject)->getConsumption();
			lPowerStillNeeded += boost::dynamic_pointer_cast<City>(
				mGraph[vertex(n, mGraph)].mObject)->getPowerNeeded();
		}
		else if(mGraph[vertex(n, mGraph)].mObject->getType() == ePowerplant) {
			lPowerAvailable += boost::dynamic_pointer_cast<Powerplant>(
				mGraph[vertex(n, mGraph)].mObject)->getPowerInMW();
			lPowerNotUsed += boost::dynamic_pointer_cast<Powerplant>(
				mGraph[vertex(n, mGraph)].mObject)->getPowerLeft();
		}
	}

	int lPowerGiven = lPowerAvailable - lPowerNotUsed;
	int lPowerConsumed = lPowerConsumption - lPowerStillNeeded;

	if ((lPowerAvailable == 0) || (lPowerConsumption == 0)) {
		mEfficiency = -1;
	}
	else if (lPowerAvailable > lPowerConsumption) {
		mEfficiency = (((float)lPowerConsumption/(float)lPowerAvailable)
			- ((float)lPowerStillNeeded/(float)lPowerAvailable)) * 100;
	}
	else if (lPowerAvailable < lPowerConsumption) {
		mEfficiency = (((float)lPowerAvailable/(float)lPowerConsumption)
			- ((float)lPowerNotUsed/(float)lPowerConsumption)) * 100;
	} else {
		mEfficiency = 100;
	}
}

/*-----------------------------------------------------------------------------------------------*/

void PowerNet::updatePowerNet(EventData* pData)
{
	updateDistribution();
}

/*-----------------------------------------------------------------------------------------------*/

void PowerNet::serializeIntoXMLElement(TiXmlElement* pParentElement)
{
	for (unsigned int n = 0; n < num_vertices(mGraph); n++) {
		if(mGraph[vertex(n, mGraph)].mObject->getType() == ePowerplant) {
			boost::shared_ptr< Powerplant > lPowerplant = 
				boost::dynamic_pointer_cast< Powerplant >(mGraph[vertex(n, mGraph)].mObject);
			TiXmlElement* lPowerplantSerialized = new TiXmlElement("pn_building");

			lPowerplantSerialized->SetAttribute("game_id", lPowerplant->getID().c_str());
			lPowerplantSerialized->SetAttribute("type_id", (int)ePowerplant);
			lPowerplantSerialized->SetAttribute("subtype_id", (int)lPowerplant->getSubtype());
			lPowerplantSerialized->SetAttribute("pos_x", lPowerplant->getPosition().x);
			lPowerplantSerialized->SetAttribute("pos_y", lPowerplant->getPosition().y);
			lPowerplantSerialized->SetAttribute("inherit", lPowerplant->getInheritSettings() ? "true" : "false");
			lPowerplantSerialized->SetAttribute("maintenance", lPowerplant->getMaintenance());
			lPowerplantSerialized->SetAttribute("damaged", lPowerplant->getDamaged() ? "true" : "false");
			lPowerplantSerialized->SetAttribute("year", lPowerplant->getConstructionYear());
			lPowerplantSerialized->SetAttribute("condition", lPowerplant->getCondition());

			pParentElement->LinkEndChild(lPowerplantSerialized);
		}
		if (mGraph[vertex(n, mGraph)].mObject->getType() == ePole) {
			boost::shared_ptr< Pole > lPole = 
				boost::dynamic_pointer_cast<Pole>(mGraph[vertex(n, mGraph)].mObject);
			TiXmlElement* lPoleSerialized = new TiXmlElement("pn_building");

			lPoleSerialized->SetAttribute("game_id", lPole->getID().c_str());
			lPoleSerialized->SetAttribute("type_id", (int)ePole);
			lPoleSerialized->SetAttribute("subtype_id", (int)lPole->getSubtype());
			lPoleSerialized->SetAttribute("pos_x", lPole->getPosition().x);
			lPoleSerialized->SetAttribute("pos_y", lPole->getPosition().y);

			pParentElement->LinkEndChild(lPoleSerialized);
		}
		if (mGraph[vertex(n, mGraph)].mObject->getType() == eCity) {
			boost::shared_ptr< City > lCity = 
				boost::dynamic_pointer_cast<City>(mGraph[vertex(n, mGraph)].mObject);
			TiXmlElement* lCitySerialized = new TiXmlElement("pn_building");

			lCitySerialized->SetAttribute("game_id", lCity->getID().c_str());
			lCitySerialized->SetAttribute("type_id", (int)eCity);
			lCitySerialized->SetAttribute("subtype_id", 0);
			lCitySerialized->SetAttribute("pos_x", 0);
			lCitySerialized->SetAttribute("pos_y", 0);
			lCitySerialized->SetAttribute("inherit", lCity->getInheritSettings() ? "true" : "false");
			lCitySerialized->SetAttribute("price", lCity->getPrice());
			lCitySerialized->SetAttribute("advertising", lCity->getAdvertising());
			lCitySerialized->SetAttribute("customers", lCity->getCustomers());

			pParentElement->LinkEndChild(lCitySerialized);
		}
	}

	boost::graph_traits< PowerNetGraph >::edge_iterator lEdgeI, lEdgeIEnd;
	boost::tie(lEdgeI, lEdgeIEnd) = edges(mGraph);

	for (boost::tie(lEdgeI, lEdgeIEnd) = edges(mGraph); lEdgeI != lEdgeIEnd; ++lEdgeI) {
		std::string lFromID = boost::dynamic_pointer_cast< Collidable >(
			mGraph[source(*lEdgeI, mGraph)].mObject)->getID();
		std::string lToID = boost::dynamic_pointer_cast< Collidable >(
			mGraph[target(*lEdgeI, mGraph)].mObject)->getID();

		TiXmlElement* lLineSerialized= new TiXmlElement("pn_line");
		lLineSerialized->SetAttribute("from_id", lFromID.c_str());
		lLineSerialized->SetAttribute("to_id", lToID.c_str());
		pParentElement->LinkEndChild(lLineSerialized);
	}
}

/*-----------------------------------------------------------------------------------------------*/

void PowerNet::deserializeFromXMLElement(TiXmlNode* pParentNode, boost::shared_ptr<Map> pMap,
										 Ogre::SceneManager* pSceneManager, boost::shared_ptr<Company> pCompany)
{
	TiXmlNode* lBuildingNode = pParentNode->FirstChildElement("pn_building");

	while (lBuildingNode) {
		int lID = toNumber< int >(lBuildingNode->ToElement()->Attribute("game_id"));
		eGameObjectType lType = 
			(eGameObjectType)toNumber< int >(lBuildingNode->ToElement()->Attribute("type_id"));
		eCompanyObjectType lSubtype = 
			(eCompanyObjectType)toNumber< int >(lBuildingNode->ToElement()->Attribute("subtype_id"));
		int lX = toNumber< int >(lBuildingNode->ToElement()->Attribute("pos_x"));
		int lY = toNumber< int >(lBuildingNode->ToElement()->Attribute("pos_y"));

		if (lType == ePowerplant) {
			boost::shared_ptr<Powerplant> lTempPlant;
			lTempPlant.reset(new Powerplant(Ogre::Vector2(lX, lY), lSubtype,
				pMap->findNearestCity(Ogre::Vector2(lX, lY)), pCompany,
				toNumber< int >(lBuildingNode->ToElement()->Attribute("year"))));
			lTempPlant->setID(lID);
			lTempPlant->addToSceneManager(pSceneManager, pMap->getTerrain());

			lTempPlant->setInheritSettings((((std::string)lBuildingNode->ToElement()
        ->Attribute("inherit")) == "false") ? false : true);
			lTempPlant->setMaintenance(toNumber<int>(
        lBuildingNode->ToElement()->Attribute("maintenance")));

			lTempPlant->setCondition(toNumber<int>(
        lBuildingNode->ToElement()->Attribute("condition")));

			if(((std::string)lBuildingNode->ToElement()->Attribute("damaged")) == "true")
				lTempPlant->setDamaged();

			pMap->addCollidable(lTempPlant, true);
			add_vertex(VertexProperties(lTempPlant), mGraph);
		}
		else if (lType == ePole) {
			boost::shared_ptr<Pole> lTempPole;
			lTempPole.reset(new Pole(Ogre::Vector2(lX, lY), lSubtype));
			lTempPole->setID(lID);
			lTempPole->addToSceneManager(pSceneManager, pMap->getTerrain());
			pMap->addCollidable(lTempPole, true);
			add_vertex(VertexProperties(lTempPole), mGraph);
		}
		else if (lType == eCity) {
			add_vertex(VertexProperties(pMap->getCityByID(lID)), mGraph);

			boost::dynamic_pointer_cast< City >(
				pMap->getCityByID(lID))->setInheritSettings(
				(((std::string)lBuildingNode->ToElement()->Attribute("price")) == "false") ? false : true);
			boost::dynamic_pointer_cast< City >(
				pMap->getCityByID(lID))->setPrice(
				toNumber<int>(lBuildingNode->ToElement()->Attribute("price")));
			boost::dynamic_pointer_cast< City >(
				pMap->getCityByID(lID))->setAdvertising(
				toNumber<int>(lBuildingNode->ToElement()->Attribute("advertising")));
			boost::dynamic_pointer_cast< City >(
				pMap->getCityByID(lID))->setCustomers(
				toNumber<int>(lBuildingNode->ToElement()->Attribute("customers")));
		}


		lBuildingNode = lBuildingNode->NextSiblingElement("pn_building");
	}

	lBuildingNode = pParentNode->FirstChildElement("pn_line");

	while(lBuildingNode) {
		int lFromID = toNumber< int >(lBuildingNode->ToElement()->Attribute("from_id"));
		int lToID = toNumber< int >(lBuildingNode->ToElement()->Attribute("to_id"));

		boost::shared_ptr< Collidable > lFromNode, lToNode;

		vertex_descriptor lV1, lV2;

		for(unsigned int k = 0; k < num_vertices(mGraph); k++) {
      // find vertices to attach edge to
			if(toNumber<int>(mGraph[vertex(k, mGraph)].mObject->getID()) == lFromID) {
				lV1 = vertex(k, mGraph);
				lFromNode = boost::dynamic_pointer_cast< Collidable >(
					mGraph[vertex(k, mGraph)].mObject);
			}
			if(toNumber<int>(mGraph[vertex(k, mGraph)].mObject->getID()) == lToID) {
				lV2 = vertex(k, mGraph);
				lToNode = boost::dynamic_pointer_cast< Collidable >(
					mGraph[vertex(k, mGraph)].mObject);
			}
		}

		boost::shared_ptr< PowerLine > lTempLine;
		lTempLine.reset(new PowerLine(lFromNode, lToNode, pMap->getTerrain(), pMap));
		lTempLine->addToSceneManager(pSceneManager);

		add_edge(lV1, lV2, EdgeProperties(lTempLine), mGraph);

		lBuildingNode = lBuildingNode->NextSiblingElement("pn_line");
	}

	updateDistribution();
}

/*-----------------------------------------------------------------------------------------------*/

size_t PowerNet::getLargePoleCount(void)
{
	size_t lVertexCount = num_vertices(mGraph);
	size_t lPoleCount = 0;

	for (int i = 0; i != lVertexCount; ++i)
		if(mGraph[vertex(i, mGraph)].mObject->getType() == ePole)
			if(boost::dynamic_pointer_cast< Pole >(
				mGraph[vertex(i, mGraph)].mObject)->getSubtype() == eDIPoleLarge)
				lPoleCount++;

	return lPoleCount;
}

/*-----------------------------------------------------------------------------------------------*/

size_t PowerNet::getSmallPoleCount(void)
{
	size_t lVertexCount = num_vertices(mGraph);
	size_t lPoleCount = 0;

	for (int i = 0; i != lVertexCount; ++i)
		if(mGraph[vertex(i, mGraph)].mObject->getType() == ePole)
			if(boost::dynamic_pointer_cast< Pole >(
				mGraph[vertex(i, mGraph)].mObject)->getSubtype() == eDIPoleSmall)
				lPoleCount++;

	return lPoleCount;
}

/*-----------------------------------------------------------------------------------------------*/

void PowerNet::destroyRandomPole(boost::shared_ptr<Map> pMap)
{
	while (true) {
		int lRandom = rand() % num_vertices(mGraph);

		if (mGraph[vertex(lRandom, mGraph)].mObject->getType() == ePole) {
			TickerMessage lPoleMessage;
			lPoleMessage.mMessage = StrLoc::get()->PoleCollapsed();
			lPoleMessage.mPointOfInterest = mGraph[vertex(lRandom, mGraph)].mObject->getPosition();
			lPoleMessage.mDetail = StrLoc::get()->PoleCollapsedDetail();
			EventHandler::raiseEvent(eTickerMessage, new EventArg< TickerMessage >(lPoleMessage));

			boost::dynamic_pointer_cast< Pole >(mGraph[vertex(lRandom, mGraph)].mObject)
        ->removeFromSceneManager();
			pMap->removeCollidable(mGraph[vertex(lRandom, mGraph)].mObject->getID());
			removeNode(
        boost::dynamic_pointer_cast< Collidable >(mGraph[vertex(lRandom, mGraph)].mObject));

			break;
		}
	}
}