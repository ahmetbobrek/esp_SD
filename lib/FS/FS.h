#if !defined(FS_H)
#define FS_H
#include <Arduino.h>
#include <memory>
namespace fs{
#define FILE_READ       "r"
#define FILE_WRITE      "w"
#define FILE_APPEND     "a"

class File;

class FileImpl;
typedef std::shared_ptr<FileImpl>FileImplPtr;
class FSImpl;
typedef std::shared_ptr<FSImpl>FSImplPtr;

enum SeekMode{
    SeekSet=0,
    SeekCur=1,
    SeekEnd=2
    };

class File  :   public Stream
{
public:


File(FileImplPtr p=FileImplPtr()) : _p(p){
        _timeout=0;
    }

    size_t write(uint8_t) override;
    size_t write(const uint8_t *buf, size_t size) override;
    int available() override;
    int read() override;
    int peek() override;
    void flush() override;
    size_t read(uint8_t* buf,size_t size);   
    size_t readBytes(char *buffer,size_t length)
    {
        return read((uint8_t*)buffer,length);
    }
    bool seek(uint32_t pos, SeekMode mode);
    bool seek(uint32_t pos)
    {
        return seek(pos,SeekSet);
    }
  
    size_t position() const;
    size_t size() const;
    void close();
    operator bool() const;
    time_t getLastWrite();
    const char* name() const;

    boolean isDirectory(void);
    File openNextFile(const char* mode=FILE_READ);
    void rewindDirectory(void);


protected:
FileImplPtr _p;

};

class FS
{
public:
    FS(FSImplPtr impl): _impl(impl){}
    File open(const char* path,const char* mode=FILE_READ);
    File open(const String& path,const char* mode=FILE_READ);

    bool exists(const char* path);
    bool exists(const String& path);

    bool remove(const char* path);
    bool remove(const String& path);

    bool rename(const char* pathFrom, const char* pathTo);
    bool rename(const String& pathFrom,const String& pathTo);
    /*bool mkdir(const char *path);
    bool mkdir(const String &path);

    bool rmdir(const char *path);
    bool rmdir(const String &path);*/
   
    bool mkdir(const char *path);
    bool mkdir(const String &path);

    bool rmdir(const char *path);
    bool rmdir(const String &path);


protected:
    FSImplPtr _impl;
};
}
#ifndef FS_NO_GLOBALS
using fs::FS;
using fs::File;
using fs::SeekMode;
using fs::SeekSet;
using fs::SeekCur;
using fs::SeekEnd;
#endif //FS_NO_GLOBALS

#endif // FS_H
