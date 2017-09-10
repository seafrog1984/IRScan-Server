// FGInterface.cpp: implementation of the FGInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "CaptureDlg.h"
#include "FGInterface.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



    //
    // Dummy functions that will get called incase dll load fails
    //
    // The application should check to make sure FGInterface.m_dll!=NULL
    // for proper error checking
    //
int __stdcall nofunc_int_floatptr_floatptr(float *, float *)
    {
    return 0;
    }

int __stdcall nofunc_int_int(int)
    {
    return 0;
    }


int __stdcall nofunc_int()
    {
    return 0;
    }

int __stdcall nofunc_int_float(float)
    {
    return 0;
    }

float  __stdcall nofunc_float()
    {
    return 0.0;
    }

void * __stdcall nofunc_vptr()
    {
    return NULL;
    }

long __stdcall nofunc_lll(long, long)
    {
    return 0;
    }

void __stdcall nofunc_paint(HDC hdc, RECT r, void *image_data8, void *pal, int width, int height)
    {
    }

int __stdcall nofunc_nuc(HWND w)
    {
    return 0;
    }

int __stdcall nofunc_getsize(long *width, long *height)
{
    // set 0 width & height
    // watch for bad pointers!
    // return error
    try {
        *width = 0;
        *height = 0;
        }
    catch (...) {
        }
    return 0;
}


 void  __stdcall nofunc_eswl(float w1, float w2)
 {
 }

 void  __stdcall nofunc_esp(float _ambient, float _emis, float _trans)
 {
 }

 float __stdcall nofunc_ec(float observed)
 {
     return observed;
 }


 
    // These interrogate the focus motor hardware directly!
    // UpdateFocusReadings() reads from the motor... The rest read that data
long __stdcall nofunc_UpdateFocusReadings(void)
{
    return 0;
}

long __stdcall nofunc_GetFocusPosition(long *position)
{
    return 0;
}

long __stdcall nofunc_GetFocusSpeed(long *speed, long *direction)
{
    return 0;
}

long __stdcall nofunc_GetCameraSerialNumber()
{
	return 0;
}
    // send command to focus to specific position
long __stdcall nofunc_SetFocusPosition(long position)
{
    return 0;
}

long         __stdcall nofunc_ImgDeleteHandle(long ihandle)
{
    return 0;
}

long         __stdcall nofunc_ImgGetBitmap(long ihandle)
{
    return 1;
}

const char * __stdcall nofunc_ImgGetErrorDescription( void )
{
    return "Error loading dll or wrong version";
}

long         __stdcall nofunc_ImgGetPixelTemperature(long ihandle, float *buf, int x, int y)
{
    return 1;
}

long         __stdcall nofunc_ImgGetTemperatures(long ihandle, float *buf, int buffwidth, int stx, int sty, int endx, int endy)
{
    return 1;
}

long         __stdcall nofunc_ImgLoad(const char *name)
{
    return 1;
}

long __stdcall nofunc_ImgGetPalette(long ihandle, char *paletteName, unsigned char *paletteValues)
{
	return 1;
}

int __stdcall nofunc_EnableAutocalibration(bool bEnable)
{
	return 1;
}

int __stdcall nofunc_GetNextImageFloatLabView(unsigned short *pImage, unsigned char *pDisplayImage)
{
	return 1;
}

int __stdcall nofunc_SetAutomaticCalibrationPeriod( const unsigned short nAutomaticCalibrationPeriodInMinutes )
{
	return 1;
}

class PathSplitJoin {
    public:
        PathSplitJoin()
            {
            m_Path[0]=0;
            m_Drive[0]=0;
            m_Dir[0]=0;
            m_Name[0]=0;
            m_Ext[0]=0;
            }

        char m_Path[_MAX_PATH];

        char m_Drive[_MAX_DRIVE];
        char m_Dir[_MAX_DIR];
        char m_Name[_MAX_FNAME];
        char m_Ext[_MAX_EXT];
        
        void Split(const char *path)
            {
            _splitpath_s( path, m_Drive, m_Dir, m_Name, m_Ext );
            }

        const char *Join(void)
            {
            _makepath_s( m_Path, m_Drive, m_Dir, m_Name, m_Ext );
            return m_Path;
            }

        const char *GetAppPath(void)
            {
            TCHAR szAppPath[MAX_PATH];
            GetModuleFileName(NULL, szAppPath, MAX_PATH);
            Split(szAppPath);
            
            m_Name[0]=0;
            m_Ext[0]=0;

            return Join();
            }

        const char *GetAppName(void)
            {
            TCHAR szAppPath[MAX_PATH];
            GetModuleFileName(NULL, szAppPath, MAX_PATH);
            Split(szAppPath);

            return m_Path;
            }
    };



