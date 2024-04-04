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
#include "v4l2_capture.h"
#include "v4l2_capture_udmabuf.h"
#include "v4l2_capture_dma_heap.h"

#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <chrono>
#include <vector>
#include <numeric>

class v4l2_capture_test_bench
{
  class TimePonts
  {
   public:
    std::chrono::steady_clock::time_point start;
    std::chrono::steady_clock::time_point wait_done;
    std::chrono::steady_clock::time_point dequeue_done;
    std::chrono::steady_clock::time_point check_done;
    std::chrono::steady_clock::time_point enqueue_done;
    std::chrono::steady_clock::time_point done;

    void now_start()        {start        = std::chrono::steady_clock::now();}
    void now_wait_done()    {wait_done    = std::chrono::steady_clock::now();}
    void now_dequeue_done() {dequeue_done = std::chrono::steady_clock::now();}
    void now_check_done()   {check_done   = std::chrono::steady_clock::now();}
    void now_enqueue_done() {enqueue_done = std::chrono::steady_clock::now();}
    void now_done()         {done         = std::chrono::steady_clock::now();}

    double duration(std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point done)
    {
      return std::chrono::duration_cast<std::chrono::microseconds>(done - start).count();
    }
    double total_time()   {return duration(start       , done);        }
    double wait_time()    {return duration(start       , wait_done);   }
    double dequeue_time() {return duration(wait_done   , dequeue_done);}
    double check_time()   {return duration(dequeue_done, check_done);  }
    double enqueue_time() {return duration(check_done  , enqueue_done);}
  };
  
 public:
  int           _queue_size;
  int           _try_count;
  v4l2_capture* _capture;
  TimePonts*    _time_point_list;

  v4l2_capture_test_bench(v4l2_capture* capture, int queue_size, int try_count) :
    _queue_size(queue_size),
    _try_count(try_count),
    _capture(capture)
  {
    _time_point_list = new TimePonts[try_count];
  }

  bool set_format(int width, int height, int pixelformat)
  {
    if (_capture->set_format(width, height, pixelformat) == false)
    {
      std::cerr << "Can not set format" << std::endl;
      return false;
    }
    if (_capture->get_format() == false)
    {
      std::cerr << "Can not get format" << std::endl;
      return false;
    }
    return true;
  }

  bool prepare_buffers()
  {
    bool init_error = false;
    //
    // Request Buffers
    //
    if (_capture->request_buffers(_queue_size) == false)
    {
      std::cerr << "Can not request buffers size=" << _queue_size << std::endl;
      return false;
    }
    //
    // Memory Map Buffers
    //
    if (_capture->memmap_buffers() == false)
    {
      std::cerr << "Can not memmap buffers" << std::endl;
      return false;
    }
    //
    // Initialize Buffers
    //
    int buffers_size = _capture->buffers_size();
    for (int buf_index = 0; buf_index < buffers_size; buf_index++)
    {
      std::uint32_t* buf_ptr;
      size_t         buf_size;
      if (_capture->get_buffer_map(buf_index, (void**)(&buf_ptr), &buf_size) == false)
      {
        std::cerr << "Can not mapped buffer[" << buf_index << "]" << std::endl;
        init_error = true;
      }
      for (int i = 0; i < buf_size/sizeof(std::uint32_t) ; i++)
      {
        buf_ptr[i] = 0xFFFFFFFF;
      }
    }
    if (init_error == true)
      return false;
    else
      return true;
  }

  void print_buffers_info(std::ostream& out)
  {
    _capture->print_buffers_info(out);
  }
  void print_buffers_info()
  {
    print_buffers_info(std::cout);
  }
  
  bool run(std::ostream& out, bool log)
  {
    //
    // Enqueue Buffers
    //
    if (_capture->enqueue_buffers() == false)
    {
      std::cerr << "Can not enqueue buffers" << std::endl;
      return false;
    }
    //
    // Start Stream
    //
    if (_capture->start_stream() == false)
    {
      std::cerr << "Can not start stream" << std::endl;
      return false;
    }
    //
    // Check Captured Buffers
    //
    bool try_error = false;
    for (int try_num = 0; try_num < _try_count; try_num++)
    {
      int            buf_index;
      std::uint32_t* buf_ptr;
      size_t         buf_size;
      int            mismatch;
      _time_point_list[try_num].now_start();
      if (_capture->wait_stream() == false)
      {
        std::cerr << "Can not wait stream" << std::endl;
        try_error = true;
        break;
      }
      _time_point_list[try_num].now_wait_done();
      buf_index = _capture->dequeue_buffer();
      if (buf_index < 0)
      {
        std::cerr << "Can not dequeue buffer" << std::endl;
        try_error = true;
        break;
      }
      _time_point_list[try_num].now_dequeue_done();
      if (log == true)
        out << "## Dequeue buffer[" << buf_index << "]";
      if (_capture->get_buffer_map(buf_index, (void**)(&buf_ptr), &buf_size) == false)
      {
        std::cerr << "Can not mapped buffer[" << buf_index << "]" << std::endl;
        try_error = true;
        break;
      }
      mismatch = 0;
      for (int i = 0; i < buf_size/sizeof(std::uint32_t) ; i++)
      {
        if (buf_ptr[i] != i) {
          if (mismatch == 0) {
          // std::cerr << "Mismatch data[0x" << std::hex << i << "] = 0x" << std::hex << buf_ptr[i] << std::endl;
          }
          mismatch++;
        }
      }
      if (log == true)
      {
        if (mismatch > 0)
          out << "  Check Buffer[" << buf_index << "] Data... Mismatch(=" << mismatch << ")!";
        else
          out << "  Check Buffer[" << buf_index << "] Data... Ok!";
      }
      _time_point_list[try_num].now_check_done();
      if (_capture->enqueue_buffer(buf_index) == false)
      {
        std::cerr << "Can not enqueue buffer[" << buf_index << "]" << std::endl;
        try_error = true;
        break;
      }
      _time_point_list[try_num].now_enqueue_done();
      if (log == true)
        out << "  Enqueue Buffer[" << buf_index << "]"<< std::endl;
      _time_point_list[try_num].now_done();
    }
    //
    // Stop Stream
    //
    if (_capture->stop_stream() == false)
    {
      std::cerr << "Can not stop stream" << std::endl;
      return false;
    }
    if (try_error == true)
      return false;
    else
      return true;
  }
  bool run(bool log)
  {
    return run(std::cout, log);
  }

