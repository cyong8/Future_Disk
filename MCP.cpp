MCP::MCP(void)
{
}

MCP::~MCP(void)
{
	delete mRoot;
}

void MCP::createScene(void)
{
	// initializing light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0,0,0));
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
		/* more light sources here */

	// setting up the camera
	mCamera->setPosition(Ogre::Vector3(0.0f, (height/3.0f), (height* 2.0f/3.0f)));
    mCamera->lookAt(Ogre::Vector3(0.0f, 0.0f, 0.0f));

	// 10 planes for the room
}