/*
 *  Mesh.h
 *  The OpenGL Obj Loader
 *
 *  Created by Seb on 07/12/05.
 *  Copyright 2005 The Old Game Factory. All rights reserved.
 *
 */

#ifndef __MESH_HPP__
#define __MESH_HPP__

#include "Vertex.hpp"
#include "PolygonFace.hpp"
#include "Material.hpp"

class CTexture;


//
class CMesh {
public:
	CMesh();

	void clear();
		//  PROMISE: this mesh is empty and ready to be reused.
	void animate(const float dt);
	void draw() const;
		//  REQUIRE: must be const.
	void drawBox() const;
		//  REQUIRE: must be const.

	void addVertex(const SVertex& v);
		//	does some bounds assignations too.
	void addTextureCoord(const SVertex& vt) { m_texVector.push_back(vt); }
	void addNormal(const SVertex& vn) { m_normVector.push_back(vn); }
	void addFace(const SPolygonFace& face) { m_faceVector.push_back(face); }
	void generateFlatNormals();
		//	PROMISE: Generates flat mesh normals. Does nothing if normals are already defined (in the parsed file).
	void setTexture(const std::shared_ptr<CTexture>& rhs);
	void setTexture(const std::string& strFilePath);
	void setMaterial(const SMaterial& mtl) { m_material = mtl; }

	const bool hasNormals() const { return static_cast<int>(m_normVector.size()) > FIRST_INDEX; }
	const bool hasTextureCoords() const { return static_cast<int>(m_texVector.size()) > FIRST_INDEX; }
	const bool hasTexture() const { return NULL != m_pTexture; }
	const int getNumVertices() const { return static_cast<int>(m_vertVector.size())-FIRST_INDEX; }
	const int getNumTextureCoords() const { return static_cast<int>(m_texVector.size())-FIRST_INDEX; }
	const int getNumNormals() const { return static_cast<int>(m_normVector.size())-FIRST_INDEX; }
	const int getNumFaces() const { return m_faceVector.size(); }

	const int getByteSize() const;
		//  PROMISE: returns the size currently occupied by this mesh's containers.
	void shrinkToFit();
		//  PROMISE: all this mesh's containers' capacities are set back to minimum (i.e. their actual sizes).
	void dumpBounds() const;	//  Remove me: dev.func.

	SVertex m_minVert;
	SVertex m_maxVert;

private:
	static const int FIRST_INDEX;	//	0 or 1
	std::vector<SVertex> m_vertVector;	//	Vertex data
	std::vector<SVertex> m_texVector;	//	Texture coordinates data
	std::vector<SVertex> m_normVector;	//	Normal data

	std::vector<SPolygonFace> m_faceVector;	//	Face data
	std::shared_ptr<CTexture> m_pTexture;
	SMaterial m_material;
};

#endif //__MESH_HPP__
