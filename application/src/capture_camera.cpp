/*

   (c) 2014 Séverin Lemaignan <severin.lemaignan@epfl.ch>
   (c) 2008 Hans de Goede <hdegoede@redhat.com> for yuyv_to_rgb24

 This program is free software; you can redistribute it and/or modify it
 under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2.1 of the License, or (at
 your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335  USA

 */

#include <stdlib.h>
#include <assert.h>
#include <fcntl.h> /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <string.h> // strerrno
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <stdexcept>
#include <jpeglib.h>
#include <vector>
#include <iostream>
#include <fstream>

#include <linux/videodev2.h>

#include "capture_camera.h"

#define CLEAR(x) memset(&(x), 0, sizeof(x))

using namespace std;

static int xioctl(int fh, unsigned long int request, void *arg)
{
    int r;

    do
    {
        r = ioctl(fh, request, arg);
    } while (-1 == r && EINTR == errno);

    return r;
}

/*****
 * Taken from libv4l2 (in v4l-utils)
 *
 * (C) 2008 Hans de Goede <hdegoede@redhat.com>
 *
 * Released under LGPL
 */
#define CLIP(color) (unsigned char)(((color) > 0xFF) ? 0xff : (((color) < 0) ? 0 : (color)))

static void v4lconvert_yuyv_to_rgb24(const unsigned char *src,
                                     unsigned char *dest,
                                     int width, int height,
                                     int stride)
{
    int j;

    while (--height >= 0)
    {
        for (j = 0; j + 1 < width; j += 2)
        {
            int u = src[1];
            int v = src[3];
            int u1 = (((u - 128) << 7) + (u - 128)) >> 6;
            int rg = (((u - 128) << 1) + (u - 128) +
                      ((v - 128) << 2) + ((v - 128) << 1)) >>
                     3;
            int v1 = (((v - 128) << 1) + (v - 128)) >> 1;

            *dest++ = CLIP(src[0] + v1);
            *dest++ = CLIP(src[0] - rg);
            *dest++ = CLIP(src[0] + u1);

            *dest++ = CLIP(src[2] + v1);
            *dest++ = CLIP(src[2] - rg);
            *dest++ = CLIP(src[2] + u1);
            src += 4;
        }
        src += stride - (width * 2);
    }
}

/**
 * @brief Convert raw data to JPEG image
*/
uint64_t YUYVtoJPEG(const uint8_t *input, const int width, const int height, uint8_t *&outbuffer)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_ptr[1];
    int row_stride;

    // uint8_t* outbuffer = NULL;
    long unsigned int outlen = 0;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_mem_dest(&cinfo, &outbuffer, &outlen);

    // jrow is a libjpeg row of samples array of 1 row pointer
    cinfo.image_width = width & -1;
    cinfo.image_height = height & -1;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_YCbCr; // libJPEG expects YUV 3bytes, 24bit, YUYV --> YUV YUV YUV

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 92, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    std::vector<uint8_t> tmprowbuf(width * 3);
    JSAMPROW row_pointer[1];
    row_pointer[0] = &tmprowbuf[0];

    while (cinfo.next_scanline < cinfo.image_height)
    {
        unsigned i, j;
        unsigned offset = cinfo.next_scanline * cinfo.image_width * 2; // offset to the correct row
        for (i = 0, j = 0; i < cinfo.image_width * 2; i += 4, j += 6)
        {                                             // input strides by 4 bytes, output strides by 6 (2 pixels)
            tmprowbuf[j + 0] = input[offset + i + 0]; // Y (unique to this pixel)
            tmprowbuf[j + 1] = input[offset + i + 1]; // U (shared between pixels)
            tmprowbuf[j + 2] = input[offset + i + 3]; // V (shared between pixels)
            tmprowbuf[j + 3] = input[offset + i + 2]; // Y (unique to this pixel)
            tmprowbuf[j + 4] = input[offset + i + 1]; // U (shared between pixels)
            tmprowbuf[j + 5] = input[offset + i + 3]; // V (shared between pixels)
        }

        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    return outlen;
}
/*******************************************************************/

Webcam::Webcam(const string &device, int width, int height) : device(device),
                                                              xres(width),
                                                              yres(height)
{
    open_device();
    init_device();
    // xres and yres are set to the actual resolution provided by the cam

    // frame stored as RGB888 (ie, RGB24)
    rgb_frame.width = xres;
    rgb_frame.height = yres;
    rgb_frame.size = xres * yres * 3;
    rgb_frame.data = (unsigned char *)malloc(rgb_frame.size * sizeof(char));

    // start_capturing();
}

Webcam::~Webcam()
{
    stop_capturing();
    uninit_device();
    close_device();

    free(rgb_frame.data);
}

const RGBImage &Webcam::frame(int timeout)
{
    for (;;)
    {
        fd_set fds;
        struct timeval tv;
        int r;

        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        /* Timeout. */
        tv.tv_sec = timeout;
        tv.tv_usec = 0;

        r = select(fd + 1, &fds, NULL, NULL, &tv);

        if (-1 == r)
        {
            if (EINTR == errno)
                continue;
            throw runtime_error("select");
        }

        if (0 == r)
        {
            throw runtime_error(device + ": select timeout");
        }
        rgb_frame.size = read_frame();

        if (rgb_frame.size != 0)
        {
            return rgb_frame;
        }
        /* EAGAIN - continue select loop. */
    }
}

void Webcam::capture(std::string pathfile)
{
    start_capturing();
    struct RGBImage framedata = frame();
    stop_capturing();
    ofstream image;
    image.open(pathfile);
    image.write((char *) framedata.data, framedata.size);
    image.close();
}

