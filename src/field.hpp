// Inches and degrees
// Only for red on the right

#ifndef FIELD_HPP
#define FIELD_HPP

#include "main.h"

using namespace lemlib;

float tile = 24;
float ringOD = 7;
float ringID = 3;
float ringThick = 2;

// Starting positions
Pose startUpper = Pose(tile * 2, tile * 1, 90);  // face the right wall
Pose startLower = Pose(tile * 2, -tile * 1, 90); // face the right wall
Pose startLowerFar = Pose(tile * 2.5, -tile * 2.5, -45);

// Mobile goal center
Pose mogoFarBottom = Pose(0, -tile * 2);
Pose mogoBottom = Pose(tile, -tile);
Pose mogoTop = Pose(tile, tile);

// Ring center, y then x
Pose ringCenterUpper = Pose(ringOD / 2, ringOD / 2);
Pose ringCenterLower = Pose(ringOD / 2, -ringOD / 2);

Pose ringTopUpper = Pose(ringOD / 2, tile * 2 + ringOD / 2);
Pose ringTopLower = Pose(ringOD / 2, tile * 2 - ringOD / 2);
Pose ringTopOuter = Pose(tile, tile * 2);

Pose ringBottomInner = Pose(tile, -tile * 2);
Pose ringBottomOuter = Pose(tile * 2, -tile * 2);

Pose ringRightInner = Pose(tile * 2, 0);
Pose ringRightOuter = Pose(tile * 2.5, 0);

Pose ringTopCorner = Pose(tile * 3 - ringOD / 2, -tile * 3 + ringOD / 2);
Pose ringBottomCorner = Pose(tile * 3 - ringOD / 2, tile * 3 - ringOD / 2);

#endif
