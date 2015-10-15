// Define the variables necessary for the export and import of DLL functions

/*
#ifdef SU_MAKEDLL
   #define  SU_DECLSPEC  __declspec(dllexport)
#elif SU_USEDLL
   #define  SU_DECLSPEC  __declspec(dllimport)
#else
   #define 	SU_DECLSPEC
#endif*/


//#ifdef SU_MAKEDLL
   #define  SU_DECLSPEC  __declspec(dllexport)
//#else
  // #define  SU_DECLSPEC  __declspec(dllimport)
//#endif


#define DLLAPI __stdcall


