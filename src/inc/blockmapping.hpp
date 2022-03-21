#pragma once
#include "inc/glsl.hpp"

// This file was handwritten according to tools/genblocks.py

const uint shape_mask       = 0b0000000000011111;
const uint emissive_mask    = 0b0000000001000000;
const uint translucent_mask = 0b0000000010000000;
const uint selector_mask    = 0b1111111100000000;
const uint facing_mask      = 0b0000011100000000;

/* clang-format off */
bool isCube(uint ID)         { return (shape_mask & ID) <= 2; }
bool isFacingCube(uint ID)   { return (shape_mask & ID) == 2; }
bool isLowCube(uint ID)      { return (shape_mask & ID) == 4; }
bool isPane(uint ID)         { return (shape_mask & ID) == 5; }
bool isSubVoxel(uint ID)     { return (shape_mask & ID) == 6; }
bool isTorch(uint ID)        { return (shape_mask & ID) == 7; }
bool isWallTorch(uint ID)    { return (shape_mask & ID) == 8; }
bool isLantern(uint ID)      { return (shape_mask & ID) == 9; }
bool isEndRod(uint ID)       { return (shape_mask & ID) == 10; }
bool isFence(uint ID)        { return (shape_mask & ID) == 11; }
bool isFire(uint ID)         { return (shape_mask & ID) == 13; }
bool isCrossShape(uint ID)   { return (shape_mask & ID) == 14; }
bool isCropShape(uint ID)    { return (shape_mask & ID) == 15; }

bool isEmissive(uint ID)     { return (emissive_mask & ID) > 0; }
bool isTranslucent(uint ID)  { return (translucent_mask & ID) > 0; }

const uint facing_north = 0;
const uint facing_south = 1;
const uint facing_east  = 2;
const uint facing_west  = 3;
const uint facing_up    = 4;
const uint facing_down  = 5;
uint getFacing(uint ID)       { return (facing_mask & ID) >> 8; }

const uint east_mask  = 0b0000100000000000;
const uint north_mask = 0b0000010000000000;
const uint south_mask = 0b0000001000000000;
const uint west_mask  = 0b0000000100000000;

bool isEast(uint ID)         { return (east_mask & ID) > 0; }
bool isNorth(uint ID)        { return (north_mask & ID) > 0; }
bool isSouth(uint ID)        { return (south_mask & ID) > 0; }
bool isWest(uint ID)         { return (west_mask & ID) > 0; }

const uint hanging_mask  = 0b0000000100000000;
bool isHanging(uint ID)      { return (hanging_mask & ID) > 0; }
/* clang-format on */