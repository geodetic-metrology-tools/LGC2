/*
© Copyright CERN 2000-2019. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef KEYWORD_AUTHORIZATIONS_H
#define KEYWORD_AUTHORIZATIONS_H
#pragma once


//STL
#include <vector>
#include <string>
#include <array>

namespace {

	//measurements
#define TSTN "TSTN"
#define V0 "V0"
#define PLR3D "PLR3D"
#define PLR3D_ANGL "PLR3D:ANGL"
#define PLR3D_ZEND "PLR3D:ZEND"
#define PLR3D_DIST "PLR3D:DIST"
#define ANGL "ANGL"
#define ZEND "ZEND"
#define ZENI "ZENI" //lgc1
#define ZENH "ZENH" //lgc1
#define DIST "DIST"
#define DTHE "DTHE" //lgc1
#define DMES "DMES" //lgc1
#define ECTH "ECTH"
#define DHOR "DHOR"
#define CAM "CAM"
#define UVEC "UVEC"
#define UVEC_X "UVEC:X"
#define UVEC_Y "UVEC:Y"
#define UVD "UVD"
#define UVD_X "UVD:X"
#define UVD_Y "UVD:Y"
#define UVD_DIST "UVD:DIST"
#define DSPT "DSPT"
#define DLEV "DLEV"
#define DVER "DVER"
#define ECHO "ECHO"
#define ECSP "ECSP"
#define ECDIR "ECDIR"
#define ECVE "ECVE"
#define ORIE "ORIE"
#define RADI "RADI"
#define OBSXYZ "OBSXYZ"
#define INCLY   "INCLY"
#define ECWS "ECWS"
#define ECWI "ECWI"
	//points
#define CALA "CALA"
#define POIN "POIN"
#define PDOR "PDOR"
#define VXY "VXY"
#define VXZ "VXZ"
#define VYZ "VYZ"
#define VZ "VZ"
	//instruments
#define POLAR "POLAR"
#define CAMD "CAMD"
#define EDM "EDM"
#define LEVEL "LEVEL"
#define SCALE "SCALE"
#define INCL "INCL"
#define HLSR "HLSR"
#define WPSR "WPSR"
	//referential
#define OLOC "OLOC"
#define RS2K "RS2K"
#define LEP "LEP"
#define SPHE "SPHE"
	//header - options
#define FAUT "FAUT"
#define APRI "APRI" 
#define DEFA "DEFA"
#define PUNC "PUNC"
#define PREC "PREC"
#define MICR "MICR" //lgc1
#define CLIC "CLIC" //lgc1
#define DIXI "DIXI" //lgc1
#define TOL "TOL" //lgc1
#define NOSPC "NOSPC" //lgc1
#define PRES "PRES"
#define SIMU "SIMU"
#define LIBR "LIBR"
#define ALLFIXED "ALLFIXED"
#define INSTR "INSTR"
#define FRAME "FRAME"
#define ENDFRAME "ENDFRAME"
#define END "END"
#define TITR "TITR"
#define FMTP "FMTP"
#define NODUP "NODUP"
#define SOBS "SOBS"
#define SAGELEMENT "SAGELEMENT"
#define SAGCONNECT "SAGCONNECT"
#define HIST "HIST"
#define EREL "EREL"
#define ERELFRAME "ERELFRAME"
#define COVAR "COVAR"
#define CHABA "CHABA"
#define CONSI "CONSI"
#define JSON "JSON"


	//measurements

	//
	const int nb_allowed_tstn = 1;
	const int nb_allowed_v0 = 7;
	const int nb_allowed_plr3d = 25;
	const int nb_allowed_angl = 25;
	const int nb_allowed_zend = 25;
	const int nb_allowed_dist = 25;
	const int nb_allowed_ecth = 25;
	const int nb_allowed_ecdir = 25;
	const int nb_allowed_dhor = 25;
	const int nb_allowed_cam = 2;
	const int nb_allowed_uvec = 19;
	const int nb_allowed_uvd = 19;
	const int nb_allowed_dspt = 18;
	const int nb_allowed_dlev = 17;
	const int nb_allowed_dver = 17;
	const int nb_allowed_echo = 17;
	const int nb_allowed_ecve = 17;
	const int nb_allowed_ecsp = 17;
	const int nb_allowed_orie = 17;
	const int nb_allowed_radi = 17;
	const int nb_allowed_OBSXYZ = 18;
	const int nb_allowed_incly = 19;
	const int nb_allowed_ecws = 17;
	const int nb_allowed_ecwi = 17;
	//points
	const int nb_allowed_cala = 23;
	const int nb_allowed_poin = 23;
	const int nb_allowed_pdor = 23;
	const int nb_allowed_vxy = 23;
	const int nb_allowed_vxz = 23;
	const int nb_allowed_vyz = 23;
	const int nb_allowed_vz = 23;
	// sag connections // same number of rights as the point types
	const int nb_allowed_sagConnections = 23;
	//instruments
	const int nb_allowed_polar = 31;
	const int nb_allowed_camd = 31;
	const int nb_allowed_edm = 31;
	const int nb_allowed_level = 31;
	const int nb_allowed_scale = 31;
	const int nb_allowed_incl = 31;
	const int nb_allowed_hlsr = 31;
	const int nb_allowed_wpsr = 31;
	//referentials
	const int nb_allowed_oloc = 27;
	const int nb_allowed_rs2k = 27;
	const int nb_allowed_lep = 27;
	const int nb_allowed_sphe = 27;
	//options
	const int nb_allowed_faut = 31;
	const int nb_allowed_defa = 31;
	const int nb_allowed_apri = 31;
	const int nb_allowed_punc = 31;
	const int nb_allowed_prec = 31;
	const int nb_allowed_pres = 31;
	const int nb_allowed_fmtp = 31;
	const int nb_allowed_nodup = 31;
	const int nb_allowed_sobs = 31;
	const int nb_allowed_hist = 31;
	const int nb_allowed_cons_check = 31;
	const int nb_allowed_erel = 31;
	const int nb_allowed_erelFrame = 31;
	const int nb_allowed_simu = 31;
	const int nb_allowed_libr = 31;
	const int nb_allowed_allfixed = 28;
	const int nb_allowed_covar = 31;
	const int nb_allowed_chaba = 31;
	const int nb_allowed_json = 31;

	//header
	const int nb_allowed_instr = 20; // instruments + points + frame + round of measurements without intruments
	const int nb_allowed_frame = 9;
	const int nb_allowed_sagelement = 20; // new adjustable object, only after INSTR
	const int nb_allowed_ef = 24;
	const int nb_allowed_end = 0;
	const int nb_allowed_titr = 4;

	//lgc1
	const int nb_allowed_angl_lgc1 = 15;
	const int nb_allowed_zeni_lgc1 = 15;
	const int nb_allowed_zenh_lgc1 = 15;
	const int nb_allowed_dmes_lgc1 = 15;
	const int nb_allowed_dthe_lgc1 = 15;
	const int nb_allowed_ecsp_lgc1 = 15;
	const int nb_allowed_ecth_lgc1 = 15;
	const int nb_allowed_dhor_lgc1 = 15;
	const int nb_allowed_dlev_lgc1 = 15;
	const int nb_allowed_dver_lgc1 = 15;
	const int nb_allowed_echo_lgc1 = 15;
	const int nb_allowed_ecve_lgc1 = 15;
	const int nb_allowed_orie_lgc1 = 15;
	const int nb_allowed_radi_lgc1 = 15;

	const int nb_allowed_faut_lgc1 = 29;
	const int nb_allowed_defa_lgc1 = 29;
	const int nb_allowed_apri_lgc1 = 29;
	const int nb_allowed_punc_lgc1 = 29;
	const int nb_allowed_prec_lgc1 = 25;
	const int nb_allowed_micr_lgc1 = 25;
	const int nb_allowed_clic_lgc1 = 25;
	const int nb_allowed_dixi_lgc1 = 25;
	const int nb_allowed_tol_lgc1 = 25;
	const int nb_allowed_nospc_lgc1 = 29;
	const int nb_allowed_pres_lgc1 = 29;
	const int nb_allowed_fmtp_lgc1 = 29;
	const int nb_allowed_nodup_lgc1 = 29;
	const int nb_allowed_sobs_lgc1 = 29;
	const int nb_allowed_hist_lgc1 = 29;
	const int nb_allowed_erel_lgc1 = 29;
	const int nb_allowed_simu_lgc1 = 28;
	const int nb_allowed_libr_lgc1 = 28;
	const int nb_allowed_allfixed_lgc1 = 27;

	const int nb_allowed_oloc_lgc1 = 25;
	const int nb_allowed_rs2k_lgc1 = 25;
	const int nb_allowed_lep_lgc1 = 25;
	const int nb_allowed_sphe_lgc1 = 25;

	const int nb_allowed_cala_lgc1 = 19;
	const int nb_allowed_poin_lgc1 = 19;
	const int nb_allowed_pdor_lgc1 = 20;
	const int nb_allowed_vxy_lgc1 = 19;
	const int nb_allowed_vxz_lgc1 = 19;
	const int nb_allowed_vyz_lgc1 = 19;
	const int nb_allowed_vz_lgc1 = 19;



	//TSTN measurments
	const char* allowed_TSTN[nb_allowed_tstn] = { V0 };
	const char* allowed_V0[nb_allowed_v0] = { PLR3D, ANGL, ZEND, DIST, DHOR, ECTH, ECDIR };
	const char* allowed_PLR3D[nb_allowed_plr3d] = { TSTN, PLR3D, ANGL, ZEND, DIST, ECTH, DHOR, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECDIR, ECVE, ORIE, RADI, OBSXYZ, FRAME, ENDFRAME, END,INCLY,ECWS,ECWI, SAGCONNECT };
	const char* allowed_ANGL[nb_allowed_angl] = { TSTN, PLR3D, ANGL, ZEND, DIST, ECTH, DHOR, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECDIR, ECVE, ORIE, RADI, OBSXYZ, FRAME, ENDFRAME, END,INCLY,ECWS,ECWI, SAGCONNECT };
	const char* allowed_ZEND[nb_allowed_zend] = { TSTN, PLR3D, ANGL, ZEND, DIST, ECTH, DHOR, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECDIR, ECVE, ORIE, RADI, OBSXYZ, FRAME, ENDFRAME, END,INCLY,ECWS,ECWI, SAGCONNECT };
	const char* allowed_DIST[nb_allowed_dist] = { TSTN, PLR3D, ANGL, ZEND, DIST, ECTH, DHOR, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECDIR, ECVE, ORIE, RADI, OBSXYZ, FRAME, ENDFRAME, END,INCLY,ECWS,ECWI, SAGCONNECT };
	const char* allowed_ECTH[nb_allowed_ecth] = { TSTN, PLR3D, ANGL, ZEND, DIST, ECTH, DHOR, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECDIR, ECVE, ORIE, RADI, OBSXYZ, FRAME, ENDFRAME, END,INCLY,ECWS,ECWI, SAGCONNECT };
	const char* allowed_ECDIR[nb_allowed_ecdir] = { TSTN, PLR3D, ANGL, ZEND, DIST, ECTH, DHOR, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECDIR, ECVE, ORIE, RADI, OBSXYZ, FRAME, ENDFRAME, END,INCLY,ECWS,ECWI, SAGCONNECT };
	const char* allowed_DHOR[nb_allowed_dhor] = { TSTN, PLR3D, ANGL, ZEND, DIST, ECTH, DHOR, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECDIR, ECVE, ORIE, RADI, OBSXYZ, FRAME, ENDFRAME, END,INCLY,ECWS,ECWI, SAGCONNECT };
	//CAM measurments
	const char* allowed_CAM[nb_allowed_cam] = { UVEC, UVD };
	const char* allowed_UVEC[nb_allowed_uvec] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, UVEC, UVD, FRAME, ENDFRAME, END, OBSXYZ,INCLY,ECWS,ECWI, SAGCONNECT };
	const char* allowed_UVD[nb_allowed_uvd] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, UVEC, UVD, FRAME, ENDFRAME, END, OBSXYZ,INCLY,ECWS,ECWI, SAGCONNECT };
	// Others measurments
	const char* allowed_DSPT[nb_allowed_dspt] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, FRAME, ENDFRAME, END,INCLY,ECWS,ECWI, SAGCONNECT };
	const char* allowed_DLEV[nb_allowed_dlev] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, FRAME, END,INCLY,ECWS,ECWI, SAGCONNECT };
	const char* allowed_DVER[nb_allowed_dver] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, FRAME, END,INCLY,ECWS,ECWI, SAGCONNECT };
	const char* allowed_ECHO[nb_allowed_echo] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, FRAME, END,INCLY,ECWS,ECWI, SAGCONNECT };
	const char* allowed_ECVE[nb_allowed_ecve] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, FRAME, END,INCLY,ECWS,ECWI, SAGCONNECT };
	const char* allowed_ECSP[nb_allowed_ecsp] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, FRAME, END,INCLY,ECWS,ECWI, SAGCONNECT };
	const char* allowed_ORIE[nb_allowed_orie] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, FRAME, END,INCLY,ECWS,ECWI, SAGCONNECT };
	const char* allowed_RADI[nb_allowed_radi] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, FRAME, END,INCLY,ECWS,ECWI, SAGCONNECT };
	const char* allowed_OBSXYZ[nb_allowed_OBSXYZ] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, FRAME, ENDFRAME, CALA, POIN,INCLY,ECWS,ECWI, SAGCONNECT };
	const char* allowed_INCLY[nb_allowed_incly] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, FRAME, ENDFRAME, CALA, INCLY, POIN, OBSXYZ,ECWS,ECWI, SAGCONNECT };
	const char* allowed_ECWS[nb_allowed_ecws] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, FRAME, END,INCLY, ECWS,ECWI, SAGCONNECT };
	const char* allowed_ECWI[nb_allowed_ecwi] = {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, FRAME, END, INCLY, ECWS, ECWI, SAGCONNECT};

	//CALA & POIN & others point DEF
	const char* allowed_CALA[nb_allowed_cala] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, POIN, VXY, VXZ, VYZ, VZ, FRAME, ENDFRAME, END, INCLY, ECWS,ECWI, SAGCONNECT };
	const char* allowed_POIN[nb_allowed_poin] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, CALA, VXY, VXZ, VYZ, VZ, FRAME, ENDFRAME, END, INCLY, ECWS,ECWI, SAGCONNECT };
	const char* allowed_VXY[nb_allowed_vxy] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, CALA, POIN, VXZ, VYZ, VZ, FRAME, ENDFRAME, END, INCLY, ECWS,ECWI, SAGCONNECT };
	const char* allowed_VXZ[nb_allowed_vxz] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, CALA, POIN, VXY, VYZ, VZ, FRAME, ENDFRAME, END, INCLY, ECWS,ECWI, SAGCONNECT };
	const char* allowed_VYZ[nb_allowed_vyz] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, CALA, POIN, VXY, VXZ, VZ, FRAME, ENDFRAME, END, INCLY, ECWS,ECWI, SAGCONNECT };
	const char* allowed_VZ[nb_allowed_vz] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, CALA, POIN, VXY, VXZ, VYZ, FRAME, ENDFRAME, END, INCLY, ECWS,ECWI, SAGCONNECT };
	const char* allowed_PDOR[nb_allowed_pdor] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, CALA, POIN, VXY, VXZ, VYZ, VZ, FRAME, END, INCLY, ECWS,ECWI, SAGCONNECT };
	const char* allowed_SAGCONNECT[nb_allowed_sagConnections] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, CALA, POIN, VXY, VXZ, VYZ, FRAME, ENDFRAME, END, INCLY, ECWS,ECWI, SAGCONNECT };
	//Instruments
	const char* allowed_POLAR[nb_allowed_polar] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, CALA, POIN, VXY, VXZ, VYZ, VZ, PDOR, POLAR, CAMD, EDM, LEVEL, SCALE, INCL, FRAME, HLSR,WPSR, ECWS,ECWI, SAGCONNECT, SAGELEMENT };
	const char* allowed_CAMD[nb_allowed_camd] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, CALA, POIN, VXY, VXZ, VYZ, VZ, PDOR, POLAR, CAMD, EDM, LEVEL, SCALE, INCL, FRAME, HLSR,WPSR, ECWS,ECWI, SAGCONNECT, SAGELEMENT };
	const char* allowed_EDM[nb_allowed_edm] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, CALA, POIN, VXY, VXZ, VYZ, VZ, PDOR, POLAR, CAMD, EDM, LEVEL, SCALE, INCL, FRAME, HLSR,WPSR, ECWS,ECWI, SAGCONNECT, SAGELEMENT };
	const char* allowed_LEVEL[nb_allowed_level] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, CALA, POIN, VXY, VXZ, VYZ, VZ, PDOR, POLAR, CAMD, EDM, LEVEL, SCALE, INCL, FRAME, HLSR,WPSR, ECWS,ECWI, SAGCONNECT, SAGELEMENT };
	const char* allowed_SCALE[nb_allowed_scale] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, CALA, POIN, VXY, VXZ, VYZ, VZ, PDOR, POLAR, CAMD, EDM, LEVEL, SCALE, INCL, FRAME, HLSR,WPSR, ECWS,ECWI, SAGCONNECT, SAGELEMENT };
	const char* allowed_INCL[nb_allowed_incl] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, CALA, POIN, VXY, VXZ, VYZ, VZ, PDOR, POLAR, CAMD, EDM, LEVEL, SCALE, INCL, FRAME, HLSR,WPSR, ECWS,ECWI, SAGCONNECT, SAGELEMENT };
	const char* allowed_HLSR[nb_allowed_hlsr] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, CALA, POIN, VXY, VXZ, VYZ, VZ, PDOR, POLAR, CAMD, EDM, LEVEL, SCALE, INCL, FRAME, HLSR,WPSR, ECWS,ECWI, SAGCONNECT, SAGELEMENT };
	const char* allowed_WPSR[nb_allowed_wpsr] = {TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, OBSXYZ, CALA, POIN, VXY, VXZ, VYZ, VZ, PDOR, POLAR, CAMD, EDM, LEVEL, SCALE, INCL, FRAME, HLSR, WPSR, ECWS, ECWI, SAGCONNECT, SAGELEMENT };

	// Reference systems
	const char* allowed_OLOC[nb_allowed_oloc] = { DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, INSTR, CALA, POIN, VXY, VXZ, VYZ, VZ, PDOR, FMTP, NODUP, SOBS, HIST, EREL, ERELFRAME, COVAR , CHABA , CONSI, JSON};
	const char* allowed_RS2K[nb_allowed_rs2k] = { DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, INSTR, CALA, POIN, VXY, VXZ, VYZ, VZ, PDOR, FMTP, NODUP, SOBS, HIST, EREL, ERELFRAME, COVAR , CHABA , CONSI, JSON};
	const char* allowed_LEP[nb_allowed_lep]   = { DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, INSTR, CALA, POIN, VXY, VXZ, VYZ, VZ, PDOR, FMTP, NODUP, SOBS, HIST, EREL, ERELFRAME, COVAR , CHABA , CONSI, JSON};
	const char* allowed_SPHE[nb_allowed_sphe] = { DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, INSTR, CALA, POIN, VXY, VXZ, VYZ, VZ, PDOR, FMTP, NODUP, SOBS, HIST, EREL, ERELFRAME, COVAR , CHABA , CONSI, JSON};
	// Options
	const char* allowed_DEFA[nb_allowed_defa] = { APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, INSTR, FRAME, PDOR, FMTP, NODUP, SOBS, HIST, EREL, ERELFRAME, COVAR , CHABA , CONSI, JSON};
	const char* allowed_APRI[nb_allowed_apri] = { DEFA, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, INSTR, FRAME, PDOR, FMTP, NODUP, SOBS, HIST, EREL, ERELFRAME, COVAR , CHABA , CONSI, JSON};
	const char* allowed_PREC[nb_allowed_prec] = { DEFA, APRI, SIMU, LIBR, ALLFIXED, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, INSTR, FRAME, PDOR, FMTP, NODUP, SOBS, HIST, EREL, ERELFRAME, COVAR , CHABA , CONSI, JSON};
	const char* allowed_PRES[nb_allowed_pres] = { DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, INSTR, FRAME, PDOR, FMTP, NODUP, SOBS, HIST, EREL, ERELFRAME, COVAR , CHABA , CONSI, JSON};
	const char* allowed_FAUT[nb_allowed_faut] = { DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, INSTR, FRAME, PDOR, FMTP, NODUP, SOBS, HIST, EREL, ERELFRAME, COVAR , CHABA , CONSI, JSON};
	const char* allowed_PUNC[nb_allowed_punc] = { DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, INSTR, FRAME, PDOR, FMTP, NODUP, SOBS, HIST, EREL, ERELFRAME, COVAR , CHABA , CONSI, JSON};
	const char* allowed_FMTP[nb_allowed_fmtp] = { DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, INSTR, FRAME, PDOR, NODUP, SOBS, HIST, EREL, ERELFRAME, COVAR , CHABA , CONSI, JSON};
	const char* allowed_NODUP[nb_allowed_nodup] = { DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, INSTR, FRAME, PDOR, FMTP, SOBS, HIST, EREL, ERELFRAME, COVAR, CHABA , CONSI, JSON};
	const char* allowed_SOBS[nb_allowed_sobs] = { DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, INSTR, FRAME, PDOR, FMTP, NODUP, HIST, EREL, ERELFRAME, COVAR , CHABA , CONSI, JSON};
	const char* allowed_HIST[nb_allowed_hist] = { DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, INSTR, FRAME, PDOR, FMTP, NODUP, SOBS, EREL, ERELFRAME, COVAR , CHABA , CONSI, JSON};
	const char* allowed_CONSI[nb_allowed_cons_check] = { DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, INSTR, FRAME, PDOR, FMTP, NODUP, HIST, SOBS, EREL, ERELFRAME, COVAR , CHABA, JSON};
	const char* allowed_EREL[nb_allowed_erel] = { DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, INSTR, FRAME, PDOR, FMTP, NODUP, SOBS, HIST, ERELFRAME, COVAR , CHABA , CONSI, JSON};
	const char *allowed_ERELFRAME[nb_allowed_erelFrame] = {DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, INSTR, FRAME, PDOR, FMTP, NODUP, SOBS, HIST, EREL, COVAR, CHABA, CONSI, JSON};
	const char* allowed_SIMU[nb_allowed_simu] = { FMTP, DEFA, APRI, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, INSTR, FRAME, PDOR, LIBR, NODUP, SOBS, HIST, EREL, ERELFRAME, COVAR, LIBR, CHABA , CONSI, JSON};
	const char* allowed_LIBR[nb_allowed_libr] = { FMTP, DEFA, APRI, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, INSTR, FRAME, PDOR, SIMU, NODUP, SOBS, HIST, EREL, ERELFRAME, COVAR, SIMU, CHABA , CONSI, JSON};
	const char* allowed_ALLFIXED[nb_allowed_allfixed] = { FMTP, DEFA, APRI, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, INSTR, FRAME, NODUP, SOBS, HIST, EREL, ERELFRAME, SIMU, CHABA , CONSI, JSON};
	const char* allowed_COVAR[nb_allowed_covar] = { FMTP, DEFA, APRI, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, INSTR, FRAME, PDOR, SIMU, NODUP, SOBS, HIST, EREL, ERELFRAME, LIBR, SIMU, CHABA , CONSI, JSON};
	const char* allowed_CHABA[nb_allowed_chaba] = { FMTP, DEFA, APRI, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, INSTR, FRAME, PDOR, SIMU, NODUP, SOBS, HIST, EREL, ERELFRAME, LIBR, SIMU, COVAR , CONSI, JSON};
	const char* allowed_JSON[nb_allowed_json] = { DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, INSTR, FRAME, PDOR, FMTP, NODUP, HIST, SOBS, EREL, ERELFRAME, COVAR , CHABA, CONSI};
	// Others
	const char *allowed_INSTR[nb_allowed_instr] = {POLAR, CAMD, EDM, LEVEL, SCALE, FRAME, PDOR, CALA, POIN, VXY, VXZ, VYZ, VZ, INCL, HLSR, WPSR, OBSXYZ, DVER, RADI, SAGELEMENT };
	const char *allowed_SAG[nb_allowed_sagelement] = { POLAR, CAMD, EDM, LEVEL, SCALE, FRAME, PDOR, CALA, POIN, VXY, VXZ, VYZ, VZ, INCL, HLSR, WPSR, OBSXYZ, DVER, RADI, SAGELEMENT };
	const char* allowed_FRAME[nb_allowed_frame] = { CAM, DSPT, CALA, POIN, FRAME, ENDFRAME, OBSXYZ, TSTN, INCLY};
	const char* allowed_ENDFRAME[nb_allowed_ef] = { TSTN, CAM, DSPT, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, CALA, POIN, VXY, VXZ, VYZ, VZ, FRAME, ENDFRAME, END, OBSXYZ, INCLY, ECWS,ECWI, SAGCONNECT};
	const char* allowed_END[1] = {};
	const char* allowed_TITR[nb_allowed_titr] = { OLOC, RS2K, LEP, SPHE };




	//lgc1
	const char* allowed_ANGL_lgc1[nb_allowed_angl_lgc1] = { ANGL, ZENI, ZENH, DMES, DTHE, ECTH, DHOR, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, END };
	const char* allowed_ZENI_lgc1[nb_allowed_zeni_lgc1] = { ANGL, ZENI, ZENH, DMES, DTHE, ECTH, DHOR, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, END };
	const char* allowed_ZENH_lgc1[nb_allowed_zenh_lgc1] = { ANGL, ZENI, ZENH, DMES, DTHE, ECTH, DHOR, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, END };
	const char* allowed_DMES_lgc1[nb_allowed_dmes_lgc1] = { ANGL, ZENI, ZENH, DMES, DTHE, ECTH, DHOR, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, END };
	const char* allowed_DTHE_lgc1[nb_allowed_dthe_lgc1] = { ANGL, ZENI, ZENH, DMES, DTHE, ECTH, DHOR, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, END };
	const char* allowed_ECTH_lgc1[nb_allowed_ecth_lgc1] = { ANGL, ZENI, ZENH, DMES, DTHE, ECTH, DHOR, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, END };
	const char* allowed_DHOR_lgc1[nb_allowed_dhor_lgc1] = { ANGL, ZENI, ZENH, DMES, DTHE, ECTH, DHOR, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, END };
	const char* allowed_ECSP_lgc1[nb_allowed_ecsp_lgc1] = { ANGL, ZENI, ZENH, DMES, DTHE, ECTH, DHOR, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, END };
	const char* allowed_DLEV_lgc1[nb_allowed_dlev_lgc1] = { ANGL, ZENI, ZENH, DMES, DTHE, ECTH, DHOR, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, END };
	const char* allowed_DVER_lgc1[nb_allowed_dver_lgc1] = { ANGL, ZENI, ZENH, DMES, DTHE, ECTH, DHOR, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, END };
	const char* allowed_ECHO_lgc1[nb_allowed_echo_lgc1] = { ANGL, ZENI, ZENH, DMES, DTHE, ECTH, DHOR, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, END };
	const char* allowed_ECVE_lgc1[nb_allowed_ecve_lgc1] = { ANGL, ZENI, ZENH, DMES, DTHE, ECTH, DHOR, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, END };
	const char* allowed_ORIE_lgc1[nb_allowed_orie_lgc1] = { ANGL, ZENI, ZENH, DMES, DTHE, ECTH, DHOR, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, END };
	const char* allowed_RADI_lgc1[nb_allowed_radi_lgc1] = { ANGL, ZENI, ZENH, DMES, DTHE, ECTH, DHOR, DLEV, DVER, ECHO, ECSP, ECVE, ORIE, RADI, END };

	const char* allowed_CALA_lgc1[nb_allowed_cala_lgc1] = { DTHE, DLEV, DVER, ECHO, ECVE, ORIE, RADI, POIN, VXY, VXZ, VYZ, VZ, END, DMES, ANGL, ZENI, ZENH, ECTH, DHOR };
	const char* allowed_POIN_lgc1[nb_allowed_poin_lgc1] = { DTHE, DLEV, DVER, ECHO, ECVE, ORIE, RADI, CALA, VXY, VXZ, VYZ, VZ, END, DMES, ANGL, ZENI, ZENH, ECTH, DHOR };
	const char* allowed_VXY_lgc1[nb_allowed_vxy_lgc1] = { DTHE, DLEV, DVER, ECHO, ECVE, ORIE, RADI, CALA, POIN, VXZ, VYZ, VZ, END, DMES, ANGL, ZENI, ZENH, ECTH, DHOR };
	const char* allowed_VXZ_lgc1[nb_allowed_vxz_lgc1] = { DTHE, DLEV, DVER, ECHO, ECVE, ORIE, RADI, CALA, POIN, VXY, VYZ, VZ, END, DMES, ANGL, ZENI, ZENH, ECTH, DHOR };
	const char* allowed_VYZ_lgc1[nb_allowed_vyz_lgc1] = { DTHE, DLEV, DVER, ECHO, ECVE, ORIE, RADI, CALA, POIN, VXY, VXZ, VZ, END, DMES, ANGL, ZENI, ZENH, ECTH, DHOR };
	const char* allowed_VZ_lgc1[nb_allowed_vz_lgc1] = { DTHE, DLEV, DVER, ECHO, ECVE, ORIE, RADI, CALA, POIN, VXY, VXZ, VYZ, END, DMES, ANGL, ZENI, ZENH, ECTH, DHOR };
	const char* allowed_PDOR_lgc1[nb_allowed_pdor_lgc1] = { DTHE, DLEV, DVER, ECHO, ECVE, ORIE, RADI, CALA, POIN, VXY, VXZ, VYZ, VZ, END, DMES, ANGL, ZENI, ZENH, ECTH, DHOR };

	const char* allowed_DEFA_lgc1[nb_allowed_defa_lgc1] = { NOSPC, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, PDOR, FMTP, NODUP, SOBS, HIST, EREL, MICR, CLIC, DIXI, TOL };
	const char* allowed_APRI_lgc1[nb_allowed_apri_lgc1] = { NOSPC, DEFA, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, PDOR, FMTP, NODUP, SOBS, HIST, EREL, MICR, CLIC, DIXI, TOL };
	const char* allowed_PREC_lgc1[nb_allowed_prec_lgc1] = { NOSPC, DEFA, APRI, SIMU, LIBR, ALLFIXED, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, PDOR, FMTP, NODUP, SOBS, HIST, EREL };
	const char* allowed_MICR_lgc1[nb_allowed_micr_lgc1] = { NOSPC, DEFA, APRI, SIMU, LIBR, ALLFIXED, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, PDOR, FMTP, NODUP, SOBS, HIST, EREL };
	const char* allowed_CLIC_lgc1[nb_allowed_clic_lgc1] = { NOSPC, DEFA, APRI, SIMU, LIBR, ALLFIXED, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, PDOR, FMTP, NODUP, SOBS, HIST, EREL };
	const char* allowed_DIXI_lgc1[nb_allowed_dixi_lgc1] = { NOSPC, DEFA, APRI, SIMU, LIBR, ALLFIXED, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, PDOR, FMTP, NODUP, SOBS, HIST, EREL };
	const char* allowed_TOL_lgc1[nb_allowed_tol_lgc1] = { NOSPC, DEFA, APRI, SIMU, LIBR, ALLFIXED, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, PDOR, FMTP, NODUP, SOBS, HIST, EREL };
	const char* allowed_NOSPC_lgc1[nb_allowed_nospc_lgc1] = { DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, PDOR, FMTP, NODUP, SOBS, HIST, EREL, MICR, CLIC, DIXI, TOL };
	const char* allowed_PRES_lgc1[nb_allowed_pres_lgc1] = { NOSPC, DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, PDOR, FMTP, NODUP, SOBS, HIST, EREL, MICR, CLIC, DIXI, TOL };
	const char* allowed_FAUT_lgc1[nb_allowed_faut_lgc1] = { NOSPC, DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, PDOR, FMTP, NODUP, SOBS, HIST, EREL, MICR, CLIC, DIXI, TOL };
	const char* allowed_PUNC_lgc1[nb_allowed_punc_lgc1] = { NOSPC, DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, PDOR, FMTP, NODUP, SOBS, HIST, EREL, MICR, CLIC, DIXI, TOL };
	const char* allowed_FMTP_lgc1[nb_allowed_fmtp_lgc1] = { NOSPC, DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, PDOR, NODUP, SOBS, HIST, EREL, MICR, CLIC, DIXI, TOL };
	const char* allowed_NODUP_lgc1[nb_allowed_nodup_lgc1] = { NOSPC, DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, PDOR, FMTP, SOBS, HIST, EREL, MICR, CLIC, DIXI, TOL };
	const char* allowed_SOBS_lgc1[nb_allowed_sobs_lgc1] = { NOSPC, DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, PDOR, FMTP, NODUP, HIST, EREL, MICR, CLIC, DIXI, TOL };
	const char* allowed_HIST_lgc1[nb_allowed_hist_lgc1] = { NOSPC, DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, PDOR, FMTP, NODUP, SOBS, EREL, MICR, CLIC, DIXI, TOL };
	const char* allowed_EREL_lgc1[nb_allowed_erel_lgc1] = { NOSPC, DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, PDOR, FMTP, NODUP, SOBS, HIST, MICR, CLIC, DIXI, TOL };
	const char* allowed_SIMU_lgc1[nb_allowed_simu_lgc1] = { NOSPC, DEFA, APRI, LIBR, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, PDOR, FMTP, NODUP, SOBS, HIST, EREL, MICR, CLIC, DIXI, TOL };
	const char* allowed_LIBR_lgc1[nb_allowed_libr_lgc1] = { NOSPC, DEFA, APRI, SIMU, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, PDOR, FMTP, NODUP, SOBS, HIST, EREL, MICR, CLIC, DIXI, TOL };
	const char* allowed_ALLFIXED_lgc1[nb_allowed_allfixed_lgc1] = { NOSPC, DEFA, APRI, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, OLOC, RS2K, LEP, SPHE, PDOR, FMTP, NODUP, SOBS, HIST, EREL, MICR, CLIC, DIXI, TOL };

	const char* allowed_OLOC_lgc1[nb_allowed_oloc_lgc1] = { DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, PDOR, FMTP, NODUP, SOBS, HIST, EREL, MICR, CLIC, DIXI, TOL };
	const char* allowed_RS2K_lgc1[nb_allowed_rs2k_lgc1] = { DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, PDOR, FMTP, NODUP, SOBS, HIST, EREL, MICR, CLIC, DIXI, TOL };
	const char* allowed_LEP_lgc1[nb_allowed_lep_lgc1] = { DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, PDOR, FMTP, NODUP, SOBS, HIST, EREL, MICR, CLIC, DIXI, TOL };
	const char* allowed_SPHE_lgc1[nb_allowed_sphe_lgc1] = { DEFA, APRI, SIMU, LIBR, ALLFIXED, PREC, PRES, FAUT, PUNC, CALA, POIN, VXY, VXZ, VYZ, VZ, PDOR, FMTP, NODUP, SOBS, HIST, EREL, MICR, CLIC, DIXI, TOL };
}

#endif