  void print_format(std::ostream& out)
  {
    out << "Format: " << std::endl;
    out << "  Width        : " << _capture->_v4l2_pix_format.width        << std::endl;
    out << "  Height       : " << _capture->_v4l2_pix_format.height       << std::endl;
    out << "  BytesPerLine : " << _capture->_v4l2_pix_format.bytesperline << std::endl;
    out << "  SizeImage    : " << _capture->_v4l2_pix_format.sizeimage    << std::endl;
  }

  void print_format()
  {
    print_format(std::cout);
  }

  void print_run_times(std::ostream& out)
  {
    out << "Times: # microseconds " << std::endl;
    std::vector<double> loop_times;
    std::vector<double> wait_times;
    std::vector<double> dequeue_times;
    std::vector<double> check_times;
    std::vector<double> enqueue_times;
    for (int try_num = 0; try_num < _try_count; try_num++)
    {
      double loop_time    = _time_point_list[try_num].total_time();
      double wait_time    = _time_point_list[try_num].wait_time();
      double dequeue_time = _time_point_list[try_num].dequeue_time();
      double check_time   = _time_point_list[try_num].check_time();
      double enqueue_time = _time_point_list[try_num].enqueue_time();

      loop_times   .push_back(loop_time   );
      wait_times   .push_back(wait_time   );
      dequeue_times.push_back(dequeue_time);
      check_times  .push_back(check_time  );
      enqueue_times.push_back(enqueue_time);
    
      out << "  "    << try_num << ": "; 
      out << "{ Total: "   << loop_time;
      out << ", Wait: "    << wait_time;
      out << ", Dequeue: " << dequeue_time;
      out << ", Check: "   << check_time;
      out << ", Enqueue: " << enqueue_time;
      out << " }" << std::endl;
    }
    {
      out << "  Avarage: "; 
      out << "{ Total: "   << std::accumulate(loop_times.begin()   , loop_times.end(), 0.0)    / loop_times.size();
      out << ", Wait: "    << std::accumulate(wait_times.begin()   , wait_times.end(), 0.0)    / wait_times.size();
      out << ", Dequeue: " << std::accumulate(dequeue_times.begin(), dequeue_times.end(), 0.0) / dequeue_times.size();
      out << ", Check: "   << std::accumulate(check_times.begin()  , check_times.end(), 0.0)   / check_times.size();
      out << ", Enqueue: " << std::accumulate(enqueue_times.begin(), enqueue_times.end(), 0.0) / enqueue_times.size();
      out << "}" <<  std::endl;
    }  
  }
  void print_run_times()
  {
    print_run_times(std::cout);
  }
};

int main()
{
  int          queue_size =  4;
  int          try_count  = 10;
  const char*  video_device_name = "/dev/video0";
//v4l2_capture cap(video_device_name);
//v4l2_capture_udmabuf cap(video_device_name, "/dev/udmabuf0");
  v4l2_capture_dma_heap cap(video_device_name, "/dev/dma_heap/reserved");

  if (cap.opend() == false)
  {
    std::cerr << "Can not open " << video_device_name << std::endl;
    exit(EXIT_FAILURE);
  }
  std::cout << "Video Device Name    : " << video_device_name << std::endl;

  v4l2_capture_test_bench tb(&cap, queue_size, try_count);

  if (tb.set_format(512, 512, V4L2_PIX_FMT_RGB32) == false)
    exit(EXIT_FAILURE);
  tb.print_format();
  if (tb.prepare_buffers() == false)
    exit(EXIT_FAILURE);
  tb.print_buffers_info();
  if (tb.run(false) == false)
    exit(EXIT_FAILURE);
  tb.print_run_times();
  cap.close();
}
