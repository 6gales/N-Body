#include "computer.h"

Computer* getInstanceOf()
{
    return new SequentialComputer();
}
