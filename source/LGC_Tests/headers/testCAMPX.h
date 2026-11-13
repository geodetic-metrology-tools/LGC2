/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _LGC_TESTS_CAMPX_H
#define _LGC_TESTS_CAMPX_H

#include <string>
#include <utility>

/*Tests of the pixel camera: instrument *CAMPX with its interior orientation (*IO) and pixel coordinate measurements (*UV).
The network of the estimation tests is generated in testCAMPX.cpp.*/
namespace TestCAMPX
{
/*A pixel camera with two targets, an interior orientation with FREE and SE, and the options of a UV measurement*/
char const *const CAMPX_READER = R"(*TITR
Pixel camera reader test
*OLOC
*INSTR
*CAMPX CAMA TG1 SENSOR 4096 3000
TG1 0.02 0.03 % sigma u, sigma v [px]
TG2 0.05 0.06
*IO
C 7000 FREE
CX 2048.5
CY 1501 FREE SE 2.5
K1 -0.1 FREE % radial distortion
K2 0.05 SE 0.01
*CALA
C1 0 0 0
P1 0.1 0.2 4
P2 -0.1 0.3 5
*CAM C1 CAMA
*UV
P1 2223.0 1850.0
P2 1908.0 1920.0 TRGT TG2 USE 0.5 VSE 0.6 ID OBS7
*END
)";

/*Parts of the inputs of the reader errors*/
const std::string READER_HEADER = "*TITR\nPixel camera reader errors\n*OLOC\n*INSTR\n";
const std::string READER_CAMERA_IO = "*CAMPX CAMA TG\nTG 0.02 0.02\n*IO\n";
const std::string READER_PIXEL_CAMERA = READER_CAMERA_IO + "C 7000 FREE\n";
const std::string READER_SENSOR_CAMERA = "*CAMPX CAMA TG SENSOR 4000 3000\nTG 0.02 0.02\n*IO\nC 7000\n";
const std::string READER_VECTOR_CAMERA = "*CAMD CAMB TG 0.0\nTG 5 5 0.5 0.5\n";
const std::string READER_POINTS = "*CALA\nC1 0 0 0\nP1 0.1 0.2 4\n";

/*Wrong pixel camera definitions, reading each of them has to fail*/
const std::pair<const char *, std::string> CAMPX_WRONG_INPUTS[] = {
	{"Unknown interior orientation parameter", READER_HEADER + READER_CAMERA_IO + "F 7000 FREE\n" + READER_POINTS + "*END\n"},
	{"Interior orientation parameter without its value", READER_HEADER + READER_CAMERA_IO + "C\n" + READER_POINTS + "*END\n"},
	{"Interior orientation parameter given twice", READER_HEADER + READER_PIXEL_CAMERA + "C 7100\n" + READER_POINTS + "*END\n"},
	{"Unknown option of an interior orientation parameter", READER_HEADER + READER_CAMERA_IO + "C 7000 VAR\n" + READER_POINTS + "*END\n"},
	{"A priori sigma without its value", READER_HEADER + READER_CAMERA_IO + "C 7000 SE\n" + READER_POINTS + "*END\n"},
	{"Negative a priori sigma", READER_HEADER + READER_CAMERA_IO + "C 7000 SE -1\n" + READER_POINTS + "*END\n"},
	{"*IO with an argument", READER_HEADER + "*CAMPX CAMA TG\nTG 0.02 0.02\n*IO CAMA\nC 7000\n" + READER_POINTS + "*END\n"},
	{"Interior orientation of a unit vector camera", READER_HEADER + READER_VECTOR_CAMERA + "*IO\nC 7000 FREE\n" + READER_POINTS + "*END\n"},
	{"Pixel camera without camera constant", READER_HEADER + READER_CAMERA_IO + "CX 2000\n" + READER_POINTS + "*CAM C1 CAMA\n*UV\nP1 2000 1500\n*END\n"},
	{"UV measured with a unit vector camera", READER_HEADER + READER_VECTOR_CAMERA + READER_POINTS + "*CAM C1 CAMB\n*UV\nP1 2000 1500\n*END\n"},
	{"UVEC measured with a pixel camera", READER_HEADER + READER_PIXEL_CAMERA + READER_POINTS + "*CAM C1 CAMA\n*UVEC\nP1 0 0 1\n*END\n"},
	{"UVD measured with a pixel camera", READER_HEADER + READER_PIXEL_CAMERA + READER_POINTS + "*CAM C1 CAMA\n*UVD\nP1 0 0 1 4\n*END\n"},
	{"UV measurement without values", READER_HEADER + READER_PIXEL_CAMERA + READER_POINTS + "*CAM C1 CAMA\n*UV\nP1\n*END\n"},
	{"Sensor without its height", READER_HEADER + "*CAMPX CAMA TG SENSOR 4000\nTG 0.02 0.02\n*IO\nC 7000\n" + READER_POINTS + "*END\n"},
	{"Unknown option of the pixel camera", READER_HEADER + "*CAMPX CAMA TG SIZE 4000 3000\nTG 0.02 0.02\n*IO\nC 7000\n" + READER_POINTS + "*END\n"},
	{"Sensor of zero width", READER_HEADER + "*CAMPX CAMA TG SENSOR 0 3000\nTG 0.02 0.02\n*IO\nC 7000\n" + READER_POINTS + "*END\n"},
	{"Instrument centering of a pixel camera station", READER_HEADER + READER_PIXEL_CAMERA + READER_POINTS + "*CAM C1 CAMA ICSE 0.1\n*UV\nP1 2000 1500\n*END\n"},
	{"Target centering of a UV measurement", READER_HEADER + READER_PIXEL_CAMERA + READER_POINTS + "*CAM C1 CAMA\n*UV\nP1 2000 1500 TCSE 0.1\n*END\n"}};

