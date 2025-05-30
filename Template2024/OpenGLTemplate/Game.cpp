/* 
OpenGL Template for INM376 / IN3005
City University London, School of Mathematics, Computer Science and Engineering
Source code drawn from a number of sources and examples, including contributions from
 - Ben Humphrey (gametutorials.com), Michal Bubner (mbsoftworks.sk), Christophe Riccio (glm.g-truc.net)
 - Christy Quinn, Sam Kellett and others

 For educational use by Department of Computer Science, City University London UK.

 This template contains a skybox, simple terrain, camera, lighting, shaders, texturing

 Potential ways to modify the code:  Add new geometry types, shaders, change the terrain, load new meshes, change the lighting, 
 different camera controls, different shaders, etc.
 
 Template version 5.0a 29/01/2017
 Dr Greg Slabaugh (gregory.slabaugh.1@city.ac.uk) 

 version 6.0a 29/01/2019
 Dr Eddie Edwards (Philip.Edwards@city.ac.uk)

 version 6.1a 13/02/2022 - Sorted out Release mode and a few small compiler warnings
 Dr Eddie Edwards (Philip.Edwards@city.ac.uk)

*/


#include "game.h"
#include "CatmullRom.h"


// Setup includes
#include "HighResolutionTimer.h"
#include "GameWindow.h"

// Game includes
#include "Camera.h"
#include "Skybox.h"
#include "Plane.h"
#include "Shaders.h"
#include "FreeTypeFont.h"
#include "Sphere.h"
#include "MatrixStack.h"
#include "OpenAssetImportMesh.h"
#include "Audio.h"
#include "Diamond.h"
#include "Cube.h"

// Constructor
Game::Game()
{
	m_pSkybox = NULL;
	m_pCamera = NULL;
	m_pShaderPrograms = NULL;
	m_pPlanarTerrain = NULL;
	m_pFtFont = NULL;
	m_pBarrelMesh = NULL;
	m_pHorseMesh = NULL;
	m_pPoliceCarMesh = NULL;
	m_pCarMesh = NULL;
	m_pRock = NULL;
	m_pSphere = NULL;
	m_pHighResolutionTimer = NULL;
	m_pAudio = NULL;
	m_pCatmullRom = NULL;
	m_pDiamond = NULL;
	m_pCube = NULL;


	m_dt = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;
	m_currentDistance = 0.0f;
	m_currentDistance1 = 20.0f;
	m_currentDistance2 = -100.0f;
	m_multiplier = 0.05f;
	m_cameraRotation = 0.0f;
	m_offSet = 0.0f;
	m_bCam = false;
	m_bAlive = true;
	m_Speed = 0.05;
	m_score = 0.0;
	m_topScore = 0.0;
	m_scoreMultiplier = 1.0;
}

// Destructor
Game::~Game() 
{ 
	//game objects
	delete m_pCamera;
	delete m_pSkybox;
	delete m_pPlanarTerrain;
	delete m_pFtFont;
	delete m_pBarrelMesh;
	delete m_pHorseMesh;
	delete m_pCarMesh;
	delete m_pPoliceCarMesh;
	delete m_pRock;
	delete m_pSphere;
	delete m_pAudio;
	delete m_pCatmullRom;
	delete m_pDiamond;
	delete m_pCube;


	if (m_pShaderPrograms != NULL) {
		for (unsigned int i = 0; i < m_pShaderPrograms->size(); i++)
			delete (*m_pShaderPrograms)[i];
	}
	delete m_pShaderPrograms;

	//setup objects
	delete m_pHighResolutionTimer;
}

