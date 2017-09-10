// FGInterface.h: interface for the FGInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FGINTERFACE_H__07B5F54C_6C11_4D1B_915C_1BA52EE96B9A__INCLUDED_)
#define AFX_FGINTERFACE_H__07B5F54C_6C11_4D1B_915C_1BA52EE96B9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

    // Return values from GetStatus function
enum ICI7320_CAMERA_STATUS {
    CS_SHUT_DOWN=0,             // api not initialized
    CS_SEARCHING_FOR_CAMERA,    // camera not found
    CS_INITIALIZING_CAMERA,     // camera found, starting up
    CS_CAMERA_ERROR,            // camera found, but error
    CS_CAMERA_RUNNING,          // camera found, started & running 
    CS_reserved1,
    CS_reserved2,
    };

enum CAMERA_TYPE
{
	ICI_CAMERA_7000 = 0,
	ICI_CAMERA_8000,
	ICI_CAMERA_9000,
	ICI_CAMERA_SWIR
};
    //
    // Function prototypes
    //
extern "C" {
    typedef int                 (__stdcall *dll_InitializeCamera)(int);
    typedef int                 (__stdcall *dll_DestroyCamera)(void);
    typedef int                 (__stdcall *dll_GetImageReady)(void);
    typedef unsigned short *    (__stdcall *dll_GetNextImage)(void);
    typedef float *             (__stdcall *dll_GetNextImageFloat)(void);
    //typedef int                 (__stdcall *dll_AutoFocusCamera)(void);
    typedef int                 (__stdcall *dll_SetCameraFocus)(float focus);
    typedef float               (__stdcall *dll_GetCameraFocus)(void);
    typedef long                (__stdcall *dll_SetCameraFocusSpeed)(long direction, long speed);

    typedef void                (__stdcall *dll_GHPaintImage)(HDC hdc, RECT r, void *image_data8, void *pal, int width, int height);
    typedef int                 (__stdcall *dll_NucCorrection)(HWND wnd);

    // Added 8/28/07
    typedef int (__stdcall *dll_SetCalibrationFormula)(int index);                   
    typedef int (__stdcall *dll_GetCalibrationFormula)(void);                        

    typedef int (__stdcall *dll_SetFrameAveraging)(int count);                       
    typedef int (__stdcall *dll_GetFrameAveraging)(void);                            

    typedef int (__stdcall *dll_GetLastSensorReadings)(float *lens, float *fpa);  

    // Added Jan 29 08
    typedef float (__stdcall *dll_GetAverageFPS)(void);

    // Added Sept 4 08
    typedef int  (__stdcall *dll_GetCameraStatus)(void);

    // Added May 6 09   rc 1 on success
    typedef int (__stdcall *dll_GetImageSize)(long *width, long *height);

    // added Sep 30, 09 - emissivity correction functions
    typedef void  (__stdcall *dll_EmisSetWavelength)(float w1, float w2);
    typedef void  (__stdcall *dll_EmisSetParameters)(float _ambient, float _emis, float _trans);
    typedef float (__stdcall *dll_EmisCorrect)(float observed);

    // added feb 1/2010
    // functions to control positioning of the focus motor on cameras
    // that have one
    typedef long  (__stdcall *dll_UpdateFocusReadings)(void);                     // read parameters from focus motor controller
    typedef long  (__stdcall *dll_GetFocusPosition)(long *position);              // get current position of focus (call UpdateFocusReadings first)
    typedef long  (__stdcall *dll_GetFocusSpeed)(long *speed, long *direction);   // get current focus speed and direction (call UpdateFocusReadings first)

        // send command to focus to specific position
    typedef long  (__stdcall *dll_SetFocusPosition)(long position);               // command focus to specific location

		//Retrieve Camera Serial Number
	typedef long (__stdcall *dll_GetCameraSerialNumber)();

        // new functions 5/2010
        // Image manipulation functions
        // For loading images saved from IRFlash and retreiving temps 
    typedef long         (__stdcall *dll_ImgDeleteHandle)(long ihandle);
    typedef long         (__stdcall *dll_ImgGetBitmap)(long ihandle);
    typedef const char * (__stdcall *dll_ImgGetErrorDescription)( void );
    typedef long         (__stdcall *dll_ImgGetPixelTemperature)(long ihandle, float *buf, int x, int y);
    typedef long         (__stdcall *dll_ImgGetTemperatures)(long ihandle, float *buf, int buffwidth, int stx, int sty, int endx, int endy);
    typedef long         (__stdcall *dll_ImgLoad)(const char *name);

	typedef long        (__stdcall *dll_ImgGetPalette)(long ihandle, char *paletteName, unsigned char *paletteValues);

	typedef int(__stdcall *dll_EnableAutocalibration)(bool bEnable);

	typedef int(__stdcall *dll_GetNextImageFloatLabView)(float *pTemperatureImage);

	//added on 10/6/2015
	typedef int( __stdcall *dll_SetAutomaticCalibrationPeriod )( const unsigned short nAutomaticCalibrationPeriodInMinutes );
    };

