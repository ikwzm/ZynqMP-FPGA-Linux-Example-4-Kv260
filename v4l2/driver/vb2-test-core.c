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
#include <linux/cdev.h>
#include <linux/clk.h>
#include <linux/dma-mapping.h>
#include <linux/fs.h>
#include <linux/idr.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/sysctl.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/scatterlist.h>
#include <linux/pagemap.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/version.h>

#include <media/videobuf-vmalloc.h>
#include <media/videobuf2-core.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-ctrls.h>
#include <media/videobuf2-dma-contig.h>

/**
 * DOC: VideoBuf2-Test Constants 
 */

MODULE_DESCRIPTION("VideoBuf2 Test device driver");
MODULE_AUTHOR("ikwzm");
MODULE_LICENSE("Dual BSD/GPL");

#define DRIVER_VERSION     "0.0.1"
#define DRIVER_NAME        "vb2-test"
#define CAPTURE_DRV_NAME   "Traffic driver"
#define PVI_MODULE_NAME    "Traffic"
#define MAX_WIDTH          8192
#define MAX_HEIGHT         2048
#define PIXEL_FORMAT       V4L2_PIX_FMT_RGB32
#define BYTES_PER_PIXEL    (32/8)

struct xxxx_port;
struct xxxx_device;
struct xxxx_buffer;

struct xxxx_port {
	struct v4l2_fh          v4l2_fh;  /* v4l2_fh must be first */
	struct xxxx_device*     xdev;
	struct video_device*    vdev;
	struct vb2_queue        vb2_queue;
	struct list_head        xbuf_list;
	bool                    open;
	bool			streaming;
	bool			stop;
	unsigned int            sequence;
	unsigned int            width;
	unsigned int            height;
	unsigned int            bytesperpixel;
	unsigned int            bytesperline;
	unsigned int            sizeimage;
};
struct xxxx_buffer {
	struct vb2_v4l2_buffer  v4l2_buf; /* v4l2_buf must be first */
	struct list_head        list;
	bool                    allow_dq;
};
struct xxxx_device {
	spinlock_t              slock;
	struct mutex            mutex;
	bool                    setup_done;
	struct list_head        xbuf_list;
	struct xxxx_port*       port;
	struct platform_device* pdev;
	struct video_device*    vdev;
	struct v4l2_device      v4l2_dev;
	void __iomem*           regs_addr;
	int                     irq;
};

#define    MW_ADDR_REGS_ADDR   0x0020
#define    MW_SIZE_REGS_ADDR   0x0028
#define    MW_MODE_REGS_ADDR   0x002C
#define    MW_STAT_REGS_ADDR   0x002E
#define    MW_CTRL_REGS_ADDR   0x002F

#define    MW_MODE_IRQ_ENABLE  (0x1 <<  0)
#define    MW_MODE_AXI_CACHE   (0xF <<  4)
#define    MW_MODE_AXI_PROT    (0x2 <<  8)
#define    MW_MODE_SPECULATIVE (0x1 << 14)

#define    MW_CTRL_RESET          0x80
#define    MW_CTRL_PAUSE          0x40
#define    MW_CTRL_STOP           0x20
#define    MW_CTRL_START          0x10
#define    MW_CTRL_IRQ_ENABLE     0x04
#define    MW_CTRL_FIRST          0x02
#define    MW_CTRL_LAST           0x01

static inline void xxxx_clear_status(struct xxxx_device* xdev)
{
	iowrite8(0, xdev->regs_addr+MW_STAT_REGS_ADDR);
}

static inline void xxxx_set_dma_addr(struct xxxx_device* xdev, dma_addr_t dma_addr)
{
	iowrite32((u32)((dma_addr >>  0) & 0xFFFFFFFF), xdev->regs_addr+MW_ADDR_REGS_ADDR+0);
	iowrite32((u32)((dma_addr >> 32) & 0xFFFFFFFF), xdev->regs_addr+MW_ADDR_REGS_ADDR+4);
}

static inline void xxxx_set_dma_size(struct xxxx_device* xdev, size_t dma_size)
{
	iowrite32((u32)((dma_size >>  0) & 0xFFFFFFFF), xdev->regs_addr+MW_SIZE_REGS_ADDR);
}