// Initialisation:  This method only runs once at startup
void Game::Initialise() 
{
	// Set the clear colour and depth
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);


	/// Create objects
	m_pCamera = new CCamera;
	m_pSkybox = new CSkybox;
	m_pShaderPrograms = new vector <CShaderProgram *>;
	m_pPlanarTerrain = new CPlane;
	m_pFtFont = new CFreeTypeFont;
	m_pBarrelMesh = new COpenAssetImportMesh;
	m_pHorseMesh = new COpenAssetImportMesh;
	m_pCarMesh = new COpenAssetImportMesh;
	m_pPoliceCarMesh = new COpenAssetImportMesh;
	m_pRock = new COpenAssetImportMesh;
	m_pSphere = new CSphere;
	m_pAudio = new CAudio;
	m_pCatmullRom = new CCatmullRom;
	m_pDiamond = new CDiamond;
	m_pCube = new CCube;


	//m_pCatmullRom->CreatePath(p0,p1,p2,p3);
	m_pCatmullRom -> CreateCentreline();
	m_pCatmullRom->CreateOffsetCurves();
	m_pCatmullRom->CreatePath("resources\\textures\\black-gypsum-wall.jpg"); //https://www.freepik.com/free-photo/black-gypsum-wall_1037501.htm#query=asphalt%20texture%20seamless&position=1&from_view=keyword&track=ais&uuid=dad16982-4819-4efd-b576-3032b7b4c1f1#position=1&query=asphalt%20texture%20seamless
	m_pCube->Create("resources\\textures\\concrete-wall-texture.jpg");
	m_t = 0;
	m_spaceShipPosition = glm::vec3(0,0,0);
	m_spaceShipOrientation = glm::mat4(0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0);



	RECT dimensions = m_gameWindow.GetDimensions();

	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

	// Set the orthographic and perspective projection matrices based on the image size
	m_pCamera->SetOrthographicProjectionMatrix(width, height); 
	m_pCamera->SetPerspectiveProjectionMatrix(45.0f, (float) width / (float) height, 0.5f, 5000.0f);

	// Load shaders
	vector<CShader> shShaders;
	vector<string> sShaderFileNames;
	sShaderFileNames.push_back("mainShader.vert");
	sShaderFileNames.push_back("mainShader.frag");
	sShaderFileNames.push_back("textShader.vert");
	sShaderFileNames.push_back("textShader.frag");
	sShaderFileNames.push_back("diamondShader.vert");
	sShaderFileNames.push_back("diamondShader.frag");
	sShaderFileNames.push_back("lightingShader.vert");
	sShaderFileNames.push_back("lightingShader.frag");
	//sShaderFileNames.push_back("mainShader.vert");
	//sShaderFileNames.push_back("mainShader.frag");


	for (int i = 0; i < (int) sShaderFileNames.size(); i++) {
		string sExt = sShaderFileNames[i].substr((int) sShaderFileNames[i].size()-4, 4);
		int iShaderType;
		if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
		else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
		else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
		else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
		else iShaderType = GL_TESS_EVALUATION_SHADER;
		CShader shader;
		shader.LoadShader("resources\\shaders\\"+sShaderFileNames[i], iShaderType);
		shShaders.push_back(shader);
	}

	// Create the main shader program
	CShaderProgram *pMainProgram = new CShaderProgram;
	pMainProgram->CreateProgram();
	pMainProgram->AddShaderToProgram(&shShaders[0]);
	pMainProgram->AddShaderToProgram(&shShaders[1]);
	pMainProgram->LinkProgram();
	m_pShaderPrograms->push_back(pMainProgram);

	// Create a shader program for fonts
	CShaderProgram *pFontProgram = new CShaderProgram;
	pFontProgram->CreateProgram();
	pFontProgram->AddShaderToProgram(&shShaders[2]);
	pFontProgram->AddShaderToProgram(&shShaders[3]);
	pFontProgram->LinkProgram();
	m_pShaderPrograms->push_back(pFontProgram);

	CShaderProgram* pDiamondProgram = new CShaderProgram;
	pDiamondProgram->CreateProgram();
	pDiamondProgram->AddShaderToProgram(&shShaders[4]);
	pDiamondProgram->AddShaderToProgram(&shShaders[5]);
	pDiamondProgram->LinkProgram();
	m_pShaderPrograms->push_back(pDiamondProgram);

	// You can follow this pattern to load additional shaders

	// Create the skybox
	// Skybox downloaded from http://www.akimbo.in/forum/viewtopic.php?f=10&t=9
	m_pSkybox->Create(2500.0f);
	
	// Create the planar terrain
	m_pPlanarTerrain->Create("resources\\textures\\", "Sci-fi_Floor_003_basecolor.jpg", 2000.0f, 2000.0f, 50.0f); // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013

	m_pFtFont->LoadSystemFont("arial.ttf", 32);
	m_pFtFont->SetShaderProgram(pFontProgram);

	// Load some meshes in OBJ format
	m_pBarrelMesh->Load("resources\\models\\Barrel\\Barrel02.obj");  // Downloaded from http://www.psionicgames.com/?page_id=24 on 24 Jan 2013
	m_pHorseMesh->Load("resources\\models\\Horse\\Horse2.obj");  // Downloaded from http://opengameart.org/content/horse-lowpoly on 24 Jan 2013
	m_pCarMesh->Load("resources\\models\\Car\\bmw.obj");
	m_pPoliceCarMesh->Load("resources\\models\\Car\\policesedan.3ds");
	m_pRock->Load("resources\\models\\Rock\\stones.obj");

	// Create a sphere
	m_pSphere->Create("resources\\textures\\", "dirtpile01.jpg", 25, 25);  // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013
	m_pDiamond->Create();
	glEnable(GL_CULL_FACE);

	// Initialise audio and play background music
	m_pAudio->Initialise();
	m_pAudio->LoadEventSound("resources\\Audio\\Boing.wav");					// Royalty free sound from freesound.org
	//m_pAudio->LoadMusicStream("resources\\Audio\\DST-Garote.mp3");	// Royalty free music from http://www.nosoapradio.us/
	m_pAudio->PlayMusicStream();

	for (int i = 0; i < 20; i++) {
		RockPositions.push_back(m_pCatmullRom->RandomPos());
	}

	for (int i = 0; i < 20; i++) {
		DiamondPositions.push_back(m_pCatmullRom->RandomPos());
	}
}

