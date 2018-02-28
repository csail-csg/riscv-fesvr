// See LICENSE for license details.

#ifndef __MEMIF_H
#define __MEMIF_H

#include <stdint.h>
#include <stddef.h>
#include <stdexcept>
#include <string.h>

typedef uint64_t reg_t;
typedef int64_t sreg_t;
typedef reg_t addr_t;

class htif_t;

// A wrapper that calls into htif::read_chunk/write_chunk to read/write the
// memory of target (riscv proc)
class memif_t
{
public:
  memif_t(htif_t* _htif) : htif(_htif) {}
  virtual ~memif_t(){}

  // read and write byte arrays
  virtual void read(addr_t addr, size_t len, void* bytes);
  virtual void write(addr_t addr, size_t len, const void* bytes);

  // read and write 8-bit words
  virtual uint8_t read_uint8(addr_t addr);
  virtual int8_t read_int8(addr_t addr);
  virtual void write_uint8(addr_t addr, uint8_t val);
  virtual void write_int8(addr_t addr, int8_t val);

  // read and write 16-bit words
  virtual uint16_t read_uint16(addr_t addr);
  virtual int16_t read_int16(addr_t addr);
  virtual void write_uint16(addr_t addr, uint16_t val);
  virtual void write_int16(addr_t addr, int16_t val);

  // read and write 32-bit words
  virtual uint32_t read_uint32(addr_t addr);
  virtual int32_t read_int32(addr_t addr);
  virtual void write_uint32(addr_t addr, uint32_t val);
  virtual void write_int32(addr_t addr, int32_t val);

  // read and write 64-bit words
  virtual uint64_t read_uint64(addr_t addr);
  virtual int64_t read_int64(addr_t addr);
  virtual void write_uint64(addr_t addr, uint64_t val);
  virtual void write_int64(addr_t addr, int64_t val);

protected:
  htif_t* htif;
};

// dummy memif which does nothing
class dummy_memif_t : public memif_t
{
public:
  dummy_memif_t() : memif_t(nullptr) {}
  virtual ~dummy_memif_t() {}

  // read and write byte arrays
  virtual void read(addr_t addr, size_t len, void* bytes) {}
  virtual void write(addr_t addr, size_t len, const void* bytes) {}
};

// memif to wrap up a blob of bytes
class blob_memif_t : public memif_t
{
public:
  blob_memif_t(char *bytes, addr_t b, size_t sz)
   : memif_t(nullptr), blob(bytes), base(b), size(sz) {}
  virtual ~blob_memif_t() {}

  // read and write byte arrays
  virtual void read(addr_t addr, size_t len, void* bytes) {
    if (addr < base) {
      throw std::runtime_error("blob memif read underflow");
    }
    if (addr - base + len > size) {
      throw std::runtime_error("blob memif read overflow");
    }
    memcpy(bytes, blob + addr - base, len);
  }
  virtual void write(addr_t addr, size_t len, const void* bytes) {
    if (addr < base) {
      throw std::runtime_error("blob memif write underflow");
    }
    if (addr - base + len > size) {
      throw std::runtime_error("blob memif write overflow");
    }
    memcpy(blob + addr - base, bytes, len);
  }

private:
  char *blob;
  addr_t base; // we subtract base from any intput addr
  size_t size;
};

#endif // __MEMIF_H
