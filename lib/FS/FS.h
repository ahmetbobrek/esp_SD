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

File(FileImpl p=FileImplPtr()) : _p(p){
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

}








}//namespace fs


#endif // FS_H
