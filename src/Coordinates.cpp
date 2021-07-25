#include "Coordinates.h"

GridCoordinates::GridCoordinates()
    : r(0), q(0)
{
}

GridCoordinates::GridCoordinates(int r, int q)
    : r(r), q(q)
{
}

GridCoordinates::~GridCoordinates()
{

}

Coordinates::Coordinates()
    : x(0.0), y(0.0)
{
}

Coordinates::Coordinates(double x, double y)
    : x(x), y(y)
{
}

Coordinates::~Coordinates()
{

}