// Render method runs repeatedly in a loop
void Game::Render()
{

	// Clear the buffers and enable depth testing (z-buffering)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();

	// Use the main shader program 
	CShaderProgram* pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("sampler0", 0);
	// Note: cubemap and non-cubemap textures should not be mixed in the same texture unit.  Setting unit 10 to be a cubemap texture.
	int cubeMapTextureUnit = 10;
	pMainProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);


	// Set the projection matrix
	pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());

	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
	// Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
	modelViewMatrixStack.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();
	glm::mat3 viewNormalMatrix = m_pCamera->ComputeNormalMatrix(viewMatrix);


	// Set light and materials in main shader program
	glm::vec4 lightPosition1 = glm::vec4(-100, 100, -100, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light1.position", viewMatrix * lightPosition1); // Position of light source *in eye coordinates*
	pMainProgram->SetUniform("light1.La", glm::vec3(0.5f));		// Ambient colour of light
	pMainProgram->SetUniform("light1.Ld", glm::vec3(0.5f));		// Diffuse colour of light
	pMainProgram->SetUniform("light1.Ls", glm::vec3(0.5f));		// Specular colour of light
	pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.0f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(0.0f));	// Specular material reflectance
	pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property


	// Render the skybox and terrain with full ambient reflectance 
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("renderSkybox", true);
	// Translate the modelview matrix to the camera eye point so skybox stays centred around camera
	glm::vec3 vEye = m_pCamera->GetPosition();
	modelViewMatrixStack.Translate(vEye);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pSkybox->Render(cubeMapTextureUnit);
	pMainProgram->SetUniform("renderSkybox", false);
	modelViewMatrixStack.Pop();

	// Render the planar terrain
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pPlanarTerrain->Render();
	modelViewMatrixStack.Pop();


	// Turn on diffuse + specular materials
	pMainProgram->SetUniform("material1.Ma", glm::vec3(0.5f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.5f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(1.0f));	// Specular material reflectance	


	// Render the horse 
	/*modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(0.0f, 0.0f, 0.0f));
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
		modelViewMatrixStack.Scale(2.5f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pHorseMesh->Render();
	modelViewMatrixStack.Pop();*/

	if (m_bAlive) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_spaceShipPosition);
		modelViewMatrixStack *= m_spaceShipOrientation;
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(180.0f));
		modelViewMatrixStack.Scale(0.02f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pCarMesh->Render();
		modelViewMatrixStack.Pop();

		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_PoliceCarPosition);
		modelViewMatrixStack *= m_PoliceCarOrientation;
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(90.0f));
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(90.0f));
		modelViewMatrixStack.Scale(2.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pPoliceCarMesh->Render();
		modelViewMatrixStack.Pop();

		for (int i = 0; i < 7; i++) {
			modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate(RockPositions[i]);
			modelViewMatrixStack.Scale(2.0f);
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			m_pRock->Render();
			modelViewMatrixStack.Pop();
		}


	}
	//render diamond------------------------
	CShaderProgram* pDiamondProgram = (*m_pShaderPrograms)[2];
	pDiamondProgram->UseProgram();
	// Set the projection matrix
	pDiamondProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());



	// Set light and materials in main shader program
	pDiamondProgram->SetUniform("light1.position", viewMatrix * lightPosition1); // Position of light source *in eye coordinates*
	pDiamondProgram->SetUniform("light1.La", glm::vec3(1.0f));		// Ambient colour of light
	pDiamondProgram->SetUniform("light1.Ld", glm::vec3(1.0f));		// Diffuse colour of light
	pDiamondProgram->SetUniform("light1.Ls", glm::vec3(1.0f));		// Specular colour of light
	pDiamondProgram->SetUniform("material1.Ma", glm::vec3(1.0f));	// Ambient material reflectance
	pDiamondProgram->SetUniform("material1.Md", glm::vec3(0.0f));	// Diffuse material reflectance
	pDiamondProgram->SetUniform("material1.Ms", glm::vec3(0.0f));	// Specular material reflectance
	pDiamondProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property

	pDiamondProgram->SetUniform("material1.Ma", glm::vec3(0.5f));	// Ambient material reflectance
	pDiamondProgram->SetUniform("material1.Md", glm::vec3(0.5f));	// Diffuse material reflectance
	pDiamondProgram->SetUniform("material1.Ms", glm::vec3(1.0f));	// Specular material reflectance

	for (int i = 0; i < 5; i++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(DiamondPositions[i]);
		modelViewMatrixStack.Scale(0.1f);
		pDiamondProgram->SetUniform("modelViewMatrix", modelViewMatrixStack.Top());
		pDiamondProgram->SetUniform("normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		pDiamondProgram->SetUniform("projectionMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
		m_pDiamond->Render();
		modelViewMatrixStack.Pop();
	}
	
	pMainProgram->UseProgram();
	// Render the barrel 
	/*modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(100.0f, 0.0f, 0.0f));
		modelViewMatrixStack.Scale(5.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pBarrelMesh->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(150.0f, 0.0f, 0.0f));
	modelViewMatrixStack.Scale(5.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pBarrelMesh->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(200.0f, 0.0f, 0.0f));
	modelViewMatrixStack.Scale(5.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pBarrelMesh->Render();
	modelViewMatrixStack.Pop();
	*/
	

	// Render the sphere
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(0.0f, 2.0f, 150.0f));
		modelViewMatrixStack.Scale(2.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		// To turn off texture mapping and use the sphere colour only (currently white material), uncomment the next line
		//pMainProgram->SetUniform("bUseTexture", false);
		m_pSphere->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(0.0f, 6.0f, 160.0f));
	modelViewMatrixStack.Scale(2.0f * 3);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	// To turn off texture mapping and use the sphere colour only (currently white material), uncomment the next line
	//pMainProgram->SetUniform("bUseTexture", false);
	m_pSphere->Render();
	modelViewMatrixStack.Pop();

	
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("bUseTexture", false); // turn off texturing
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix",
		m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	// Render your object here
	m_pCatmullRom->RenderCentreline();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("bUseTexture", false); // turn off texturing
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix",
		m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	// Render your object here
	//m_pCatmullRom->RenderOffsetCurves();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("bUseTexture", true); // turn off texturing
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix",
		m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	// Render your object here
	m_pCatmullRom->RenderPath();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("bUseTexture", true); // turn off texturing
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix",
		m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	// Render your object here
	m_pCube->Render();
	modelViewMatrixStack.Pop();
		
	// Draw the 2D graphics after the 3D graphics
	DisplayFrameRate();

	// Swap buffers to show the rendered image
	SwapBuffers(m_gameWindow.Hdc());		

}