class FGInterface  
{
public:
	FGInterface();
	virtual ~FGInterface();



        HMODULE m_dll;

        dll_InitializeCamera        InitializeCamera;
        dll_DestroyCamera           DestroyCamera;
        dll_GetImageReady           GetImageReady;
        dll_GetNextImage            GetNextImage;
        dll_GetNextImageFloat       GetNextImageFloat;
        //dll_AutoFocusCamera         AutoFocusCamera;
        dll_SetCameraFocus          SetCameraFocus;

        dll_SetCameraFocusSpeed     SetCameraFocusSpeed;

        dll_GHPaintImage            GHPaintImage;

        dll_NucCorrection           NucCorrection;

    // Added 8/28/07
    // Advanced features, not necessary for proper operation

        dll_SetCalibrationFormula   SetCalibrationFormula;
        dll_GetCalibrationFormula   GetCalibrationFormula;

        dll_SetFrameAveraging       SetFrameAveraging;                       
        dll_GetFrameAveraging       GetFrameAveraging;                 

        dll_GetLastSensorReadings   GetLastSensorReadings;

        dll_GetAverageFPS           GetAverageFPS;

    // Added 9/4/08 - to get camera status
        dll_GetCameraStatus         GetCameraStatus;

        dll_GetImageSize            GetImageSize;

        // added Sep 30, 09 - emissivity correction functions
        dll_EmisSetWavelength   EmisSetWavelength;
        dll_EmisSetParameters   EmisSetParameters;
        dll_EmisCorrect         EmisCorrect;


        // added feb 1/2010
        // functions to control positioning of the focus motor on cameras
        // that have one
        dll_UpdateFocusReadings UpdateFocusReadings;  // read parameters from focus motor controller
        dll_GetFocusPosition    GetFocusPosition;     // get current position of focus (call UpdateFocusReadings first)
        dll_GetFocusSpeed       GetFocusSpeed;        // get current focus speed and direction (call UpdateFocusReadings first)

        // send command to focus to specific position
        dll_SetFocusPosition    SetFocusPosition;     // command focus to specific location

		dll_GetCameraSerialNumber     GetCameraSerialNumber;

            // new functions 5/2010
            // Image manipulation functions
            // For loading images saved from IRFlash and retreiving temps 
        dll_ImgDeleteHandle         ImgDeleteHandle;            // delete image handle
        dll_ImgGetBitmap            ImgGetBitmap;               // get HBITMAP from image handle
        dll_ImgGetErrorDescription  ImgGetErrorDescription;     // get error text from the last function call in Img lib
        dll_ImgGetPixelTemperature  ImgGetPixelTemperature;     // get single pixel temperature
        dll_ImgGetTemperatures      ImgGetTemperatures;         // get array if pixel temperatures
        dll_ImgLoad                 ImgLoad;                    // load an image
		dll_ImgGetPalette           ImgGetPalette;                    // load an image

		//12/3/2014 Added by AB on thermalwave imaging request.
		dll_EnableAutocalibration           EnableAutocalibration;

		dll_GetNextImageFloatLabView GetNextImageFloatLabView;

		dll_SetAutomaticCalibrationPeriod SetAutomaticCalibrationPeriod;
    };


#endif // !defined(AFX_FGINTERFACE_H__07B5F54C_6C11_4D1B_915C_1BA52EE96B9A__INCLUDED_)
