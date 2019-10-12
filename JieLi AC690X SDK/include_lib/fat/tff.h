#ifndef _FATFS_H
#define _FATFS_H


#include "typedef.h"
#include "diskio.h"


#define MAX_DEEPTH		9				/* 0~9 deepth of system */

#define LFN_MAX_SIZE    512   //不能超过512
/*******************************************************/
/********************驱动调试信息***********************/
/*******************************************************/
//#define FS_DEBUG

#ifdef FS_DEBUG
#define fs_deg    			printf
#define fs_deg_puts    		puts
#define fs_deg_buf   		printf_buf
#define fs_deg_data   		printf_data
#else
#define fs_deg(...)
#define fs_deg_puts(...)
#define fs_deg_buf(...)
#define fs_deg_data(...)
#endif

#define FS_LD_WORD(p)        ld_word_func(p)//(u16)(*(u16*)(u8*)(ptr))
#define FS_LD_DWORD(p)       ld_dword_func(p)//(u32)(*(u32*)(u8*)(ptr))

#define FS_ST_WORD(ptr,val)         st_word_func((u8 *)(ptr),(u16)(val)) //(u16)(*(u16*)(u8*)(ptr))
#define FS_ST_DWORD(ptr,val)        st_dword_func((u8 *)(ptr),(u32)(val)) //(u32)(*(u32*)(u8*)(ptr))




typedef u32	 CLUST;

#define FS_WRITE_EN             /* 是否允许写文件 */
#ifdef  FS_WRITE_EN
#define FAT12_WRITE_EN
//#define EXFAT_WRITE_EN
#endif
#define WIN_DATA_DIRTY  0x08
/* File system API info */
typedef struct _FSAPIMSG
{
    u32	    musicdir_counter;    // 包含指定文件的文件夹序号
    u32     dir_counter;			// 文件夹序号
    u32     file_total_indir;     // 当前目录的根下有效文件的个数
    u32     file_total_outdir;	// 当前目录前的文件总数，目录循环模式下，需要用它来计算文件序号
    u32	    file_number;			// 当前文件序号
    u32     file_counter;		//用于搜索文件计数
    u8	    deepth;				// dir deepth for search
} FSAPIMSG;

typedef struct _SWIN_BUF
{
    u8  start[512];
	u32  sector;
    struct _FATFS  *fs;
    u8   flag;
}SWIN_BUF;
/*
#define FS_WIN_START  fs->win->start
#define FS_WIN_SECTOR fs->win->sector
#define FS_WIN_FLAG   fs->win->flag
*/
#define FS_WIN_START  fs->win.start
#define FS_WIN_SECTOR fs->win.sector
#define FS_WIN_FLAG   fs->win.flag

/* File system object structure */
struct _FATFS
{
    //SWIN_BUF *win;		        /* Disk access window for Directory/FAT/File */
    u32		fatbase;		/* FAT start sector */
    u32		dirbase;		/* Root directory start sector */
    u32		database;		/* Data start sector */
    u32 	n_fatent;		/* Maximum cluster# + 1 */
    u32     total_file;		/* 当前设备的匹配文件总数*/
    u16		n_rootdir;		/* Number of root directory entries */
#ifdef FS_WRITE_EN
    u8      write_en;       //文件系统是否允许写操作：只允许FAT16，FAT32并且sector size只能是512bytes
    u8	    fsi_flag;		/* fsinfo dirty flag (1:must be written back) */
    u32	    last_clust;		/* Last allocated cluster */
    u32	    fsi_sector;		/* fsinfo sector (FAT32) */
#endif
    u32     boot_sect;
    u8		s_size;			/* sector size, 2 power n*/
    u8		csize;			/* Number of sectors per cluster,2 power n */
    u8		fs_type;		/* FAT sub type */
    u8		sector_512size;	/* size of per sector */
    u32 	(*disk_read)(void *hdev,u8 _xdata * buf, u32 lba);		/* device read function */
    u32		(*disk_write)(void *hdev,u8 _xdata * buf, u32 lba);		/* device write function */
	u32     fat_time;
//    u8      *lfn;           //长文件名buffer
//	u16     lfn_cnt;
//    char    *tpath;       //路径名称buffer
    void *hdev;
    SWIN_BUF win;		        /* Disk access window for Directory/FAT/File */
    u8 drive_cnt;
    //FSAPIMSG *fs_msg;
};
typedef struct _FATFS FATFS;


