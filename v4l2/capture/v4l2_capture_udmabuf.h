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
#ifndef   V4L2_CAPTURE_UDMABUF_H
#define   V4L2_CAPTURE_UDMABUF_H

#include "v4l2_capture.h"

class v4l2_capture_udmabuf: public v4l2_capture
{
 public:
  int           _udmabuf_device_handle;
  std::string   _udmabuf_device_name;

  v4l2_capture_udmabuf() :
    v4l2_capture(V4L2_MEMORY_USERPTR),
    _udmabuf_device_handle(-1)
  {
  }
  v4l2_capture_udmabuf(const char* video_device_name, const char* udmabuf_device_name) :
    v4l2_capture(video_device_name, V4L2_MEMORY_USERPTR)
  {
    _udmabuf_device_name   = udmabuf_device_name;
    _udmabuf_device_handle = ::open(_udmabuf_device_name.c_str(), O_RDWR);
  }
  bool opend() override
  {
    if (v4l2_capture::opend() == false)
      return false;
    if (_udmabuf_device_handle < 0)
      return false;
    return true;
  }
  bool memmap_buffers() override
  {
    //
    // Calc udmabuf_size and clear _buffers.buf[].size 
    //
    size_t udmabuf_size = 0;
    for (int i = 0; i < _buffers.size; i++)
    {
      struct v4l2_buffer v4l2_buf = {0};
      v4l2_buf.type   = _v4l2_buf_type;
      v4l2_buf.memory = _v4l2_mem_type;
      v4l2_buf.index  = i;
      if (try_ioctl(VIDIOC_QUERYBUF, &v4l2_buf) == false) 
      {
        _buffers.buf[i].size  = 0;
      }
      else
      {
        _buffers.buf[i].size  = v4l2_buf.length;
        udmabuf_size += v4l2_buf.length;
      }
    }
    if (udmabuf_size == 0)
      return false;
    //
    // mmap udmabuf
    //
    void*  udmabuf_start = mmap(NULL,
                                udmabuf_size,
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED,
                                _udmabuf_device_handle,
                                0);
    if (udmabuf_start == MAP_FAILED)
    {
      for (int i = 0; i < _buffers.size; i++)
      {
         _buffers.buf[i].size  = 0;
         _buffers.buf[i].start = NULL;
      }
      return false;
    }
    //
    // Clear udmabuf
    //
    {
      std::uint64_t* udmabuf_ptr = static_cast<std::uint64_t*>(udmabuf_start);
      for (int i = 0; i < udmabuf_size/sizeof(std::uint64_t) ; i++)
      {
        udmabuf_ptr[i] = 0;
      }
    }
    //
    // Map udmabuf to _buffers
    //
    {
      std::uint8_t*  udmabuf_ptr = static_cast<std::uint8_t* >(udmabuf_start);
      for (int i = 0; i < _buffers.size; i++)
      {
        if (_buffers.buf[i].size == 0)
        {
           _buffers.buf[i].start = NULL;
        }
        else
        {
          _buffers.buf[i].start = udmabuf_ptr;
          udmabuf_ptr += _buffers.buf[i].size;
        }
      }
    }
    return true;
  }
  bool enqueue_buffer(int index) override
  {
    void*  buf_start;
    size_t buf_size;
    if (opend() == false)
      return false;
    if (get_buffer_map(index, &buf_start, &buf_size) == false)
      return false;
    struct v4l2_buffer v4l2_buf = {0};
    v4l2_buf.type      = _v4l2_buf_type;
    v4l2_buf.memory    = _v4l2_mem_type;
    v4l2_buf.index     = index;
    v4l2_buf.length    = buf_size;
    v4l2_buf.m.userptr = reinterpret_cast<unsigned long>(buf_start);
    return try_ioctl(VIDIOC_QBUF, &v4l2_buf);
  }
};

#endif // V4L2_CAPTURE_UDMABUF_H
