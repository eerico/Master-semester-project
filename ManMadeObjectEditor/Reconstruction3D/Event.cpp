#include "Event.h"

Event::Event(float z)
    :z(z)
{
}

Event::~Event() {

}

float Event::getZ()
{
    return z;
}
