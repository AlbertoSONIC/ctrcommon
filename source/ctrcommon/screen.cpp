#include <sys/unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include <3ds.h>

#include "ctrcommon/common.hpp"

static unsigned char asciiData[128][8] = {
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x3E, 0x41, 0x55, 0x41, 0x55, 0x49, 0x3E },
		{ 0x00, 0x3E, 0x7F, 0x6B, 0x7F, 0x6B, 0x77, 0x3E },
		{ 0x00, 0x22, 0x77, 0x7F, 0x7F, 0x3E, 0x1C, 0x08 },
		{ 0x00, 0x08, 0x1C, 0x3E, 0x7F, 0x3E, 0x1C, 0x08 },
		{ 0x00, 0x08, 0x1C, 0x2A, 0x7F, 0x2A, 0x08, 0x1C },
		{ 0x00, 0x08, 0x1C, 0x3E, 0x7F, 0x3E, 0x08, 0x1C },
		{ 0x00, 0x00, 0x1C, 0x3E, 0x3E, 0x3E, 0x1C, 0x00 },
		{ 0xFF, 0xFF, 0xE3, 0xC1, 0xC1, 0xC1, 0xE3, 0xFF },
		{ 0x00, 0x00, 0x1C, 0x22, 0x22, 0x22, 0x1C, 0x00 },
		{ 0xFF, 0xFF, 0xE3, 0xDD, 0xDD, 0xDD, 0xE3, 0xFF },
		{ 0x00, 0x0F, 0x03, 0x05, 0x39, 0x48, 0x48, 0x30 },
		{ 0x00, 0x08, 0x3E, 0x08, 0x1C, 0x22, 0x22, 0x1C },
		{ 0x00, 0x18, 0x14, 0x10, 0x10, 0x30, 0x70, 0x60 },
		{ 0x00, 0x0F, 0x19, 0x11, 0x13, 0x37, 0x76, 0x60 },
		{ 0x00, 0x08, 0x2A, 0x1C, 0x77, 0x1C, 0x2A, 0x08 },
		{ 0x00, 0x60, 0x78, 0x7E, 0x7F, 0x7E, 0x78, 0x60 },
		{ 0x00, 0x03, 0x0F, 0x3F, 0x7F, 0x3F, 0x0F, 0x03 },
		{ 0x00, 0x08, 0x1C, 0x2A, 0x08, 0x2A, 0x1C, 0x08 },
		{ 0x00, 0x66, 0x66, 0x66, 0x66, 0x00, 0x66, 0x66 },
		{ 0x00, 0x3F, 0x65, 0x65, 0x3D, 0x05, 0x05, 0x05 },
		{ 0x00, 0x0C, 0x32, 0x48, 0x24, 0x12, 0x4C, 0x30 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x7F, 0x7F },
		{ 0x00, 0x08, 0x1C, 0x2A, 0x08, 0x2A, 0x1C, 0x3E },
		{ 0x00, 0x08, 0x1C, 0x3E, 0x7F, 0x1C, 0x1C, 0x1C },
		{ 0x00, 0x1C, 0x1C, 0x1C, 0x7F, 0x3E, 0x1C, 0x08 },
		{ 0x00, 0x08, 0x0C, 0x7E, 0x7F, 0x7E, 0x0C, 0x08 },
		{ 0x00, 0x08, 0x18, 0x3F, 0x7F, 0x3F, 0x18, 0x08 },
		{ 0x00, 0x00, 0x00, 0x70, 0x70, 0x70, 0x7F, 0x7F },
		{ 0x00, 0x00, 0x14, 0x22, 0x7F, 0x22, 0x14, 0x00 },
		{ 0x00, 0x08, 0x1C, 0x1C, 0x3E, 0x3E, 0x7F, 0x7F },
		{ 0x00, 0x7F, 0x7F, 0x3E, 0x3E, 0x1C, 0x1C, 0x08 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18 },
		{ 0x00, 0x36, 0x36, 0x14, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36 },
		{ 0x00, 0x08, 0x1E, 0x20, 0x1C, 0x02, 0x3C, 0x08 },
		{ 0x00, 0x60, 0x66, 0x0C, 0x18, 0x30, 0x66, 0x06 },
		{ 0x00, 0x3C, 0x66, 0x3C, 0x28, 0x65, 0x66, 0x3F },
		{ 0x00, 0x18, 0x18, 0x18, 0x30, 0x00, 0x00, 0x00 },
		{ 0x00, 0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06 },
		{ 0x00, 0x60, 0x30, 0x18, 0x18, 0x18, 0x30, 0x60 },
		{ 0x00, 0x00, 0x36, 0x1C, 0x7F, 0x1C, 0x36, 0x00 },
		{ 0x00, 0x00, 0x08, 0x08, 0x3E, 0x08, 0x08, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x60 },
		{ 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x60 },
		{ 0x00, 0x00, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x00 },
		{ 0x00, 0x3C, 0x66, 0x6E, 0x76, 0x66, 0x66, 0x3C },
		{ 0x00, 0x18, 0x18, 0x38, 0x18, 0x18, 0x18, 0x7E },
		{ 0x00, 0x3C, 0x66, 0x06, 0x0C, 0x30, 0x60, 0x7E },
		{ 0x00, 0x3C, 0x66, 0x06, 0x1C, 0x06, 0x66, 0x3C },
		{ 0x00, 0x0C, 0x1C, 0x2C, 0x4C, 0x7E, 0x0C, 0x0C },
		{ 0x00, 0x7E, 0x60, 0x7C, 0x06, 0x06, 0x66, 0x3C },
		{ 0x00, 0x3C, 0x66, 0x60, 0x7C, 0x66, 0x66, 0x3C },
		{ 0x00, 0x7E, 0x66, 0x0C, 0x0C, 0x18, 0x18, 0x18 },
		{ 0x00, 0x3C, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x3C },
		{ 0x00, 0x3C, 0x66, 0x66, 0x3E, 0x06, 0x66, 0x3C },
		{ 0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00 },
		{ 0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x30 },
		{ 0x00, 0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06 },
		{ 0x00, 0x00, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x00 },
		{ 0x00, 0x60, 0x30, 0x18, 0x0C, 0x18, 0x30, 0x60 },
		{ 0x00, 0x3C, 0x66, 0x06, 0x1C, 0x18, 0x00, 0x18 },
		{ 0x00, 0x38, 0x44, 0x5C, 0x58, 0x42, 0x3C, 0x00 },
		{ 0x00, 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66 },
		{ 0x00, 0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x7C },
		{ 0x00, 0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C },
		{ 0x00, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7C },
		{ 0x00, 0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x7E },
		{ 0x00, 0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x60 },
		{ 0x00, 0x3C, 0x66, 0x60, 0x60, 0x6E, 0x66, 0x3C },
		{ 0x00, 0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66 },
		{ 0x00, 0x3C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C },
		{ 0x00, 0x1E, 0x0C, 0x0C, 0x0C, 0x6C, 0x6C, 0x38 },
		{ 0x00, 0x66, 0x6C, 0x78, 0x70, 0x78, 0x6C, 0x66 },
		{ 0x00, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E },
		{ 0x00, 0x63, 0x77, 0x7F, 0x6B, 0x63, 0x63, 0x63 },
		{ 0x00, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x63, 0x63 },
		{ 0x00, 0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C },
		{ 0x00, 0x7C, 0x66, 0x66, 0x66, 0x7C, 0x60, 0x60 },
		{ 0x00, 0x3C, 0x66, 0x66, 0x66, 0x6E, 0x3C, 0x06 },
		{ 0x00, 0x7C, 0x66, 0x66, 0x7C, 0x78, 0x6C, 0x66 },
		{ 0x00, 0x3C, 0x66, 0x60, 0x3C, 0x06, 0x66, 0x3C },
		{ 0x00, 0x7E, 0x5A, 0x18, 0x18, 0x18, 0x18, 0x18 },
		{ 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3E },
		{ 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18 },
		{ 0x00, 0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63 },
		{ 0x00, 0x63, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x63 },
		{ 0x00, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18 },
		{ 0x00, 0x7E, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x7E },
		{ 0x00, 0x1E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1E },
		{ 0x00, 0x00, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x00 },
		{ 0x00, 0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78 },
		{ 0x00, 0x08, 0x14, 0x22, 0x41, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F },
		{ 0x00, 0x0C, 0x0C, 0x06, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x3C, 0x06, 0x3E, 0x66, 0x3E },
		{ 0x00, 0x60, 0x60, 0x60, 0x7C, 0x66, 0x66, 0x7C },
		{ 0x00, 0x00, 0x00, 0x3C, 0x66, 0x60, 0x66, 0x3C },
		{ 0x00, 0x06, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3E },
		{ 0x00, 0x00, 0x00, 0x3C, 0x66, 0x7E, 0x60, 0x3C },
		{ 0x00, 0x1C, 0x36, 0x30, 0x30, 0x7C, 0x30, 0x30 },
		{ 0x00, 0x00, 0x3E, 0x66, 0x66, 0x3E, 0x06, 0x3C },
		{ 0x00, 0x60, 0x60, 0x60, 0x7C, 0x66, 0x66, 0x66 },
		{ 0x00, 0x00, 0x18, 0x00, 0x18, 0x18, 0x18, 0x3C },
		{ 0x00, 0x0C, 0x00, 0x0C, 0x0C, 0x6C, 0x6C, 0x38 },
		{ 0x00, 0x60, 0x60, 0x66, 0x6C, 0x78, 0x6C, 0x66 },
		{ 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18 },
		{ 0x00, 0x00, 0x00, 0x63, 0x77, 0x7F, 0x6B, 0x6B },
		{ 0x00, 0x00, 0x00, 0x7C, 0x7E, 0x66, 0x66, 0x66 },
		{ 0x00, 0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x3C },
		{ 0x00, 0x00, 0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60 },
		{ 0x00, 0x00, 0x3C, 0x6C, 0x6C, 0x3C, 0x0D, 0x0F },
		{ 0x00, 0x00, 0x00, 0x7C, 0x66, 0x66, 0x60, 0x60 },
		{ 0x00, 0x00, 0x00, 0x3E, 0x40, 0x3C, 0x02, 0x7C },
		{ 0x00, 0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x18 },
		{ 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3E },
		{ 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x3C, 0x18 },
		{ 0x00, 0x00, 0x00, 0x63, 0x6B, 0x6B, 0x6B, 0x3E },
		{ 0x00, 0x00, 0x00, 0x66, 0x3C, 0x18, 0x3C, 0x66 },
		{ 0x00, 0x00, 0x00, 0x66, 0x66, 0x3E, 0x06, 0x3C },
		{ 0x00, 0x00, 0x00, 0x3C, 0x0C, 0x18, 0x30, 0x3C },
		{ 0x00, 0x0E, 0x18, 0x18, 0x30, 0x18, 0x18, 0x0E },
		{ 0x00, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18 },
		{ 0x00, 0x70, 0x18, 0x18, 0x0C, 0x18, 0x18, 0x70 },
		{ 0x00, 0x00, 0x00, 0x3A, 0x6C, 0x00, 0x00, 0x00 },
		{ 0x00, 0x08, 0x1C, 0x36, 0x63, 0x41, 0x41, 0x7F }
};

