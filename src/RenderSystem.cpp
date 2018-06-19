#include <RenderSystem.h>
#include "shaders.h"
#include "ObjLoader.h"
#include "Loader/Mesh.hpp"
#include "Loader/ObjMeshParser.hpp"
#include "Loader/Exception.hpp"
#include "SolidSphere.h"



GLint uniformOriginX, uniformOriginY, uniformOriginZ, uniformZoom;
GLint uniformDiffuse;
GLint uniform_s_GBuffer;
GLint lightPos, lightCol;

// Mesh 1
// std::vector<glm::vec4> vertices;
// std::vector<glm::vec3> normals;
//// std::vector<GLushort> elements;
std::shared_ptr<CMesh> pMesh(new CMesh());
std::shared_ptr<CMesh> pMesh2(new CMesh());
//CMesh* pMesh = 0;


RenderSystem::RenderSystem()
{
   // Geometry pass shaders

  	const char vertexShaderSource[] =
  		"attribute vec4 vPosition;		                     \n"
			"attribute vec4 vNormal;		                     \n"
  		"uniform float originX, originY, originZ;            \n"
  		"uniform float zoom;                                 \n"
      "uniform vec3 diffuse;  \n"
  		"varying vec3 color;                                 \n" // To FS
      "varying vec3 normal;                                 \n"
      "varying vec3 pos;                                 \n"

  		"void main()                                         \n"
  		"{                                                   \n"
  		"   gl_Position = vPosition;                         \n"
  		"   gl_Position.x = (originX + gl_Position.x) * zoom;\n"
  		"   gl_Position.y = (originY + gl_Position.y) * zoom;\n"
      "   gl_Position.z = (originZ + gl_Position.z) * zoom;\n"
      "   color = diffuse;             \n"
  		"   pos = vec3(0.5*gl_Position.x + 0.5, 0.5*gl_Position.y + 0.5, gl_Position.z);             \n"
      "   normal = vNormal.xyz;             \n"
  		"}                                                   \n";

  	const char fragmentShaderSource[] =
    "    #extension GL_EXT_draw_buffers : require \n"
    "   #extension GL_EXT_frag_depth : enable \n "
  //		"precision mediump float;                     \n"
  		 "precision highp float;                     \n"
  		 "varying vec3 color;                          \n"   // From VS
       "varying vec3 normal;                          \n"
       "varying vec3 pos;                          \n"
       "vec3 pack (vec3 normal) \n"
       "{ \n"
        "    float p = sqrt(normal.z*8.0+8.0); \n"
        "    return vec3(normal.xy/p + 0.5,0); \n"
       // "    return vec3(normal.x + 0.5, normal.y+0.5, normal.z); \n"

       "} \n"
  		"void main()                                  \n"
  		"{                                            \n"
       "  gl_FragData[0] = vec4 ( color, 1.0 );        \n"
       "  gl_FragData[1] = vec4 ( pack(normal), 1.0 );        \n"
       "  gl_FragData[2] = vec4 ( pos, 1.0 );        \n"
  		"}                                            \n";

  	//load vertex and fragment shaders
  	GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderSource);
  	GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
  	programObject = buildProgram(vertexShader, fragmentShader, "vPosition", "vNormal");

  	//save location of uniform variables
  	uniformOriginX = glGetUniformLocation(programObject, "originX");
  	uniformOriginY = glGetUniformLocation(programObject, "originY");
    uniformOriginZ  = glGetUniformLocation(programObject, "originZ");
  	uniformZoom = glGetUniformLocation(programObject, "zoom");
    uniformDiffuse = glGetUniformLocation(programObject, "diffuse");

		// Set up G-Buffer stuff
		unsigned int SCR_WIDTH = 1280;
		unsigned int SCR_HEIGHT = 720;

		glGenFramebuffers(1, &m_gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);

		// - position/color/normal buffer [create]
    GLuint buffer[3];
    glGenTextures(3, buffer);
    m_colBuffer = buffer[0];
    m_norBuffer = buffer[1];
    m_posBuffer = buffer[2];

		glBindTexture(GL_TEXTURE_2D, m_colBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, m_norBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, m_posBuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// - position/color/normal buffer [resize]
		glBindTexture(GL_TEXTURE_2D, m_colBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glBindTexture(GL_TEXTURE_2D, m_norBuffer);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RG, GL_FLOAT, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    //glBindTexture(GL_TEXTURE_2D, m_depBuffer);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    glBindTexture(GL_TEXTURE_2D, m_posBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

		// - fbo [resize]
		glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_norBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_posBuffer, 0);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depBuffer, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// ----
		const char vertexShaderSource2[] =
			"attribute vec3 vPosition;		                     \n"
			"attribute vec2 uv;																	\n"
			"varying vec3 color;                                 \n"
    	"varying vec2 vUV;                                 \n"
			"void main()                                         \n"
			"{                                                   \n"
			"   vUV = uv;																				\n"
			"   gl_Position = vec4(vPosition, 1.0);                         \n"
//			"   color = 0.005 * gl_Position.xyz + vec3(vUV.x, vUV.y, 0.5);             \n"
			"}                                                   \n";

		const char fragmentShaderSource2[] =
    "    #extension GL_EXT_draw_buffers : require \n"
    "   #extension GL_EXT_frag_depth : enable \n "
		//	"precision mediump float;                     \n"
      "precision highp float; \n"
    "uniform sampler2D BaseMap;									\n"
    "uniform sampler2D ReflectMap; \n"
    "uniform sampler2D PosMap; \n"
    "uniform sampler2D gBuffer[3]; \n"
    "uniform mat4 invProj; \n"
    "uniform vec2 viewport; \n"
    "uniform vec3 lightPos; \n"
    "uniform vec3 lightCol; \n"
//			"varying vec3 color;                          \n"
			"varying vec2 vUV;																	\n"
      "vec3 unpack(vec3 encoded) \n"
      "{  \n"
      "    vec2 fenc = encoded.xy*4.0 - 2.0; \n"
          "float f = dot(fenc,fenc); \n"
          "float g = sqrt(1.0 - f/4.0); \n"
          "vec3 normal; \n"
          "normal.xy = fenc*g; \n"
          "normal.z = 1.0 - f/2.0; \n"
          "return normal; \n"
            // "    return vec3(2.0*(encoded.x - 0.5), 2.0*(encoded.y-0.5), encoded.z); \n"
      "}\n"
			"void main()                                  \n"
			"{                                            \n"
			"  vec3 fragCol = texture2D(BaseMap, vUV).rgb; \n"
      "  vec3 fragNor = texture2D(ReflectMap, vUV).rgb; \n"
      "  vec3 normal = unpack(fragNor); \n"
      //"  float fragDepth = 0.2; /*texture2D(PosMap, vUV).r;*/ \n"
      "  vec3 fragPos = texture2D(PosMap, vUV).rgb; \n"
      "  vec3 world_position = vec3(fragPos.x - 0.5, fragPos.y - 0.5, fragPos.z); \n"
      //"  vec3 light_position = vec3(0.25, 0.25, -0.125); \n"
      "  vec3 light_position = lightPos; \n"
      "  float distance  = length(light_position - world_position); \n"
      "  float size = 0.60; \n"
      //"  float attenuation = 1.0 / (0.30 * pow(distance, 2.0)); \n"
      "  float attenuation = 1.0 - pow( clamp(distance/size, 0.0, 1.0), 2.0); \n"
      "  vec3 lightDir = normalize(light_position - world_position); \n"
      "  float light_transfer = max(dot(normal, lightDir), 0.0); \n"
      "  vec4 finalCol = vec4 ( attenuation * light_transfer * fragCol * lightCol, 1.0 ); \n"
      //"  gl_FragColor = vec4(fragPos, 1.0); \n"
      //"  gl_FragColor = vec4(normal, 1.0); \n"
      //"  gl_FragColor = vec4(vec3(1.0 - distance), 1.0); \n"
      "  gl_FragColor = finalCol; \n"
			// "  gl_FragColor = vec4 ( fragCol, 1.0 );        \n"
      // "  if (vUV[0] > 0.5) {  \n"
      // "     gl_FragColor = vec4 ( fragNor, 1.0 );        \n"
      // "       if (vUV[1] > 0.5) gl_FragColor = vec4 ( fragPos, 1.0 );        \n"
      // "  } else { \n"
      // "       if (vUV[1] > 0.5) gl_FragColor = finalCol;        \n"
      // "  } \n"
//      "  if (vUV[1] > 0.5)  gl_FragColor = vec4 ( vec3(vUV, 0.0), 1.0 );        \n"
//        "  gl_FragColor = vec4 ( vec3(vUV, 0.0), 1.0 );        \n"
			"}                                            \n";

		//load vertex and fragment shaders
		GLuint vertexShader2 = loadShader(GL_VERTEX_SHADER, vertexShaderSource2);
		GLuint fragmentShader2 = loadShader(GL_FRAGMENT_SHADER, fragmentShaderSource2);
		m_programObject2 = buildProgram(vertexShader2, fragmentShader2, "vPosition", "uv");

    uniform_s_GBuffer = glGetUniformLocation(m_programObject2, "s_GBuffer");
    int i[] = {0,1};
    glUniform1iv(uniform_s_GBuffer, 2, i);

    lightPos = glGetUniformLocation(m_programObject2, "lightPos");
    lightCol = glGetUniformLocation(m_programObject2, "lightCol");
}




