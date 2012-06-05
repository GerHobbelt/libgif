/******************************************************************************
 
gif_lib.h - service library for decoding and encoding GIF images
                                                                             
*****************************************************************************/

#ifndef _GIF_LIB_H_
#define _GIF_LIB_H_ 1

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GIFLIB_MAJOR 5
#define GIFLIB_MINOR 0
#define GIFLIB_RELEASE 0

#define GIF_ERROR   0
#define GIF_OK      1

#include <stdbool.h>

#define GIF_STAMP "GIFVER"          /* First chars in file - GIF stamp.  */
#define GIF_STAMP_LEN sizeof(GIF_STAMP) - 1
#define GIF_VERSION_POS 3           /* Version first character in stamp. */
#define GIF87_STAMP "GIF87a"        /* First chars in file - GIF stamp.  */
#define GIF89_STAMP "GIF89a"        /* First chars in file - GIF stamp.  */

typedef unsigned char GifPixelType;
typedef unsigned char *GifRowType;
typedef unsigned char GifByteType;
typedef unsigned int GifPrefixType;
typedef int GifWord;

typedef struct GifColorType {
    GifByteType Red, Green, Blue;
} GifColorType;

typedef struct ColorMapObject {
    int ColorCount;
    int BitsPerPixel;
    GifColorType *Colors;    /* on malloc(3) heap */
} ColorMapObject;

typedef struct GifImageDesc {
    GifWord Left, Top, Width, Height;   /* Current image dimensions. */
    bool Interlace;                     /* Sequential/Interlaced lines. */
    ColorMapObject *ColorMap;           /* The local color map */
} GifImageDesc;

typedef struct ExtensionBlock {
    int ByteCount;
    GifByteType *Bytes;    /* on malloc(3) heap */
    int Function;   /* Holds the type of the Extension block. */
} ExtensionBlock;

typedef struct ExtensionBlockList {
    int ExtensionBlockCount;         /* # of extensions past last image */
    ExtensionBlock *ExtensionBlocks; /* Extensions past last image */    
} ExtensionBlockList;

typedef struct GifFileType {
    GifWord SWidth, SHeight,        /* Screen dimensions. */
      SColorResolution,         /* How many colors can we generate? */
      SBackGroundColor;         /* I hope you understand this one... */
    ColorMapObject *SColorMap;  /* NULL if not exists. */
    int ImageCount;             /* Number of current image */
    GifImageDesc Image;         /* Block describing current image */
    struct SavedImage *SavedImages; /* Use this to accumulate file state */
    ExtensionBlockList Trailing;    /* Extension blocks past last image */
    void *UserData;            /* hook to attach user data (TVT) */
    void *Private;             /* Don't mess with this! */
} GifFileType;

typedef enum {
    UNDEFINED_RECORD_TYPE,
    SCREEN_DESC_RECORD_TYPE,
    IMAGE_DESC_RECORD_TYPE, /* Begin with ',' */
    EXTENSION_RECORD_TYPE,  /* Begin with '!' */
    TERMINATE_RECORD_TYPE   /* Begin with ';' */
} GifRecordType;

/* func type to read gif data from arbitrary sources (TVT) */
typedef int (*InputFunc) (GifFileType *, GifByteType *, int);

/* func type to write gif data to arbitrary targets.
 * Returns count of bytes written. (MRB)
 */
typedef int (*OutputFunc) (GifFileType *, const GifByteType *, int);

/******************************************************************************
 *  GIF89 structures and extension function codes
******************************************************************************/

typedef struct {
    int DisposalMode;
#define DISPOSAL_UNSPECIFIED      0       /* No disposal specified. */
#define DISPOSE_DO_NOT            1       /* Leave image in place */
#define DISPOSE_BACKGROUND        2       /* Set area too background color */
#define DISPOSE_PREVIOUS          3       /* Restore to previous content */
    bool UserInputFlag;      /* User confirmation required before disposal */
    int DelayTime;           /* pre-display delay in 0.01sec units */
    int TransparentIndex;    /* Palette index for transparency, -1 if none */
#define NO_TRANSPARENT_INDEX	-1
} GraphicsControlBlock;

#define COMMENT_EXT_FUNC_CODE     0xfe    /* comment */
#define GRAPHICS_EXT_FUNC_CODE    0xf9    /* graphics control */
#define PLAINTEXT_EXT_FUNC_CODE   0x01    /* plaintext */
#define APPLICATION_EXT_FUNC_CODE 0xff    /* application block */

