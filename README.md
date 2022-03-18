# 3D-Maze
A 3D game coded in C++ where the player is trying to reach the goal at the end of the maze.

## Description:

#### GENERAL IDEA:
- IMPLEMENT A 3D MAZE GAME.
- THE MAIN IDEA OF THE GAME IS THAT THERE IS A PLAYER TRYING TO REACH THE GOAL AT THE END OF THE MAZE.
- WHILE THE PLAYER MOVES THROUGH THE PATHS, THE PLAYER SHOULD COLLECT COINS RANDOMLY DISTRIBUTED IN THE MAZE.
- MOREOVER, THERE WILL BE POWER-UPS TO HELP THE PLAYER WIN THE GAME.
- THE GAME ENDS AFTER A SPECIFIC INTERVAL OF TIME.
- THE PLAYER WILL LOSE THE GAME IF HE DIDN’T REACH THE GOAL IN TIME.

#### THE MAZE:
- IT IS DIVIDED INTO EQUAL LANES IN TERMS OF WIDTH AND HEIGHT WITH EQUAL DISTANCES BETWEEN EACH TWO LANES.
- THE LANES ARE PARALLEL TO EACH OTHER.
- EACH LANE BORDER WILL START AND END AT THE BORDERS OF THE MAZE.
- MOREOVER, THE MAZE ITSELF IS SURROUNDED BY FOUR BARRIERS.
- FOR THE NEXT LEVEL, THE NUMBER OF LANES WOULD INCREASE AND THE TIME OF THE BRIDGE SWITCHES WOULD DECREASE TO INCREASE THE DIFFICULTY.

#### THE PLAYER: 
THE PLAYER MUST APPEAR IN A RANDOM POSITION IN THE FIRST LANE AND FREE TO MOVE EITHER UP OR DOWN OR LEFT OR RIGHT USING KEYBOARD OR MOUSE FUNCTIONS.

#### THE GOAL:
- IT IS AN OBJECT.
- IT MUST APPEAR IN THE LAST LANE OF THE MAZE.
- IT CANNOT APPEAR IN THE SAME LANE AS THE PLAYER ONCE THE GAME STARTS.

#### THE POWER-UPS:
- THREE DIFFERENT POWER-UPS ARE REQUIRED AND EACH ONE HAS A DIFFERENT FUNCTIONALITY.
- THE FIRST ONE WOULD BE DOUBLESCORE. IF THE PLAYER COLLECTED THE POWER-UP, THEIR CURRENT SCORE WOULD BE DOUBLED AND THIS CHANGE WOULD ONLY BE APPLIED ON THE SCORE OF THE ALREADY COLLECTED COINS. (PINK SPHERE)
- THE SECOND ONE WOULD BE TIMEBOOST, IF THE PLAYER COLLECTED THE POWER-UP, THE GLOBAL TIME LIMIT WOULD BE INCREASED BY AN ADDITIONAL +20 SECONDS. (PURPLE SPHERE)
- THE THIRD ONE WOULD BE SKIPLANE, IF THE PLAYER COLLECTED THE POWER-UP, THE PLAYER WOULD SKIP A LANE AND BE TRANSFERRED TO THE NEXT ONE. (RED SPHERE)
- THESE POWER-UPS APPEAR IN RANDOM FIXED POSITIONS EVERY NEW GAME.
- ONCE THE PLAYER GETS A POWER-UP, THIS POWER-UP MUST DISAPPEAR.
 
#### THE COINS:
- THESE COINS APPEAR IN RANDOM FIXED POSITIONS EVERY NEW GAME SPREAD ALL OVER THE LANES WITH A RANDOM AMOUNT
- ONCE THE PLAYER COLLECTS A COIN, IT MUST DISAPPEAR AND INCREMENT THE SCORE +1.

#### THE BRIDGES:
- THEY MUST RANDOMLY APPEAR ONCE FOR EACH LANE BORDER ONCE THE GAME STARTS (THE NUMBER OF BRIDGES PER LANE BORDER IS ONLY ONE BRIDGE).
- THEY CHANGE THEIR POSITIONS RANDOMLY AFTER SPECIFIC INTERVAL OF TIME.

#### THE SCORE:
- THE SCORE INCREASES WHEN THE PLAYER COLLECTS THE COINS AND DOUBLED WHEN THE DOUBLESCORE POWER UP IS COLLECTED
- WHEN THE GAME ENDS, THE SCORE IS DISPLAYED.

#### THE COLLISIONS:
- WITH THE FOUR DIFFERENT BORDERS AND THE MAZE LANES, THE PLAYER STOPS
- WITH THE POWER-UPS, EACH DIFFERENT POWER-UP EFFECT IS ACTIVATED.
- WITH THE COINS, THE SCORE IS INCREMENTED +10.

#### THE THEMES:
- THE FIRST LEVEL: A GARDEN MAZE ACCOMPANIED WITH ITS CORRESPONDING ELEMENTS SUCH AS GREENS, TREES AND THE TEXTURES OF THE MAZE ITSELF IS MADE UP OF LEAVES.
- THE SECOND LEVEL: A DUNGEON MAZE ACCOMPANIED WITH ITS CORRESPONDING ELEMENTS SUCH AS DARK COLORS AND METALLIC OBJECT.

#### THE PERSPECTIVES:
- THIRD PERSON CAMERA WOULD BE ABLE TO VIEW THE PLAYER FROM BEHIND WHILE SHOWING EVERYTHING ELSE, AS IF THE CAMERA IS OUTSIDE AND FROM ABOVE OF THE PLAYER’S BODY (PRESS ‘F’ OR ‘T’ ON KEYBOARD).
- FIRST PERSON CAMERA IS SEEING THROUGH THE PLAYER’S EYES.
- CAMERA NAVIGATION USING THE FOLLOWING (‘Q’ TO ZOOM IN, ‘E’ TO ZOOM OUT, ‘W’ TO MOVE TO FRONT, ‘S’ TO MOVE BACK. ‘A’ MOVE LEFT, ‘D’ MOVE RIGHT) AND ROTATE THE CAMERA THROUGH THE FOLLOWING (‘J’ LEFT, ‘K’ DOWN, ‘L’ RIGHT, ‘I’ UP).

#### THE NAVIGATION:
- PLAYER MOVES USING ARROWS ON KEYBOARD

#### THE ANIMATION:
- GENERATION OF SOUND WITH EVERY USER INTERACTION
- COINS ROTATE

## How to run:
- Download project on computer.
- Import it into visual studio and run it.
