// Image shown for 15 seconds, with about half a second transition time, audio is played first then image is shown.

static int intro(int fade, u32 kDown, bool introFlag, C2D_Sprite* sprites) {
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
	C2D_DrawSprite(&sprites[0]);

	// Decrement fade counter (Fade in)
	if((fade > 7) && (introFlag == true)) {
		fade = fade - 8;
	} else if(introFlag == true) {
		fade = 0;
	}

	// Increment fade counter (Fade out)
	if((fade < 248) && (introFlag == false)) {
		fade = fade + 8;
	} else if (introFlag == false) {
		fade = 255;
	}

	printf("\x1b[8;1H%d", fade);

	return fade;
}