/*A correct pixel camera with a UV measurement, and a UV measurement on the corner of the sensor*/
const std::string CAMPX_CORRECT = READER_HEADER + READER_PIXEL_CAMERA + READER_POINTS + "*CAM C1 CAMA\n*UV\nP1 2000 1500\n*END\n";
const std::string CAMPX_SENSOR_CORNER = READER_HEADER + READER_SENSOR_CAMERA + READER_POINTS + "*CAM C1 CAMA\n*UV\nP1 4000 3000\n*END\n";

/*UV measurements right of and above the sensor, read with a warning*/
const std::string CAMPX_RIGHT_OF_SENSOR = READER_HEADER + READER_SENSOR_CAMERA + READER_POINTS + "*CAM C1 CAMA\n*UV\nP1 4000.5 1500\n*END\n";
const std::string CAMPX_ABOVE_SENSOR = READER_HEADER + READER_SENSOR_CAMERA + READER_POINTS + "*CAM C1 CAMA\n*UV\nP1 2000 -0.5\n*END\n";

/*The camera stands on a free point of ROOT and looks along the z axis of ROOT, the targets are in a free frame. Only the
derivatives are tested, the pixel coordinates need only be plausible.*/
char const *const CAMPX_CAMERA_IN_ROOT = R"(*TITR
Pixel camera in ROOT observing a free frame
*OLOC
*INSTR
*CAMPX CAMA TG
TG 0.02 0.02
*IO
C 5000 FREE
CX 1010 FREE
CY 990 FREE
K1 -0.08 FREE
K2 0.05
K3 0.02
P1 0.0004
P2 -0.0003
B1 1.5
B2 -0.7
*POIN
CAM1 0.1 -0.2 0.3
*FRAME OBJ 0.2 -0.1 4.0 5.0 -7.0 12.0 1 TX TY TZ RX RY RZ
*CALA
Q1 -0.6 -0.6 0.0
Q2 0.6 -0.6 0.3
Q3 0.6 0.6 0.0
Q4 -0.6 0.6 -0.3
Q5 0.0 -0.5 0.1
Q6 0.5 0.0 -0.1
Q7 0.0 0.5 0.2
Q8 -0.5 0.0 -0.2
*POIN
Q9 0.0 0.0 0.2
Q10 0.3 -0.2 -0.2
*ENDFRAME
*CAM CAM1 CAMA
*UV
Q1 280.0 310.0
Q2 1810.0 240.0
Q3 1650.0 1820.0
Q4 190.0 1640.0
Q5 1020.0 330.0
Q6 1690.0 1010.0
Q7 980.0 1650.0
Q8 330.0 990.0
Q9 1000.0 1020.0
Q10 1390.0 760.0
*END
)";