/******************************************************************************
 * O.K., here are the routines one can access in order to encode GIF file:     
 * (GIF_LIB file EGIF_LIB.C).                              
******************************************************************************/

/* Main entry points */
GifFileType *EGifOpenFileName(const char *GifFileName,
                              const bool GifTestExistence);
GifFileType *EGifOpenFileHandle(const int GifFileHandle);
GifFileType *EGifOpen(void *userPtr, OutputFunc writeFunc);
int EGifSpew(GifFileType * GifFile);
void EGifSetGifVersion(const char *Version);
char *EGifGetGifVersion(GifFileType *GifFile); /* new in 5.x */
int EGifCloseFile(GifFileType * GifFile);

#define E_GIF_ERR_OPEN_FAILED    1    /* And EGif possible errors. */
#define E_GIF_ERR_WRITE_FAILED   2
#define E_GIF_ERR_HAS_SCRN_DSCR  3
#define E_GIF_ERR_HAS_IMAG_DSCR  4
#define E_GIF_ERR_NO_COLOR_MAP   5
#define E_GIF_ERR_DATA_TOO_BIG   6
#define E_GIF_ERR_NOT_ENOUGH_MEM 7
#define E_GIF_ERR_DISK_IS_FULL   8
#define E_GIF_ERR_CLOSE_FAILED   9
#define E_GIF_ERR_NOT_WRITEABLE  10

/* These are legacy.  You probably do not want to call them directly */
int EGifPutScreenDesc(GifFileType *GifFile,
                      const int GifWidth, const int GifHeight, 
		      const int GifColorRes,
                      const int GifBackGround,
                      const ColorMapObject *GifColorMap);
int EGifPutImageDesc(GifFileType *GifFile, 
		     const int GifLeft, const int GifTop,
                     const int GifWidth, const int GifHeight, 
		     const bool GifInterlace,
                     const ColorMapObject *GifColorMap);
int EGifPutLine(GifFileType *GifFile, GifPixelType *GifLine,
                int GifLineLen);
int EGifPutPixel(GifFileType *GifFile, const GifPixelType GifPixel);
int EGifPutComment(GifFileType *GifFile, const char *GifComment);
int EGifPutExtensionFirst(GifFileType *GifFile, const int GifExtCode,
                          const int GifExtLen, const void *GifExtension);
int EGifPutExtensionNext(GifFileType *GifFile, const int GifExtCode,
                         const int GifExtLen, const void *GifExtension);
int EGifPutExtensionLast(GifFileType *GifFile, const int GifExtCode,
                         const int GifExtLen, const void *GifExtension);
int EGifPutExtension(GifFileType *GifFile, const int GifExtCode, 
		     const int GifExtLen,
                     const void *GifExtension);
int EGifPutCode(GifFileType *GifFile, int GifCodeSize,
                const GifByteType *GifCodeBlock);
int EGifPutCodeNext(GifFileType *GifFile,
                    const GifByteType *GifCodeBlock);

/******************************************************************************
 * O.K., here are the routines one can access in order to decode GIF file:     
 * (GIF_LIB file DGIF_LIB.C).                              
 *****************************************************************************/

/* Main entry points */
GifFileType *DGifOpenFileName(const char *GifFileName);
GifFileType *DGifOpenFileHandle(int GifFileHandle);
int DGifSlurp(GifFileType * GifFile);
GifFileType *DGifOpen(void *userPtr, InputFunc readFunc);    /* new one (TVT) */
int DGifCloseFile(GifFileType * GifFile);

#define D_GIF_ERR_OPEN_FAILED    101    /* And DGif possible errors. */
#define D_GIF_ERR_READ_FAILED    102
#define D_GIF_ERR_NOT_GIF_FILE   103
#define D_GIF_ERR_NO_SCRN_DSCR   104
#define D_GIF_ERR_NO_IMAG_DSCR   105
#define D_GIF_ERR_NO_COLOR_MAP   106
#define D_GIF_ERR_WRONG_RECORD   107
#define D_GIF_ERR_DATA_TOO_BIG   108
#define D_GIF_ERR_NOT_ENOUGH_MEM 109
#define D_GIF_ERR_CLOSE_FAILED   110
#define D_GIF_ERR_NOT_READABLE   111
#define D_GIF_ERR_IMAGE_DEFECT   112
#define D_GIF_ERR_EOF_TOO_SOON   113