RenderSystem::~RenderSystem()
{


}


void RenderSystem::setCommonUniforms() //shader, view, modelToWorldTransform)
{

}


void RenderSystem::drawObjModel() //model, modelToWorldTransform, view):
{
	static SolidSphere sphere(0.08, 12, 24);
  static SolidSphere earth(100.0, 120, 240);

  //enable our shader program
	glUseProgram(programObject);

	float list_x[] = { 1.1f, 2.2f, 3.3f, 4.4f,
										 1.1f, 2.2f, 3.3f, 4.4f,
									   1.1f, 2.2f, 3.3f, 4.4f,
									 	 1.1f, 2.2f, 3.3f, 4.4f };

  float list_y[] = { 1.2f, 1.1f, 1.2f, 1.1f,
										 2.1f, 2.2f, 2.1f, 2.2f,
									   3.2f, 3.1f, 3.2f, 3.1f,
									 	 4.1f, 4.2f, 4.1f, 4.2f };


  float colour0[] = { 1.0, 1.0,  1.0 };
  float colour1[] = { 0.7, 0.7,  0.7 };
  float colour2[] = { 0.9, 0.9,  0.9 };
  float colour3[] = { 0.85, 1.0,  0.85 };
  float colour4[] = { 0.85, 0.85,  0.85 };
  float colour5[] = { 0.85, 0.80,  1.0 };
  float colour6[] = { 0.5, 0.5,  0.5 };
  glUniform3fv(uniformDiffuse, 1, colour0);

  glUniform1f(uniformZoom, 0.4f);

  glUniform1f(uniformOriginZ, 99.90);
  glUniform1f(uniformOriginX, 0.0);
  glUniform1f(uniformOriginY, 0.0);
  earth.draw(1, 2, 3);

  glUniform1f(uniformOriginZ, 0);



  for (int x = 0; x < 3; ++x) for (int y = 0; y < 3; ++y)
  for (int i = 0; i < 16; ++i)
	{
		glUniform1f(uniformOriginX, 0.25*list_x[i]-x-y*0.25+0.5);
		glUniform1f(uniformOriginY, 0.25*list_y[i]-y);

    switch ((i+x+3*y)%7)
    {
      case 0: glUniform3fv(uniformDiffuse, 1, colour0); break;
      case 1: glUniform3fv(uniformDiffuse, 1, colour1); break;
      case 2: glUniform3fv(uniformDiffuse, 1, colour2); break;
      case 3: glUniform3fv(uniformDiffuse, 1, colour3); break;
      case 4: glUniform3fv(uniformDiffuse, 1, colour4); break;
      case 5: glUniform3fv(uniformDiffuse, 1, colour5); break;
      case 6: glUniform3fv(uniformDiffuse, 1, colour6); break;
    };

		sphere.draw(1, 2, 3);
	}
}


