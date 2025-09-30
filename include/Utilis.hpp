#pragma once

#include "Entity.hpp"

namespace Utilis
{
    struct Intersect { bool result; Vec2f pos; };

    Intersect LineIntersect(Vec2f a, Vec2f b, Vec2f c, Vec2f d) {
        Vec2f r = (b - a);
        Vec2f s = (d - c);
        float rxs = r.x * s.y - r.y * s.x;

        // Early exit if lines are parallel or nearly parallel
        if (std::abs(rxs) < 1e-6f) {
            return { false, Vec2f(0.0f, 0.0f) };
        }

        Vec2f cma = c - a;
        float t = (cma.x * s.y - cma.y * s.x) / rxs;
        float u = (cma.x * r.y - cma.y * r.x) / rxs;

        // Use a small epsilon for boundary checks
        const float eps = 1e-4f;
        if (t >= -eps && t <= 1.0f + eps && u >= -eps && u <= 1.0f + eps) {
            return { true, Vec2f(a.x + t * r.x, a.y + t * r.y) };
        }
        return { false, Vec2f(0.0f, 0.0f) };
    }

    float CalculateAngle(Vec2f basePoint, Vec2f point) {
        return std::atan2(point.y - basePoint.y, point.x - basePoint.x);
    }

    Vec2f castToEdge(Vec2f& startPoint, Vec2f& directionPoint, const sf::RenderWindow& window)
    {
        //This basically gets the displacement from the start vector to the direction vector
        Vec2 direction = { directionPoint.x - startPoint.x, directionPoint.y - startPoint.y };

        //Basically get the length of the verticies current hypothenuse in refrence to (0,0) on the pixel grid
        float magnitudeOfLine = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        //If the length is zero, it means you are at the starting point
        if (magnitudeOfLine == 0) { return startPoint; }

        //This is bascially a unit verision of the length, it normalizes it, so gets rid of negatives
        //and what not, also basically makes the vector only be within the (1, 1) coordiante on a 
        //standard mathimatical graph, it has a magnitude of 1, but it preserves the direction of the og line
        Vec2f dirNorm = direction / magnitudeOfLine;

        //Were basically casting out the line in all four of the possible walls of the window

        //This is going to store all of the four point values as 't' values
        float tValues[4];

        // Left edge (x = 0)
        if (dirNorm.x != 0) tValues[0] = (0 - startPoint.x) / dirNorm.x;
        else tValues[0] = std::numeric_limits<float>::max();

        // Right edge (x = window width)
        if (dirNorm.x != 0) tValues[1] = (window.getSize().x - startPoint.x) / dirNorm.x;
        else tValues[1] = std::numeric_limits<float>::max();

        // Top edge (y = 0)
        if (dirNorm.y != 0) tValues[2] = (0 - startPoint.y) / dirNorm.y;
        else tValues[2] = std::numeric_limits<float>::max();

        // Bottom edge (y = window height)
        if (dirNorm.y != 0) tValues[3] = (window.getSize().y - startPoint.y) / dirNorm.y;
        else tValues[3] = std::numeric_limits<float>::max();

        // Find the smallest positive t value
        float tMin = std::numeric_limits<float>::max();
        for (int i = 0; i < 4; ++i) {
            if (tValues[i] > 0 && tValues[i] < tMin) {
                tMin = tValues[i];
            }
        }
        //This returns the current start point value, in the directional normal of
        //the given direction input, and then also finds the minimal value of the
        //closest possible window

        //This basically takes the startpoint as the origin for the line
        //Then it takes the normalized vector for the angle, to basically have the line facing that direction
        //Then it takes the tMin in order to find the actual length that the ray needs to be
        return startPoint + dirNorm * tMin;
    }
}