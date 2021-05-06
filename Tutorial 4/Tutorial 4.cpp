#include <iostream>
#include "VisualDebugger.h"

using namespace std;

/*
GOLFBALL
the game features a standard golf ball that is interactable with the putter - its material, shape and dynamics generally represent a standard golf ball.
It directly interacts with the track and the track walls. The game also features different shape balls that the user can change in-game with the (1,2,3) keys.
the user can change between a standard golf ball, a pyramid ball (very difficult to hit) and a wedge type ball. All of the balls can be hit along the track until
the end of the course where they can be hit into a hole and the player can win the game - they will be prompted through the console that they have won.

PUTTER
The game features a swinging standard shaped putter. the putter consists of a joint object, a hinge and the putter itself. The joint object that the putter is connected to
can be moved around the play environment using the 'T, F, G, H, B' keys. the putter can be swung with an initial wind up and then a keypress to swing the putter. the
putter swing keys are 'B' for the backswing and 'n' to fire the golf club. The putter can also be rotated by the joint object using the 'R, Y and V' keys rotation the putter
to the left, right and resetting the angle of the club respectively.

SCORE
When the putter is swung by the user - the player's total swings are increased by 1, even if they miss. Players can decide to replay the level to try and decrease
their total number of swings for the course. the score is shown to the player through the console.

FLAG
the flag and sinkhole are on a piece of standard generated track that contains a floor and 3 walls. inside of the hole is a trigger object that when colliding with
the ball will indicate to the player that they have won the game. A flag is a compound object that includes a simulated cloth object affected by physics.

CAMERA
The user can change camera angles using the 'W, A, S, D, Q, Z' keys to forward, left, backwards, right, up and down respectively.

TRAMPOLINE
The trampoline is featured at the end of the track before the hole. players must navigate over the trampoline to reach the goal. the trampoline is a 5 spring
object that consists of three parts, the top bouncy part of the trampoline, the springs, and the base. the trampoline has tight springs that initially bounce and will
move under the weight of the ball.

TRACK
The track is made up of a track section system that contains different numbers of walls for each track piece that is required. the track actors are all compound shapes
that can be manipulated via their constructors so that single class objects can exist for different pieces of the track and they can be altered whenever they are created.
This is what allows for multiple places of different track pieces to create a long and interesting track. The track also features 2 ramps which are simple non-compound
shapes.

Here's a link to the youtube video that demonstrates all of the features - https://www.youtube.com/watch?v=jig9OiwyVGg
*/
int main()
{
	try 
	{ 
		VisualDebugger::Init("Tutorial 4", 800, 800); 
	}
	catch (Exception exc) 
	{ 
		cerr << exc.what() << endl;
		return 0; 
	}

	VisualDebugger::Start();

	return 0;
}