//DONE
#pragma once

#include "Entity.hpp" 
namespace Physics
{
	Vec2f GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
	{
        
        float sumOfHalfLengthX = a->get<CBoundingBox>().halfsize.x + b->get<CBoundingBox>().halfsize.x;
        float sumOfHalfLengthY = a->get<CBoundingBox>().halfsize.y + b->get<CBoundingBox>().halfsize.y;

        float distanceBetweenObjectsX = std::abs(a->get<CTransform>().pos.x - b->get<CTransform>().pos.x);
        float distanceBetweenObjectsY = std::abs(a->get<CTransform>().pos.y - b->get<CTransform>().pos.y);

        float xIntersectVal = sumOfHalfLengthX - distanceBetweenObjectsX;
        float yIntersectVal = sumOfHalfLengthY - distanceBetweenObjectsY;

        if (xIntersectVal > 0 and yIntersectVal > 0)
        {
            return Vec2f(xIntersectVal, yIntersectVal);
        }

        return Vec2f(0, 0);
	}

    Vec2f GetShiftedOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
    {

        float sumOfHalfLengthX = a->get<CBoundingBox>().halfsize.x + b->get<CBoundingBox>().halfsize.x;
        float sumOfHalfLengthY = a->get<CBoundingBox>().halfsize.y + b->get<CBoundingBox>().halfsize.y;

        float distanceBetweenObjectsX = std::abs(a->get<CTransform>().pos.x - b->get<CTransform>().pos.x);
        float distanceBetweenObjectsY = std::abs(a->get<CTransform>().pos.y + 2 - b->get<CTransform>().pos.y);

        float xIntersectVal = sumOfHalfLengthX - distanceBetweenObjectsX;
        float yIntersectVal = sumOfHalfLengthY - distanceBetweenObjectsY;

        if (xIntersectVal > 0 and yIntersectVal > 0)
        {
            return Vec2f(xIntersectVal, yIntersectVal);
        }

        return Vec2f(0, 0);
    }

	Vec2f GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
	{
        float sumOfHalfLengthX = a->get<CBoundingBox>().halfsize.x + b->get<CBoundingBox>().halfsize.x;
        float sumOfHalfLengthY = a->get<CBoundingBox>().halfsize.y + b->get<CBoundingBox>().halfsize.y;

        float distanceBetweenObjectsX = std::abs(a->get<CTransform>().prevPos.x - b->get<CTransform>().prevPos.x);
        float distanceBetweenObjectsY = std::abs(a->get<CTransform>().prevPos.y - b->get<CTransform>().prevPos.y);

        float xIntersectVal = sumOfHalfLengthX - distanceBetweenObjectsX;
        float yIntersectVal = sumOfHalfLengthY - distanceBetweenObjectsY;

        if (xIntersectVal > 0 or yIntersectVal > 0)
        {
            return Vec2f(xIntersectVal, yIntersectVal);
        }
        return Vec2f(0, 0);
	}
}