u8* fb = NULL;
u16 fbWidth = 0;
u16 fbHeight = 0;

bool screen_begin_draw(Screen screen) {
	if(fb != NULL) {
		return false;
	}

	fb = gfxGetFramebuffer(screen == TOP_SCREEN ? GFX_TOP : GFX_BOTTOM, GFX_LEFT, &fbWidth, &fbHeight);
	return true;
}

bool screen_end_draw() {
	if(fb == NULL) {
		return false;
	}

	fb = NULL;
	fbWidth = 0;
	fbHeight = 0;
	return true;
}

void screen_swap_buffers_quick() {
	gfxFlushBuffers();
	gfxSwapBuffers();
}

void screen_swap_buffers() {
	gfxFlushBuffers();
	gspWaitForVBlank();
	gfxSwapBuffers();
}

u16 screen_get_width() {
	// Use fbHeight since the framebuffer is rotated 90 degrees to the left.
	return fbHeight;
}

u16 screen_get_height() {
	// Use fbWidth since the framebuffer is rotated 90 degrees to the left.
	return fbWidth;
}

u32 screen_get_index(int x, int y) {
	u16 height = screen_get_height();
	// Reverse the y coordinate when finding the index.
	// This is done as the framebuffer is rotated 90 degrees to the left.
	return (u32) (((height - y) + x * height) * 3);
}