FGInterface::FGInterface()
{
        // Macro for loading functions from dll
    #define DLL_LOAD(proto, name, dummyfunc)                    \
        if (m_dll) name = (proto)GetProcAddress(m_dll, #name);  \
        if (!m_dll || name==(proto)NULL) name=(proto)dummyfunc

    PathSplitJoin psj;
    psj.GetAppPath();
    strcpy_s(psj.m_Name, "ICI");
    strcpy_s(psj.m_Ext, "dll");

    const char *lib_locations[] = {
        psj.Join(),
        "ICI.dll",   // search %PATH%
        "..\\API\\ICI.dll",      // search SDK development env
        "..\\..\\API\\ICI.dll",  // search SDK development env
#ifdef _DEBUG
        "..\\..\\AirApi\\Debug\\ICI.dll",   // our dev environment, debug version
#else
        "..\\..\\AirApi\\Release\\ICI.dll", // our dev environment, release version
#endif
        NULL
        };

    // go through the above library locations looking for the dll
    const char *libname;
    m_dll = NULL;
    for (int i=0; m_dll==NULL && lib_locations[i]!=NULL; i++) {
        libname = lib_locations[i];
        m_dll = ::LoadLibrary(libname);
        }

        
    if (m_dll) {
        OutputDebugString("FGInterface::FGInterface(): Loading ");
        OutputDebugString(libname);
        OutputDebugString("\n");
        }
    else {
        OutputDebugString("ICI.dll not found\n");
        }



    DLL_LOAD(dll_SetCalibrationFormula,       SetCalibrationFormula,    nofunc_int_int);
    DLL_LOAD(dll_GetCalibrationFormula,       GetCalibrationFormula,    nofunc_int);
    DLL_LOAD(dll_SetFrameAveraging    ,       SetFrameAveraging,        nofunc_int_int);
    DLL_LOAD(dll_GetFrameAveraging    ,       GetFrameAveraging,        nofunc_int);
    DLL_LOAD(dll_GetLastSensorReadings,       GetLastSensorReadings,    nofunc_int_floatptr_floatptr);

    DLL_LOAD(dll_InitializeCamera,      InitializeCamera,       nofunc_int_int      );
    DLL_LOAD(dll_DestroyCamera,         DestroyCamera,          nofunc_int      );
    DLL_LOAD(dll_GetImageReady,         GetImageReady,          nofunc_int      );
    DLL_LOAD(dll_GetNextImage,          GetNextImage,           nofunc_vptr     );
    DLL_LOAD(dll_GetNextImageFloat,     GetNextImageFloat,      nofunc_vptr     );
//    DLL_LOAD(dll_AutoFocusCamera,       AutoFocusCamera,        nofunc_int      );
    DLL_LOAD(dll_SetCameraFocus,        SetCameraFocus,         nofunc_int_float);
    DLL_LOAD(dll_SetCameraFocusSpeed,   SetCameraFocusSpeed,    nofunc_lll      );
    DLL_LOAD(dll_GHPaintImage,          GHPaintImage,           nofunc_paint    );
    DLL_LOAD(dll_NucCorrection,         NucCorrection,          nofunc_float      );

    DLL_LOAD(dll_GetAverageFPS, GetAverageFPS, nofunc_float);


    DLL_LOAD(dll_GetCameraStatus,  GetCameraStatus, nofunc_int);

    DLL_LOAD(dll_GetImageSize,  GetImageSize, nofunc_getsize);

    DLL_LOAD(dll_EmisSetWavelength, EmisSetWavelength, nofunc_eswl);
    DLL_LOAD(dll_EmisSetParameters, EmisSetParameters, nofunc_esp);
    DLL_LOAD(dll_EmisCorrect      , EmisCorrect,       nofunc_ec);

    // added feb 1/2010
    // functions to control positioning of the focus motor on cameras
    // that have one
    DLL_LOAD(dll_UpdateFocusReadings, UpdateFocusReadings, nofunc_UpdateFocusReadings);    // read parameters from focus motor controller
    DLL_LOAD(dll_GetFocusPosition, GetFocusPosition, nofunc_GetFocusPosition);             // get current position of focus (call UpdateFocusReadings first)
    DLL_LOAD(dll_GetFocusSpeed, GetFocusSpeed, nofunc_GetFocusSpeed);                      // get current focus speed and direction (call UpdateFocusReadings first)

    // send command to focus to specific position
    DLL_LOAD(dll_SetFocusPosition, SetFocusPosition, nofunc_SetFocusPosition);             // command focus to specific location

	DLL_LOAD(dll_GetCameraSerialNumber,GetCameraSerialNumber,nofunc_GetCameraSerialNumber);

	DLL_LOAD(dll_EnableAutocalibration, EnableAutocalibration, nofunc_EnableAutocalibration);

	DLL_LOAD(dll_GetNextImageFloatLabView, GetNextImageFloatLabView, nofunc_GetNextImageFloatLabView);

	DLL_LOAD( dll_SetAutomaticCalibrationPeriod, SetAutomaticCalibrationPeriod, nofunc_SetAutomaticCalibrationPeriod );

#define load( func ) DLL_LOAD( dll_##func, func, nofunc_##func )
        load( ImgDeleteHandle );            // delete image handle
        load( ImgGetBitmap );               // get HBITMAP from image handle
        load( ImgGetErrorDescription );     // get error text from the last function call in Img lib
        load( ImgGetPixelTemperature );     // get single pixel temperature
        load( ImgGetTemperatures );         // get array if pixel temperatures
        load( ImgLoad );                    // load an image
		load(ImgGetPalette);

    if (!m_dll) {
        OutputDebugString("*** FGInterface::FGInterface(): ICI.dll Load Failed, using dummy functions ***\n");
        OutputDebugString("*** ICI.dll should be located in the same folder as the application ***\n");
        return;
        }
}

FGInterface::~FGInterface()
{
    if (m_dll) {
	FreeLibrary(m_dll);
	}
}

