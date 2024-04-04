/*********************************************************************************
 *
 *       Copyright (C) 2024 Ichiro Kawazome
 *       All rights reserved.
 * 
 *       Redistribution and use in source and binary forms, with or without
 *       modification, are permitted provided that the following conditions
 *       are met:
 * 
 *         1. Redistributions of source code must retain the above copyright
 *            notice, this list of conditions and the following disclaimer.
 * 
 *         2. Redistributions in binary form must reproduce the above copyright
 *            notice, this list of conditions and the following disclaimer in
 *            the documentation and/or other materials provided with the
 *            distribution.
 * 
 *       THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *       "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *       LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *       A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
 *       OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *       SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *       LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *       DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *       THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *       (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *       OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 ********************************************************************************/
#ifndef   V4L2_CAPTURE_H
#define   V4L2_CAPTURE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/videodev2.h>

#include <iostream>
#include <string>

class v4l2_capture
{
  struct buffer
  {
    void*   start;
    size_t  size;
  };

  class Buffers
  {
   public:
    buffer* buf;
    int     size;
    Buffers() : buf(NULL), size(0) {}
  };

 public:
  int                    _video_device_handle;
  std::string            _video_device_name;
  Buffers                _buffers;
  int                    _poll_timeout;
  struct v4l2_pix_format _v4l2_pix_format;
  enum   v4l2_buf_type   _v4l2_buf_type;
  enum   v4l2_memory     _v4l2_mem_type;
  int                    _ioctl_status;
  
 protected:
  bool try_ioctl(unsigned long ioctl_code, void* parameter, bool fail_if_busy = true, int attempts = 10)
  {
    _ioctl_status = 0;
    while(true)
    {
      errno = 0;
      int result = ioctl(_video_device_handle, ioctl_code, parameter);
      int err    = errno;
      if (result != -1)
        return true;
      const bool is_busy = (err == EBUSY);
      if (is_busy && fail_if_busy)
      {
        _ioctl_status = EBUSY;
        return false;
      }
      if (!(is_busy || errno == EAGAIN))
      {
        _ioctl_status = errno;
        return false;
      }
      if (--attempts == 0)
      {
        _ioctl_status = EBUSY;
        return false;
      }
    }
    return true;
  }

 public:
  v4l2_capture(enum v4l2_memory v4l2_mem_type) :
    _video_device_handle(-1),
    _poll_timeout(5000),
    _v4l2_buf_type(V4L2_BUF_TYPE_VIDEO_CAPTURE),
    _v4l2_mem_type(v4l2_mem_type),
    _buffers()
  {
  }

  v4l2_capture() : v4l2_capture(V4L2_MEMORY_MMAP)
  {
  }

  v4l2_capture(const char* device_name, enum v4l2_memory v4l2_mem_type) : v4l2_capture(v4l2_mem_type)
  {
    open(device_name);
  }

  v4l2_capture(const char* device_name) : v4l2_capture(device_name, V4L2_MEMORY_MMAP)
  {
  }
  
    ~v4l2_capture()
  {
    if (_buffers.size > 0) {
      free(_buffers.buf);
      _buffers.buf  = NULL;
      _buffers.size = 0;
    }
    if (opend())
      close();
  }
  
  virtual bool opend()
  {
    return (_video_device_handle >= 0) ? true : false;
  }

  bool open(const char* device_name)
  {
    _video_device_name   = device_name;
    _video_device_handle = ::open(_video_device_name.c_str(), O_RDWR | O_NONBLOCK, 0);
    if (_video_device_handle < 0)
      return false;
    return true;
  }

  bool close()
  {
    if (_video_device_handle < 0)
      return false;
    ::close(_video_device_handle);
    _video_device_handle = -1;
    return true;
  }

  bool set_format(int width, int height, int pixelformat)
  {
    if (_video_device_handle < 0)
      return false;
    struct v4l2_format v4l2_format  = {0};
    v4l2_format.type                = _v4l2_buf_type;
    v4l2_format.fmt.pix.width       = width;
    v4l2_format.fmt.pix.height      = height;
    v4l2_format.fmt.pix.pixelformat = pixelformat;
    return try_ioctl(VIDIOC_S_FMT, &v4l2_format);
  }    

  bool get_format()
  {
    if (_video_device_handle < 0)
      return false;
    struct v4l2_format v4l2_format  = {0};
    v4l2_format.type                = _v4l2_buf_type;
    if (try_ioctl(VIDIOC_G_FMT, &v4l2_format) == false)
      return false;
    _v4l2_pix_format = v4l2_format.fmt.pix;
    return true;
  }

