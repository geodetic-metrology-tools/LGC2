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
#define ECSP "ECSP"
#define ECVE "ECVE"
#define ORIE "ORIE"
#define RADI "RADI"
#define CALA "CALA"
#define POIN "POIN"
#define PDOR "PDOR"
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
#define APRI "APRI" 
#define PUNC "PUNC"
#define PREC "PREC"
#define SIMU "SIMU"
#define INSTR "INSTR"
#define FRAME "FRAME"
#define ENDFRAME "ENDFRAME"
#define END "END"
#define TITR "TITR"

const int nb_allowed_tstn		= 1;
const int nb_allowed_v0			= 6;
const int nb_allowed_plr3d		= 18;
const int nb_allowed_angl		= 18;
const int nb_allowed_zend		= 18;
const int nb_allowed_dist		= 18;
const int nb_allowed_ecth		= 18;
const int nb_allowed_dhor		= 18;
const int nb_allowed_cam		= 2;
const int nb_allowed_uvec		= 14;
const int nb_allowed_uvd		= 14;
const int nb_allowed_dspt		= 13;
const int nb_allowed_dlev		= 12;
const int nb_allowed_dver		= 12;
const int nb_allowed_echo		= 12;
const int nb_allowed_ecsp		= 12;
const int nb_allowed_ecve		= 12;
const int nb_allowed_orie		= 12;
const int nb_allowed_radi       = 12;
const int nb_allowed_cala		= 18;
const int nb_allowed_poin		= 18;
const int nb_allowed_pdor       = 18;
const int nb_allowed_vxy		= 18;
const int nb_allowed_vxz		= 18;
const int nb_allowed_vyz		= 18;
const int nb_allowed_vz			= 18;
const int nb_allowed_polar		= 23;
const int nb_allowed_camd		= 23;
const int nb_allowed_edm		= 23;
const int nb_allowed_level		= 23;
const int nb_allowed_scale		= 23;
const int nb_allowed_oloc		= 13;
const int nb_allowed_rs2k		= 13;
const int nb_allowed_lep		= 13;
const int nb_allowed_sphe		= 13;
const int nb_allowed_faut		= 17;
const int nb_allowed_apri		= 17;
const int nb_allowed_punc		= 17;
const int nb_allowed_prec		= 17;
const int nb_allowed_simu		= 17;
const int nb_allowed_instr		= 5;
const int nb_allowed_frame		= 6;
const int nb_allowed_ef			= 19;
const int nb_allowed_end		= 0;
const int nb_allowed_titr		= 4;


