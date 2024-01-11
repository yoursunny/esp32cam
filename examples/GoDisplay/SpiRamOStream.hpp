#ifndef SPIRAM_OSTREAM_HPP
#define SPIRAM_OSTREAM_HPP

#include <Arduino.h>

/** @brief write-only Stream backed by fixed-size SPIRAM buffer. */
class SpiRamOStream : public Stream {
public:
  explicit SpiRamOStream(size_t cap)
    : m_buf(reinterpret_cast<uint8_t*>(heap_caps_malloc(cap, MALLOC_CAP_SPIRAM)))
    , m_len(0)
    , m_cap(cap) {}

  ~SpiRamOStream() {
    free(m_buf);
  }

  const uint8_t* data() const {
    return m_buf;
  }

  const size_t size() const {
    return m_len;
  }

  size_t write(const uint8_t* buffer, size_t size) override {
    size_t count = min(size, m_cap - m_len);
    memcpy(m_buf + m_len, buffer, count);
    m_len += count;
    return count;
  }

  size_t write(uint8_t data) override {
    return write(&data, 1);
  }

  int available() override {
    return 0;
  }

  int read() override {
    return -1;
  }

  int peek() override {
    return -1;
  }

  void flush() override {}

private:
  uint8_t* m_buf;
  size_t m_len;
  size_t m_cap;
};

#endif // SPIRAM_OSTREAM_HPP
