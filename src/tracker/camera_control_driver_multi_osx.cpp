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

#include "camera_control_driver.h"

extern "C" {
CameraControl *camera_control_driver_ps3eye_new(int camera_id, int width, int height, int framerate);
int camera_control_driver_ps3eye_get_preferred_camera();
int camera_control_driver_ps3eye_count_connected();

CameraControl *camera_control_driver_avfoundation_new(int camera_id, int width, int height, int framerate);
int camera_control_driver_avfoundation_get_preferred_camera();
int camera_control_driver_avfoundation_count_connected();
}

extern "C" CameraControl *
camera_control_driver_new(int camera_id, int width, int height, int framerate)
{
    if (camera_control_driver_ps3eye_count_connected() > 0) {
        CameraControl *cc = camera_control_driver_ps3eye_new(camera_id, width, height, framerate);
        if (cc != nullptr) {
            return cc;
        }
    }

    return camera_control_driver_avfoundation_new(camera_id, width, height, framerate);
}

extern "C" int
camera_control_driver_get_preferred_camera()
{
    if (camera_control_driver_ps3eye_count_connected() > 0) {
        return camera_control_driver_ps3eye_get_preferred_camera();
    }

    return camera_control_driver_avfoundation_get_preferred_camera();
}

extern "C" int
camera_control_driver_count_connected()
{
    int ps3eye_count = camera_control_driver_ps3eye_count_connected();
    if (ps3eye_count > 0) {
        return ps3eye_count;
    }

    return camera_control_driver_avfoundation_count_connected();
}