// Update method runs repeatedly with the Render method
void Game::Update() 
{
	// Update the camera using the amount of time that has elapsed to avoid framerate dependent motion
	m_pCamera->Update(m_dt);
	m_score += m_dt;
	
	static float t = 0.0f;
	t += 0.0003f * (float)m_dt;
	if (t > 1.0f)
		t = 0.0f;

	m_currentDistance += m_dt * m_Speed;
	glm::vec3 p;
	glm::vec3 pNext;
	m_pCatmullRom->Sample(m_currentDistance, p);
	m_pCatmullRom->Sample(m_currentDistance +1.0f, pNext);
	if(m_bCam)
		m_pCatmullRom->Sample(m_currentDistance - 1.0f, pNext);
	glm::vec3 T = glm::normalize(pNext - p);
	glm::vec3 y(0, 1, 0);
	glm::vec3 N = glm::normalize(glm::cross(T, y));
	glm::vec3 B = glm::normalize(glm::cross(N, T));

	//-------------------------------------------
	m_currentDistance1 += m_dt * m_Speed;
	glm::vec3 p1;
	glm::vec3 pNext1;
	m_pCatmullRom->Sample(m_currentDistance1, p1);
	m_pCatmullRom->Sample(m_currentDistance1 + 1.0f, pNext1);
	glm::vec3 T1 = glm::normalize(pNext1 - p1);
	glm::vec3 y1(0, 1, 0);
	glm::vec3 N1 = glm::normalize(glm::cross(T1, y1));
	glm::vec3 B1 = glm::normalize(glm::cross(N1, T1));
	//-------------------------------------------------------------------------------------
	
	m_currentDistance2 += m_dt * m_multiplier;
	glm::vec3 p2;
	glm::vec3 pNext2;
	m_pCatmullRom->Sample(m_currentDistance2, p2);
	m_pCatmullRom->Sample(m_currentDistance2 + 1.0f, pNext2);
	glm::vec3 T2 = glm::normalize(pNext2 - p2);
	glm::vec3 y2(0, 1, 0);
	glm::vec3 N2 = glm::normalize(glm::cross(T2, y2));
	glm::vec3 B2 = glm::normalize(glm::cross(N2, T2));


	glm::vec3 offSetPosition = p1 + (m_offSet * N1);
	m_spaceShipPosition = offSetPosition;
	//m_spaceShipPosition = p1;
	//m_spaceShipPosition = p + 20.0f * T;
	m_spaceShipOrientation = glm::mat4(glm::mat3(T1, B1, N1));

	glm::vec3 offSetPosition2 = p2 + (m_offSet * N2);
	m_PoliceCarPosition = offSetPosition2;
	m_PoliceCarOrientation = glm::mat4(glm::mat3(T2, B2, N2));
	m_multiplier += 0.000001f;
	//----------------------------------------------------------------------------
	glm::vec3 up = glm::normalize(glm::rotate(glm::vec3(0, 1, 0), m_cameraRotation, T));

	p.y = 8.0f;

	glm::vec3 viewPoint = p + 10.0f * T;
	if(m_bAlive)
	m_pCamera->Set(p, viewPoint, up);

	if (glm::distance(m_PoliceCarPosition, m_spaceShipPosition) <= 12.0f) {
		m_bAlive = false;
	}
	
	for (int i = 0; i < 6; i++) {
		if (glm::length(RockPositions[i] - m_spaceShipPosition) < 6.0f) {
			m_Speed = m_Speed*0.95;
			RockPositions[i] = glm::vec3(0.0f, 0.0f, 0.0f); //try to store objects themselves in vectors rather than just the positions
			break;
		}
	}
	
	for (int i = 0; i < 6; i++) {
		if (glm::length(DiamondPositions[i] - m_spaceShipPosition) < 4.0f) {
			m_Speed = m_Speed * 1.10;
			DiamondPositions[i] = glm::vec3(0.0f, 0.0f, 0.0f); //try to store objects themselves in vectors rather than just the positions
			break;
		}
	}

	//glm::vec3 point = m_pCatmullRom->pointOnCircle(radius,t, center);
	//m_pCatmullRom->angle = m_pCatmullRom->angle + 0.01;
	//if (m_pCatmullRom->angle > 360)
	//	m_pCatmullRom->angle = 1.0;
	
	//glm::vec3 p = m_pCatmullRom->Interpolate(p0, p1, p2, p3, t);
	//m_pCamera->Set(glm::vec3 (point.x,50,point.z), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	m_pAudio->Update();
}