static inline void xxxx_set_ctrl_stat_mode(struct xxxx_device* xdev, u8 ctrl, u8 stat, u16 mode)
{
  	u32 ctrl_stat_mode = ((u32)(ctrl) << 24) |
                             ((u32)(stat) << 16) |
                             ((u32)(mode) <<  0) ;
	iowrite32(ctrl_stat_mode, xdev->regs_addr+MW_MODE_REGS_ADDR);
}

static inline void xxxx_set_ctrl(struct xxxx_device* xdev, u8 ctrl)
{
	iowrite8(ctrl, xdev->regs_addr+MW_CTRL_REGS_ADDR);
}

static void xxxx_start(struct xxxx_device* xdev,
                       struct xxxx_buffer* xbuf)
{
	struct xxxx_port*  port      = xdev->port;
	struct vb2_buffer* vb2_buf   = &xbuf->v4l2_buf.vb2_buf;
	unsigned int       sizeimage = port->sizeimage;
	const u16          mode      = MW_MODE_IRQ_ENABLE  |
                                       MW_MODE_AXI_CACHE   |
                                       MW_MODE_AXI_PROT    |
                                       MW_MODE_SPECULATIVE ;
	const u8           stat      = 0;
	const u8           ctrl      = MW_CTRL_START       |
                                       MW_CTRL_IRQ_ENABLE  |
                                       MW_CTRL_FIRST       |
                                       MW_CTRL_LAST        ;
	dma_addr_t         dma_addr = vb2_dma_contig_plane_dma_addr(vb2_buf,0);
	xxxx_set_dma_addr(xdev, dma_addr );
	xxxx_set_dma_size(xdev, sizeimage);
	xxxx_set_ctrl_stat_mode(xdev, ctrl, stat, mode);
}

static void xxxx_stop(struct xxxx_device* xdev)
{
	// stop disabled due to hardwre bug!
	// const u8  ctrl = MW_CTRL_STOP        |
	//                  MW_CTRL_IRQ_ENABLE  |
	//                  MW_CTRL_FIRST       |
	//                  MW_CTRL_LAST        ;
	// xxxx_set_ctrl(xdev, ctrl);
}

static void xxxx_reset(struct xxxx_device* xdev)
{
	const u8  ctrl = MW_CTRL_RESET;
	xxxx_set_ctrl(xdev, ctrl);
	xxxx_set_ctrl(xdev,    0);
}

static int v4l2_debug = 0;
module_param(v4l2_debug, int, 0644);
MODULE_PARM_DESC(v4l2_debug, "debug level (0-2)");