struct _FS_NAME
{
    char    tpath[128];       //路径名称buffer
    char    *lfn;             //长文件名buffer，长度LFN_MAX_SIZE×2，（前LFN_MAX_SIZE）为长文件名，（后LFN_MAX_SIZE）为长文件夹名
    char    *ldn;             //长文件名buffer，长度LFN_MAX_SIZE×2，（前LFN_MAX_SIZE）为长文件名，（后LFN_MAX_SIZE）为长文件夹名
	u16     lfn_cnt;
	u16     ldn_cnt;
};
typedef struct _FS_NAME FS_NAME;

typedef struct _LONG_FILE_NAME
{
	u16     lfn_cnt;
    char    lfn[LFN_MAX_SIZE];             //长文件名buffer
}LONG_FILE_NAME;//整理后的长文件名

typedef u32 (*disk_read)(u8 _xdata * buf, u32 lba);
typedef u32 (*disk_write)(u8 _xdata * buf, u32 lba);

typedef struct _FS_DISP_NAME
{
    char* tpath;       //路径名称buffer
    LONG_FILE_NAME* file_name;
    LONG_FILE_NAME* dir_name;
    u8 update_flag; //update info flag
}FS_DISP_NAME;

typedef struct
{
    LONG_FILE_NAME file_name;
    LONG_FILE_NAME dir_name;
    u8 update_flag; //bit0--file_name
}FS_DISP_NAME_T;

/* Directory object structure */
typedef struct _DIR
{
    u32 	clust;		        /* Current cluster */
    u32		csect;		        /* Current sector */
    u32		sclust;		        /* Start cluster */
    u16		cindex;		        /* Current index */
	//u16     lfn_index;		/* 长文件名index号 */
} DIR;

/* DIR status structure */
typedef struct _DIRINFO
{
    DIR     dj;
    u32 	sclust;			/* File start cluster */
    u32		fsize;			/* Size */
    u8		fattrib;		/* Attribute */
    char    fname[12];          /* */
} DIRINFO;

/* File object structure */
/*
typedef struct _FIL
{
    u32		fptr;				// File R/W pointer
    //u32		fsize;				// File size
    u8      flag;
	SWIN_BUF *data_win;
    u32		csect;			// Current sector
    u32 	clust;			// Current cluster
    //u32 	sclust;			// File start cluster
#ifdef FS_WRITE_EN
    SWIN_BUF *dir_win;
	u8       dir_duty;
#endif
	u32     start_clust;
	u32     end_clust;
    FATFS   *fs;
	DIRINFO dir_info;             //文件的目录项信息
	FSAPIMSG fs_msg;
} FIL;
*/



typedef struct _FIL0
{
    u32		fptr;				// File R/W pointer
    //u32		fsize;				// File size
    u8      flag;
	SWIN_BUF data_win;
    u32		csect;			// Current sector
    u32 	clust;			// Current cluster
    //u32 	sclust;			// File start cluster
#ifdef FS_WRITE_EN
    SWIN_BUF dir_win;
	u8       dir_duty;
#endif
	u32     start_clust;
	u32     end_clust;
    FATFS   *fs;
} FIL0;

typedef struct _FIL
{
    FIL0 fil;
    DIRINFO dir_info;             //文件的目录项信息
    FSAPIMSG fs_msg;
    //u8 filename[512];///<[1][512];
    //u8 path_name[128];///<128
    DIR f_dj[MAX_DEEPTH];
    FS_NAME fs_n;
} FIL;

typedef struct _LDIR_INFO
{
    u8 ldir_ord;
    u8 ldir_name1[10];             //文件的目录项信息
    u8 ldir_attr0;
    u8 ldir_type0;
    u8 ldir_chksum0;
    u8 ldir_name2[12];
    u16 ldir_fst_clus_lo;
    u8 ldir_name3[4];
} LDIR_INFO;