void RenderSystem::setupObjModelShader()
{


}


void  RenderSystem::draw()
{
	unsigned int SCR_WIDTH = 1280;
	unsigned int SCR_HEIGHT = 720;

	// 1st Setup
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);
  GLenum buffers3[] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
  };
  glDrawBuffers(3, buffers3);
  glCullFace(GL_BACK);

	// glBindTexture(GL_TEXTURE_2D, m_posBuffer);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_HALF_FLOAT_OES, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_posBuffer, 0);

	// G-Buffer pass
	draw1Pass();

	// Lighting setup
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // default_
  glDrawBuffers(1, buffers3);
  //glDrawBuffer(GL_NONE);

	// Lighting pass
	draw2Pass();
}


void  RenderSystem::draw1Pass()
{
	//glClearColor(1.f, 0.5, 0.63, 1);
  glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear( GL_COLOR_BUFFER_BIT );

  glBindTexture(GL_TEXTURE_2D, 0);

	drawObjModel();

  //RenderQuad();
}


void  RenderSystem::draw2Pass()
{
	//glClearColor(0, 0.1, 0.23, 1);
  glClearColor(0, 0, 0, 1);
	glClear( GL_COLOR_BUFFER_BIT );

	glClearColor(0, 0, 0, 1);
	glClear( GL_DEPTH_BUFFER_BIT );

  glUseProgram(m_programObject2);

  GLint texLoc;
  texLoc = glGetUniformLocation(m_programObject2, "BaseMap");
  glUniform1i(texLoc, 0);

  texLoc = glGetUniformLocation(m_programObject2, "ReflectMap");
  glUniform1i(texLoc, 1);

  texLoc = glGetUniformLocation(m_programObject2, "PosMap");
  glUniform1i(texLoc, 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_colBuffer);

  glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_norBuffer);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, m_posBuffer);


	//shaderLightingPass.setVec3("viewPos", camera.Position);

  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_ONE, GL_ONE);

  float position0[] = { 0.0, 0.0,  0.0 };
  // float position1[] = { 0.124, 0.125,  0.0 };
  // float position2[] = { 0.104, 0.100, -0.025 };
  // float position3[] = { 0.25, 0.25, -0.025 };

  float colour0[] = { 0.45, 0.005,  0.45 };
  float colour1[] = { 0.05, 0.105,  0.65 };
  float colour2[] = { 0.15, 0.45,  0.45 };
  float colour3[] = { 0.45, 0.45,  0.15 };
  float colour4[] = { 0.15, 0.65,  0.15 };
  float colour5[] = { 0.55, 0.19,  0.21 };
  float colour6[] = { 1.0, 1.0,  1.0 };
  // glUniform3fv(lightPos, 1, position1);
	// RenderQuad();
  //
  // glUniform3fv(lightPos, 1, position2);
  // RenderQuad();
  //
  // glUniform3fv(lightPos, 1, position3);
  // RenderQuad();

  for ( int i = 0; i < 8; ++i )
  {
    for ( int j = 0; j < 8; ++j )
    {
      // position0[0] = 1.2-0.101 * i;
      // position0[1] = 0-0.103 *    j;
      position0[0] = 0.11 * i - 0.225;//0.124 * (i % 4);
      position0[1] = 0.17 * j - 0.425;
      // position0[0] = (i + 10*j - 50) * 0.1250;
      // position0[1] = (i % 2 == 0) ? 0.25 : -0.15;
      position0[2] = -0.45;
      glUniform3fv(lightPos, 1, position0);


      switch (i%5) {
        case 0 : glUniform3fv(lightCol, 1, colour1); break;
        case 1 : glUniform3fv(lightCol, 1, colour2); break;
        case 2 : glUniform3fv(lightCol, 1, colour3); break;
        case 3 : glUniform3fv(lightCol, 1, colour4); break;
        case 4 : glUniform3fv(lightCol, 1, colour5); break;
      }

      // colour0[0] = 0.002;
      //colour0[2] = 0.002;
      //if (i%2 == j%2) {
      //}

      RenderQuad();
    }
  }

  glDisable(GL_BLEND);

  glBindTexture(GL_TEXTURE_2D, 0);
}


