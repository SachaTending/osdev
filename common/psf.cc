// Because in limine v5.0 the terminal feature is removed, no limine's terminal(
#include "stdint.h"
#include "logger.h"
#include "pmm.h"
#include "limine_int.h"

logger psf("PSF(Font format, ok?)");

#define PSF1_FONT_MAGIC 0x0434

typedef struct {
    uint16_t magic; // Magic bytes for idnetiifcation.
    uint8_t fontMode; // PSF font mode
    uint8_t characterSize; // PSF character size.
} PSF1_Header;


#define PSF_FONT_MAGIC 0x864ab572

typedef struct {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
} PSF_font;

// I will use premade font for... initrd cannot find file XD

extern char _binary_font_psf_start;
extern char _binary_font_psf_end;

uint16_t *unicode;

void psf_init() {
    uint16_t glyph;
    PSF_font *font = (PSF_font *)&_binary_font_psf_start;
    psf.log("Width: %u\n", font->width);
    psf.log("Height: %u\n", font->height);
    psf.log("Characters: %u\n", font->numglyph);
    psf.log("Flags: %s\n", font->flags?"non unicode":"unicode");
    if (font->flags) {
        unicode = NULL;
        return;
    }
    char *s = (char *)(
        (unsigned char *)&_binary_font_psf_start +
        font->headersize +
        font->numglyph * font->bytesperglyph
    );
    unicode = (uint16_t *)calloc(USHRT_MAX, 2);
    psf.log("Translating unicode to... uhh, maybe 437 encoding?\n");
    while((char)s>_binary_font_psf_end) {
        uint16_t uc = (uint16_t)((unsigned char *)s[0]);
        if(uc == 0xFF) {
            glyph++;
            s++;
            continue;
        } else if(uc & 128) {
            /* UTF-8 to unicode */
            if((uc & 32) == 0 ) {
                uc = ((s[0] & 0x1F)<<6)+(s[1] & 0x3F);
                s++;
            } else
            if((uc & 16) == 0 ) {
                uc = ((((s[0] & 0xF)<<6)+(s[1] & 0x3F))<<6)+(s[2] & 0x3F);
                s+=2;
            } else
            if((uc & 8) == 0 ) {
                uc = ((((((s[0] & 0x7)<<6)+(s[1] & 0x3F))<<6)+(s[2] & 0x3F))<<6)+(s[3] & 0x3F);
                s+=3;
            } else
                uc = 0;
        }
        /* save translation */
        unicode[uc] = glyph;
        s++;
    }
}