void Game::DisplayFrameRate()
{


	CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;

	// Increase the elapsed time and frame counter
	m_elapsedTime += (m_dt*m_scoreMultiplier);
	m_frameCount++;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1000)
    {
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;

		// Reset the frames per second
		m_frameCount = 0;
    }

	if (m_framesPerSecond > 0) {
		// Use the font shader program and render the text
		fontProgram->UseProgram();
		glDisable(GL_DEPTH_TEST);
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		m_pFtFont->Render(20, height - 20, 20, "FPS: %d", m_framesPerSecond);
	}

	glDisable(GL_DEPTH_TEST);
	fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
	fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
	fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	m_pFtFont->Render(20, height - 40, 20, "CAPSLOCK to change camera", NULL);

	fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
	fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
	fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	m_pFtFont->Render(300, height - 40, 20, "Score %f", m_score);


	if (!m_bAlive) {
		glDisable(GL_DEPTH_TEST);
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		m_pFtFont->Render(200, height - 200, 50, "GAME OVER !", NULL);
	}
}

// The game loop runs repeatedly until game over
void Game::GameLoop()
{
	/*
	// Fixed timer
	dDt = pHighResolutionTimer->Elapsed();
	if (dDt > 1000.0 / (double) Game::FPS) {
		pHighResolutionTimer->Start();
		Update();
		Render();
	}
	*/
	
	
	// Variable timer
	m_pHighResolutionTimer->Start();
	Update();
	Render();
	m_dt = m_pHighResolutionTimer->Elapsed();
	

}


