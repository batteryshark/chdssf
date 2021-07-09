#ifndef __CHD_HELPER
#define __CHD_HELPER

#define CD_MAX_TRACKS		(99)	/* AFAIK the theoretical limit */
#define CD_MAX_SECTOR_DATA	(2352)
#define CD_MAX_SUBCODE_DATA	(96)

#define CD_FRAME_SIZE		(CD_MAX_SECTOR_DATA + CD_MAX_SUBCODE_DATA)
#define CD_FRAMES_PER_HUNK	(4)

#define CD_METADATA_WORDS	(1+(CD_MAX_TRACKS * 6))
enum
{
	CD_TRACK_MODE1 = 0, 	/* mode 1 2048 bytes/sector */
	CD_TRACK_MODE1_RAW,	/* mode 1 2352 bytes/sector */
	CD_TRACK_MODE2,		/* mode 2 2336 bytes/sector */
	CD_TRACK_MODE2_FORM1,	/* mode 2 2048 bytes/sector */
	CD_TRACK_MODE2_FORM2,	/* mode 2 2324 bytes/sector */
	CD_TRACK_MODE2_FORM_MIX, /* mode 2 2336 bytes/sector */
	CD_TRACK_MODE2_RAW,	/* mode 2 2352 bytes / sector */
	CD_TRACK_AUDIO		/* redbook audio track 2352 bytes/sector (588 samples) */
};

enum
{
	CD_SUB_NORMAL = 0, 	/* "cooked" 96 bytes per sector */
	CD_SUB_RAW,		/* raw uninterleaved 96 bytes per sector */
	CD_SUB_NONE		/* no subcode data stored */
};



/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

typedef struct _cdrom_file cdrom_file;


typedef struct _cdrom_track_info cdrom_track_info;
struct _cdrom_track_info
{
	/* fields used by CHDMAN and in MAME */
	unsigned int trktype;     /* track type */
	unsigned int subtype;     /* subcode data type */
	unsigned int datasize;    /* size of data in each sector of this track */
	unsigned int subsize;     /* size of subchannel data in each sector of this track */
	unsigned int frames;      /* number of frames in this track */
	unsigned int extraframes; /* number of "spillage" frames in this track */
	unsigned int pregap;      /* number of pregap frames */
	unsigned int postgap;     /* number of postgap frames */
	unsigned int pgtype;      /* type of sectors in pregap */
	unsigned int pgsub;       /* type of subchannel data in pregap */
	unsigned int pgdatasize;  /* size of data in each sector of the pregap */
	unsigned int pgsubsize;   /* size of subchannel data in each sector of the pregap */

	/* fields used in CHDMAN only */
	unsigned int padframes;   /* number of frames of padding to add to the end of the track; needed for GDI */

	/* fields used in MAME/MESS only */
	unsigned int logframeofs; /* logical frame of actual track data - offset by pregap size if pregap not physically present */
	unsigned int physframeofs; /* physical frame of actual track data in CHD data */
	unsigned int chdframeofs; /* frame number this track starts at on the CHD */
};


typedef struct _cdrom_track_input_info cdrom_track_input_info;
struct _cdrom_track_input_info	/* used only at compression time */
{
	char fname[CD_MAX_TRACKS][256];	/* filename for each track */
	UINT32 offset[CD_MAX_TRACKS];	/* offset in the data file for each track */
};


typedef struct _cdrom_toc cdrom_toc;
struct _cdrom_toc
{
	UINT32 numtrks;		/* number of tracks */
	cdrom_track_info tracks[CD_MAX_TRACKS];
};

static struct _cdrom_file
{
	int *			chd;				/* CHD file */
	cdrom_toc			cdtoc;				/* TOC for the CD */
	unsigned int				hunksectors;		/* sectors per hunk */
	unsigned int				cachehunk;			/* which hunk is cached */
	unsigned char *				cache;				/* cache of the current hunk */
}cdrf;

#endif