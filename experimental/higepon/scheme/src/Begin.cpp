#include "Begin.h"

using namespace monash;

Begin::Begin(Objects* actions) : actions_(actions)
{
}

Begin::~Begin()
{
}

std::string Begin::toString()
{
    Objects::iterator o = actions_->begin();
    return "begin: " + (*o)->toString();
}

int Begin::type() const
{
    return Object::BEGIN;
}
