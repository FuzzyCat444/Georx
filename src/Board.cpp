#include "Board.h"

Board::Board(Resources* res, int width, int height)
    : res(res), width(width), height(height),
      mouse(0.0, 0.0), selected(0, 0),
      removeShape(false),
      addPyramid(false), addCube(false),
      addCylinder(false), addSphere(false),
      doMove(false),
      animationTime(0.0),
      currentClick(0),
      solutionShown(false),
      win(false)
{
    for (int j = 0; j < size2; j++)
    {
        for (int i = 0; i < size2; i++)
        {
            solutionGrid[j][i] = Tile::EMPTY;
            startingGrid[j][i] = Tile::EMPTY;
            grid[j][i] = Tile::EMPTY;
        }
    }
}

Board::~Board()
{
}

void Board::input(SDL_Event* event)
{
    if (event->type == SDL_MOUSEMOTION)
    {
        mouse = Coordinates(event->motion.x, event->motion.y);
        mouse.x = (mouse.x - width / 2.0) / hexScale;
        mouse.y = (mouse.y - height / 2.0) / hexScale;
    }
    else if (event->type == SDL_KEYDOWN)
    {
        if (event->key.keysym.sym == SDLK_SPACE)
            removeShape = true;
        else if (event->key.keysym.sym == SDLK_1)
            addPyramid = true;
        else if (event->key.keysym.sym == SDLK_2)
            addCube = true;
        else if (event->key.keysym.sym == SDLK_3)
            addCylinder = true;
        else if (event->key.keysym.sym == SDLK_4)
            addSphere = true;
        else if (event->key.keysym.sym == SDLK_s)
            solutionShown = true;
        else if (event->key.keysym.sym == SDLK_p)
        {
            if (debug)
                printLevelData();
        }
    }
    else if (event->type == SDL_KEYUP)
    {
        if (event->key.keysym.sym == SDLK_SPACE)
            removeShape = false;
        else if (event->key.keysym.sym == SDLK_1)
            addPyramid = false;
        else if (event->key.keysym.sym == SDLK_2)
            addCube = false;
        else if (event->key.keysym.sym == SDLK_3)
            addCylinder = false;
        else if (event->key.keysym.sym == SDLK_4)
            addSphere = false;
        else if (event->key.keysym.sym == SDLK_s)
            solutionShown = false;
    }
    else if (event->type == SDL_MOUSEBUTTONUP)
        if (event->button.button == SDL_BUTTON_LEFT)
            doMove = true;
}

void Board::update()
{
    GridCoordinates newSelected = selectFromLinear(mouse);
    if (newSelected.r != 999)
        selected = newSelected;
    if (inBounds(selected) && !solutionShown && debug)
    {
        if (removeShape)
            addShape(Tile::EMPTY);
        if (addPyramid)
            addShape(Tile::PYRAMID);
        if (addCube)
            addShape(Tile::CUBE);
        if (addCylinder)
            addShape(Tile::CYLINDER);
        if (addSphere)
            addShape(Tile::SPHERE);
    }
    if (doMove && !locked() && !didWin())
        startMove();
    doMove = false;
    updateMove();
}

void Board::undo()
{
    if (currentClick <= 0 || locked() || didWin())
        return;

    GridCoordinates gc = clickHistory.at(currentClick - 1);
    Tile tile = getTile(gc);
    for (int i = 0; i < tileCycle(tile) - 1; i++)
        moveInstant(gc);
    currentClick--;
}

void Board::redo()
{
    if (currentClick >= clickHistory.size() || locked() || didWin())
        return;

    GridCoordinates gc = clickHistory.at(currentClick);
    moveInstant(gc);
    currentClick++;
}

void Board::reset()
{
    if (locked())
        return;
    resetClickHistory();
    win = false;
    grid = startingGrid;
}

bool Board::locked()
{
    return animatingMove() || solutionShown;
}

bool Board::didWin()
{
    return win;
}

