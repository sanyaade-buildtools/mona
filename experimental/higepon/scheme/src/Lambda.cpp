#include "Lambda.h"

using namespace monash;

Lambda::Lambda(Objects* body, Variables* parameters) : body_(body), parameters_(parameters)
{
}

Lambda::~Lambda()
{
}

std::string Lambda::toString()
{
    return "lambda : " + body_->at(0)->toString();
}

int Lambda::type() const
{
    return Object::LAMBDA;
}
