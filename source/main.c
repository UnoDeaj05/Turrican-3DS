// Simple citro2d sprite drawing example
// Images borrowed from:
//   https://kenney.nl/assets/space-shooter-redux
#include <citro2d.h>
#include <3ds.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SPRITES   768
#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240

// Simple sprite struct
typedef struct
{
	C2D_Sprite spr;
} Sprite;

static C2D_SpriteSheet spriteSheet;
static Sprite sprites[MAX_SPRITES];
static size_t numSprites = 1;

//---------------------------------------------------------------------------------
static void initSprites() {
//---------------------------------------------------------------------------------
	Sprite* introSprite = &sprites[0];

	// Set intro image, centre and position
	C2D_SpriteFromSheet(&introSprite->spr, spriteSheet, 0);
	C2D_SpriteSetCenter(&introSprite->spr, 0.0f, 0.0f);
	C2D_SpriteSetPos(&introSprite->spr, 52, 0);
}

static int intro(int fade, u32 kDown, bool introFlag) {
	/*int mstime = 15000;
	clock_t start_time = clock();
			int i = 0;

	while(clock() > mstime) {
		if (kDown & KEY_UP) {
			i++;
			printf("\x1b[9;1H%d", i);}
			//break;
	}*/

	C2D_Fade(C2D_Color32(0, 0, 0, fade)); //255 blank, 0 visible
	C2D_DrawSprite(&sprites[0].spr);

	//Decrement fade counter
	if((fade > 0) && (introFlag == true)) {
		fade--;
	}

	if((fade < 255) && (introFlag == false)) {
		fade++;
	}

	printf("\x1b[8;1H%d", fade);

	return fade;
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

	// Initialize sprites
	initSprites();

	int fade = 255;
	bool introFlag = true;

	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();

		// Respond to user input
		u32 kDown = hidKeysDown();
		
		// Image shown for 15 seconds, with about half a second transition time, audio is played first then image is shown.

		printf("\x1b[1;1HSprites: %zu/%u\x1b[K", numSprites, MAX_SPRITES);
		printf("\x1b[2;1HCPU:     %6.2f%%\x1b[K", C3D_GetProcessingTime()*6.0f);
		printf("\x1b[3;1HGPU:     %6.2f%%\x1b[K", C3D_GetDrawingTime()*6.0f);
		printf("\x1b[4;1HCmdBuf:  %6.2f%%\x1b[K", C3D_GetCmdBufUsage()*100.0f);
		

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f));
		C2D_SceneBegin(top);
		fade = intro(fade, kDown, introFlag);
		if((kDown & KEY_A) && (fade == 0)) {
			introFlag = false;
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
