#include "GeneralEvent.h"

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
