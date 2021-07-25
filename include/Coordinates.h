#ifndef COORDINATES_H
#define COORDINATES_H

struct GridCoordinates
{
    GridCoordinates();
    GridCoordinates(int r, int q);
    ~GridCoordinates();
    int r;
    int q;
};

struct Coordinates
{
    Coordinates();
    Coordinates(double x, double y);
    ~Coordinates();
    double x;
    double y;
};

#endif // COORDINATES_H