  bool request_buffers(int count)
  {
    if (_video_device_handle < 0)
      return false;

    struct v4l2_requestbuffers req = {0};
    req.count  = count;
    req.type   = _v4l2_buf_type;
    req.memory = _v4l2_mem_type;
    if (try_ioctl(VIDIOC_REQBUFS, &req) == false)
      return false;
    if (req.count < count)
      return false;

    _buffers.buf  = (struct buffer*)calloc(req.count, sizeof(struct buffer));
    if (_buffers.buf == NULL)
      return false;
    _buffers.size = req.count;
    return true;
  }
  virtual bool memmap_buffers()
  {
    for (int i = 0; i < _buffers.size; i++)
    {
      struct v4l2_buffer v4l2_buf = {0};
      v4l2_buf.type   = _v4l2_buf_type;
      v4l2_buf.memory = _v4l2_mem_type;
      v4l2_buf.index  = i;
      if (try_ioctl(VIDIOC_QUERYBUF, &v4l2_buf) == false)
      {
        _buffers.buf[i].size  = 0;
        _buffers.buf[i].start = NULL;
      }
      else
      {
        _buffers.buf[i].size  = v4l2_buf.length;
        _buffers.buf[i].start = mmap(NULL,
                                     v4l2_buf.length,
                                     PROT_READ | PROT_WRITE,
                                     MAP_SHARED,
                                     _video_device_handle,
                                     v4l2_buf.m.offset);
      }
    }
    return true;
  }

  virtual bool enqueue_buffer(int index)
  {
    if (_video_device_handle < 0)
      return false;
    struct v4l2_buffer v4l2_buf = {0};
    v4l2_buf.type   = _v4l2_buf_type;
    v4l2_buf.memory = _v4l2_mem_type;
    v4l2_buf.index  = index;
    return try_ioctl(VIDIOC_QBUF, &v4l2_buf);
  }

  bool enqueue_buffers()
  {
    if (_video_device_handle < 0)
      return false;
    for (int i = 0; i < _buffers.size; i++)
    {
      if (enqueue_buffer(i) == false)
      {
        return false;
      }
    }
    return true;
  }

  int dequeue_buffer()
  {
    if (_video_device_handle < 0)
      return false;
    struct v4l2_buffer v4l2_buf = {0};
    v4l2_buf.type   = _v4l2_buf_type;
    v4l2_buf.memory = _v4l2_mem_type;
    if (try_ioctl(VIDIOC_DQBUF, &v4l2_buf) == false)
    {
      return -1;
    }
    return v4l2_buf.index;
  }

  virtual void print_buffer_info(std::ostream& out, int index)
  {
    std::uint32_t* buf_ptr;
    size_t         buf_size;
    if (get_buffer_map(index, (void**)(&buf_ptr), &buf_size) == false)
    {
      out << "  "  << index << ": {start: NULL, size: 0}" << std::endl;
    }
    else
    {
      out << "  "  << index << ": {start: " << std::hex << buf_ptr << ", size: " << std::dec << buf_size << "}" << std::endl;
    }
  }

  virtual void print_buffers_info(std::ostream& out)
  {
    out << "Buffers: " << std::endl;
    out << "  Type : V4L2_MEMORY_MMAP"  << std::endl;
    out << "  Size : " << _buffers.size << std::endl;
    for (int index = 0; index < _buffers.size; index++)
    {
      print_buffer_info(out, index);
    }
  }

  bool start_stream()
  {
    if (_video_device_handle < 0)
      return false;
    enum v4l2_buf_type type = _v4l2_buf_type;
    return try_ioctl(VIDIOC_STREAMON, &type);
  }

  bool stop_stream()
  {
    if (_video_device_handle < 0)
      return false;
    enum v4l2_buf_type type = _v4l2_buf_type;
    return try_ioctl(VIDIOC_STREAMOFF, &type);
  }

  bool wait_stream()
  {
    if (_video_device_handle < 0)
      return false;
    struct pollfd fds[1];
    fds[0].fd     = _video_device_handle;
    fds[0].events = POLLIN;
    int result = poll(fds, 1, _poll_timeout);
    if (-1 == result)
    {
      return false;
    }
    return true;
  }

  int  buffers_size()
  {
    return _buffers.size;
  }

  bool get_buffer_map(int index, void** start, size_t* size)
  {
    if ((index < 0) || (_buffers.size <= index))
      return false;
    if (_buffers.buf[index].start == NULL)
      return false;
    if (_buffers.buf[index].start == MAP_FAILED)
      return false;
    *start = _buffers.buf[index].start;
    *size  = _buffers.buf[index].size;
    return true;    
  }
};

#endif // V4L2_CAPTURE_H