bool screen_read_pixels(u8* dest, int srcX, int srcY, int dstX, int dstY, u16 width, u16 height) {
	if(fb == NULL || srcX + width < 0 || srcY + height < 0 || srcX >= screen_get_width() || srcY >= screen_get_height()) {
		return false;
	}

	u16 copyWidth = width;
	u16 copyHeight = height;
	if(srcX < 0) {
		dstX -= srcX;
		copyWidth += srcX;
		srcX = 0;
	}

	if(srcY < 0) {
		dstY -= srcY;
		copyHeight += srcY;
		srcY = 0;
	}

	if(srcX + copyWidth > screen_get_width()) {
		copyWidth = (u16) (screen_get_width() - srcX);
	}

	if(srcY + copyHeight > screen_get_height()) {
		copyHeight = (u16) (screen_get_height() - srcY);
	}

	for(int cx = 0; cx < copyWidth; cx++) {
		for(int cy = 0; cy < copyHeight; cy++) {
			u8* src = fb + screen_get_index(srcX + cx, srcY + cy);
			u8* dst = dest + ((dstY + cy) * width + (dstX + cx));
			dst[0] = src[0];
			dst[1] = src[1];
			dst[2] = src[2];
		}
	}

	return true;
}

bool screen_take_screenshot() {
	u16 imgWidth = 400;
	u16 imgHeight = 480;
	u16 imgBytesPerPixel = 3;
	u16 imgHeaderSize = 0x36;
	u32 imgDataSize = imgWidth * imgHeight * imgBytesPerPixel;

	u8 temp[imgHeaderSize + imgDataSize];
	memset(temp, 0, imgHeaderSize + imgDataSize);

	*(u16*) &temp[0x0] = 0x4D42;
	*(u32*) &temp[0x2] = imgHeaderSize + imgDataSize;
	*(u32*) &temp[0xA] = imgHeaderSize;
	*(u32*) &temp[0xE] = 0x28;
	*(u32*) &temp[0x12] = imgWidth;
	*(u32*) &temp[0x16] = imgHeight;
	*(u32*) &temp[0x1A] = 0x00180001;
	*(u32*) &temp[0x22] = imgDataSize;

	screen_begin_draw(TOP_SCREEN);
	u16 topWidth = screen_get_width();
	u16 topHeight = screen_get_height();
	screen_read_pixels(temp + imgHeaderSize, 0, 0, 0, 0, topWidth, topHeight);
	screen_end_draw();

	screen_begin_draw(BOTTOM_SCREEN);
	screen_read_pixels(temp + imgHeaderSize, 0, 0, (imgWidth - screen_get_width()) / 2, topHeight, screen_get_width(), screen_get_height());
	screen_end_draw();

	char file[256];
	snprintf(file, 256, "sdmc:/screenshot_%08d.bmp", (int) (svcGetSystemTick() / 446872));
	FILE* fd = fopen(file, "wb");
	if(!fd) {
		return false;
	}

	fwrite(temp, 1, imgHeaderSize + imgDataSize, fd);
	fclose(fd);
	return true;
}

