
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h> 

#define MEDIUM 1.8
#define SMALL 2.4
#define LARGE 1.35

// --- parameters customization ---

//set here the value of watts that will be the reference for the maximum value displayed (top of the graph)
static const double maxWatts = 50;
//set here the value specified by the black horizontal lines
static const double lineEveryWatt = 1.5;

//set here the colors of the lines and axes
SDL_Color bg_color = {240,240,240,255};
SDL_Color axes_color = {120,120,120,255};
SDL_Color horiz_color = {230,230,230,225};
SDL_Color draw_line = {0,0,178,255};

//set here the size of the window (SMALL, MEDIUM, LARGE)
static const double yProportion = MEDIUM;

// --- end of parameters customization ---

//offset of the vertical and horizontal axes from the window border
static const int axisOffset = 10;
static const int xAxisThickness = 3;
static const int yAxisThickness = 3;

bool running = true;

void processEvents (SDL_Event*);
void quit (SDL_Renderer* renderer, SDL_Window* window);

int main(int argc, char **argv) {
  
    if (geteuid() != 0) {
        printf("run as root\n");
        exit(1);
    }

    printf("load msr module if the program crashes\n");

    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Getting current display resolution to scale the window
    SDL_DisplayMode DISP;
    SDL_GetCurrentDisplayMode(0, &DISP);
    //printf("screensize: %d x %d\n", DISP.w, DISP.h);
    const int height = (int)(DISP.h / yProportion);
    const int width = (int)(height/2);
    //printf("window size: %d x %d\n", width, height);

    //number of measured data samples to maintain in memory for the graph
    int storedSamples = (int)((width * 0.95)/10.0);

    //number of horizontal black lines
    double numberOfSections = (maxWatts/lineEveryWatt);

    // Create a SDL window
    SDL_Window *window = SDL_CreateWindow("Graphic intel power gadget", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL);

    // Create a renderer (accelerated and in sync with the display refresh rate)
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    SDL_Event event;

    // initializing array of measured values
    float history[storedSamples];
    for (int i=0; i<storedSamples; i++) history[i] = 0;
    int t = 0; // index in history;

    while(running) {
        // Process events
        processEvents(&event);
		  
        // Clear screen white
        SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
        SDL_RenderClear(renderer);
        
        //Draw sections lines
        SDL_SetRenderDrawColor(renderer, horiz_color.r, horiz_color.g, horiz_color.b, horiz_color.a);
        for (int i=1; i<=numberOfSections; i++) {
            SDL_RenderDrawLine(renderer, axisOffset, height-axisOffset-(height*0.95*i/numberOfSections), width*0.95+xAxisThickness, height-axisOffset-(height*0.95*i/numberOfSections));
        }

        // Draw vertical axis
        SDL_SetRenderDrawColor(renderer, axes_color.r, axes_color.g, axes_color.b, axes_color.a);
        SDL_Rect vert_rect = {axisOffset, height-(height*0.95)-axisOffset, yAxisThickness, height*0.95+xAxisThickness};
        SDL_RenderFillRect(renderer, &vert_rect);

	    //Draw horizontal axis
        SDL_SetRenderDrawColor(renderer, axes_color.r, axes_color.g, axes_color.b, axes_color.a);
        SDL_Rect horiz_rect = {axisOffset, height-axisOffset, width*0.95, xAxisThickness};
        SDL_RenderFillRect(renderer, &horiz_rect);


        FILE *fp;
        char path[1035];

        // Calling the intel utility for measurement
        fp = popen("/usr/local/sbin/watts -e 100 -d 0.5 | grep \"Average Processor Power\" | cut -d= -f2", "r");
        if (fp == NULL) {
            printf("Failed to run command\n" );
            exit(1);
        } 

        float readValue = atof(fgets(path, sizeof(path)-1, fp));
        if (argc > 1) printf("read value : %4.2f\n", readValue); 

        SDL_SetRenderDrawColor(renderer, draw_line.r, draw_line.g, draw_line.b, draw_line.a);
        double scale = (height*0.95)/maxWatts;

        history[t] = readValue;
        int time = 0;
        for (int i=t+1; i<t+storedSamples; i++) {
            if (history[i % storedSamples] != 0 && history[(i+1) % storedSamples] != 0) {
                SDL_RenderDrawLine(renderer, axisOffset+yAxisThickness+time*10, height-axisOffset-history[i % storedSamples]*scale, axisOffset+yAxisThickness+(time+1)*10, height-axisOffset-history[(i+1) % storedSamples]*scale);
            }
            time++;
        }
        t = (t+1) % storedSamples;
        
        SDL_RenderPresent(renderer);

        pclose(fp);
        }

    // Release resources
	quit(renderer, window);
    return 0;
}

void processEvents (SDL_Event* event) {
	while(SDL_PollEvent(event)) {
		if(event->type == SDL_QUIT) {
			running = false;
		}
   }
}

void quit (SDL_Renderer* renderer, SDL_Window* window) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
