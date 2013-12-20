#include "GeneralEvent.h"

GeneralEvent::GeneralEvent(float x, float y, float z)
    :Event(y), x(x), z(z)
{
}

GeneralEvent::~GeneralEvent()
{

}

void GeneralEvent::handle()
{

}