bool screen_draw(int x, int y, u8 r, u8 g, u8 b) {
	if(fb == NULL || x < 0 || y < 0 || x >= screen_get_width() || y >= screen_get_height()) {
		return false;
	}

	u32 idx = screen_get_index(x, y);
	fb[idx + 0] = b;
	fb[idx + 1] = g;
	fb[idx + 2] = r;
	return true;
}

bool screen_fill(int x, int y, u16 width, u16 height, u8 r, u8 g, u8 b) {
	if(fb == NULL) {
		return false;
	}

	u16 swidth = screen_get_width();
	u16 sheight = screen_get_height();
	if(x + width < 0 || y + height < 0 || x >= swidth || y >= sheight) {
		return false;
	}

	if(x < 0) {
		width += x;
		x = 0;
	}

	if(y < 0) {
		height += y;
		y = 0;
	}

	if(x + width >= swidth){
		width = (u16) (swidth - x);
	}

	if(y + height >= sheight){
		height = (u16) (sheight - y);
	}

	u8 colorLine[height * 3];
	for(int ly = 0; ly < height; ly++) {
		colorLine[ly * 3 + 0] = b;
		colorLine[ly * 3 + 1] = g;
		colorLine[ly * 3 + 2] = r;
	}

	u8* fbAddr = fb + screen_get_index(x, y) - (height * 3);
	for(int dx = 0; dx < width; dx++) {
		memcpy(fbAddr, colorLine, (size_t) (height * 3));
		fbAddr += sheight * 3;
	}

	return true;
}

