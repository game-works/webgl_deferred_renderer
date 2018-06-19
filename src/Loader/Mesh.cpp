/*
 *  Mesh.cpp
 *  The OpenGL Obj Loader
 *
 *  Created by Seb on 07/12/05.
 *  Copyright 2005 The Old Game Factory. All rights reserved.
 *
 */
#include "Mesh.hpp"
#include "Texture.hpp"
#include <math.h>
// #include "graphics.h"
// #include "shaders.h"
#include <GLFW/glfw3.h>


// Static member
const int CMesh::FIRST_INDEX = 1;


// Does a few inits
CMesh::CMesh()
	: m_minVert(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max())
	, m_maxVert(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max())
	// : m_minVert(999.f, 999.f, 999.f)
	// , m_maxVert(-999.f, -999.f, -999.f)
{
	//	Pushing some dummy front values.
	//	Note: this is uggly, find a better way.
	m_vertVector.push_back(SVertex(0, 0, 0));
	m_texVector.push_back(SVertex(0, 0, 0));
	m_normVector.push_back(SVertex(0, 0, 0));
}


// Empties the mesh.
void CMesh::clear() {
	m_minVert.set(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	m_maxVert.set(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

	m_vertVector.clear();
	m_texVector.clear();
	m_normVector.clear();
	m_faceVector.clear();

	//	Cf. ctor.
	m_vertVector.push_back(SVertex(0, 0, 0));
	m_texVector.push_back(SVertex(0, 0, 0));
	m_normVector.push_back(SVertex(0, 0, 0));
}

void CMesh::generateFlatNormals()
{
	if (hasNormals())
		return;

	// for each face...
	for (std::vector<SPolygonFace>::iterator itFace = m_faceVector.begin(); itFace != m_faceVector.end(); ++itFace)
	{
		const SVertex& v1 = m_vertVector[(*itFace).v[0]];
		const SVertex& v2 = m_vertVector[(*itFace).v[1]];
		const SVertex& v3 = m_vertVector[(*itFace).v[2]];
		SVertex v(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
		SVertex w(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
		SVertex vn(v.y*w.z - v.z*w.y, v.z*w.x - v.x*w.z, v.x*w.y - v.y*w.x);
		float length = sqrtf(vn.x*vn.x+vn.y*vn.y+vn.z*vn.z);
		if (0 < length) {
			const float a = 1/length;
			vn.x *= a;
			vn.y *= a;
			vn.z *= a;
		}
		m_normVector.push_back(vn);
		//	register the generated normal through the face
		fill((*itFace).vn.begin(), (*itFace).vn.end(), m_normVector.size()-1);
	}
}


//
void CMesh::setTexture(const std::shared_ptr<CTexture>& rhs) {
	m_pTexture = rhs;
}
void CMesh::setTexture(const std::string& strFilePath) {
	setTexture(std::shared_ptr<CTexture>(new CTexture(strFilePath)));
}

//
void CMesh::animate(const float dt)
{
	//
	if (!hasTexture() && m_material.map_Kd != "")
		setTexture(m_material.map_Kd);
	//	pass the texture to OpenGL if needed
	if (hasTexture() && !m_pTexture->isRegistred())
		m_pTexture->registerGL();
}

//
void CMesh::draw() const
{
	// bool isTransparent;
	// // bind the texture
	// if (hasTexture()) {
	// 	glBindTexture (GL_TEXTURE_2D, m_pTexture->getID());
	// 	isTransparent = m_pTexture->isTransparent();
	// }
	// else isTransparent = false;
	//
	// bool isDepthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
	// if (isTransparent)
	// 	glDisable(GL_DEPTH_TEST);
	// glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_material.Ka);
	// glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_material.Kd);
	// glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_material.Ks);
	// glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_material.Ns);
	// draw each face...
	for (std::vector<SPolygonFace>::const_iterator itFace = m_faceVector.begin(); itFace != m_faceVector.end(); ++itFace)
	{
		// glColor3f(1, 1, 0);
		// glBegin(GL_POLYGON);
		// //	TODO: passer �� for_each
		// for (unsigned i = 0; i < (*itFace).v.size(); ++i) {
		// 	glNormal3fv(m_normVector[(*itFace).vn[i]].getVertex3fv());
		// 	if (hasTexture() && hasTextureCoords())
		// 		glTexCoord3fv(m_texVector[(*itFace).vt[i]].getVertex3fv());
		// 	glVertex3fv(m_vertVector[(*itFace).v[i]].getVertex3fv());
		// }
		// glEnd();	//	GL_POLYGON

		GLfloat vVertices[] = {
			0.0f, 0.5f, 0.0f,
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f
		};
		GLfloat vNormals[] = {
			0.f,  0.f, 1.0f,
			0.f,  0.f, 1.0f,
			0.f,  0.f, 1.0f
		};

		if ((*itFace).v.size() == 3)
			for (unsigned i = 0; i < 3; ++i)
			{
				vVertices[i * 3 + 0]  = m_vertVector[(*itFace).v[i]].getVertex3fv()[0];
				vVertices[i * 3 + 1]  = m_vertVector[(*itFace).v[i]].getVertex3fv()[1];
				vVertices[i * 3 + 2]  = m_vertVector[(*itFace).v[i]].getVertex3fv()[2];
			}

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, vNormals);
		glEnableVertexAttribArray(1);
		//draw the triangle
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	//	restore depth test
	// if (isDepthTestEnabled)
	// 	glEnable(GL_DEPTH_TEST);
	// else
	// 	glDisable(GL_DEPTH_TEST);

}


//
void CMesh::drawBox() const
{
	/*
	glColor3f(1, 0, 1);
	glPointSize(3);
	glBegin(GL_POINTS);
	glVertex3fv(m_minVert.getVertex3fv());
	glVertex3f(m_minVert.x, m_minVert.y, m_maxVert.z);
	glVertex3f(m_minVert.x, m_maxVert.y, m_minVert.z);
	glVertex3f(m_maxVert.x, m_minVert.y, m_minVert.z);
	glVertex3fv(m_maxVert.getVertex3fv());
	glVertex3f(m_maxVert.x, m_maxVert.y, m_minVert.z);
	glVertex3f(m_maxVert.x, m_minVert.y, m_maxVert.z);
	glVertex3f(m_minVert.x, m_maxVert.y, m_maxVert.z);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex3fv(m_minVert.getVertex3fv());
	glVertex3f(m_minVert.x, m_minVert.y, m_maxVert.z);
	glVertex3f(m_maxVert.x, m_minVert.y, m_maxVert.z);
	glVertex3f(m_maxVert.x, m_minVert.y, m_minVert.z);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex3fv(m_maxVert.getVertex3fv());
	glVertex3f(m_maxVert.x, m_maxVert.y, m_minVert.z);
	glVertex3f(m_minVert.x, m_maxVert.y, m_minVert.z);
	glVertex3f(m_minVert.x, m_maxVert.y, m_maxVert.z);
	glEnd();
	glBegin(GL_LINES);
	glVertex3fv(m_maxVert.getVertex3fv());
	glVertex3f(m_maxVert.x, m_minVert.y, m_maxVert.z);
	glVertex3f(m_minVert.x, m_maxVert.y, m_minVert.z);
	glVertex3f(m_minVert.x, m_minVert.y, m_minVert.z);
	glVertex3f(m_minVert.x, m_maxVert.y, m_maxVert.z);
	glVertex3f(m_minVert.x, m_minVert.y, m_maxVert.z);
	glVertex3f(m_maxVert.x, m_maxVert.y, m_minVert.z);
	glVertex3f(m_maxVert.x, m_minVert.y, m_minVert.z);
	glEnd();
	*/
}


//
void CMesh::addVertex(const SVertex& v)
{
	m_vertVector.push_back(v);
	//  x bound assignation
	if (v.x < m_minVert.x)
		m_minVert.x = v.x;
	if (m_maxVert.x < v.x)
		m_maxVert.x = v.x;
	//  y
	if (v.y < m_minVert.y)
		m_minVert.y = v.y;
	if (m_maxVert.y < v.y)
		m_maxVert.y = v.y;
	//  z
	if (v.z < m_minVert.z)
		m_minVert.z = v.z;
	if (m_maxVert.z < v.z)
		m_maxVert.z = v.z;
}


//
const int CMesh::getByteSize() const
{
	int size = sizeof(SVertex)*(m_vertVector.capacity()+m_texVector.capacity()+m_normVector.capacity());
	size += sizeof(SPolygonFace)*m_faceVector.capacity();
	//	go through the faces
	for (std::vector<SPolygonFace>::const_iterator it = m_faceVector.begin(); it != m_faceVector.end(); ++it)
		size += sizeof(int)*((*it).v.capacity()+(*it).vt.capacity()+(*it).vn.capacity());
	return size;
}


//	TODO: render this one useless if necessary. It's not an expensive operation though.
//	release unnecessarily allocated memory (uses the swap trick, cf. S. Meyers' Effective STL, Item 17)
void CMesh::shrinkToFit()
{
	std::vector<SVertex>(m_vertVector).swap(m_vertVector);
	std::vector<SVertex>(m_texVector).swap(m_texVector);
	std::vector<SVertex>(m_normVector).swap(m_normVector);
	std::vector<SPolygonFace>(m_faceVector).swap(m_faceVector);
	for (std::vector<SPolygonFace>::iterator it = m_faceVector.begin(); it != m_faceVector.end(); ++it) {
		std::vector<int>((*it).v).swap((*it).v);
		std::vector<int>((*it).vt).swap((*it).vt);
		std::vector<int>((*it).vn).swap((*it).vn);
	}
}


//
void CMesh::dumpBounds() const
{
	//CLogger::get() << "Mesh bounds: (" << m_minVert.x << ", " << m_minVert.y << ", " << m_minVert.z << ") , (";
	//CLogger::get() << m_maxVert.x << ", " << m_maxVert.y << ", " << m_maxVert.z << ").\n";
}
