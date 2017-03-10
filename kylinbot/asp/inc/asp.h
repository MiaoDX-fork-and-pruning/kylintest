/**
 * Copyright (c) 2011-2016, Jack Mo (mobangjack@foxmail.com).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#ifndef __ASP_H__
#define __ASP_H__

/**
 * Stand-Alone Application Support Packages (Platform Independent)
 *
 */
 
#ifdef __cplusplus
extern "C" {
#endif

#include "ahrs.h"  // Attitude Heading Reference System
#include "cbus.h"  // Control Bus
#include "crc16.h" // 16bit CRC
#include "dbus.h"  // DBUS Protocol (DBUS = RCP + HCP)
#include "ekf.h"   // Kalman Filter
#include "fifo.h"  // FIFO Data Structure and Algorithm
#include "fun.h"   // Helper Functions
#include "gdf.h"   // Gaussian Distribution Function
#include "hcp.h"   // Host Control Protocol
#include "maf.h"   // Moving Average Filter
#include "mec.h"   // Mecanum Power Transmission System
#include "pid.h"   // PID Regulator
#include "rcp.h"   // Remote Control Protocol
#include "rmp.h"   // Ramp Generator
#include "stack.h" // Stack Data Structure and Algorithm

#ifdef __cplusplus
}
#endif

#endif