/*Two cameras in ROOT looking along +z, each with its own instrument, observing without error the free target T, 4 m
behind CAM1 and 8 m in front of CAM2, and F, in front of both: u = CX + C * px / pz, v = CY + C * py / pz, for T seen
from CAM1 (pz < 0) the mirrored projection*/
char const *const CAMPX_TARGET_BEHIND = R"(*TITR
Target behind the camera
*OLOC
*INSTR
*CAMPX CAMA TG
TG 0.1 0.1
*IO
C 1000
CX 500
CY 400
*CAMPX CAMB TG
TG 0.1 0.1
*IO
C 2000
CX 600
CY 500
*CALA
CAM1 0 0 0
CAM2 0 0 -12
*POIN
T 0.5 0.2 -4
F 0.5 0.2 4
*CAM CAM1 CAMA
*UV
T 375 350
F 625 450
*CAM CAM2 CAMB
*UV
T 725 550
F 662.5 525
*END
)";

/*The target T lies in the plane of the projection centre of the camera, it has no projection*/
char const *const CAMPX_TARGET_IN_PLANE = R"(*TITR
Target in the plane of the projection centre
*OLOC
*INSTR
*CAMPX CAMA TG
TG 0.1 0.1
*IO
C 1000
*CALA
T 1 0.2 0
F 0.5 0.2 4
*FRAME S1 0 0 0 0 0 0 1 RX RY RZ
*CALA
CAM1 0 0 0
*CAM CAM1 CAMA
*UV
T 600 450
F 125 50
*ENDFRAME
*END
)";

/*A wide angle pixel camera with the rational radial distortion K4, K5, K6 (OpenCV CALIB_RATIONAL_MODEL) at the origin of ROOT,
looking along its z axis. The UV values are those of OpenCV 4.13 cv2.projectPoints(points, rvec = 0, tvec = 0, cameraMatrix,
distCoeffs) with fx = C + B1, fy = C, cx = CX, cy = CY and distCoeffs = (K1, K2, P1, P2, K3, K4, K5, K6), printed with repr().
B2 is zero, cv2.projectPoints ignores the skew of the camera matrix. All the interior orientation parameters are free.*/
char const *const CAMPX_OPENCV_RATIONAL = R"(*TITR
Pixel camera with rational distortion against OpenCV
*OLOC
*PREC 7
*INSTR
*CAMPX CAMA TG
TG 0.1 0.1
*IO
C 2900 FREE
CX 2010.25 FREE
CY 1495.75 FREE
K1 0.12 FREE
K2 -0.31 FREE
K3 0.05 FREE
K4 0.09 FREE
K5 -0.25 FREE
K6 0.03 FREE
P1 4.0e-4 FREE
P2 -3.0e-4 FREE
B1 3.5 FREE
B2 0 FREE
*CALA
C1 0 0 0
P01 -1.3 -0.95 2.0
P02 -1.3 0.1 2.3
P03 -1.3 1.0 2.6
P04 -0.4 -0.95 2.3
P05 -0.4 0.1 2.6
P06 -0.4 1.0 2.0
P07 0.5 -0.95 2.6
P08 0.5 0.1 2.0
P09 0.5 1.0 2.3
P10 1.4 -0.95 2.0
P11 1.4 0.1 2.3
P12 1.4 1.0 2.6
*CAM C1 CAMA
*UV
P01 123.0100918201017 119.43949604270051
P02 361.5332540249424 1622.772758052849
P03 551.8935701258961 2616.405649927858
P04 1503.3459999396393 293.95222196016334
P05 1563.1612886076598 1607.4111449261582
P06 1426.6041850331217 2952.8113098966305
P07 2570.160838077005 433.11774275457265
P08 2737.2180651185417 1641.0550594984516
P09 2643.884155345222 2762.17594781164
P10 4036.3129573824335 122.98649084071621
P11 3783.551107534794 1622.7168052397176
P12 3578.4759399528757 2615.3411302386594
*END
)";

} // namespace TestCAMPX

#endif