typedef struct _FS_DIR_INFO
{
//    DIR d_j;                	//dir info
	u32 sclust;					//dir sclust
	u16 dir_type;            	// 0-folder,1-file
	u16 fn_type;            	// 0-sfn,1-lfn
	LONG_FILE_NAME lfn_buf;     //long file name
}FS_DIR_INFO;

enum
{
    SEEK_SET = 0x01,
    SEEK_CUR = 0x02
};

/* File function return code (FRESULT) */

typedef enum
{
    FR_OK = 0,
    FR_FIND_DIR = 0x80,
    FR_FIND_FILE,
    FR_DIR_END,         //前面几个位置不能变
    FR_NO_FILE,
    FR_NO_PATH,
    FR_EXIST,
    FR_INVALID_NAME,
    FR_INVALID_DRIVE,
    FR_DENIED,
    FR_RW_ERROR,
    FR_WRITE_PROTECTED,
    FR_NO_FILESYSTEM,
    FR_DEEP_LIMIT,
    FR_END_PATH,
    FR_FILE_LIMIT,
    FR_END_FILE,
    FR_LFN_ERR,
    FR_MKFS_ABORTED,
    FR_DIR_DELETE,
    FR_DISK_ERROR,
    FR_FILE_END,
	FR_FILE_ERR,
    FR_NO_WINBUF,
	FR_INT_ERR,				/* (2) Assertion failed */
	FR_NO_SEL_DRIVE,
	FR_PART_PATH_OK,
	FR_ALL_PATH_OK,
} FRESULT;


    /* File access control and file status flags (FIL.flag) */

#define	FA_OPEN_EXISTING	0x00
#ifdef FS_WRITE_EN
#define FA_CREATE_HIDDEN    0x02
#define	FA_WRITE			0x04            //是否允许写文件
#define	FA_CREATE_NEW		0x08            //文件不存在时创建
#define	FA_CREATE_ALWAYS	0x10            //无论文件中否存在，均创建
#endif

#define FA__ERROR			0x80			//文件错误
#define FDISK__ERROR		0x40			//设备错误


#define	DDE					0xE5	/* Deleted directory entry mark in DIR_Name[0] */
#define	NDDE				0x05	/* Replacement of the character collides with DDE */

#define SZ_DIR      32


//#ifdef __SMART_CPU__

//#endif


/* File function return code (FRESULT) */


#define _DF1S      1
/* Character code support macros */

#define IsUpper(c)	(((c)>='A')&&((c)<='Z'))
#define IsLower(c)	(((c)>='a')&&((c)<='z'))

#if _DF1S		/* DBCS configuration */

#ifdef _DF2S	/* Two 1st byte areas */
#define IsDBCS1(c)	(((u8)(c) >= _DF1S && (u8)(c) <= _DF1E) || ((u8)(c) >= _DF2S && (u8)(c) <= _DF2E))
#else			/* One 1st byte area */
//#define IsDBCS1(c)	((u8)(c) >= _DF1S && (u8)(c) <= _DF1E)
#define IsDBCS1(c)	((u8)(c) < ' ' && (u8)(c) > '~')
#endif

#ifdef _DS3S	/* Three 2nd byte areas */
#define IsDBCS2(c)	(((u8)(c) >= _DS1S && (u8)(c) <= _DS1E) || ((u8)(c) >= _DS2S && (u8)(c) <= _DS2E) || ((u8)(c) >= _DS3S && (u8)(c) <= _DS3E))
#else			/* Two 2nd byte areas */
#define IsDBCS2(c)	(((u8)(c) >= _DS1S && (u8)(c) <= _DS1E) || ((u8)(c) >= _DS2S && (u8)(c) <= _DS2E))
#endif

#else			/* SBCS configuration */

#define IsDBCS1(c)	0
#define IsDBCS2(c)	0

#endif /* _DF1S */

