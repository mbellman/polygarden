#pragma once

#include "subsystem/traits/Positionable.h"
#include "subsystem/traits/Orientable.h"
#include "subsystem/traits/Scalable.h"

class Transformable : public Positionable, public Orientable, public Scalable {};