void Board::startMove()
{
    if (inBounds(selected))
    {
        Tile tile = getTile(selected);
        if (tile != Tile::EMPTY)
        {
            getMoves();
            bool canDoMove = true;
            for (unsigned int i = 0; i < moves.size(); i++)
            {
                Move mv = moves.at(i);
                if (!inBounds(mv.src) || !inBounds(mv.dst))
                    canDoMove = false;
            }
            if (canDoMove)
            {
                Mix_Chunk* moveSound = rand() % 2 == 0 ? res->s.move1 : res->s.move2;
                Mix_PlayChannel(-1, moveSound, 0);
                animationTime = 1.0;
                for (unsigned int i = 0; i < moves.size(); i++)
                {
                    Move mv = moves.at(i);
                    setTile(mv.src, Tile::EMPTY);
                }
                if (currentClick != clickHistory.size())
                    clickHistory.resize(currentClick);
                clickHistory.push_back(selected);
                currentClick++;
            }
            else
                moves.clear();
        }
    }
}

void Board::updateMove()
{
    bool wasAnimating = animatingMove();
    animationTime -= animationRate;
    if (!animatingMove())
    {
        for (unsigned int i = 0; i < moves.size(); i++)
        {
            Move mv = moves.at(i);
            if (mv.tile != Tile::EMPTY)
                setTile(mv.dst, mv.tile);
        }
        moves.clear();
        if (wasAnimating)
            checkWin();
    }
}

void Board::checkWin()
{
    if (grid == solutionGrid && !didWin())
    {
        Mix_PlayChannel(-1, res->s.fanfare, 0);
        win = true;
    }
}

bool Board::animatingMove()
{
    return animationTime > 0.0;
}

void Board::getMoves()
{
    switch (getTile(selected))
    {
    case Tile::PYRAMID:
        moves.push_back(Move(rel(GridCoordinates(0, -1)), rel(GridCoordinates(-1, 1))));
        moves.push_back(Move(rel(GridCoordinates(-1, 1)), rel(GridCoordinates(1, 0))));
        moves.push_back(Move(rel(GridCoordinates(1, 0)), rel(GridCoordinates(0, -1))));
        break;
    case Tile::CUBE:
        moves.push_back(Move(rel(GridCoordinates(-1, 0)), rel(GridCoordinates(1, 0))));
        moves.push_back(Move(rel(GridCoordinates(1, 0)), rel(GridCoordinates(-1, 0))));
        moves.push_back(Move(rel(GridCoordinates(-1, 1)), rel(GridCoordinates(1, -1))));
        moves.push_back(Move(rel(GridCoordinates(1, -1)), rel(GridCoordinates(-1, 1))));
        break;
    case Tile::CYLINDER:
        moves.push_back(Move(rel(GridCoordinates(0, 1)), rel(GridCoordinates(-2, 1))));
        moves.push_back(Move(rel(GridCoordinates(-2, 1)), rel(GridCoordinates(0, -1))));
        moves.push_back(Move(rel(GridCoordinates(0, -1)), rel(GridCoordinates(2, -1))));
        moves.push_back(Move(rel(GridCoordinates(2, -1)), rel(GridCoordinates(0, 1))));
        break;
    case Tile::SPHERE:
        moves.push_back(Move(rel(GridCoordinates(1, -1)), rel(GridCoordinates(2, -2))));
        moves.push_back(Move(rel(GridCoordinates(2, -2)), rel(GridCoordinates(1, -1))));
        moves.push_back(Move(rel(GridCoordinates(0, -1)), rel(GridCoordinates(0, -2))));
        moves.push_back(Move(rel(GridCoordinates(0, -2)), rel(GridCoordinates(0, -1))));
        moves.push_back(Move(rel(GridCoordinates(-1, 0)), rel(GridCoordinates(-2, 0))));
        moves.push_back(Move(rel(GridCoordinates(-2, 0)), rel(GridCoordinates(-1, 0))));
        moves.push_back(Move(rel(GridCoordinates(-1, 1)), rel(GridCoordinates(-2, 2))));
        moves.push_back(Move(rel(GridCoordinates(-2, 2)), rel(GridCoordinates(-1, 1))));
        moves.push_back(Move(rel(GridCoordinates(0, 1)), rel(GridCoordinates(0, 2))));
        moves.push_back(Move(rel(GridCoordinates(0, 2)), rel(GridCoordinates(0, 1))));
        moves.push_back(Move(rel(GridCoordinates(1, 0)), rel(GridCoordinates(2, 0))));
        moves.push_back(Move(rel(GridCoordinates(2, 0)), rel(GridCoordinates(1, 0))));
        break;
    default: break;
    }
    for (unsigned int i = 0; i < moves.size(); i++)
    {
        Move mv = moves.at(i);
        mv.tile = getTile(mv.src);
        moves.at(i) = mv;
    }
}