/* FAT sub-type boundaries */
/* Note that the FAT spec by Microsoft says 4085 but Windows works with 4087! */
#define MIN_FAT16	4086	/* Minimum number of clusters for FAT16 */
#define	MIN_FAT32	65526	/* Minimum number of clusters for FAT32 */


#define ROOT_DIR    0

/* FAT sub type */
#define FS_FAT12	1
#define FS_FAT16	2
#define FS_FAT32	3
#define FS_EXFAT	4


/* File attribute bits for directory entry */

#define	AM_RDO	0x01	/* Read only */
#define	AM_HID	0x02	/* Hidden */
#define	AM_SYS	0x04	/* System */
#define	AM_VOL	0x08	/* Volume label */
#define AM_LFN	0x0F	/* LFN entry */
#define AM_DIR	0x10	/* Directory */
#define AM_ARC	0x20	/* Archive */
#define AM_FCH	0x80    /* exFAT下,文件簇连续标志 */



/* Offset of FAT structure members */

#define BS_jmpBoot			0
#define BS_OEMName			3
#define BPB_BytsPerSec_l	11
#define BPB_BytsPerSec_h	12
#define BPB_SecPerClus		13
#define BPB_RsvdSecCnt		14
#define BPB_NumFATs			16
#define BPB_RootEntCnt		17
#define BPB_TotSec16		19
#define BPB_Media			21
#define BPB_FATSz16			22
#define BPB_SecPerTrk		24
#define BPB_NumHeads		26
#define BPB_HiddSec			28
#define BPB_TotSec32		32
#define BS_55AA				510

#define BS_DrvNum			36
#define BS_BootSig			38
#define BS_VolID			39
#define BS_VolLab			43
#define BS_FilSysType		54

#define BPB_FATSz32			36
#define BPB_ExtFlags		40
#define BPB_FSVer			42
#define BPB_RootClus		44
#define BPB_FSInfo			48
#define BPB_BkBootSec		50
#define BS_DrvNum32			64
#define BS_BootSig32		66
#define BS_VolID32			67
#define BS_VolLab32			71
#define BS_FilSysType32		82
#define BS_FileSysTypeexFAT				5
#define BPB_FatOffset					80
#define BPB_FatLength					84
#define BPB_ClusterHeapOffset			88
#define BPB_ClusterCount				92
#define BPB_FirstClusterOfRootDirectory	96
#define BPB_VolumeFlags					106
#define BPB_BytesPerSectorShift			108
#define BPB_SectorsPerClusterShift		109
#define BPB_NumberOfFats				110
#define MBR_Table			            446
#define	FSI_LeadSig			0	/* FSI: Leading signature (4) */
#define	FSI_StrucSig		484	/* FSI: Structure signature (4) */
#define	FSI_Free_Count		488	/* FSI: Number of free clusters (4) */
#define	FSI_Nxt_Free		492	/* FSI: Last allocated cluster (4) */

///for FAT12/FAT16/FAT32
#define	DIR_Name			0	/* Short file name (11) */
#define	DIR_Attr			11	/* Attribute (1) */
#define	DIR_NTres			12	/* NT flag (1) */
#define DIR_CrtTimeTenth	13	/* Created time sub-second (1) */
#define	DIR_CrtTime			14	/* Created time (2) */
#define	DIR_CrtDate			16	/* Created date (2) */
#define DIR_LstAccDate		18	/* Last accessed date (2) */
#define	DIR_FstClusHI		20	/* Higher 16-bit of first cluster (2) */
#define	DIR_WrtTime			22	/* Modified time (2) */
#define	DIR_WrtDate			24	/* Modified date (2) */
#define	DIR_FstClusLO		26
#define	DIR_FileSize		28
#define	LDIR_Attr			11	/* LFN attribute (1) */
#define	LDIR_Type			12	/* LFN type (1) */
#define	LDIR_Chksum			13	/* Sum of corresponding SFN entry */
////for exFAT
#define DIR_FileChainFlags	1
#define DIR_NameLen		    3
#define DIR_AttrexFAT		4
#define DIR_FileSizeexFAT   8
#define DIR_FstClustexFAT	20





