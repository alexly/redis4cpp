#include "bytebuffer.h"
#include <string.h>
#include <netinet/in.h>

namespace redis4cpp {

template<class T> inline T _min(T a, T b) { return (a > b) ? b : a; }
template<class T> inline T _max(T a, T b) { return (a < b) ? b : a; }
  
ByteBuffer::ByteBuffer(size_t size) {
    start_ = 0;
    end_   = 0;
    size_  = size;
    bytes_ = new char[size_];
}

ByteBuffer::ByteBuffer(const char* bytes, size_t size ) {
    start_ = 0;
    end_   = size;
    size_  = size;
    bytes_ = new char[size_];
    memcpy(bytes_, bytes, end_);
}

ByteBuffer::ByteBuffer(const char* bytes) {
    start_ = 0;
    end_   = strlen(bytes);
    size_  = end_;
    bytes_ = new char[size_];
    memcpy(bytes_, bytes, end_);
}

ByteBuffer::~ByteBuffer() {
    delete bytes_;
}

const std::string& ByteBuffer::Str() const
{
    _b.append(Data(), Data() + Length());
    return _b;
}

bool ByteBuffer::ReadUInt8(boost::uint8_t& val) {
    return ReadBytes(reinterpret_cast<char*>(&val), sizeof(boost::uint8_t));
}

bool ByteBuffer::ReadUInt16(boost::uint16_t& val) {
    boost::uint16_t v;
    if (!ReadBytes(reinterpret_cast<char*>(&v), sizeof(boost::uint16_t))) {
        return false;
    }
    else {
        //host to network
        val = htons(v);
        return true;
    }
}

bool ByteBuffer::ReadUInt32(boost::uint32_t& val) {
    boost::uint32_t v;
    if (!ReadBytes(reinterpret_cast<char*>(&v), sizeof(boost::uint32_t))) {
        return false;
    }
    else {
        val = htonl(v);
        return true;
    }
}

bool ByteBuffer::ReadString(std::string& val, size_t len) {
    if (len > Length()) {
        return false;
    } else {
        val.append(bytes_ + start_, len);
        start_ += len;
        return true;
    }
}

bool ByteBuffer::ReadBytes(char* val, size_t len) {
  if (len > Length()) {
    return false;
  } else {
    memcpy(val, bytes_ + start_, len);
    start_ += len;
    return true;
  }
}

void ByteBuffer::WriteUInt8(boost::uint8_t val) {
    WriteBytes(reinterpret_cast<const char*>(&val), sizeof(boost::uint8_t));
}

void ByteBuffer::WriteUInt16(boost::uint16_t val) {
    boost::uint8_t v = ntohs(val);
    WriteBytes(reinterpret_cast<const char*>(&v), sizeof(boost::uint8_t));
}

void ByteBuffer::WriteUInt32(boost::uint32_t val) {
    boost::uint32_t v = ntohl(val);
    WriteBytes(reinterpret_cast<const char*>(&v), 4);
}

void ByteBuffer::WriteString(const std::string& val) {
  WriteBytes(val.c_str(), val.size());
}

void ByteBuffer::WriteBytes(const char* val, size_t len) {
  if (Length() + len > Capacity())
    Resize(Length() + len);

  memcpy(bytes_ + end_, val, len);
  end_ += len;
}

void ByteBuffer::Resize(size_t size) {
  if (size > size_)
    size = _max(size, 3 * size_ / 2);

  size_t len = _min(end_ - start_, size);
  char* new_bytes = new char[size];
  memcpy(new_bytes, bytes_ + start_, len);
  delete [] bytes_;

  start_ = 0;
  end_   = len;
  size_  = size;
  bytes_ = new_bytes;
}

void ByteBuffer::Shift(size_t size) {
  if (size > Length())
    return;

  end_ = Length() - size;
  memmove(bytes_, bytes_ + start_ + size, end_);
  start_ = 0;
}

}