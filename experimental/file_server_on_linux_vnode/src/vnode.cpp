#include "vnode.h"

using namespace std;
using namespace io;

VnodeManager::VnodeManager()
{
}

VnodeManager::~VnodeManager()
{

}

int VnodeManager::lookup(Vnode* directory, const string& file, Vnode** found)
{
    return directory->fs->lookup(directory, file, found);
}

int VnodeManager::open(const std::string& name, int mode, bool create, dword tid, dword* fileID)
{
    if (create)
    {
        // do something. fix me.
    }

    // now fullpath only. fix me
    if (name.compare(0, 1, "/") != 0) return MONA_ERROR_INVALID_ARGUMENTS;

    // remove first '/'. fix me
    string filename = name.substr(1, name.size() - 1);

    Vnode* file;
    if (lookup(root_, filename, &file) != MONA_OK)
    {
        return MONA_ERROR_ENTRY_NOT_FOUND;
    }

    int ret = file->fs->open(file, mode);
    if (MONA_OK != ret)
    {
        return ret;
    }

    *fileID = this->fileID(file, tid);
    if (fileInfoMap_.find(*fileID) != fileInfoMap_.end())
    {
        printf("error fix me!!! %s %s:%d\n", __func__, __FILE__, __LINE__);
        exit(-1);
    }

    FileInfo* fileInfo = new FileInfo;
    fileInfo->vnode = file;
    fileInfo->context.tid = tid;
    fileInfoMap_.insert(pair< dword, FileInfo* >(*fileID, fileInfo));
    return MONA_OK;
}

Vnode* VnodeManager::alloc()
{
    Vnode* v = new Vnode;
    assert(v != NULL);
    return v;
}

int VnodeManager::read(dword fileID, dword size, monapi_cmemoryinfo** mem)
{
    FileInfoMap::iterator it = fileInfoMap_.find(fileID);
    if (it == fileInfoMap_.end())
    {
        return MONA_ERROR_ENTRY_NOT_FOUND;
    }

    io::FileInfo* fileInfo = (*it).second;
    io::Context* context = &(fileInfo->context);
    context->size = size;

    context->memory = monapi_cmemoryinfo_new();
    if (!monapi_cmemoryinfo_create(context->memory, size, MONAPI_FALSE))
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        monapi_cmemoryinfo_delete(context->memory);
        return MONA_ERROR_MEMORY_NOT_ENOUGH;
    }
    *mem = context->memory;
    return fileInfo->vnode->fs->read(fileInfo->vnode, context);
}

int VnodeManager::seek(dword fileID, dword offset, dword origin)
{
    FileInfoMap::iterator it = fileInfoMap_.find(fileID);
    if (it == fileInfoMap_.end())
    {
        return MONA_ERROR_ENTRY_NOT_FOUND;
    }

    FileInfo* fileInfo = (*it).second;
    Vnode* file = fileInfo->vnode;
    int ret = file->fs->seek(file, offset, origin);
    if (MONA_OK != ret)
    {
        return ret;
    }
    fileInfo->context.offset = offset;
    fileInfo->context.origin = origin;
    return MONA_OK;
}