/* These are legacy.  You probably do not want to call them directly */
int DGifGetScreenDesc(GifFileType *GifFile);
int DGifGetRecordType(GifFileType *GifFile, GifRecordType *GifType);
int DGifGetImageDesc(GifFileType *GifFile);
int DGifGetLine(GifFileType *GifFile, GifPixelType *GifLine, int GifLineLen);
int DGifGetPixel(GifFileType *GifFile, GifPixelType GifPixel);
int DGifGetComment(GifFileType *GifFile, char *GifComment);
int DGifGetExtension(GifFileType *GifFile, int *GifExtCode,
                     GifByteType **GifExtension);
int DGifGetExtensionNext(GifFileType *GifFile, GifByteType **GifExtension);
int DGifGetCode(GifFileType *GifFile, int *GifCodeSize,
                GifByteType **GifCodeBlock);
int DGifGetCodeNext(GifFileType *GifFile, GifByteType **GifCodeBlock);
int DGifGetLZCodes(GifFileType *GifFile, int *GifCode);


/******************************************************************************
 * O.K., here are the routines from GIF_LIB file QUANTIZE.C.              
******************************************************************************/
int GifQuantizeBuffer(unsigned int Width, unsigned int Height,
                   int *ColorMapSize, GifByteType * RedInput,
                   GifByteType * GreenInput, GifByteType * BlueInput,
                   GifByteType * OutputBuffer,
                   GifColorType * OutputColorMap);

/******************************************************************************
 * O.K., here are the routines from GIF_LIB file GIF_ERR.C.              
******************************************************************************/
extern int GifError(void);             /* new in 2012 - ESR */
extern char *GifErrorString(void);     /* new in 2012 - ESR */
extern int GifLastError(void);

/*****************************************************************************
 *
 * Everything below this point is new after version 1.2, supporting `slurp
 * mode' for doing I/O in two big belts with all the image-bashing in core.
 *
 *****************************************************************************/

/******************************************************************************
 * Color Map handling from ALLOCGIF.C                          
 *****************************************************************************/

extern ColorMapObject *MakeMapObject(int ColorCount,
                                     const GifColorType *ColorMap);
extern void FreeMapObject(ColorMapObject *Object);
extern ColorMapObject *UnionColorMap(const ColorMapObject *ColorIn1,
                                     const ColorMapObject *ColorIn2,
                                     GifPixelType ColorTransIn2[]);
extern int BitSize(int n);

/******************************************************************************
 * Support for the in-core structures allocation (slurp mode).              
 *****************************************************************************/

/* This holds an image header, its unpacked raster bits, and extensions */
typedef struct SavedImage {
    GifImageDesc ImageDesc;
    GifByteType *RasterBits;  /* on malloc(3) heap */
    ExtensionBlockList Leading;
} SavedImage;

extern void ApplyTranslation(SavedImage *Image, GifPixelType Translation[]);
extern int AddExtensionBlock(ExtensionBlockList *New, int Function, 
				 unsigned int Len, unsigned char ExtData[]);
extern void FreeExtensions(ExtensionBlockList *ExtensionList);
extern SavedImage *MakeSavedImage(GifFileType *GifFile,
                                  const SavedImage *CopyFrom);
extern void FreeSavedImages(GifFileType *GifFile);

/******************************************************************************
 * 5.x functions for GIF89 graphics control blocks
 *****************************************************************************/

int DGifExtensionToGCB(const GifByteType *GifExtension,
		       GraphicsControlBlock *GCB);
size_t EGifGCBToExtension(const GraphicsControlBlock *GCB,
		       GifByteType *GifExtension);

int DGifSavedExtensionToGCB(GifFileType *GifFile, int ImageIndex, 
			    GraphicsControlBlock *GCB);
int EGifGCBToSavedExtension(const GraphicsControlBlock *GCB, 
			    GifFileType *GifFile, int ImageIndex);

/******************************************************************************
 * The library's internal utility font                          
 *****************************************************************************/

#define GIF_FONT_WIDTH  8
#define GIF_FONT_HEIGHT 8
extern const unsigned char AsciiTable[][GIF_FONT_WIDTH];

extern void GifDrawText(SavedImage *Image,
                     const int x, const int y,
                     const char *legend, const int color);

extern void GifDrawBox(SavedImage *Image,
                    const int x, const int y,
                    const int w, const int d, const int color);

extern void GifDrawRectangle(SavedImage *Image,
                   const int x, const int y,
                   const int w, const int d, const int color);

extern void GifDrawBoxedText(SavedImage *Image,
                          const int x, const int y,
                          const char *legend,
                          const int border, const int bg, const int fg);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _GIF_LIB_H */
