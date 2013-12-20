#include "GeneralEvent.h"

float GeneralEvent::getX()
{
    return x;
}

float GeneralEvent::getY()
{
    return y;
}

GeneralEvent::GeneralEvent(float x, float y, float z)
    :Event(z), x(x), y(y)
{
}

GeneralEvent::~GeneralEvent()
{

}

void GeneralEvent::handle()
{

}
