#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>
#include <stdlib.h>

void sdl_check(bool res) {
  if(!res) {
    SDL_Log("SDL Failed: %s", SDL_GetError());
    //exit(0);
      }
}


typedef struct {
  SDL_FRect rect;
  const char *text;
  bool hovered;
  bool pressed;
} Button;


// Mainly used for button status detection
static bool point_in_rect(float x, float y, const SDL_FRect *r) {
  return x>=r->x && x<=r->x + r->w && y >= r->y && y<= r->y + r->h; 
}

static void draw_button_text(SDL_Renderer *renderer, TTF_Font *font,
                             const char *text, SDL_FRect box) {
  SDL_Color color = {255, 255, 255, 255};
  SDL_Surface* surface = TTF_RenderText_Blended(font, text, 0, color);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_DestroySurface(surface);

  // Font width and height
  float tw, th = 0.0f;
  sdl_check(SDL_GetTextureSize(texture, &tw, &th));

  // make font centered
  SDL_FRect dst = {
    box.x + (box.w - tw) / 2.0f,
    box.y + (box.h - th) / 2.0f,
    tw,
    th 
  };
  SDL_RenderTexture(renderer, texture, NULL, &dst);
  SDL_DestroyTexture(texture);
}

static void draw_button(SDL_Renderer *renderer, TTF_Font * font, Button *btn) {
  if(btn->pressed) {
    SDL_SetRenderDrawColor(renderer, 40, 90, 170, 255);
  } else if(btn->hovered) {
    SDL_SetRenderDrawColor(renderer, 70, 130, 220, 255);
  } else {
    SDL_SetRenderDrawColor(renderer, 50, 100, 120, 255);
  }
  SDL_RenderFillRect(renderer, &btn->rect);

  //SDL_SetRenderDrawColor(renderer, 255, 255, 255, 25);
  //SDL_RenderFillRect(renderer, &btn->rect);

  draw_button_text(renderer, font, btn->text, btn->rect);
}

int main(void) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("SDL Failed: %s", SDL_GetError());
    return 1;
  }

  sdl_check(TTF_Init());

  TTF_Font *font =
      TTF_OpenFont("/System/Library/Fonts/Supplemental/Arial.ttf", 32.0f);
  if (!font) {
    SDL_Log("Failed to load dont: %s", SDL_GetError());
    exit(0);
  }

  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;
  
  sdl_check(
      SDL_CreateWindowAndRenderer("Hello", 800, 600, 0, &window, &renderer));


  
  SDL_Color white = {255, 255, 255, 255};

  SDL_Surface *surface =
      TTF_RenderText_Blended(font, "Hello and Welcome", 0, white);
  
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

  float tw, th = 0;
  SDL_GetTextureSize(texture, &tw, &th);

   SDL_FRect dst = {
    800/2 - tw/2, 50.0f, (float) surface->w, (float) surface->h
   };
  
  SDL_DestroySurface(surface);

  Button btn = {
    .rect = {300.0f, 250.0f, 200.0f, 70.0f},
    .text = "Open file",
    .hovered = false,
    .pressed = false
  };

  
  
  bool running = true;
  while(running) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      if(event.type == SDL_EVENT_QUIT) {
	running = false;
      }

      if(event.type == SDL_EVENT_MOUSE_MOTION) {
	btn.hovered = point_in_rect(event.motion.x, event.motion.y, &btn.rect); 
      }

      if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
	if(point_in_rect(event.button.x, event.button.y, &btn.rect)) {
	  printf("CLICKED\n");
	}
      }
    }
    SDL_SetRenderDrawColor(renderer, 20, 20,30,255);
    SDL_RenderClear(renderer);

    SDL_RenderTexture(renderer, texture, NULL, &dst);

    draw_button(renderer, font, &btn);
    SDL_RenderPresent(renderer);
  }
  
  
  SDL_Quit();
  printf("Hello,world\n");
  return 0;
}
