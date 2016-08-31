INFO3220 Assignment Stage 2
By Vasilios Moisiadis, 3/May/2015

Extension of: BaseCodeVersionC


Installation:
--------------

- Load and configure project into Qt Creator from AssignmentStage2_VasiliosMoisiadis (henceforth referred to as Source Folder)
	-> Double-Click Assignment2.pro, or
	-> File->Open File or Project...->Select Assignment2.pro

- Compile or build program to produce the build directory. Running the project will do this, but the program will not be
	in a functional state. For reference, the build directory contains the program's Makefiles.

- Copy all files from within Source Folder->assets into the build directory. Do not copy the folder itself.


Using the config file:
-----------------------

The program will not run as expected without the config file (found as config.ini). This file is included with
those from the Source Folder->assets folder. It is highly customisable, and will dynamically change the program.

It is divided into three parts: sections, keys, and values. They are found, and expected to be, in the format below:

[Section]
Key=value

The legend used for expected format of the values are:
(-)## = Only whole numbers (no decimal points). Negative values allowed.
## = Only positive whole number (no decimal points).
string [Expected: str1, str2] = Only predefined strings of the format within the square brackets.

Additionally, any value marked with '[S2]' at the end denotes that the key is only required for Stage 2. The [S2] is
not included in the value itself. Including [S2] will not make the program operate. The key can be included for Stage 1
functionality, but it will not be used.

Any deviation from the format laid out in the legend will cause an error and the program will not run. The order
of the keys do not matter within the section itself, but the keys are case-sensitive.


Sections of the config file:
-----------------------------

Stickman
=========

The Stickman is the hero of the story. The keys, and their expected format, are found below, where:

[Stickman]
Size=string [Expected: tiny, normal, large, giant]
StartX=##
StartY=##
Velocity=##
MaxJumps=##[S2]
JumpHeight=##[S2]


Background
===========

The Background is the backdrop of the story level. The keys, and their expected format, are found below, where:

[Background]
Image=string [Expected: background.jpg]
WindowHeight=##[S2]
WindowWidth=##[S2]


Obstacles
===========

The Obstacles are what the Stickman must jump over. The keys, and their expected format, are found below, where:

[Obstacles]
Count=##[S2]
Enabled=string[S2] [Expected: true, false]


Obstacle#
===========

The count included in the Obstacles section must be less than or equal to the number of individual Obstacle sections.
For each obstacle, the title must be in the format of 'Obstacle#', where # corresponds to the position of the obstacle
relative to the others. The Obstacle# sections do not need to be sequential, but there must be at least one Obstacle#
for each number between 1 and #. That is, if Obstacles->Count = 3, there must be be Obstacle1, Obstacle2, Obstacle3
sections within the config file.

The keys, and their expected format, are found below, where:

[Obstacle#]
Width=##[S2]
Height=##[S2]
Shape=string[S2] [Expected: triangle, circle, rectangle]
StartY=##[S2]
Spacing=##[S2]
Color=string[S2] [Expected: green, red, white, yellow]


Extensions Implemented:
-----------------------

- Multiple jumps. Changing the MaxJumps and JumpHeight keys within the Stickman section of the config file will
	allow the Stickman to have multiple jumps of the set height.

- Variable obstacle shapes. The shapes of the obstacles can be either rectangle, circle, or triangle, as set within
	the Shape key of the Obstacle# sections.
	
- Variable obstacle colors. The colors of the obstacles can be either green, red, white, or yellow, as set within
	the Color key of the Obstacle# sections.
