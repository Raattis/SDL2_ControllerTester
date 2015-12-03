// ControllerTest.cpp : Defines the entry point for the console application.
//

#include "SDL.h"
#include <cstdio>
#include <stdarg.h>
#include <iostream>

#if _MSC_VER == 1900
// Fixes a linker error in VS2015 with an SDL 2.0.3 Dll/Lib built with VS2013
extern "C" { FILE __iob_func[3] = { *stdin,*stdout,*stderr }; }
extern "C" int __cdecl __imp_fprintf(FILE *stream, const char *format, ...)
{
	va_list arguments;
	return vfprintf(stream, format, arguments);
}
#endif

using namespace std;

int main(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_Window *win = SDL_CreateWindow("SDL2 ControllerTester, Raattis, 2015", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
	if (win == nullptr) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	cout << "Press C to change to GameController mode." << endl;

	SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	SDL_Surface* surf = SDL_GetWindowSurface(win);
	SDL_Event event;
	

	SDL_Joystick* sticks[32] = { NULL };
	
	SDL_JoystickEventState(SDL_ENABLE);
	for (int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		cout << "Opening joystick nro: " << i << endl;
		sticks[i] = SDL_JoystickOpen(i);
	}

	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	float axes[16] = {0.0f};
	bool buttons[30] = { false };
	Uint8 hats[8] = {0};


	bool doControllersNext = false;
	bool ending = false;
	while (!ending)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				ending = true;
			}
			else if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					ending = true;
				}
				else if (event.key.keysym.sym == SDLK_c)
				{
					ending = true;
					doControllersNext = true;
				}
			}

			if (event.type == SDL_JOYAXISMOTION)
			{
				cout << "Axis #" << int(event.jaxis.axis) << ", val: " << event.jaxis.value << endl;
				axes[event.jaxis.axis] = event.jaxis.value;
			}
			else if (event.type == SDL_JOYHATMOTION)
			{
				cout << "Hat #" << int(event.jhat.hat) << ", val: " << int(event.jhat.value) << endl;
				hats[event.jhat.hat] = event.jhat.value;
			}
			else if (event.type == SDL_JOYBUTTONDOWN)
			{
				cout << "Button #" << int(event.jbutton.button) << " press" << endl;
				buttons[int(event.jbutton.button)] = true;
			}
			else if (event.type == SDL_JOYBUTTONUP)
			{
				cout << "Button #" << int(event.jbutton.button) << " release" << endl;
				buttons[int(event.jbutton.button)] = false;
			}
			else if (event.type == SDL_JOYDEVICEADDED || event.type == SDL_JOYDEVICEREMOVED)
			{
				cout << "Joysticks added or removed." << endl;
				for (int i = 0; i < 32; ++i)
				{
					if (sticks[i])
					{
						cout << "Closing joystick #" << i << endl;
						SDL_JoystickClose(sticks[i]);
						sticks[i] = NULL;
					}
				}
				for (int i = 0; i < SDL_NumJoysticks(); ++i)
				{
					cout << "Opening joystick #" << i << endl;
					sticks[i] = SDL_JoystickOpen(i);
				}
			}
			else
			{
				continue;
			}
			cout << "event_type: " << event.type << endl;
		}

		SDL_RenderClear(renderer);

		SDL_FillRect(surf, NULL, 0xffffffff);

		SDL_Rect rect = SDL_Rect();
		for (int i = 0; i < 16; ++i)
		{
			rect.x = i * 20 + 10;
			rect.y = 240;
			rect.h = int(axes[i] * 230 / 33000);
			if (rect.h < 0)
			{
				rect.h = -rect.h;
				rect.y = 240 - rect.h;
			}
			rect.w = 10;
			SDL_FillRect(surf, &rect, 0xff0000);
		}
		for (int i = 0; i < 30; ++i)
		{
			rect.x = i % 5 * 15 + 16 * 20 + 10;
			rect.y = 10 + 15 * (i / 5);
			rect.h = 10;
			rect.w = 10;
			SDL_FillRect(surf, &rect, buttons[i] ? 0x00ff00 : 0x0f0f0f);
		}
		for (int i = 0; i < 8; ++i)
		{
			rect.x = (i % 3) * 40 + 16 * 20 + 10;
			rect.y = 120 + 40 * (i / 3);
			rect.h = 5 * (1 + (hats[i]) % 8);
			rect.w = 5 * (1 + (hats[i]) / 8);
			SDL_FillRect(surf, &rect, 0xff00ff);
		}

		SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
		SDL_RenderCopy(renderer, tex, NULL, NULL);
		SDL_RenderPresent(renderer);
		SDL_DestroyTexture(tex);
	}

	for (int i = 0; i < 32; ++i)
	{
		if (sticks[i])
		{
			cout << "Closing joystick #" << i << endl;
			SDL_JoystickClose(sticks[i]);
			sticks[i] = NULL;
		}
	}
	//SDL_JoystickEventState(SDL_IGNORE);
	
	if (doControllersNext)
	{
		// Controllers begin

		cout << "Switching to GameController mode." << endl;

		SDL_GameController* ctrls[32] = { NULL };
		SDL_GameControllerEventState(SDL_ENABLE);

		bool openedSomething = false;
		for (int i = 0; i < SDL_NumJoysticks(); ++i)
		{
			if (SDL_IsGameController(i))
			{
				openedSomething = true;
				cout << "Opening controller nro: " << i << endl;
				ctrls[i] = SDL_GameControllerOpen(i);
			}
		}

		ending = false;
		if (!openedSomething)
		{
			cout << "No game controllers found... Press some key to quit." << endl;
			while (!SDL_PollEvent(&event) || event.type != SDL_KEYDOWN) {  }
			ending = true;
		}

		while (!ending)
		{
			while (SDL_PollEvent(&event))
			{
				cout << "event_type: " << event.type << endl;
				if (event.type == SDL_QUIT)
				{
					ending = true;
				}
				else if (event.type == SDL_KEYDOWN)
				{
					if (event.key.keysym.sym == SDLK_ESCAPE)
					{
						ending = true;
					}
				}

				if (event.type == SDL_CONTROLLERAXISMOTION)
				{
					cout << "Axis #" << int(event.jaxis.axis) << ", val: " << event.jaxis.value << endl;
					axes[event.jaxis.axis] = event.jaxis.value;
				}
				//else if (event.type == SDL_CONTROLLERhat)
				//{
				//	cout << "Hat #" << int(event.jhat.hat) << ", val: " << int(event.jhat.value) << endl;
				//	hats[event.jhat.hat] = event.jhat.value;
				//}
				else if (event.type == SDL_CONTROLLERBUTTONDOWN)
				{
					cout << "Button #" << int(event.jbutton.button) << " press" << endl;
					buttons[int(event.jbutton.button)] = true;
				}
				else if (event.type == SDL_CONTROLLERBUTTONUP)
				{
					cout << "Button #" << int(event.jbutton.button) << " release" << endl;
					buttons[int(event.jbutton.button)] = false;
				}
				else if (event.type == SDL_CONTROLLERDEVICEADDED || event.type == SDL_CONTROLLERDEVICEREMOVED)
				{
					cout << "Controllers added or removed." << endl;
					for (int i = 0; i < 32; ++i)
					{
						if (ctrls[i])
						{
							cout << "Closing controller #" << i << endl;
							SDL_GameControllerClose(ctrls[i]);
							ctrls[i] = NULL;
						}
					}
					for (int i = 0; i < SDL_NumJoysticks(); ++i)
					{
						if (SDL_IsGameController(i))
						{
							cout << "Opening joystick #" << i << endl;
							ctrls[i] = SDL_GameControllerOpen(i);
						}
					}
				}
				else
				{
					continue;
				}
			}

			SDL_RenderClear(renderer);

			SDL_FillRect(surf, NULL, 0xffffffff);

			SDL_Rect rect = SDL_Rect();
			for (int i = 0; i < 16; ++i)
			{
				rect.x = i * 20 + 10;
				rect.y = 240;
				rect.h = int(axes[i] * 230 / 33000);
				if (rect.h < 0)
				{
					rect.h = -rect.h;
					rect.y = 240 - rect.h;
				}
				rect.w = 10;
				SDL_FillRect(surf, &rect, 0xff0000);
			}
			for (int i = 0; i < 30; ++i)
			{
				rect.x = i % 5 * 15 + 16 * 20 + 10;
				rect.y = 10 + 15 * (i / 5);
				rect.h = 10;
				rect.w = 10;
				SDL_FillRect(surf, &rect, buttons[i] ? 0x00ff00 : 0x0f0f0f);
			}
			for (int i = 0; i < 8; ++i)
			{
				rect.x = (i % 3) * 40 + 16 * 20 + 10;
				rect.y = 120 + 40 * (i / 3);
				rect.h = 5 * (1 + (hats[i]) % 8);
				rect.w = 5 * (1 + (hats[i]) / 8);
				SDL_FillRect(surf, &rect, 0xff00ff);
			}

			SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
			SDL_RenderCopy(renderer, tex, NULL, NULL);
			SDL_RenderPresent(renderer);
			SDL_DestroyTexture(tex);
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	SDL_Quit();

    return 0;
}