struct Vertex {
	GLfloat * p;
	GLfloat * uv;
};

void  RenderSystem::RenderQuad()
{

  Vertex v;

	GLfloat vVertices[] = {
		 1.f,  1.f, 0.0f,
		-1.f, -1.f, 0.0f,
		 1.f, -1.f, 0.0f,
		 1.f,  1.f, 0.0f,
 		-1.f,  1.f, 0.0f,
		-1.f, -1.f, 0.0f,
	};
	v.p = vVertices;

	GLfloat vUV[] = {
		 1.f, 1.0f,
		 0.f, 0.0f,
		 1.f, 0.0f,
		 1.f, 1.0f,
		 0.f, 1.0f,
		 0.f, 0.0f,
	};
	v.uv = vUV;

	GLfloat vVertices2[] = {
    1.f,  1.f, 0.0f, 1.f, 1.0f,
    -1.f, -1.f, 0.0f, 0.f, 0.0f,
    1.f, -1.f, 0.0f, 1.f, 0.0f,
    1.f,  1.f, 0.0f, 1.f, 1.0f,
 	  -1.f,  1.f, 0.0f, 0.f, 1.0f,
    -1.f, -1.f, 0.0f, 0.f, 0.0f,
	};

	//enable our shader program
	//glUseProgram(m_programObject2);

	// This will identify our vertex buffer
	static GLuint vertexbuffer = 0;
	if (vertexbuffer == 0)
	{
		// Generate 1 buffer, put the resulting identifier in vertexbuffer
		glGenBuffers(1, &vertexbuffer);
		// The following commands will talk about our 'vertexbuffer' buffer
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		// Give our vertices to OpenGL.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices2), vVertices2, GL_STATIC_DRAW);
	}

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	float* ptr = 0;
	glVertexAttribPointer(
	   0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
	   3,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   5*sizeof(GLfloat),                  // stride
	   (void*)(ptr+0)            // array buffer offset
	);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		 1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		 2,                  // size
		 GL_FLOAT,           // type
		 GL_FALSE,           // normalized?
		 5*sizeof(GLfloat),                  // stride
		 (void*)(ptr+3)            // array buffer offset
	);
	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 3 vertices total -> 1 triangle
	glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);

	//draw the triangle
	glDrawArrays(GL_TRIANGLES, 0, 6);

}
