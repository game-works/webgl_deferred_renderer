#define _USE_MATH_DEFINES
// #include <GL/gl.h>
// #include <GL/glu.h>
#include "graphics.h"
#include <vector>
#include <cmath>

// your framework of choice here

class SolidSphere
{
protected:
    // std::vector<GLfloat> vertices;
    // std::vector<GLfloat> normals;
    // std::vector<GLfloat> texcoords;
    struct Vertex
    {
      GLfloat position[3];
      GLfloat normal[3];
      GLfloat texcoord[2];
    };
    std::vector<Vertex> vertices;
    std::vector<GLushort> indices;

    // ...
    GLuint vertexbuffer = 0;
    GLuint indexbuffer = 0;

public:
    SolidSphere(float radius, unsigned int rings, unsigned int sectors)
    {
        float const R = 1./(float)(rings-1);
        float const S = 1./(float)(sectors-1);
        int r, s;

        // vertices.resize(rings * sectors * 3);
        // normals.resize(rings * sectors * 3);
        // texcoords.resize(rings * sectors * 2);
        // std::vector<GLfloat>::iterator v = vertices.begin();
        // std::vector<GLfloat>::iterator n = normals.begin();
        // std::vector<GLfloat>::iterator t = texcoords.begin();
        // for(r = 0; r < rings; r++) for(s = 0; s < sectors; s++) {
        //         float const y = sin( -M_PI_2 + M_PI * r * R );
        //         float const x = cos(2*M_PI * s * S) * sin( M_PI * r * R );
        //         float const z = sin(2*M_PI * s * S) * sin( M_PI * r * R );
        //
        //         *t++ = s*S;
        //         *t++ = r*R;
        //
        //         *v++ = x * radius;
        //         *v++ = y * radius;
        //         *v++ = z * radius;
        //
        //         *n++ = x;
        //         *n++ = y;
        //         *n++ = z;
        // }
        GLfloat vVertices2[] = {
        1.f,  1.f, 0.0f, 1.f, 1.0f,
      -1.f, -1.f, 0.0f, 0.f, 0.0f,
       1.f, -1.f, 0.0f, 1.f, 0.0f,
       1.f,  1.f, 0.0f, 1.f, 1.0f,
      	-1.f,  1.f, 0.0f, 0.f, 1.0f,
       -1.f, -1.f, 0.0f, 0.f, 0.0f,
        };


        vertices.resize(rings * sectors);
        std::vector<Vertex>::iterator v = vertices.begin();
        for(r = 0; r < rings; r++) for(s = 0; s < sectors; s++) {
                float const y = sin( -M_PI_2 + M_PI * r * R );
                float const x = cos(2*M_PI * s * S) * sin( M_PI * r * R );
                float const z = sin(2*M_PI * s * S) * sin( M_PI * r * R );

                (*v).texcoord[0] = s*S;
                (*v).texcoord[1] = r*R;

                 (*v).position[0] = x * radius;
                 (*v).position[1] = y * radius;
                (*v).position[2] = z * radius;

                (*v).normal[0] = x;
                (*v).normal[1] = y;
                (*v).normal[2] = z;
                v++;

                // (*v).texcoord[0] = vVertices2[5*(s%6)+3];
                // (*v).texcoord[1] = vVertices2[5*(s%6)+4];
                //
                // //(*v).position[0] = 0.25*vVertices2[5*(s%6)+0];
                // //(*v).position[1] = 0.25*vVertices2[5*(s%6)+1];
                // (*v).position[2] = 0.25*vVertices2[5*(s%6)+2];
                //
                // (*v).normal[0] = vVertices2[5*(s%6)+0];
                // (*v).normal[0] = vVertices2[5*(s%6)+1];
                // (*v).normal[0] = vVertices2[5*(s%6)+2];
                // v++;
        }

        //indices.resize(rings * sectors * 4);
        indices.resize(rings * sectors * 6);
        std::vector<GLushort>::iterator i = indices.begin();
        for(r = 0; r < rings; r++) for(s = 0; s < sectors; s++) {
                *i++ = r * sectors + s;
                *i++ = r * sectors + (s+1);
                //*i++ = (r+1) * sectors + (s+1);
                *i++ = (r+1) * sectors + s;

                //*i++ = r * sectors + s;
                *i++ = (r+1) * sectors + s;
                *i++ = r * sectors + (s+1);
                *i++ = (r+1) * sectors + (s+1);
        }

        // ...
        if (vertexbuffer == 0)
        {
          // Generate 1 buffer, put the resulting identifier in vertexbuffer
          glGenBuffers(1, &vertexbuffer);
          // The following commands will talk about our 'vertexbuffer' buffer
          glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
          // Give our vertices to OpenGL.
          glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
          glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        if (indexbuffer == 0)
        {
          glGenBuffers(1, &indexbuffer);
          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
          glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);
          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    }

    void draw(GLfloat x, GLfloat y, GLfloat z)
    {
        //glMatrixMode(GL_MODELVIEW);
        //glPushMatrix();
        //glTranslatef(x,y,z);

        //glEnableClientState(GL_VERTEX_ARRAY);
        //glEnableClientState(GL_NORMAL_ARRAY);
        //glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        // glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
        // glNormalPointer(GL_FLOAT, 0, &normals[0]);
        // glTexCoordPointer(2, GL_FLOAT, 0, &texcoords[0]);
        // glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_SHORT, &indices[0]);
        //glPopMatrix();

        // - - -

        // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, &vertices[0]);
        // glEnableVertexAttribArray(0);
        // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, &normals[0]);
        // glEnableVertexAttribArray(1);
        //
        // //glDrawArrays(GL_TRIANGLES, 0, 3);
        // glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, &indices[0]);

        // - - -

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
        float* ptr = 0;
        glVertexAttribPointer(
           0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
           3,                  // size
           GL_FLOAT,           // type
           GL_FALSE,           // normalized?
           8*sizeof(GLfloat),                  // stride
           (void*)(ptr+0)            // array buffer offset
        );
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
           1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
           3,                  // size
           GL_FLOAT,           // type
           GL_FALSE,           // normalized?
           8*sizeof(GLfloat),                  // stride
           (void*)(ptr+3)            // array buffer offset
        );

        // Draw the triangle !
        //glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, &indices[0]);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, NULL);
        //glDrawArrays(GL_TRIANGLES, 0, 12);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

    }
};
