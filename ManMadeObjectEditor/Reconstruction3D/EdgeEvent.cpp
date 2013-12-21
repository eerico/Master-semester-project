#include "EdgeEvent.h"
#include "Reconstruction3D.h"

EdgeEvent::EdgeEvent()
    :Event(0)
{
}

EdgeEvent::~EdgeEvent()
{

}

void EdgeEvent::handle(Reconstruction3D *reconstruction3D)
{

}

bool EdgeEvent::isGeneralEvent() {
    return false;
}
