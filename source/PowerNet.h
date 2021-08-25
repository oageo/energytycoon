#ifndef POWERNET_H
#define POWERNET_H

/*-----------------------------------------------------------------------------------------------*/

#include "Collidable.h"
#include "Event.h"

/*-----------------------------------------------------------------------------------------------*/

class Map;
class City;
class Powerplant;
class Company;
class PowerLine;

/*-----------------------------------------------------------------------------------------------*/

const int cMaxUniqueVertexDistance = 4096; //!< Maximum distance from any PowerNetVertex to another

/*-----------------------------------------------------------------------------------------------*/

//! Interface for all vertices in powernet
class PowerNetVertex {
public:

	//! Reset for redistribution, clear transformator stations
	virtual void reset(void) = 0;

	//! Return points to attach powerlines to
	virtual std::vector<Ogre::Vector3> getLineAttachingPoints(Ogre::Vector2 pAdjacentVertexPosition,
													bool pCheckOnly = false) = 0;

	//! Get number of attached single lines
	virtual size_t getAttachedCount(void) = 0;

	//! Return the position in 2d map space
	virtual Ogre::Vector2 getPosition(void) = 0;
};

/*-----------------------------------------------------------------------------------------------*/

//! Vertex properties for powernet graph
struct VertexProperties {
	VertexProperties() {}
	VertexProperties(boost::shared_ptr< GameObject > pObject) : mObject(pObject) {}

	boost::shared_ptr< GameObject > mObject;
};

/*-----------------------------------------------------------------------------------------------*/

//! Edge properties for powernet graph
//! Edges are always "PowerLine" objects
struct EdgeProperties {
	EdgeProperties() {}
	//! weight only for boosts dijkstra shortest path implementation
	EdgeProperties(boost::shared_ptr<PowerLine> pObject) : mObject(pObject) 
	{ 
		mWeight = (boost::edge_weight_t)1; 
	}

	boost::edge_weight_t mWeight;
	boost::shared_ptr< PowerLine > mObject;
};

/*-----------------------------------------------------------------------------------------------*/

// Some shortcuts for graph iteration
typedef boost::adjacency_list< boost::setS, boost::vecS, boost::undirectedS,
								VertexProperties, EdgeProperties > PowerNetGraph;
typedef boost::graph_traits < PowerNetGraph >::vertex_descriptor vertex_descriptor;
typedef boost::graph_traits < PowerNetGraph >::edge_descriptor edge_descriptor;

/*-----------------------------------------------------------------------------------------------*/

//! Represents all power nets in the game
class PowerNet {
public:
	
	//! Create it
	PowerNet(void);

	//! Remove a single node and its edges
	void removeNode(boost::shared_ptr< Collidable > pNode);

	//! Add a set of nodes and lines to the network
	bool addBuildNodes(std::vector< boost::shared_ptr< Collidable > > pNodes,
		std::vector< boost::shared_ptr< PowerLine > > pLines);

	//! Check if build request line is colliding
	bool lineColliding(boost::shared_ptr<PowerLine> pLine);

	//! Get average efficiency of all (at least one) power net
	int getEfficiency(void);

	//! Get number of independent subnets
	size_t getSubnetCount(void);

	//! Return all powerplants in a specific subnet
	std::vector<boost::shared_ptr<Powerplant> > getSubnetPowerplants(int pSubnetIndex);

	//! Return all cities in a specific subnet
	std::vector<boost::shared_ptr<City> > getSubnetCities(int pSubnetIndex);

	//! Get the efficiency of a specific subnet
	int getSubnetEfficiency(int SubnetIndex);

	//! Serialize
	void serializeIntoXMLElement(TiXmlElement* pParentElement);

	//! Load
	void deserializeFromXMLElement(TiXmlNode* pParentNode, boost::shared_ptr< Map > pMap,
								   Ogre::SceneManager* pSceneManager, boost::shared_ptr< Company > pCompany);

	//! Get number of poles
	size_t getLargePoleCount(void);

	//! Get number of poles
	size_t getSmallPoleCount(void);

	//! Destroy random pole (supply maintenance problem)
	void destroyRandomPole(boost::shared_ptr< Map > pMap);

private:

	//! Check if power line is already in graph
	bool isLineInGraph(boost::shared_ptr< PowerLine > pLine);

	//! Update the efficiency
	void updateEfficiency(void);

	//! Update event handler
	void updatePowerNet(EventData* pData);

	//! Align poles AND lines
	void alignPoles(void);

	//! Redistribute (recursively)
	void redistribute(PowerNetGraph pGraph);

	//! Update the complete powernet
	void updateDistribution(void);


	PowerNetGraph mGraph; //!< The graph
	int mEfficiency; //!< Current efficiency
};

/*-----------------------------------------------------------------------------------------------*/

#endif // POWERNET_H