bool screen_clear(u8 r, u8 g, u8 b) {
	return screen_fill(0, 0, screen_get_width(), screen_get_height(), r, g, b);
}

u16 screen_get_str_width(const std::string str) {
	u32 len = str.length();
	if(len == 0) {
		return 0;
	}

	u32 longestLine = 0;
	u32 currLength = 0;
	for(u32 i = 0; i < len; i++) {
		if(str[i] == '\n') {
			if(currLength > longestLine) {
				longestLine = currLength;
			}

			currLength = 0;
			continue;
		}

		currLength++;
	}

	if(currLength > longestLine) {
		longestLine = currLength;
	}

	return (u16) (longestLine * 8);
}

u16 screen_get_str_height(const std::string str) {
	u32 len = str.length();
	if(len == 0) {
		return 0;
	}

	u32 lines = 1;
	for(u32 i = 0; i < len; i++) {
		if(str[i] == '\n') {
			lines++;
		}
	}

	return (u16) (lines * 8);
}

bool screen_draw_char(char c, int x, int y, u8 r, u8 g, u8 b) {
	if(fb == NULL) {
		return false;
	}

	bool drawn = false;
	unsigned char* data = asciiData[(int) c];
	for(int cy = 0; cy < 8; cy++) {
		unsigned char l = data[cy];
		for(int cx = 0; cx < 8; cx++) {
			if((0b10000000 >> cx) & l) {
				drawn = true;
				screen_draw(x + cx, y + cy, r, g, b);
			}
		}
	}

	return drawn;
}

bool screen_draw_string(const std::string str, int x, int y, u8 r, u8 g, u8 b) {
	if(fb == NULL) {
		return false;
	}

	int len = str.length();
	int cx = x;
	int cy = y;
	for(int i = 0; i < len; i++) {
		char c = str[i];
		if(c == '\n') {
			cx = x;
			cy += 8;
			continue;
		}

		screen_draw_char(c, cx, cy, r, g, b);
		cx += 8;
	}

	return true;
}