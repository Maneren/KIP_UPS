#pragma once

#include <chrono>
#include <net/address.h>
#include <net/exception.h>
#include <net/file_descriptor.h>

namespace net {

struct Socket {
  FileDescriptor fd;

  Socket(const Address &addr, int type) : Socket(addr.family(), type) {}
  Socket(int family, int type);
  Socket(FileDescriptor fd) : fd(fd) {}

  Socket(const Socket &other) : fd(other.fd) {}
  Socket &operator=(const Socket &other) {
    this->fd = other.fd;
    return *this;
  }

  Socket(Socket &&other) : fd(std::move(other.fd)) {}
  Socket &operator=(Socket &&other) {
    this->fd = std::move(other.fd);
    return *this;
  }

  ~Socket() = default;

  template <typename T> void setopts(int level, int optname, const T &optval) {
    if (setsockopt(
            fd.fd, level, optname, &optval, static_cast<socklen_t>(sizeof(T))
        ) < 0) {
      throw io_exception("failed to set socket options");
    }
  };

  template <typename T> T getopts(int level, int optname) const {
    T optval;
    auto len = static_cast<socklen_t>(sizeof(T));
    if (getsockopt(fd.fd, level, optname, &optval, &len) < 0) {
      throw io_exception("failed to get socket options");
    }
    return optval;
  };

  void bind_to(const Address &addr);

  Socket accept(sockaddr &storage, socklen_t &len, int flags = 0) const;

  void connect(const Address &addr);
  void connect_timeout(const Address &addr, std::chrono::microseconds timeout);

  int error_state() const;

  void set_nonblocking(bool blocking);

  int read(void *buf, const size_t len) const;
  int write(const void *buf, const size_t len) const;

  int recv(void *buf, const size_t len, int flags) const;
  int send(const void *buf, const size_t len, int flags) const;
};

} // namespace net
