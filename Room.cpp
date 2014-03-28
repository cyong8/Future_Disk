#include "Room.h"
#include <vector>

Room::Room(Ogre::SceneManager *mSceneMgr, Simulator *game_simulator, int ident)
{
	width = 30.0f;
	height = 60.0f;
	floorLength = 35.0f;
	Ogre::Real heightScalingFactor = 3.0f/4.0f;
	
	Ogre::Vector3 position;
	Ogre::Plane plane;
	plane.d = 0;

	Ogre::Real tileSize = 5.0f;
	Ogre::Real tileNum = (floorLength * width)/tileSize;

	Ogre::Real posX = -width/2 + tileSize/2;
	Ogre::Real posY = -(width*heightScalingFactor)/2.0f;
	Ogre::Real posZ = -(gapSize/2.0 + floorLength - tileSize/2.0);

	floorPosition = posY;

	if(ident == 0)
	{
		// Row 1
		position = Ogre::Vector3(-width/2 + tileSize/2, -(width*heightScalingFactor)/2.0f, -(gapSize/2.0 + floorLength + tileSize/2.0));
		(new Tile("client11", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client12", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client13", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client14", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client15", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client16", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();

		// Row 2
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize); // Reset x, increment z
		(new Tile("client21", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client22", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client23", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client24", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client25", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client26", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();

		// Row 3
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize); // Reset x, increment z
		(new Tile("client31", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client32", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client33", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client34", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client35", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client36", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();

		// Row 4
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize); // Reset x, increment z
		(new Tile("client41", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client42", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client43", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client44", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client45", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client46", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();

		// Row 5
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize); // Reset x, increment z
		(new Tile("client51", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client52", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client53", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client54", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client55", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client56", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();

		// Row 6
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize); // Reset x, increment z
		(new Tile("client61", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client62", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client63", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client64", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client65", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client66", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();

		// Row 7
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize); // Reset x, increment z
		(new Tile("client71", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client72", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client73", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client74", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client75", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client76", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();

		// Row 1
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize * 3);
		(new Tile("host11", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host12", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host13", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host14", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host15", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host16", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();

		// Row 2
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize); // Reset x, increment z
		(new Tile("host21", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host22", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host23", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host24", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host25", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host26", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();

		// Row 3
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize); // Reset x, increment z
		(new Tile("host31", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host32", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host33", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host34", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host35", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host36", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();

		// Row 4
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize); // Reset x, increment z
		(new Tile("host41", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host42", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host43", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host44", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host45", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host46", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();

		// Row 5
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize); // Reset x, increment z
		(new Tile("host51", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host52", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host53", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host54", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host55", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host56", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();

		// Row 6
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize); // Reset x, increment z
		(new Tile("host61", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host62", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host63", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host64", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host65", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host66", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();

		// Row 7
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize); // Reset x, increment z
		(new Tile("host71", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host72", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host73", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host74", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host75", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host76", mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
	}
	else //ROOM FOR CLIENT
	{
		// Row 1
		position = Ogre::Vector3(-width/2 + tileSize/2, -(width*heightScalingFactor)/2.0f, -(gapSize/2.0 + floorLength + tileSize/2.0));
		(new Tile("client11", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client12", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client13", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client14", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client15", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client16", mSceneMgr, game_simulator, position, tileSize));

		// Row 2
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize); // Reset x, increment z
		(new Tile("client21", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client22", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client23", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client24", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client25", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client26", mSceneMgr, game_simulator, position, tileSize));

		// Row 3
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize); // Reset x, increment z
		(new Tile("client31", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client32", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client33", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client34", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client35", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client36", mSceneMgr, game_simulator, position, tileSize));

		// Row 4
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize); // Reset x, increment z
		(new Tile("client41", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client42", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client43", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client44", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client45", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client46", mSceneMgr, game_simulator, position, tileSize));

		// Row 5
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize); // Reset x, increment z
		(new Tile("client51", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client52", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client53", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client54", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client55", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client56", mSceneMgr, game_simulator, position, tileSize));

		// Row 6
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize); // Reset x, increment z
		(new Tile("client61", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client62", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client63", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client64", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client65", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client66", mSceneMgr, game_simulator, position, tileSize));

		// Row 7
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize); // Reset x, increment z
		(new Tile("client71", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client72", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client73", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client74", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client75", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("client76", mSceneMgr, game_simulator, position, tileSize));

		// Row 1
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize * 3);
		(new Tile("host11", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host12", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host13", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host14", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host15", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host16", mSceneMgr, game_simulator, position, tileSize));

		// Row 2
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize); // Reset x, increment z
		(new Tile("host21", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host22", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host23", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host24", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host25", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host26", mSceneMgr, game_simulator, position, tileSize));

		// Row 3
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize); // Reset x, increment z
		(new Tile("host31", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host32", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host33", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host34", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host35", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host36", mSceneMgr, game_simulator, position, tileSize));

		// Row 4
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize); // Reset x, increment z
		(new Tile("host41", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host42", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host43", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host44", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host45", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host46", mSceneMgr, game_simulator, position, tileSize));

		// Row 5
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize); // Reset x, increment z
		(new Tile("host51", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host52", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host53", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host54", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host55", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host56", mSceneMgr, game_simulator, position, tileSize));

		// Row 6
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize); // Reset x, increment z
		(new Tile("host61", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host62", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host63", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host64", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host65", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host66", mSceneMgr, game_simulator, position, tileSize));

		// Row 7
		position = Ogre::Vector3(-width/2 + tileSize/2, position.y, position.z + tileSize); // Reset x, increment z
		(new Tile("host71", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host72", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host73", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host74", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host75", mSceneMgr, game_simulator, position, tileSize));
		position = Ogre::Vector3(position.x + tileSize, position.y, position.z);
		(new Tile("host76", mSceneMgr, game_simulator, position, tileSize));
	}

	/* Plane for 2nd player Floor */
/*	plane.normal = Ogre::Vector3::UNIT_Y;
	position = Ogre::Vector3(0.0f, -(width*heightScalingFactor)/2.0f, height/2.0f + width/3.0f - floorLength/2.0f);
	Ogre::MeshManager::getSingleton().createPlane("Floor2_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width, floorLength, 20, 20, true, 1, width/4, height/4, Ogre::Vector3::UNIT_Z);

	if (ident == 0)
		(new Wall("Floor2", "Floor2_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_Y, position, Ogre::Vector3(width, 0.01f, floorLength)))->addToSimulator();
	else 
		new Wall("Floor2", "Floor2_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_Y, position, Ogre::Vector3(width, 0.01f, floorLength));
*/
	/* Plane for Ceiling */
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Y;
	position = Ogre::Vector3(0.0f, (width*heightScalingFactor)/2.0f, 0.0f);
	Ogre::MeshManager::getSingleton().createPlane("Ceiling_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width, height + width, 20, 20, true, 1, width/4.0f, height/4.0f, Ogre::Vector3::UNIT_Z);

	if (ident == 0)
		(new Wall("Ceiling", "Ceiling_Plane", mSceneMgr, game_simulator, Ogre::Vector3::NEGATIVE_UNIT_Y, position, Ogre::Vector3(width, 0.01f, height + width)))->addToSimulator();
	else 
		new Wall("Ceiling", "Ceiling_Plane", mSceneMgr, game_simulator, Ogre::Vector3::NEGATIVE_UNIT_Y, position, Ogre::Vector3(width, 0.01f, height + width));

	/* Plane for LeftWall */
	plane.normal = Ogre::Vector3::UNIT_X;
	position = Ogre::Vector3(-width/2.0f, 0.0f, 0.0f);
	Ogre::MeshManager::getSingleton().createPlane("LeftWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	height, width*heightScalingFactor, 20, 20, true, 1, height/4, width/4, Ogre::Vector3::UNIT_Y);

	if (ident == 0)
		(new Wall("LeftWall", "LeftWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_X, position, Ogre::Vector3(0.01f, width*heightScalingFactor, height)))->addToSimulator();
	else
		new Wall("LeftWall", "LeftWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_X, position, Ogre::Vector3(0.01f, width*heightScalingFactor, height));

	/* Plane for RightWall */
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_X;
	position = Ogre::Vector3(width/2.0f, 0.0f, 0.0f);
	Ogre::MeshManager::getSingleton().createPlane("RightWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	height, width*heightScalingFactor, 20, 20, true, 1, height/4, width/4, Ogre::Vector3::UNIT_Y);

	if (ident == 0)
		(new Wall("RightWall", "RightWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::NEGATIVE_UNIT_X, position, Ogre::Vector3(0.01f, width*heightScalingFactor, height)))->addToSimulator();
	else 
		new Wall("RightWall", "RightWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::NEGATIVE_UNIT_X, position, Ogre::Vector3(0.01f, width*heightScalingFactor, height));

// Front and Back Divisions
	/* Plane for FarWall */
	plane.normal = Ogre::Vector3::UNIT_Z;
	position = Ogre::Vector3(0.0f, 0.0f, -height/2.0f - width/3.0f);
	Ogre::MeshManager::getSingleton().createPlane("FarWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width/3, width*heightScalingFactor, 20, 20, true, 1, width/4, height/4, Ogre::Vector3::UNIT_Y);

	if (ident == 0)
		(new Wall("FarWall", "FarWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_Z, position, Ogre::Vector3(width/3, width*heightScalingFactor, 0.01f)))->addToSimulator();
	else 
		new Wall("FarWall", "FarWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_Z, position, Ogre::Vector3(width/3, width*heightScalingFactor, 0.01f));

	/* Plane for NearWall */
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Z;
	position = Ogre::Vector3(0.0f, 0.0f, height/2.0f + width/3.0f);
	Ogre::MeshManager::getSingleton().createPlane("NearWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width/3, width*heightScalingFactor, 20, 20, true, 1, width/4, width/4, Ogre::Vector3::UNIT_Y);

	if (ident == 0)
		(new Wall("NearWall", "NearWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::NEGATIVE_UNIT_Z, position, Ogre::Vector3(width/3, width*heightScalingFactor, 0.01f)))->addToSimulator();
	else
		new Wall("NearWall", "NearWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::NEGATIVE_UNIT_Z, position, Ogre::Vector3(width/3, width*heightScalingFactor, 0.01f));

	Ogre::Real fnSideWidth = Ogre::Math::Sqrt(Ogre::Math::Sqr(width/3) + Ogre::Math::Sqr(width/3));
	// Plane for FarLeftWall 
	plane.normal = Ogre::Vector3::UNIT_Z;
	position = Ogre::Vector3(-width/3.0f, 0.0f, -height/2.0f - width/6.0f); // change the x value (more left)
	Ogre::MeshManager::getSingleton().createPlane("FarLeftWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	fnSideWidth, width*heightScalingFactor, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	if (ident == 0)
		(new Wall("FarLeftWall", "FarLeftWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3(-1.0f, 0.0f, 1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f)))->addToSimulator();
	else 
		new Wall("FarLeftWall", "FarLeftWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3(-1.0f, 0.0f, 1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f));

	// Plane for FarRightWall 
	plane.normal = Ogre::Vector3::UNIT_Z;
	position = Ogre::Vector3(width/3.0f, 0.0f, -height/2.0f - width/6.0f); // change the x value (more right)
	Ogre::MeshManager::getSingleton().createPlane("FarRightWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	fnSideWidth, width*heightScalingFactor, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	if (ident == 0)
		(new Wall("FarRightWall", "FarRightWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.0f, 1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f)))->addToSimulator();
	else 
		new Wall("FarRightWall", "FarRightWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.0f, 1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f));
	// Plane for NearLeftWall 
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Z;
	position = Ogre::Vector3(-width/3.0f, 0.0f, height/2.0f + width/6.0f); // change the x value (more left)
	Ogre::MeshManager::getSingleton().createPlane("NearLeftWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	fnSideWidth, width*heightScalingFactor, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);
	
	if (ident == 0)
		(new Wall("NearLeftWall", "NearLeftWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.0f, -1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f)))->addToSimulator();
	else 
		new Wall("NearLeftWall", "NearLeftWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.0f, -1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f));

	// Plane for NearRightWall 
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Z;
	position = Ogre::Vector3(width/3.0f, 0.0f, height/2.0f + width/6.0f); // change the x value (more right)
	Ogre::MeshManager::getSingleton().createPlane("NearRightWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	fnSideWidth, width*heightScalingFactor, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	if (ident == 0)
		(new Wall("NearRightWall", "NearRightWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3(-1.0f, 0.0f, -1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f)))->addToSimulator();
	else
		new Wall("NearRightWall", "NearRightWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3(-1.0f, 0.0f, -1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f));
}
//-------------------------------------------------------------------------------------
Ogre::Real Room::getWidth()
{
	return width;
}
//-------------------------------------------------------------------------------------
Ogre::Real Room::getHeight()
{
	return height;
}
//-------------------------------------------------------------------------------------
Ogre::Real Room::getFloorLength()
{
	return floorLength;
}
//-------------------------------------------------------------------------------------
Ogre::Vector3 Room::getBounds()
{
	return Ogre::Vector3(width, height, floorLength);
}
Ogre::Real Room::getFloorPositionY()
{
	return floorPosition;
}