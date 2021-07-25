#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "Resources.h"
#include "Tile.h"
#include "Coordinates.h"

class Board
{
public:
    Board(Resources* res, int width, int height);
    ~Board();

    void input(SDL_Event* event);
    void update();
    void draw(SDL_Renderer* ren);

    void undo();
    void redo();
    void reset();
    bool loadLevel(std::string file);
    bool locked();
    bool didWin();
private:
    Resources* res;
    int width;
    int height;

    static constexpr int size1 = 6;
    static constexpr int size2 = size1 * 2 + 1;
    static constexpr int hexWidth = 28;
    static constexpr int hexHeight = 32;
    static constexpr int hexTopHeight = 24;
    static constexpr int hexScale = 2;
    static constexpr int hexOffsetX = -hexWidth / 2 * hexScale;
    static constexpr int hexOffsetY = -hexHeight / 2 * hexScale;
    static constexpr bool debug = false;

    Coordinates mouse;
    GridCoordinates selected;
    bool removeShape;
    bool addPyramid;
    bool addCube;
    bool addCylinder;
    bool addSphere;
    bool doMove;

    struct Move
    {
        GridCoordinates src;
        GridCoordinates dst;

        Move(GridCoordinates src, GridCoordinates dst)
        : src(src), dst(dst) {}

        Tile tile;
    };
    static constexpr double animationRate = 0.09;
    double animationTime;
    std::vector<Move> moves;
    void startMove();
    void updateMove();
    bool animatingMove();
    void getMoves();
    void moveInstant(GridCoordinates gc);
    int tileCycle(Tile tile);

    std::vector<GridCoordinates> clickHistory;
    unsigned int currentClick;
    void resetClickHistory();
    void printLevelData();

    typedef std::array<std::array<Tile, size2>, size2> HexGrid;
    HexGrid solutionGrid;
    HexGrid startingGrid;
    bool solutionShown;
    HexGrid grid;
    bool win;
    void checkWin();

    Coordinates gridToLinear(GridCoordinates gc);
    GridCoordinates linearToGrid(Coordinates lc);
    GridCoordinates rel(GridCoordinates relative);
    GridCoordinates selectFromLinear(Coordinates lc);
    bool insideHex(Coordinates lc);
    bool inBounds(GridCoordinates gc);
    void setTile(GridCoordinates gc, Tile tile);
    Tile getTile(GridCoordinates gc);
    void addShape(Tile tile);
    void drawTile(SDL_Renderer* ren, Tile tile, Coordinates lc, bool hex, bool hexSelected, bool shadow);
};

#endif // BOARD_H
