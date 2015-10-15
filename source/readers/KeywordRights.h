#ifndef KEYWORD_AUTHORIZATIONS_H
#define KEYWORD_AUTHORIZATIONS_H

#pragma once

#include <vector>
#include <string>
#include <array>

namespace {

#define TSTN "TSTN"
#define V0 "V0"
#define PLR3D "PLR3D"
#define ANGL "ANGL"
#define ZEND "ZEND"
#define DIST "DIST"
#define ECTH "ECTH"
#define DHOR "DHOR"
#define CAM "CAM"
#define UVEC "UVEC"
#define UVD "UVD"
#define DSPT "DSPT"
#define DLEV "DLEV"
#define DVER "DVER"
#define ECHO "ECHO"
#define ESCP "ESCP"
#define ECVE "ECVE"
#define ORIE "ORIE"
#define CALA "CALA"
#define POIN "POIN"
#define VXY "VXY"
#define VXZ "VXZ"
#define VYZ "VYZ"
#define VZ "VZ"
#define POLAR "POLAR"
#define CAMD "CAMD"
#define EDM "EDM"
#define LEVEL "LEVEL"
#define SCALE "SCALE"
#define OLOC "OLOC"
#define RS2K "RS2K"
#define LEP "LEP"
#define SPHE "SPHE"
#define FAUT "FAUT"
#define INSTR "INSTR"
#define FRAME "FRAME"
#define ENDFRAME "ENDFRAME"
#define END "END"
#define TITR "TITR"


const int nb_allowed_tstn		= 1;
const int nb_allowed_v0			= 4;
const int nb_allowed_plr3d		= 18;
const int nb_allowed_angl		= 18;
const int nb_allowed_zend		= 18;
const int nb_allowed_dist		= 18;
const int nb_allowed_ecth		= 18;
const int nb_allowed_dhor		= 18;
const int nb_allowed_cam		= 2;
const int nb_allowed_uvec		= 14;
const int nb_allowed_uvd		= 14;
const int nb_allowed_dspt		= 12;
const int nb_allowed_dlev		= 11;
const int nb_allowed_dver		= 11;
const int nb_allowed_echo		= 11;
const int nb_allowed_escp		= 11;
const int nb_allowed_ecve		= 11;
const int nb_allowed_orie		= 11;
const int nb_allowed_cala		= 17;
const int nb_allowed_poin		= 17;
const int nb_allowed_vxy		= 17;
const int nb_allowed_vxz		= 17;
const int nb_allowed_vyz		= 17;
const int nb_allowed_vz			= 17;
const int nb_allowed_polar		= 21;
const int nb_allowed_camd		= 21;
const int nb_allowed_edm		= 21;
const int nb_allowed_level		= 21;
const int nb_allowed_scale		= 21;
const int nb_allowed_oloc		= 8;
const int nb_allowed_rs2k		= 8;
const int nb_allowed_lep		= 8;
const int nb_allowed_sphe		= 8;
const int nb_allowed_faut		= 12;
const int nb_allowed_instr		= 5;
const int nb_allowed_frame		= 6;
const int nb_allowed_ef			= 18;
const int nb_allowed_end		= 0;
const int nb_allowed_titr		= 4;


//TSTN measurments
const char* allowed_TSTN[nb_allowed_tstn]	= {V0};
const char* allowed_V0[nb_allowed_v0]		= {PLR3D, ANGL, ZEND, DIST};
const char* allowed_PLR3D[nb_allowed_plr3d]	= {TSTN, PLR3D, ANGL, ZEND, DIST, ECTH, DHOR, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE, FRAME, ENDFRAME, END};
const char* allowed_ANGL[nb_allowed_angl]	= {TSTN, PLR3D, ANGL, ZEND, DIST, ECTH, DHOR, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE, FRAME, ENDFRAME, END};
const char* allowed_ZEND[nb_allowed_zend]	= {TSTN, PLR3D, ANGL, ZEND, DIST, ECTH, DHOR, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE, FRAME, ENDFRAME, END};
const char* allowed_DIST[nb_allowed_dist]	= {TSTN, PLR3D, ANGL, ZEND, DIST, ECTH, DHOR, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE, FRAME, ENDFRAME, END};
const char* allowed_ECTH[nb_allowed_ecth]	= {TSTN, PLR3D, ANGL, ZEND, DIST, ECTH, DHOR, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE, FRAME, ENDFRAME, END};
const char* allowed_DHOR[nb_allowed_dhor]	= {TSTN, PLR3D, ANGL, ZEND, DIST, ECTH, DHOR, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE, FRAME, ENDFRAME, END};
//CAM measurments
const char* allowed_CAM[nb_allowed_cam]		= {UVEC, UVD};
const char* allowed_UVEC[nb_allowed_uvec]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE,	UVEC, UVD, FRAME, ENDFRAME, END};
const char* allowed_UVD[nb_allowed_uvd]		= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE, UVEC, UVD, FRAME, ENDFRAME, END};
// Others measurments
const char* allowed_DSPT[nb_allowed_dspt]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE, FRAME, ENDFRAME, END};
const char* allowed_DLEV[nb_allowed_dlev]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE, FRAME, END};
const char* allowed_DVER[nb_allowed_dver]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE, FRAME, END};
const char* allowed_ECHO[nb_allowed_echo]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE, FRAME, END};
const char* allowed_ESCP[nb_allowed_escp]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE, FRAME, END};
const char* allowed_ECVE[nb_allowed_ecve]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE, FRAME, END};
const char* allowed_ORIE[nb_allowed_orie]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE, FRAME, END};
//CALA & POIN & others point DEF
const char* allowed_CALA[nb_allowed_cala]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE,		  POIN, VXY, VXZ, VYZ, VZ, FRAME, ENDFRAME, END};
const char* allowed_POIN[nb_allowed_poin]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE, CALA,   	VXY, VXZ, VYZ, VZ, FRAME, ENDFRAME, END};
const char* allowed_VXY[nb_allowed_vxy]		= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE, CALA, POIN,		 VXZ, VYZ, VZ, FRAME, ENDFRAME, END};
const char* allowed_VXZ[nb_allowed_vxz]		= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE, CALA, POIN, VXY,	  VYZ, VZ, FRAME, ENDFRAME, END};
const char* allowed_VYZ[nb_allowed_vyz]		= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE, CALA, POIN, VXY, VXZ,	   VZ, FRAME, ENDFRAME, END};
const char* allowed_VZ[nb_allowed_vz]		= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE, CALA, POIN, VXY, VXZ, VYZ,	   FRAME, ENDFRAME, END};
//Instruments
const char* allowed_POLAR[nb_allowed_polar]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE, CALA, POIN, VXY, VXZ, VYZ, VZ, POLAR, CAMD, EDM, LEVEL, SCALE, FRAME};
const char* allowed_CAMD[nb_allowed_camd]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE, CALA, POIN, VXY, VXZ, VYZ, VZ, POLAR, CAMD, EDM, LEVEL, SCALE, FRAME};
const char* allowed_EDM[nb_allowed_edm]		= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE, CALA, POIN, VXY, VXZ, VYZ, VZ, POLAR, CAMD, EDM, LEVEL, SCALE, FRAME};
const char* allowed_LEVEL[nb_allowed_level]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE, CALA, POIN, VXY, VXZ, VYZ, VZ, POLAR, CAMD, EDM, LEVEL, SCALE, FRAME};
const char* allowed_SCALE[nb_allowed_scale]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE, CALA, POIN, VXY, VXZ, VYZ, VZ, POLAR, CAMD, EDM, LEVEL, SCALE, FRAME};
// Reference systems
const char* allowed_OLOC[nb_allowed_oloc]	= {FAUT, INSTR, CALA, POIN, VXY, VXZ, VYZ, VZ};
const char* allowed_RS2K[nb_allowed_rs2k]	= {FAUT, INSTR, CALA, POIN, VXY, VXZ, VYZ, VZ};
const char* allowed_LEP[nb_allowed_lep]		= {FAUT, INSTR, CALA, POIN, VXY, VXZ, VYZ, VZ};
const char* allowed_SPHE[nb_allowed_sphe]	= {FAUT, INSTR, CALA, POIN, VXY, VXZ, VYZ, VZ};
// Options
const char* allowed_FAUT[nb_allowed_faut]	= {CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, INSTR, FRAME};
// Others
const char* allowed_INSTR[nb_allowed_instr]	= {POLAR, CAMD, EDM, LEVEL, SCALE};
const char* allowed_FRAME[nb_allowed_frame]	= {CAM, DSPT, CALA, POIN, FRAME, ENDFRAME};
const char* allowed_ENDFRAME[nb_allowed_ef]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ESCP, ECVE, ORIE, CALA, POIN, VXY, VXZ, VYZ, VZ, FRAME, ENDFRAME, END};
const char* allowed_END[1]					= {};
const char* allowed_TITR[nb_allowed_titr]	= {OLOC, RS2K, LEP, SPHE};

}

#endif