#define V4L2_DBG0(xdev, fmt, arg...) \
	v4l2_dbg(0, v4l2_debug, &xdev->v4l2_dev, fmt, ## arg)

#define V4L2_DBG1(xdev, fmt, arg...) \
	v4l2_dbg(1, v4l2_debug, &xdev->v4l2_dev, fmt, ## arg)

#define V4L2_DBG2(xdev, fmt, arg...) \
	v4l2_dbg(2, v4l2_debug, &xdev->v4l2_dev, fmt, ## arg)

#define V4L2_DBG3(xdev, fmt, arg...) \
	v4l2_dbg(3, v4l2_debug, &xdev->v4l2_dev, fmt, ## arg)

static int xxxx_vb2_queue_setup(struct vb2_queue* vb2_queue,
                                unsigned int*     n_buffers,
                                unsigned int*     n_planes,
                                unsigned int      sizes[],
                                struct device*    alloc_devs[])
{
	struct xxxx_port*   port = vb2_get_drv_priv(vb2_queue);
	struct xxxx_device* xdev = port->xdev;
	int                 ret  = 0;
	V4L2_DBG0(xdev, "%s start", __func__);
        *n_planes = 1;
	sizes[0] = port->sizeimage;
	V4L2_DBG0(xdev, "%s done(return=%d)", __func__, ret);
	return ret;
}

static void xxxx_vb2_buffer_queue(struct vb2_buffer* vb2_buf)
{
	struct xxxx_port*       port = vb2_get_drv_priv(vb2_buf->vb2_queue);
	struct xxxx_device*     xdev = port->xdev;
	struct vb2_v4l2_buffer* vbuf = to_vb2_v4l2_buffer(vb2_buf);
	struct xxxx_buffer*     xbuf = container_of(vbuf, struct xxxx_buffer, v4l2_buf);
	unsigned long           flags;
	V4L2_DBG0(xdev, "%s start", __func__);
	V4L2_DBG1(xdev, "owned_by_drv_count=%d", atomic_read(&vb2_buf->vb2_queue->owned_by_drv_count));
	spin_lock_irqsave(&xdev->slock, flags);
	list_add_tail(&xbuf->list, &port->xbuf_list);
	spin_unlock_irqrestore(&xdev->slock, flags);
	V4L2_DBG0(xdev, "%s done", __func__);
}

static int xxxx_vb2_start_streaming(struct vb2_queue* vb2_queue, unsigned int count)
{
	struct xxxx_port*   port = vb2_get_drv_priv(vb2_queue);
	struct xxxx_device* xdev = port->xdev;
	struct xxxx_buffer* xbuf;
	unsigned long       flags;
	int                 ret  = 0;

	V4L2_DBG0(xdev, "%s(count=%d) start", __func__, count);

	port->sequence = 0;
        
	spin_lock_irqsave(&xdev->slock, flags);
	xbuf = list_first_entry(&port->xbuf_list, struct xxxx_buffer, list);
	xbuf->allow_dq  = true;
	list_del(&xbuf->list);
	list_add_tail(&xbuf->list, &xdev->xbuf_list);
	port->streaming = true;
	port->stop      = false;
	spin_unlock_irqrestore(&xdev->slock, flags);
	xxxx_reset(xdev);
	xxxx_start(xdev, xbuf);

	vb2_queue->streaming = 1;

	V4L2_DBG0(xdev, "%s(count=%d) done(return=%d)", __func__, count, ret);
	return ret;
}

static void xxxx_vb2_stop_streaming(struct vb2_queue* vb2_queue)
{
	struct xxxx_port*   port = vb2_get_drv_priv(vb2_queue);
	struct xxxx_device* xdev = port->xdev;
	struct xxxx_buffer* xbuf;
	struct xxxx_buffer* next_xbuf;
	struct vb2_buffer*  vb2_buf;
	unsigned long       flags;
	bool                streaming;

	V4L2_DBG0(xdev, "%s start", __func__);
	V4L2_DBG1(xdev, "owned_by_drv_count=%d", atomic_read(&vb2_queue->owned_by_drv_count));

	xxxx_stop(xdev);
	while(true) {
		spin_lock_irqsave(&xdev->slock, flags);
		port->stop = true;
		streaming  = port->streaming;
		spin_unlock_irqrestore(&xdev->slock, flags);
		if (streaming == false)
			break;
		mdelay(1000);
	}
	
	spin_lock(&xdev->slock);
	list_for_each_entry_safe(xbuf, next_xbuf, &xdev->xbuf_list, list) {
		vb2_buf = &xbuf->v4l2_buf.vb2_buf;
		vb2_buffer_done(vb2_buf, VB2_BUF_STATE_ERROR);
		list_del(&xbuf->list);
	}
	list_for_each_entry_safe(xbuf, next_xbuf, &port->xbuf_list, list) {
		vb2_buf = &xbuf->v4l2_buf.vb2_buf;
		vb2_buffer_done(vb2_buf, VB2_BUF_STATE_ERROR);
		list_del(&xbuf->list);
	}
	spin_unlock(&xdev->slock);
	vb2_queue->streaming = 0;
	V4L2_DBG1(xdev, "owned_by_drv_count=%d", atomic_read(&vb2_queue->owned_by_drv_count));
	V4L2_DBG0(xdev, "%s done", __func__);
}

static struct vb2_ops xxxx_vb2_ops = {
	.queue_setup     = xxxx_vb2_queue_setup,
	.buf_queue       = xxxx_vb2_buffer_queue,
	.start_streaming = xxxx_vb2_start_streaming,
	.stop_streaming  = xxxx_vb2_stop_streaming,
	.wait_prepare    = vb2_ops_wait_prepare,
	.wait_finish     = vb2_ops_wait_finish,
};

static int xxxx_v4l2_file_open(struct file* file)
{
	struct xxxx_port*   port = video_drvdata(file);
	struct xxxx_device* xdev = port->xdev;
	int                 ret  = 0;

	V4L2_DBG0(xdev, "%s start", __func__);

	if (!xdev->setup_done) {
		xdev->setup_done = 1;
	}
	port->width         = MAX_WIDTH;
	port->height        = MAX_HEIGHT;
        port->bytesperpixel = BYTES_PER_PIXEL;
	port->bytesperline  = port->width  * port->bytesperpixel;
	port->sizeimage     = port->height * port->bytesperline ;
	v4l2_fh_init(&port->v4l2_fh, video_devdata(file));
	file->private_data = &port->v4l2_fh;
	v4l2_fh_add(&port->v4l2_fh);
	port->open         = 1;
	V4L2_DBG0(xdev, "%s done(return=%d)", __func__, ret);
	return ret;
}

static int xxxx_v4l2_file_release(struct file* file)
{
	struct xxxx_port*   port      = video_drvdata(file);
	struct xxxx_device* xdev      = port->xdev;
	struct vb2_queue*   vb2_queue = &port->vb2_queue;
	struct v4l2_fh*     v4l2_fh   = (struct v4l2_fh*)(file->private_data);
	int                 ret       = 0;

	V4L2_DBG0(xdev, "%s start", __func__);
	xxxx_vb2_stop_streaming(vb2_queue);
	if (v4l2_fh) {
		v4l2_fh_del(v4l2_fh);
		v4l2_fh_exit(v4l2_fh);
	}
	vb2_queue_release(vb2_queue);
	port->open = 0;
	V4L2_DBG0(xdev, "%s done(return=%d)", __func__, ret);
	return ret;
}
static const struct v4l2_file_operations xxxx_v4l2_file_ops = {
	.owner          = THIS_MODULE,
	.open           = xxxx_v4l2_file_open,
	.release        = xxxx_v4l2_file_release,
	.unlocked_ioctl = video_ioctl2,
	.mmap           = vb2_fop_mmap,
	.poll           = vb2_fop_poll,
};

static struct xxxx_port* file2port(struct file* file)
{
	return container_of(file->private_data, struct xxxx_port, v4l2_fh);
}

static int xxxx_v4l2_ioctl_querycap(struct file*            file,
			            void*                   priv,
			            struct v4l2_capability* cap)
{
	struct xxxx_port*   port = file2port(file);
	struct xxxx_device* xdev = port->xdev;
	int                 ret  = 0;
	V4L2_DBG0(xdev, "%s start", __func__);
	strncpy(cap->driver  , CAPTURE_DRV_NAME, sizeof(cap->driver)-1);
	strncpy(cap->card    , PVI_MODULE_NAME , sizeof(cap->card  )-1);
	strlcpy(cap->bus_info, PVI_MODULE_NAME , sizeof(cap->bus_info));
	cap->device_caps  = V4L2_CAP_STREAMING | V4L2_CAP_VIDEO_CAPTURE;
	cap->capabilities = cap->device_caps   | V4L2_CAP_DEVICE_CAPS;
	V4L2_DBG0(xdev, "%s done(return=%d)", __func__, ret);
	return ret;
}

static int xxxx_v4l2_ioctl_enum_fmt_vid_cap(struct file*         file,
				            void*                priv,
				            struct v4l2_fmtdesc* fmt)
{
	struct xxxx_port*   port = file2port(file);
	struct xxxx_device* xdev = port->xdev;
	int                 ret  = 0;
	V4L2_DBG0(xdev, "%s start", __func__);
	fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	strcpy(fmt->description, "RGB-8-8-8-8");
	fmt->pixelformat = V4L2_PIX_FMT_RGB32;
	V4L2_DBG0(xdev, "%s done(return=%d)", __func__, ret);
	return ret;
}

static int xxxx_v4l2_ioctl_g_fmt_vid_cap(struct file*        file,
				         void*               priv,
				         struct v4l2_format* fmt)
{
	struct xxxx_port*   port = file2port(file);
	struct xxxx_device* xdev = port->xdev;
	int                 ret  = 0;
	V4L2_DBG0(xdev, "%s start", __func__);
	fmt->fmt.pix.width        = port->width;
	fmt->fmt.pix.height       = port->height;
	fmt->fmt.pix.bytesperline = port->bytesperline;
	fmt->fmt.pix.sizeimage    = port->sizeimage;
	V4L2_DBG0(xdev, "%s done(return=%d)", __func__, ret);
	return ret;
}

static int xxxx_v4l2_ioctl_try_fmt_vid_cap(struct file*        file,
				           void*               fh,
				           struct v4l2_format* fmt)
{
	struct xxxx_port*   port = file2port(file);
	struct xxxx_device* xdev = port->xdev;
	int                 ret  = 0;
	V4L2_DBG0(xdev, "%s start", __func__);
	V4L2_DBG0(xdev, "%s done(return=%d)", __func__, ret);
	return ret;
}

static int xxxx_v4l2_ioctl_s_fmt_vid_cap(struct file*        file,
				         void*               priv,
				         struct v4l2_format* fmt)
{
	struct xxxx_port*   port = file2port(file);
	struct xxxx_device* xdev = port->xdev;
	int                 ret  = 0;
	V4L2_DBG0(xdev, "%s start", __func__);

	fmt->fmt.pix.bytesperline = fmt->fmt.pix.width;
	fmt->fmt.pix.sizeimage    = fmt->fmt.pix.bytesperline * fmt->fmt.pix.height;
	port->width        = fmt->fmt.pix.width;
	port->height       = fmt->fmt.pix.height;
	port->bytesperline = fmt->fmt.pix.bytesperline;
	port->sizeimage    = fmt->fmt.pix.sizeimage;
	V4L2_DBG0(xdev, "%s done(return=%d)", __func__, ret);
	return ret;
}

static long xxxx_v4l2_ioctl_default(struct file* file,
				    void*        fh,
				    bool         valid_prio,
				    unsigned int cmd,
				    void*        arg)
{
	switch(cmd) {
	default:
		return -ENOTTY;
	}
}

static const struct v4l2_ioctl_ops xxxx_v4l2_ioctl_ops = {
	.vidioc_querycap         = xxxx_v4l2_ioctl_querycap,
	.vidioc_enum_fmt_vid_cap = xxxx_v4l2_ioctl_enum_fmt_vid_cap,
	.vidioc_g_fmt_vid_cap    = xxxx_v4l2_ioctl_g_fmt_vid_cap,
	.vidioc_try_fmt_vid_cap  = xxxx_v4l2_ioctl_try_fmt_vid_cap,
	.vidioc_s_fmt_vid_cap    = xxxx_v4l2_ioctl_s_fmt_vid_cap,

	.vidioc_reqbufs          = vb2_ioctl_reqbufs,
	.vidioc_create_bufs      = vb2_ioctl_create_bufs,
	.vidioc_prepare_buf      = vb2_ioctl_prepare_buf,
	.vidioc_querybuf         = vb2_ioctl_querybuf,
	.vidioc_qbuf             = vb2_ioctl_qbuf,
	.vidioc_dqbuf            = vb2_ioctl_dqbuf,
	.vidioc_streamon         = vb2_ioctl_streamon,
	.vidioc_streamoff        = vb2_ioctl_streamoff,
	.vidioc_log_status       = v4l2_ctrl_log_status,
	.vidioc_default          = xxxx_v4l2_ioctl_default,
};

static int xxxx_port_alloc(struct xxxx_device* xdev)
{
	struct xxxx_port*    port;
	int                  ret;

	port = kzalloc(sizeof(*port), GFP_KERNEL);
	if (IS_ERR_OR_NULL(port)) {
		ret = (port == NULL) ? -ENOMEM : PTR_ERR(port);
		port = NULL;
		goto failed;
	}
	{
		struct vb2_queue* vb2_queue = &port->vb2_queue;
		vb2_queue->type             = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		vb2_queue->io_modes         = VB2_MMAP | VB2_DMABUF | VB2_USERPTR;
		vb2_queue->drv_priv         = port;
		vb2_queue->buf_struct_size  = sizeof(struct xxxx_buffer);
		vb2_queue->ops              = &xxxx_vb2_ops;
		vb2_queue->mem_ops          = &vb2_dma_contig_memops;
		vb2_queue->timestamp_flags  = V4L2_BUF_FLAG_TIMESTAMP_COPY;
		vb2_queue->lock             = &xdev->mutex;
		vb2_queue->dev              = &(xdev->pdev->dev);
		ret = vb2_queue_init(vb2_queue);
		if (ret) {
			goto failed;
		}
	}
	{
		struct video_device* vdev = video_device_alloc();
		if (!vdev) {
			ret = -ENOMEM;
			goto failed;
		}
		vdev->device_caps = V4L2_CAP_STREAMING | V4L2_CAP_VIDEO_CAPTURE;
		vdev->v4l2_dev    = &xdev->v4l2_dev;
		vdev->queue       = &port->vb2_queue;
		vdev->fops        = &xxxx_v4l2_file_ops;
		vdev->ioctl_ops   = &xxxx_v4l2_ioctl_ops;
		vdev->minor       = -1;
		vdev->release     = video_device_release;
		vdev->lock        = &xdev->mutex;
		snprintf(vdev->name, sizeof(vdev->name), "%s", PVI_MODULE_NAME);
		video_set_drvdata(vdev, port);

		ret = video_register_device(vdev, VFL_TYPE_VIDEO, -1);
		if (ret) {
			v4l2_err(&xdev->v4l2_dev, "Failed to register video device");
			goto failed;
		}
		port->vdev = vdev;
	}
	port->xdev = xdev;
	port->open = 0;
	INIT_LIST_HEAD(&port->xbuf_list);
	xdev->port = port;

	v4l2_info(&xdev->v4l2_dev, "Device registerd as /dev/video%d\n", port->vdev->num);
	return 0;

    failed:
	if (port != NULL) {
		if (port->vdev != NULL)
                	video_unregister_device(port->vdev);
		kfree(port);
        }
	return ret;
}

static void xxxx_port_free(struct xxxx_port* port)
{
	if (!port)
		return;
	v4l2_info(&(port->xdev->v4l2_dev), PVI_MODULE_NAME "Device /dev/video%d is removed\n", port->vdev->num);
	video_unregister_device(port->vdev);
	kfree(port);
}

static void xxxx_active_buffer_next(struct xxxx_port* port)
{
	struct xxxx_device* xdev = port->xdev;
	struct xxxx_buffer* xbuf;
	unsigned long       flags;

	spin_lock_irqsave(&xdev->slock, flags);
	if ((port->streaming == true) && !list_empty(&port->xbuf_list)) {
		xbuf =  list_first_entry(&port->xbuf_list, struct xxxx_buffer, list);
		if (list_is_last(&xbuf->list, &port->xbuf_list)) {
			xbuf->allow_dq = false;
		} else {
			xbuf->allow_dq = true;
		}
		list_del(&xbuf->list);
		list_add_tail(&xbuf->list, &xdev->xbuf_list);
		xxxx_start(xdev, xbuf);
	}
	spin_unlock_irqrestore(&xdev->slock, flags);
}

static void xxxx_process_buffer_complete(struct xxxx_port* port)
{
	struct xxxx_device* xdev = port->xdev;
	struct xxxx_buffer* xbuf;

	if (list_empty(&xdev->xbuf_list))
		return;
	xbuf = list_first_entry(&xdev->xbuf_list, struct xxxx_buffer, list);
	if (xbuf) {
		struct vb2_buffer* vb2_buf = &xbuf->v4l2_buf.vb2_buf;
	        unsigned long      flags;
		spin_lock_irqsave(&xdev->slock, flags);
		list_del(&xbuf->list);
		if (xbuf->allow_dq) {
			vb2_buffer_done(vb2_buf, VB2_BUF_STATE_DONE);
			xbuf->allow_dq = false;
		} else if (port->stop == true) {
			vb2_buffer_done(vb2_buf, VB2_BUF_STATE_DONE);
			port->streaming = 0;
                } else {
			list_add_tail(&xbuf->list, &port->xbuf_list);
		}
		spin_unlock_irqrestore(&xdev->slock, flags);
	} else {
		printk("%s:%s", __func__, "BUG().");
		BUG();
	}
	port->sequence++;
}

static irqreturn_t xxxx_irq(int irg, void* data)
{
	struct xxxx_device* xdev = (struct xxxx_device*)data;
	struct xxxx_port*   port = xdev->port;
        xxxx_clear_status(xdev);
	xxxx_process_buffer_complete(port);
	xxxx_active_buffer_next(port);
	return IRQ_HANDLED;
}

static int xxxx_platform_device_probe(struct platform_device* pdev)
{
	struct xxxx_device* xdev;
	struct resource*    regs_resource;
	struct resource*    regs_region;
	void __iomem*       regs_addr;
	int                 irq;
	int                 ret = 0;
        bool                v4l2_device_is_registerd = false;

	xdev = kzalloc(sizeof(*xdev), GFP_KERNEL);
	if (IS_ERR_OR_NULL(xdev)) {
		ret = (xdev == NULL) ? -ENOMEM : PTR_ERR(xdev);
		xdev = NULL;
		goto failed;
	}

	regs_resource = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (IS_ERR_OR_NULL(regs_resource)) {
		dev_err(&pdev->dev, "platform_get_resource() failed. return=%ld\n", PTR_ERR(regs_resource));
		ret = (regs_resource == NULL) ? -ENOMEM : PTR_ERR(regs_resource);
		goto failed;
	}
        regs_region = devm_request_mem_region(&pdev->dev, regs_resource->start, resource_size(regs_resource), pdev->name);
	if (IS_ERR_OR_NULL(regs_region)) {
		dev_err(&pdev->dev, "devm_request_mem_region() failed. return=%ld\n", PTR_ERR(regs_region));
		ret = (regs_region == NULL) ? -ENOMEM : PTR_ERR(regs_region);
		goto failed;
	}
	regs_addr = devm_ioremap(&pdev->dev, regs_region->start, resource_size(regs_region));
	if (IS_ERR_OR_NULL(regs_addr)) {
		dev_err(&pdev->dev, "devm_ioremqp() failed. return=%ld\n", PTR_ERR(regs_addr));
		ret = (regs_addr == NULL) ? -ENOMEM : PTR_ERR(regs_addr);
		goto failed;
	}
		
        irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		dev_err(&pdev->dev, "platform_get_irq() failed. return=%d\n", irq);
		ret = irq;
		goto failed;
	} 
	ret = devm_request_irq(&pdev->dev, irq, xxxx_irq, IRQF_SHARED, PVI_MODULE_NAME, xdev);
	if (ret) {
		dev_err(&pdev->dev, "devm_request_irq() failed. return=%d\n", ret);
		goto failed;
	}

	xdev->regs_addr  = regs_addr;
	xdev->irq        = irq;

	spin_lock_init(&xdev->slock);
	INIT_LIST_HEAD(&xdev->xbuf_list);

	ret = v4l2_device_register(&pdev->dev, &xdev->v4l2_dev);
	if (ret) {
		dev_err(&pdev->dev, "v4l2_device_register() failed. return=%d\n", ret);
		goto failed;
	}
	v4l2_device_is_registerd = true;

	mutex_init(&xdev->mutex);
	xdev->pdev       = pdev;
	xdev->setup_done = 0;

	ret = xxxx_port_alloc(xdev);
	if (ret) {
		dev_err(&pdev->dev, "xxxx_port_alloc() failed. return=%d\n", ret);
		goto failed;
	}

	platform_set_drvdata(pdev, xdev);
	return 0;

    failed:
	if (xdev != NULL) {
		if (xdev->port != NULL)
			xxxx_port_free(xdev->port);
		if (v4l2_device_is_registerd == true)
			v4l2_device_unregister(&xdev->v4l2_dev);
		platform_set_drvdata(pdev, NULL);
		kfree(xdev);
	}
	return ret;
}

static int xxxx_platform_device_remove(struct platform_device* pdev)
{
	struct xxxx_device* xdev = platform_get_drvdata(pdev);
	if (xdev != NULL) {
		xxxx_port_free(xdev->port);
		v4l2_device_unregister(&xdev->v4l2_dev);
		platform_set_drvdata(pdev, NULL);
		kfree(xdev);
	}
	return 0;
}

#if defined CONFIG_OF
static const struct of_device_id xxxx_of_match[] = {
	{
		.compatible = "ikwzm,vb2-test", .data = (void*)1,
	},
	{},
};
#else
#define xxxx_of_match NULL
#endif

static struct platform_driver xxxx_platform_driver = {
	.probe  = xxxx_platform_device_probe ,
	.remove = xxxx_platform_device_remove,
	.driver = {
		.name  = CAPTURE_DRV_NAME,
		.owner = THIS_MODULE,
		.of_match_table = xxxx_of_match,
	},
};

static int  xxxx_module_init(void)
{
	return platform_driver_register(&xxxx_platform_driver);
}

static void xxxx_module_exit(void)
{
	platform_driver_unregister(&xxxx_platform_driver);
}
module_init(xxxx_module_init);
module_exit(xxxx_module_exit);
