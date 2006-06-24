#ifndef __FRAME_H__
#define __FRAME_H__

#include <map>
#include <vector>
#include "Variable.h"

namespace monash {

class Frame
{
public:
    Frame();
    Frame(Variables* variables, Objects* values);
    virtual ~Frame();

    Object* lookup(Variable* variable);
    void insert(Variable* variable, Object* value);
    void remove(Variable* variable);

protected:
    typedef std::map<Variable*, Object*> FrameMap;
    FrameMap map_;
};

}; // namespace monash

#endif // __FRAME_H__
