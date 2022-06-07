#include "FS.h"
#include "FSImpl.h"

using namespace fs;
size_t File::write(uint8_t c)
{
    if(!_p)
    {
        return 0;
    }
    return _p->write(&c,1);
}

time_t File::getLastWrite()
{
    if(!_p){
        return 0;
    }
    return _p->getLastWrite();
}
size_t File::write(const uint8_t *buf,size_t size)
{
    if (!_p)
    {
       return 0;
    }
    return _p->write(buf,size);    
}

int File::available()
{
    if (!_p)
    {
        return false;
    }
    return _p->size() -_p->position();
} 
/*int File::read()
{
    if (!_p) {
        return -1;
    }

    uint8_t result;
    if (_p->read(&result, 1) != 1) {
        return -1;
    }

    return result;
}*/
int File::read()
{
    if(!_p){
        return -1;
    }
    uint8_t result;
    if (_p->read(result,1)!=1)
    {
        return -1;
    }
    
    return result;
}
