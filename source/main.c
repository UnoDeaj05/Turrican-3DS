#include <citro2d.h>
#include <opusfile.h>
#include <3ds.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "intro.h"
#include "titleScreen.h"

#define MAX_SPRITES   768
#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240

// Sprite struct
typedef struct
{
	C2D_Sprite spr;
} Sprite;

// Sprite definitions
static C2D_SpriteSheet spriteSheet;
static Sprite sprites[MAX_SPRITES];
static size_t numSprites = 1;

// Sound definitions
static const int SAMPLE_RATE = 48000; // Opus is fixed at 48kHz
static const int SAMPLES_PER_BUF = SAMPLE_RATE * 120 / 1000; // 120ms buffer
static const int CHANNELS_PER_SAMPLE = 2;
static const int THREAD_AFFINITY = -1;           // Execute thread on any core
static const int THREAD_STACK_SZ = 32 * 1024;    // 32kB stack for audio thread
static const size_t WAVEBUF_SIZE = SAMPLES_PER_BUF * CHANNELS_PER_SAMPLE * sizeof(int16_t); // Size of NDSP wavebufs

// Intro definitions
int fade = 255;
bool introFlag = true;

//---------------------------------------------------------------------------------
static void initSprites(int begin, int end) {
//---------------------------------------------------------------------------------
	for(int i = begin; i <= end; i++) {
		Sprite* sprite = &sprites[i];

		// Set intro image, centre and position
		C2D_SpriteFromSheet(&sprite->spr, spriteSheet, i);
		C2D_SpriteSetCenter(&sprite->spr, 0.0f, 0.0f);
		C2D_SpriteSetPos(&sprite->spr, 52, 0);
	}
}

void debug() {
	printf("\x1b[1;1HSprites: %zu/%u\x1b[K", numSprites, MAX_SPRITES);
	printf("\x1b[2;1HCPU:     %6.2f%%\x1b[K", C3D_GetProcessingTime()*6.0f);
	printf("\x1b[3;1HGPU:     %6.2f%%\x1b[K", C3D_GetDrawingTime()*6.0f);
	printf("\x1b[4;1HCmdBuf:  %6.2f%%\x1b[K", C3D_GetCmdBufUsage()*100.0f);
}

//---------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
//---------------------------------------------------------------------------------
	// Init libs
	romfsInit();
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	consoleInit(GFX_BOTTOM, NULL);

	// Create screens
	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

	// Load graphics
	spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	if (!spriteSheet) svcBreak(USERBREAK_PANIC);

	// Initialize intro sprites
	initSprites(0, 1);

	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();

		// Respond to user input
		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();

		debug();

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f));
		C2D_SceneBegin(top);
		fade = intro(fade, kDown, introFlag, &sprites[0]);
		if((kDown & KEY_A) && (fade == 0)) {
			introFlag = false;
		}
		if((introFlag == false) && (fade == 255)) {
			C2D_Fade(C2D_Color32(0, 0, 0, 0));
			C2D_DrawSprite(&sprites[1]);
		}

		C3D_FrameEnd(0);
	}

	// Delete graphics
	C2D_SpriteSheetFree(spriteSheet);

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();
	return 0;
}