/*-----------------------------------------------------*/
/* Tiny-FatFs module application interface             */
_banked_func FRESULT  f_seek(FIL _xdata * fp, u8 type, u32 offsize);
_banked_func u16 f_read(FIL _xdata * fp ,u8 _xdata *buff , u16 btr);
_banked_func FRESULT f_readnextdir (FATFS _xdata *fs, DIR *dj, DIRINFO _xdata* dinfo, FS_NAME _xdata *fs_n);
_banked_func void  f_opendir(FATFS _xdata *fs, DIR _xdata * dj) ;
_banked_func FRESULT  f_mount(FATFS _xdata * fs, u32 bootsect, char drive_sel) ;
_banked_func FRESULT f_mount_api(FATFS _xdata *fs, u32 bootsect,char drive_sel);
_banked_func FRESULT f_open(FATFS _xdata *fs, FIL _xdata *fp, char *path, char *lfn_buf, u8 mode);
_banked_func u8 f_tell_status(FIL _xdata * fp);
_banked_func u16 f_write ( FIL *fp,  u8 *buff, u16 btw);
_banked_func FRESULT f_mkdir(FATFS *fs, char *path, u8 mode);
_banked_func FRESULT f_sync_fs(FIL *fp);
_banked_func FRESULT f_sync_file(FIL *fp);
_banked_func FRESULT f_unlink(FIL *fp);


_banked_func bool my_mem_cmp(char *src, char *dst, u8 len);
_banked_func u8 get_powerof2(u8 n);
_banked_func u32 clust2sect (FATFS _xdata *fs,u32 clust);
_banked_func void st_clust (u8 *dir,u32 cl);
_banked_func FRESULT sync_window (SWIN_BUF *win_buf);
_banked_func FRESULT  move_window (u32 sector, SWIN_BUF *win_buf);
_banked_func u32 get_fat (FATFS _xdata *fs, u32 clust, SWIN_BUF *win_buf);
_banked_func u32 get_cluster (FIL _xdata * fp, u32 clust, SWIN_BUF *win_buf);
_banked_func FRESULT put_fat (FATFS *fs,u32 clst,u32 val,SWIN_BUF *win_buf);
_banked_func u32 ld_clust (FATFS *fs,u8 *dir);
_banked_func bool dir_sdi (FATFS *fs,DIR *dj,u16 idx);
_banked_func FRESULT dir_alloc (FATFS *fs,DIR* dj,u8 nent);
_banked_func FRESULT dir_register (FATFS *fs,DIR *dj,char *fn);
_banked_func FRESULT sync_fs (FATFS *fs);
_banked_func u8 create_name (char *sfn,const char *path);
_banked_func FRESULT dir_find (FATFS *fs,DIR *dj,DIRINFO *dir_info,char *sfn,FS_NAME *lfn);
_banked_func FRESULT follow_path (FATFS *fs,DIR *dj,char *sfn,DIRINFO *dinfo,char *path,FS_NAME *lfn);
_banked_func FRESULT remove_chain (FATFS *fs,u32 clst);
_banked_func u32 create_chain (FATFS *fs,u32 clst,SWIN_BUF *win);
_banked_func FRESULT check_fs (FATFS _xdata * fs, u32 sec);
_banked_func void get_dir_info (const u8 _xdata *dir,DIRINFO _xdata *dinfo);
_banked_func bool dir_next (FATFS _xdata * fs, DIR _xdata *dj, bool stretch);
_banked_func FRESULT f_open_dir(FATFS _xdata *fs, DIR *dj, char *path);

tu16 ld_word_func(u8 *p);
u32 ld_dword_func(u8 *p);
void st_word_func(u8 *ptr,u16 val);
void st_dword_func(u8 *ptr,u32 val);

bool decode_lfn(char _xdata *p,char _xdata *sou_p,u16 max_copy);
int find_word_pos(u8 *buf, u16 buf_len, u16 key, u8 find_type);
void fs_lfn_deal(FATFS _xdata *f_s, FIL *f_p);
#endif /* _FATFS */
