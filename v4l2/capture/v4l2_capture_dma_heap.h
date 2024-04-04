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
#ifndef   V4L2_CAPTURE_DMA_HEAP_H
#define   V4L2_CAPTURE_DMA_HEAP_H

#include "v4l2_capture.h"
#include <linux/dma-heap.h>
#include <vector>

class v4l2_capture_dma_heap: public v4l2_capture
{
 public:
  int              _dma_heap_device_handle;
  std::string      _dma_heap_device_name;
  std::vector<int> _dma_heap_fd_list;

  v4l2_capture_dma_heap() :
    v4l2_capture(V4L2_MEMORY_DMABUF),
    _dma_heap_device_handle(-1)
  {
  }
  v4l2_capture_dma_heap(const char* video_device_name, const char* dma_heap_device_name) :
    v4l2_capture(video_device_name, V4L2_MEMORY_DMABUF)
  {
    _dma_heap_device_name   = dma_heap_device_name;
    _dma_heap_device_handle = ::open(_dma_heap_device_name.c_str(), O_RDWR);
  }
  bool opend() override
  {
    if (v4l2_capture::opend() == false)
      return false;
    if (_dma_heap_device_handle < 0)
      return false;
    return true;
  }

  bool memmap_buffers() override
  {
    int error_count = 0;
    _dma_heap_fd_list.assign(_buffers.size, -1);
    for (int i = 0; i < _buffers.size; i++)
    {
      int                buf_size = 0;
      struct v4l2_buffer v4l2_buf = {0};
      v4l2_buf.type   = _v4l2_buf_type;
      v4l2_buf.memory = _v4l2_mem_type;
      v4l2_buf.index  = i;
      if (try_ioctl(VIDIOC_QUERYBUF, &v4l2_buf) == false)
      {
        _buffers.buf[i].size  = 0;
        _buffers.buf[i].start = NULL;
        _dma_heap_fd_list[i]  = -1;
        error_count++;
        continue;
      }
      buf_size = v4l2_buf.length;
      struct dma_heap_allocation_data alloc_data = {
        .len        = static_cast<__u64>(buf_size),
        .fd         = 0,
        .fd_flags   = (O_RDWR | O_CLOEXEC),
        .heap_flags = 0,
      };
      int ret = ioctl(_dma_heap_device_handle, DMA_HEAP_IOCTL_ALLOC, &alloc_data);
      if (ret < 0)
      {
        _buffers.buf[i].size  = 0;
        _buffers.buf[i].start = NULL;
        _dma_heap_fd_list[i]  = -1;
        error_count++;
        continue;
      }
      _dma_heap_fd_list[i]  = alloc_data.fd;
      void*  dma_heap_start = mmap(NULL,
                                   buf_size,
                                   PROT_READ | PROT_WRITE,
                                   MAP_SHARED,
                                   _dma_heap_fd_list[i],
                                   0);
            
      if (dma_heap_start == MAP_FAILED)
      {
        _buffers.buf[i].size  = 0;
        _buffers.buf[i].start = NULL;
        error_count++;
      }
      else
      {
        _buffers.buf[i].size  = buf_size;
        _buffers.buf[i].start = dma_heap_start;
      }
    }
    if (error_count > 0)
    {
      for (int i = 0; i < _buffers.size; i++)
      {
        if (_buffers.buf[i].start != NULL)
        {
          munmap(_buffers.buf[i].start, _buffers.buf[i].size);
          _buffers.buf[i].start = NULL;
          _buffers.buf[i].size  = 0;
        }    
        if (_dma_heap_fd_list[i] >= 0)
        {
          ::close(_dma_heap_fd_list[i]);
          _dma_heap_fd_list[i] = -1;
        }
      }
      return false;
    }
    return true;
  }

  bool enqueue_buffer(int index) override
  {
    void*  buf_start;
    size_t buf_size;
    if (opend() == false)
      return false;
    if (_dma_heap_fd_list[index] < 0)
      return false;
    if (get_buffer_map(index, &buf_start, &buf_size) == false)
      return false;
    struct v4l2_buffer v4l2_buf = {0};
    v4l2_buf.type      = _v4l2_buf_type;
    v4l2_buf.memory    = _v4l2_mem_type;
    v4l2_buf.index     = index;
    v4l2_buf.length    = buf_size;
    v4l2_buf.m.fd      = _dma_heap_fd_list[index];
    return try_ioctl(VIDIOC_QBUF, &v4l2_buf);
  }

  void print_buffer_info(std::ostream& out, int index) override
  {
    std::uint32_t* buf_ptr;
    size_t         buf_size;
    if (get_buffer_map(index, (void**)(&buf_ptr), &buf_size) == false)
    {
      out << "  "  << index << ": {start: NULL, size: 0";
    }
    else
    {
      out << "  "  << index << ": {start: " << std::hex << buf_ptr << ", size: " << std::dec << buf_size;
    }
    out << ", fd: " << _dma_heap_fd_list[index] << "}" << std::endl;
  }

  void print_buffers_info(std::ostream& out) override
  {
    out << "Buffers: " << std::endl;
    out << "  Type : V4L2_MEMORY_DMABUF"  << std::endl;
    out << "  Size : " << _buffers.size << std::endl;
    for (int index = 0; index < _buffers.size; index++)
    {
      print_buffer_info(out, index);
    }
  }
};

#endif // V4L2_CAPTURE_DMA_HEAP_H