//TSTN measurments
const char* allowed_TSTN[nb_allowed_tstn]	= {V0};
const char* allowed_V0[nb_allowed_v0]		= {PLR3D, ANGL, ZEND, DIST, DHOR, ECTH,};
const char* allowed_PLR3D[nb_allowed_plr3d]	= {TSTN, PLR3D, ANGL, ZEND, DIST, ECTH, DHOR, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, FRAME, ENDFRAME, END};
const char* allowed_ANGL[nb_allowed_angl]	= {TSTN, PLR3D, ANGL, ZEND, DIST, ECTH, DHOR, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, FRAME, ENDFRAME, END};
const char* allowed_ZEND[nb_allowed_zend]	= {TSTN, PLR3D, ANGL, ZEND, DIST, ECTH, DHOR, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, FRAME, ENDFRAME, END};
const char* allowed_DIST[nb_allowed_dist]	= {TSTN, PLR3D, ANGL, ZEND, DIST, ECTH, DHOR, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, FRAME, ENDFRAME, END};
const char* allowed_ECTH[nb_allowed_ecth]	= {TSTN, PLR3D, ANGL, ZEND, DIST, ECTH, DHOR, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, FRAME, ENDFRAME, END};
const char* allowed_DHOR[nb_allowed_dhor]	= {TSTN, PLR3D, ANGL, ZEND, DIST, ECTH, DHOR, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, FRAME, ENDFRAME, END};
//CAM measurments
const char* allowed_CAM[nb_allowed_cam]		= {UVEC, UVD};
const char* allowed_UVEC[nb_allowed_uvec]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE,	UVEC, UVD, FRAME, ENDFRAME, END};
const char* allowed_UVD[nb_allowed_uvd]		= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, UVEC, UVD, FRAME, ENDFRAME, END};
// Others measurments
const char* allowed_DSPT[nb_allowed_dspt]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, FRAME, ENDFRAME, END};
const char* allowed_DLEV[nb_allowed_dlev]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, FRAME, END};
const char* allowed_DVER[nb_allowed_dver]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, FRAME, END};
const char* allowed_ECHO[nb_allowed_echo]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, FRAME, END};
const char* allowed_ECSP[nb_allowed_ecsp]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, FRAME, END};
const char* allowed_ECVE[nb_allowed_ecve]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, FRAME, END};
const char* allowed_ORIE[nb_allowed_orie]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, FRAME, END};
const char* allowed_RADI[nb_allowed_radi]   = {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, FRAME, END };
//CALA & POIN & others point DEF
const char* allowed_CALA[nb_allowed_cala]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI,		  POIN, VXY, VXZ, VYZ, VZ, FRAME, ENDFRAME, END};
const char* allowed_POIN[nb_allowed_poin]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, CALA,   	VXY, VXZ, VYZ, VZ, FRAME, ENDFRAME, END};
const char* allowed_VXY[nb_allowed_vxy]		= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, CALA, POIN,		 VXZ, VYZ, VZ, FRAME, ENDFRAME, END};
const char* allowed_VXZ[nb_allowed_vxz]		= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, CALA, POIN, VXY,	  VYZ, VZ, FRAME, ENDFRAME, END};
const char* allowed_VYZ[nb_allowed_vyz]		= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, CALA, POIN, VXY, VXZ,	   VZ, FRAME, ENDFRAME, END};
const char* allowed_VZ[nb_allowed_vz]		= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, CALA, POIN, VXY, VXZ, VYZ,	   FRAME, ENDFRAME, END};
const char* allowed_PDOR[nb_allowed_pdor]   = {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, CALA, POIN, VXY, VXZ, VYZ, VZ, FRAME, END };
//Instruments
const char* allowed_POLAR[nb_allowed_polar]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, CALA, POIN, VXY, VXZ, VYZ, VZ, PDOR, POLAR, CAMD, EDM, LEVEL, SCALE, FRAME};
const char* allowed_CAMD[nb_allowed_camd]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, CALA, POIN, VXY, VXZ, VYZ, VZ, PDOR, POLAR, CAMD, EDM, LEVEL, SCALE, FRAME};
const char* allowed_EDM[nb_allowed_edm]		= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, CALA, POIN, VXY, VXZ, VYZ, VZ, PDOR, POLAR, CAMD, EDM, LEVEL, SCALE, FRAME};
const char* allowed_LEVEL[nb_allowed_level]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, CALA, POIN, VXY, VXZ, VYZ, VZ, PDOR, POLAR, CAMD, EDM, LEVEL, SCALE, FRAME};
const char* allowed_SCALE[nb_allowed_scale]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, CALA, POIN, VXY, VXZ, VYZ, VZ, PDOR, POLAR, CAMD, EDM, LEVEL, SCALE, FRAME};
// Reference systems
const char* allowed_OLOC[nb_allowed_oloc]	= {APRI, SIMU, PREC, FAUT, PUNC, INSTR, CALA, POIN, VXY, VXZ, VYZ, VZ, PDOR};
const char* allowed_RS2K[nb_allowed_rs2k]	= {APRI, SIMU, PREC, FAUT, PUNC, INSTR, CALA, POIN, VXY, VXZ, VYZ, VZ, PDOR};
const char* allowed_LEP[nb_allowed_lep]		= {APRI, SIMU, PREC, FAUT, PUNC, INSTR, CALA, POIN, VXY, VXZ, VYZ, VZ, PDOR};
const char* allowed_SPHE[nb_allowed_sphe]	= {APRI, SIMU, PREC, FAUT, PUNC, INSTR, CALA, POIN, VXY, VXZ, VYZ, VZ, PDOR};
// Options
const char* allowed_FAUT[nb_allowed_faut]	= {APRI, SIMU, PREC,       PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, INSTR, FRAME, PDOR};
const char* allowed_APRI[nb_allowed_apri]	= {      SIMU, PREC, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, INSTR, FRAME, PDOR};
const char* allowed_PUNC[nb_allowed_punc]	= {APRI, SIMU, PREC, FAUT,       CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, INSTR, FRAME, PDOR};
const char* allowed_PREC[nb_allowed_prec]	= {APRI, SIMU,       FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, INSTR, FRAME, PDOR};
const char* allowed_SIMU[nb_allowed_simu]	= {APRI,       PREC, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, INSTR, FRAME, PDOR};
// Others
const char* allowed_INSTR[nb_allowed_instr]	= {POLAR, CAMD, EDM, LEVEL, SCALE};
const char* allowed_FRAME[nb_allowed_frame]	= {CAM, DSPT, CALA, POIN, FRAME, ENDFRAME};
const char* allowed_ENDFRAME[nb_allowed_ef]	= {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, CALA, POIN, VXY, VXZ, VYZ, VZ, FRAME, ENDFRAME, END};
const char* allowed_END[1]					= {};
const char* allowed_TITR[nb_allowed_titr]	= {OLOC, RS2K, LEP, SPHE};

}

#endif