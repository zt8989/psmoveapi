/**
 * PS Move API - An interface for the PS Move Motion Controller
 * Copyright (c) 2024
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 **/

#include "camera_control.h"
#include "camera_control_driver.h"

#include "../psmove_private.h"

#if defined(PSMOVE_MULTI_CAMERA_DRIVERS)
#define CAMERA_CONTROL_DRIVER_NEW camera_control_driver_opencv_new
#define CAMERA_CONTROL_DRIVER_GET_PREFERRED camera_control_driver_opencv_get_preferred_camera
#define CAMERA_CONTROL_DRIVER_COUNT_CONNECTED camera_control_driver_opencv_count_connected
#else
#define CAMERA_CONTROL_DRIVER_NEW camera_control_driver_new
#define CAMERA_CONTROL_DRIVER_GET_PREFERRED camera_control_driver_get_preferred_camera
#define CAMERA_CONTROL_DRIVER_COUNT_CONNECTED camera_control_driver_count_connected
#endif

#ifdef _WIN32
static const int kOpenCVCaptureApi = cv::CAP_DSHOW;
#else
static const int kOpenCVCaptureApi = cv::CAP_ANY;
#endif

struct CameraControlOpenCVGeneric : public CameraControlOpenCV {
    CameraControlOpenCVGeneric(int camera_id, int width, int height, int framerate);
    virtual ~CameraControlOpenCVGeneric();

    virtual void set_parameters(float exposure, bool mirror) override;
    virtual PSMoveCameraInfo get_camera_info() override;
};

CameraControlOpenCVGeneric::CameraControlOpenCVGeneric(int camera_id, int width, int height, int framerate)
    : CameraControlOpenCV(camera_id, width, height, framerate)
{
    capture = new cv::VideoCapture(cameraID, kOpenCVCaptureApi);

    layout = get_frame_layout(width, height);

    capture->set(cv::CAP_PROP_FRAME_WIDTH, layout.capture_width);
    capture->set(cv::CAP_PROP_FRAME_HEIGHT, layout.capture_height);
    capture->set(cv::CAP_PROP_FPS, framerate);
}

CameraControlOpenCVGeneric::~CameraControlOpenCVGeneric()
{
}

void
CameraControlOpenCVGeneric::set_parameters(float exposure, bool mirror)
{
}

PSMoveCameraInfo
CameraControlOpenCVGeneric::get_camera_info()
{
    return PSMoveCameraInfo {
        "OpenCV Camera",
        "OpenCV",
        layout.crop_width,
        layout.crop_height,
    };
}

extern "C" CameraControl *
CAMERA_CONTROL_DRIVER_NEW(int camera_id, int width, int height, int framerate)
{
    CameraControlOpenCVGeneric *cc = new CameraControlOpenCVGeneric(camera_id, width, height, framerate);
    if (!cc->capture || !cc->capture->isOpened()) {
        delete cc;
        return nullptr;
    }

    return cc;
}

static bool
probe_camera(int camera_id)
{
    cv::VideoCapture capture(camera_id, kOpenCVCaptureApi);
    bool opened = capture.isOpened();
    capture.release();
    return opened;
}

extern "C" int
CAMERA_CONTROL_DRIVER_GET_PREFERRED()
{
    return probe_camera(0) ? 0 : -1;
}

extern "C" int
CAMERA_CONTROL_DRIVER_COUNT_CONNECTED()
{
    int count = 0;
    const int max_probe = 8;

    for (int i = 0; i < max_probe; ++i) {
        if (probe_camera(i)) {
            ++count;
        }
    }

    return count;
}
