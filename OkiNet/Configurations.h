#pragma once

// Common header for the game configurations: resolution, input, etc

namespace configs
{
	// Aspect ratio of the window
	unsigned int WindowWidth = 1280;
	unsigned int WindowHeight = 720;

	const char* WindowName = "OkiNet";

	unsigned int FrameRateCap = 60;

	const int PIXEL_SCALE_FACTOR = 4; //Scale factor applied to the sprites to upscale them
	const double PI = 3.14159265; //just pi
	
}