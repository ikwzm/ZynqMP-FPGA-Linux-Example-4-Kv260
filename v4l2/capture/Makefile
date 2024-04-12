DEBUG_FLAG   := -g

HEADER_FILES := v4l2_capture.h          \
                v4l2_capture_dma_heap.h \
                v4l2_capture_udmabuf.h  \
                $(END_OF_LINE)

v4l2_capture_test: v4l2_capture_test.cpp $(HEADER_FILES)
	g++ $(DEBUG_FLAG) -o $@ v4l2_capture_test.cpp
