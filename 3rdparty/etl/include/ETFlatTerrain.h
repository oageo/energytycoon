#ifndef __ET_FLATTERRAIN_H__
#define __ET_FLATTERRAIN_H__
/********************************************************************************
EDITABLE TERRAIN LIBRARY v3 for Ogre

Copyright (c) 2008 Holger Frydrych <frydrych@oddbeat.de>

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
********************************************************************************/

#include "ETTerrainDescription.h"
#include "ETEditable.h"
#include "ETMovable.h"
#include "ETUpdatable.h"

namespace ET
{
  class Brush;


  /**
   * The FlatTerrain class is a description of a flat piece of landscape generated from a heightmap.
   * It provides all the details of the vertices forming the terrain and has functions
   * to get the terrain's height at any position or do a ray query against the terrain.
   * Note, however, that the Terrain class is not a visual representation of your terrain.
   */
  class FlatTerrain : public TerrainDescription, public Editable, public Movable, public Updatable
  {
  public:
    /**
     * Create a Terrain from a 2D array of height values.
     * @param heights A 2D array containing the terrain heights
     * @param gridScale The scale of a single terrain vertex in 2D grid space units
     * @param maxHeight The maximum height of the terrain in Ogre units
     */
    FlatTerrain(const RealArray2D& heights, const Ogre::Vector2& gridScale, Ogre::Real maxHeight);

    ~FlatTerrain();

    /** Get the extents of the terrain in world space */
    Ogre::Vector3 getSize() const { return mSize; }

    /** Get access to the underlying 2D height array */
    const RealArray2D& getHeightmap() const { return mHeights; }

    /** Set new heightmap */
    void setHeightmap(const RealArray2D& array);

    /** Get a height value from the heightmap */
    Ogre::Real getHeightValue(unsigned int x, unsigned int z) const { return mHeights.at(x, z); }

    /** Change a height value in the heightmap */
    void setHeightValue(unsigned int x, unsigned int z, Ogre::Real value);

    /** Determine whether the given local position is inside the terrain's base plane bounds */
    bool isOverTerrain(Ogre::Real x, Ogre::Real z) const;
    /**
     * Retrieve the height of the terrain at the given position in local terrain space.
     * In local space, the terrain's lower left corner lies at the origin (0, 0).
     */
    Ogre::Real getHeightAt(Ogre::Real x, Ogre::Real z) const;

    /** Retrieve the terrain normal at the given position in local space */
    Ogre::Vector3 getNormalAt(Ogre::Real x, Ogre::Real z) const;

    /** Interpolate the terrain normal at the given position in local space from the vertex normals */
    Ogre::Vector3 interpolateNormalAt(Ogre::Real x, Ogre::Real z) const;

    /** Retrieve the terrain tangent at the given position in local space */
    Ogre::Vector3 getTangentAt(Ogre::Real x, Ogre::Real z) const;

    /** Retrieve the terrain binormal at the given position in local space */
    Ogre::Vector3 getBinormalAt(Ogre::Real x, Ogre::Real z) const;

    /** Determine whether the given world position is inside the terrain's base plane bounds */
    bool isOverTerrain(const Ogre::Vector3& pos) const;
    /**
     * Retrieve the height of the terrain at the given position in world space.
     * Technically, this will find the terrain intersection point along the normal
     * of the plane which the terrain lies in, starting from the given position.
     */
    Ogre::Vector3 getHeightAt(const Ogre::Vector3& pos) const;

    /** Retrieve the terrain normal at the given position in world space */
    Ogre::Vector3 getNormalAt(const Ogre::Vector3& pos) const;

    /** Retrieve the terrain tangent at the given position in world space */
    Ogre::Vector3 getTangentAt(const Ogre::Vector3& pos) const;

    /** Retrieve the terrain binormal at the given position in world space */
    Ogre::Vector3 getBinormalAt(const Ogre::Vector3& pos) const;

    /**
     * Perform a ray intersection test.
     * @param ray The ray in world space.
     * @return whether the ray hit the terrain and, if so, where.
     */
    std::pair<bool, Ogre::Vector3> rayIntersects(const Ogre::Ray& ray);


    /**
     * Deform the terrain at the given local position.
     * @param x X coordinate of local space position
     * @param z Z coordinate of local space position
     * @param brush The brush to use for the deformation
     * @param intensity Intensity with which to apply the brush
     */
    void deform(Ogre::Real x, Ogre::Real z, const Brush* brush, Ogre::Real intensity = 1.0);

    /** Deform at the given world space position. */
    void deform(const Ogre::Vector3& pos, const Brush* brush, Ogre::Real intensity = 1.0);


    /**
     * Carry out updating of any modifications made to the heightmap, that is
     * notify the listeners of the updated vertices. The Terrain itself is
     * updated immediately and does not require this.
     */
    virtual void update();


    /** Set a new vertex scale for the terrain */
    void setVertexScale(const Ogre::Vector2& gridScale, Ogre::Real maxHeight);


    //*******************************
    // TerrainDescription interface
    //*******************************

    /** Get the vertex scale in Ogre units */
    Ogre::Vector3 getVertexScale() const { return mScale; }
    /** Get the position of the i'th vertex */
    Ogre::Vector3 getVertexPosition(unsigned int x, unsigned int z) const;
    /** Get the normal of the i'th vertex */
    Ogre::Vector3 getVertexNormal(unsigned int x, unsigned int z) const;
    /** Get the tangent of the i'th vertex */
    Ogre::Vector3 getVertexTangent(unsigned int x, unsigned int z) const;
    /** Get the binormal of the i'th vertex */
    Ogre::Vector3 getVertexBinormal(unsigned int x, unsigned int z) const;
    /** Get the first texture coordinate of the i'th vertex */
    Ogre::Vector2 getVertexTexcoord0(unsigned int x, unsigned int z) const;
    /** Get the second texture coordinate of the i'th vertex */
    Ogre::Vector2 getVertexTexcoord1(unsigned int x, unsigned int z) const;
    /** Get the size of the triangle list of the index buffer (at full detail, no stitching) */
    unsigned int getTriListSize() const;
    /** Get the vertex index for the i'th position in the trilist */
    unsigned int getTriListIndex(unsigned int i) const;



  private:
    /** Test a single quad for ray intersection */
    std::pair<bool, Ogre::Vector3> checkQuadIntersection(int x, int z, const Ogre::Ray& ray);
    /** Find the quad the given coordinates are in */
    void getQuad(Ogre::Real posX, Ogre::Real posZ, unsigned int& lx, unsigned int& lz, Ogre::Real& fx, Ogre::Real& fz) const;

    RealArray2D mHeights;
    Ogre::Vector3 mVertexScale;
    Ogre::Vector3 mSize;
    /** List of dirty vertices for deferred updating. */
    VertexList mDirtyVertices;
  };
}

#endif