WPARAM Game::Execute() 
{
	m_pHighResolutionTimer = new CHighResolutionTimer;
	m_gameWindow.Init(m_hInstance);

	if(!m_gameWindow.Hdc()) {
		return 1;
	}

	Initialise();

	m_pHighResolutionTimer->Start();

	
	MSG msg;

	while(1) {													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
			if(msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		} else if (m_appActive) {
			GameLoop();
		} 
		else Sleep(200); // Do not consume processor power if application isn't active
	}

	m_gameWindow.Deinit();

	return(msg.wParam);
}

LRESULT Game::ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param) 
{
	LRESULT result = 0;

	switch (message) {


	case WM_ACTIVATE:
	{
		switch(LOWORD(w_param))
		{
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				m_appActive = true;
				m_pHighResolutionTimer->Start();
				break;
			case WA_INACTIVE:
				m_appActive = false;
				break;
		}
		break;
		}

	case WM_SIZE:
			RECT dimensions;
			GetClientRect(window, &dimensions);
			m_gameWindow.SetDimensions(dimensions);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;

	case WM_KEYDOWN:
		switch(w_param) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case '1':
			m_pAudio->PlayEventSound();
			break;
		case VK_F1:
			m_pAudio->PlayEventSound();
			break;
		case VK_LEFT:
			//m_cameraRotation -= m_dt * 0.005f;
			if (m_offSet > -10.0f) {
				m_offSet -= m_dt * 0.08f;
			}
			break;
		case VK_RIGHT:
			//m_cameraRotation += m_dt * 0.005f;
			if (m_offSet < 10.0f) {
				m_offSet += m_dt * 0.08f;
			}
			break;
		case VK_CAPITAL:
			m_bCam = !m_bCam;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		result = DefWindowProc(window, message, w_param, l_param);
		break;
	}

	return result;
}

Game& Game::GetInstance() 
{
	static Game instance;

	return instance;
}

void Game::SetHinstance(HINSTANCE hinstance) 
{
	m_hInstance = hinstance;
}

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return Game::GetInstance().ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int) 
{
	Game &game = Game::GetInstance();
	game.SetHinstance(hinstance);

	return int(game.Execute());
}