void Board::moveInstant(GridCoordinates gc)
{
    GridCoordinates temp = selected;
    selected = gc;
    getMoves();
    for (unsigned int i = 0; i < moves.size(); i++)
    {
        Move mv = moves.at(i);
        setTile(mv.dst, mv.tile);
    }
    moves.clear();
    selected = temp;
}

int Board::tileCycle(Tile tile)
{
    switch (tile)
    {
    case Tile::PYRAMID:
        return 3;
        break;
    case Tile::CUBE:
        return 2;
        break;
    case Tile::CYLINDER:
        return 2;
        break;
    case Tile::SPHERE:
        return 2;
        break;
    default:
        return 0;
        break;
    }
    return 0;
}

void Board::resetClickHistory()
{
    clickHistory.clear();
    currentClick = clickHistory.size();
}

void Board::printLevelData()
{
    for (int y = 0; y < size2; y++)
    {
        for (int x = 0; x < size2; x++)
        {
            std::cout << static_cast<int>(solutionGrid[y][x]) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    for (int y = 0; y < size2; y++)
    {
        for (int x = 0; x < size2; x++)
        {
            std::cout << static_cast<int>(grid[y][x]) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

bool Board::loadLevel(std::string file)
{
    std::ifstream ifs(file);
    if (ifs.fail())
    {
        ifs.close();
        return false;
    }
    std::vector<int> loaded;
    while (!ifs.eof())
    {
        int tileID;
        ifs >> tileID;
        loaded.push_back(tileID);
    }
    ifs.close();
    int i = 0;
    int j = size2 * size2;
    for (int y = 0; y < size2; y++)
    {
        for (int x = 0; x < size2; x++)
        {
            solutionGrid[y][x] = static_cast<Tile>(loaded.at(i));
            startingGrid[y][x] = static_cast<Tile>(loaded.at(j));
            i++;
            j++;
        }
    }
    grid = startingGrid;
    resetClickHistory();
    win = false;
    return true;
}

void Board::draw(SDL_Renderer* ren)
{
    HexGrid& theGrid = solutionShown ? solutionGrid : grid;
    GridCoordinates iter;
    for (iter.r = -size1; iter.r <= size1; iter.r++)
    {
        for (iter.q = -size1; iter.q <= size1; iter.q++)
        {
            if (!inBounds(iter))
                continue;

            Tile tile = theGrid[iter.r + size1][iter.q + size1];
            Coordinates lc = gridToLinear(iter);
            lc.x = lc.x * hexScale + hexOffsetX + width / 2;
            lc.y = lc.y * hexScale + hexOffsetY + height / 2;

            drawTile(ren, tile, lc, true, iter.r == selected.r && iter.q == selected.q, true);
        }
    }

    if (solutionShown)
        return;
    for (unsigned int i = 0; i < moves.size(); i++)
    {
        Move mv = moves.at(i);
        Coordinates src = gridToLinear(mv.src);
        Coordinates dst = gridToLinear(mv.dst);
        double t = animationTime;
        Coordinates interp;
        interp.x = src.x * t + dst.x * (1.0 - t);
        interp.y = src.y * t + dst.y * (1.0 - t);
        interp.x = (interp.x - hexWidth / 2) * hexScale + width / 2;
        interp.y = (interp.y - hexHeight / 2) * hexScale + height / 2;
        drawTile(ren, mv.tile, interp, false, false, false);
    }
}

Coordinates Board::gridToLinear(GridCoordinates gc)
{
    Coordinates lc;
    lc.x = (gc.q + gc.r / 2.0) * hexWidth;
    lc.y = gc.r * hexTopHeight;
    return lc;
}

GridCoordinates Board::linearToGrid(Coordinates lc)
{
    GridCoordinates gc;
    gc.r = static_cast<int>(floor(lc.y / hexTopHeight));
    gc.q = static_cast<int>(floor((lc.x - lc.y / 2.0) / hexWidth));
    return gc;
}

GridCoordinates Board::rel(GridCoordinates relative)
{
    relative.r += selected.r;
    relative.q += selected.q;
    return relative;
}

GridCoordinates Board::selectFromLinear(Coordinates lc)
{
    GridCoordinates gc = linearToGrid(lc);
    GridCoordinates gridPoints[4]
    {
        gc,
        GridCoordinates(gc.r + 1, gc.q),
        GridCoordinates(gc.r, gc.q + 1),
        GridCoordinates(gc.r + 1, gc.q + 1)
    };
    for (int i = 0; i < 4; i++)
    {
        Coordinates point = gridToLinear(gridPoints[i]);
        point.x = lc.x - point.x;
        point.y = lc.y - point.y;
        if (insideHex(point))
            return gridPoints[i];
    }
    return GridCoordinates(999, 999);
}

bool Board::insideHex(Coordinates lc)
{
    double s = 2.0 / hexHeight;
    lc.x *= s;
    lc.y *= s;
    double sqrt3 = 1.73205080757;
    double sqrt32 = sqrt3 / 2.0;
    double sqrt3Inv = 1.0 / sqrt3;
    double topLeft = -sqrt3Inv * lc.x + lc.y - 1;
    double topRight = sqrt3Inv * lc.x + lc.y - 1;
    double left = -lc.x - sqrt32;
    double right = lc.x - sqrt32;
    double bottomLeft = -sqrt3Inv * lc.x - lc.y - 1;
    double bottomRight = sqrt3Inv * lc.x - lc.y - 1;
    return topLeft < 0.0 && topRight < 0.0 &&
           left < 0.0 && right < 0.0 &&
           bottomLeft < 0.0 && bottomRight < 0.0;
}

bool Board::inBounds(GridCoordinates gc)
{
    return gc.r >= -size1 && gc.r <= size1 &&
           gc.q >= -size1 && gc.q <= size1 &&
           gc.r + gc.q >= -size1 && gc.r + gc.q <= size1;
}

void Board::setTile(GridCoordinates gc, Tile tile)
{
    grid[gc.r + size1][gc.q + size1] = tile;
}

Tile Board::getTile(GridCoordinates gc)
{
    return grid[gc.r + size1][gc.q + size1];
}

void Board::addShape(Tile tile)
{
    setTile(selected, tile);
    solutionGrid = grid;
    startingGrid = grid;
    resetClickHistory();
    win = false;
}

void Board::drawTile(SDL_Renderer* ren, Tile tile, Coordinates lc, bool hex, bool hexSelected, bool shadow)
{
    SDL_Rect dest;
    dest.x = static_cast<int>(lc.x);
    dest.y = static_cast<int>(lc.y);
    dest.w = hexWidth * hexScale;
    dest.h = hexHeight * hexScale;

    SDL_Texture* hexTex = nullptr;
    if (hex)
    {
        hexTex = res->t.hex;
        if (hexSelected)
            hexTex = res->t.hexSelected;
    }
    if (hexTex != nullptr)
        SDL_RenderCopy(ren, hexTex, nullptr, &dest);

    SDL_Texture* shapeTex = nullptr;
    switch (tile)
    {
    case Tile::PYRAMID:
        shapeTex = res->t.pyramid;
        break;
    case Tile::CUBE:
        shapeTex = res->t.cube;
        break;
    case Tile::CYLINDER:
        shapeTex = res->t.cylinder;
        break;
    case Tile::SPHERE:
        shapeTex = res->t.sphere;
        break;
    default: break;
    }
    if (shapeTex != nullptr)
    {
        if (shadow)
            SDL_RenderCopy(ren, res->t.shadow, nullptr, &dest);
        SDL_RenderCopy(ren, shapeTex, nullptr, &dest);
    }
}