uint64_t Webcam::read_frame()
{
    struct v4l2_buffer buf;

    uint64_t imgSize = 0;

    CLEAR(buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf))
    {
        switch (errno)
        {
        case EAGAIN:
            return false;

        case EIO:
            /* Could ignore EIO, see spec. */

            /* fall through */

        default:
            throw runtime_error("VIDIOC_DQBUF");
        }
    }

    assert(buf.index < n_buffers);

    // v4lconvert_yuyv_to_rgb24((unsigned char *)buffers[buf.index].data,
    //                          rgb_frame.data,
    //                          xres,
    //                          yres,
    //                          stride);

    imgSize = YUYVtoJPEG((uint8_t *)buffers[buf.index].data, xres, yres, rgb_frame.data);

    if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
        throw runtime_error("VIDIOC_QBUF");

    // uint8_t* outbuffer = NULL;
    // uint64_t outlen = YUYVtoJPEG((uint8_t*)buffers[buf.index].data, xres, yres, outbuffer);

    // // Write JPEG to file
    // std::vector<uint8_t> output = std::vector<uint8_t>(outbuffer, outbuffer + outlen);
    // std::ofstream ofs("output.jpg", std::ios_base::binary);
    // ofs.write((const char*) &output[0], output.size());
    // ofs.close();

    return imgSize;
}

void Webcam::open_device(void)
{
    struct stat st;

    if (-1 == stat(device.c_str(), &st))
    {
        throw runtime_error(device + ": cannot identify! " + to_string(errno) + ": " + strerror(errno));
    }

    if (!S_ISCHR(st.st_mode))
    {
        throw runtime_error(device + " is no device");
    }

    fd = open(device.c_str(), O_RDWR /* required */ | O_NONBLOCK, 0);

    if (-1 == fd)
    {
        throw runtime_error(device + ": cannot open! " + to_string(errno) + ": " + strerror(errno));
    }
}

void Webcam::init_mmap(void)
{
    struct v4l2_requestbuffers req;

    CLEAR(req);

    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req))
    {
        if (EINVAL == errno)
        {
            throw runtime_error(device + " does not support memory mapping");
        }
        else
        {
            throw runtime_error("VIDIOC_REQBUFS");
        }
    }

    if (req.count < 2)
    {
        throw runtime_error(string("Insufficient buffer memory on ") + device);
    }

    buffers = (buffer *)calloc(req.count, sizeof(*buffers));

    if (!buffers)
    {
        throw runtime_error("Out of memory");
    }

    for (n_buffers = 0; n_buffers < req.count; ++n_buffers)
    {
        struct v4l2_buffer buf;

        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;

        if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
            throw runtime_error("VIDIOC_QUERYBUF");

        buffers[n_buffers].size = buf.length;
        buffers[n_buffers].data =
            mmap(NULL /* start anywhere */,
                 buf.length,
                 PROT_READ | PROT_WRITE /* required */,
                 MAP_SHARED /* recommended */,
                 fd, buf.m.offset);

        if (MAP_FAILED == buffers[n_buffers].data)
            throw runtime_error("mmap");
    }
}

void Webcam::close_device(void)
{
    if (-1 == close(fd))
        throw runtime_error("close");

    fd = -1;
}

void Webcam::init_device(void)
{
    struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    struct v4l2_format fmt;
    unsigned int min;

    if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap))
    {
        if (EINVAL == errno)
        {
            throw runtime_error(device + " is no V4L2 device");
        }
        else
        {
            throw runtime_error("VIDIOC_QUERYCAP");
        }
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
    {
        throw runtime_error(device + " is no video capture device");
    }

    if (!(cap.capabilities & V4L2_CAP_STREAMING))
    {
        throw runtime_error(device + " does not support streaming i/o");
    }

    /* Select video input, video standard and tune here. */

    CLEAR(cropcap);

    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (0 == xioctl(fd, VIDIOC_CROPCAP, &cropcap))
    {
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c = cropcap.defrect; /* reset to default */

        if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop))
        {
            switch (errno)
            {
            case EINVAL:
                /* Cropping not supported. */
                break;
            default:
                /* Errors ignored. */
                break;
            }
        }
    }
    else
    {
        /* Errors ignored. */
    }

    CLEAR(fmt);

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (force_format)
    {
        fmt.fmt.pix.width = xres;
        fmt.fmt.pix.height = yres;
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
        fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

        if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
            throw runtime_error("VIDIOC_S_FMT");

        if (fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV)
            // note that libv4l2 (look for 'v4l-utils') provides helpers
            // to manage conversions
            throw runtime_error("Webcam does not support YUYV format. Support for more format need to be added!");

        /* Note VIDIOC_S_FMT may change width and height. */
        xres = fmt.fmt.pix.width;
        yres = fmt.fmt.pix.height;

        stride = fmt.fmt.pix.bytesperline;
    }
    else
    {
        /* Preserve original settings as set by v4l2-ctl for example */
        if (-1 == xioctl(fd, VIDIOC_G_FMT, &fmt))
            throw runtime_error("VIDIOC_G_FMT");
    }

    init_mmap();
}

void Webcam::uninit_device(void)
{
    unsigned int i;

    for (i = 0; i < n_buffers; ++i)
        if (-1 == munmap(buffers[i].data, buffers[i].size))
            throw runtime_error("munmap");

    free(buffers);
}

void Webcam::start_capturing(void)
{
    unsigned int i;
    enum v4l2_buf_type type;

    for (i = 0; i < n_buffers; ++i)
    {
        struct v4l2_buffer buf;

        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
            throw runtime_error("VIDIOC_QBUF");
    }
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
        throw runtime_error("VIDIOC_STREAMON");
}

void Webcam::stop_capturing(void)
{
    enum v4l2_buf_type type;

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
        throw runtime_error("VIDIOC_STREAMOFF");
}
