#include "button.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static int pageStatus = 0;

void sdl_check(bool res) {
  if (!res) {
    SDL_Log("SDL Failed: %s\n", SDL_GetError());
    exit(0);
  }
}
// Mainly used for button status detection
static bool point_in_rect(float x, float y, const SDL_FRect *r) {
  return x >= r->x && x <= r->x + r->w && y >= r->y && y <= r->y + r->h;
}

char *read_file_by_path(const char *f_path) {
  FILE *fp = fopen(f_path, "r");
  if (fp == NULL) {
    printf("Cannot read file, its unreachable: [%s]\n", f_path);
    return NULL;
  }
  // fseek moves the file position indicator associated with the FILE stream.
  // SEED_END is a macro in C std lib, C uses API from the OS to
  // check file's metadata to get the file size
  // SEED_END seems can only be used in file, not network stream, pipe stream...
  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  rewind(fp);
  printf("Try to read a file with size of %ld\n", size);
  char *text = malloc(size + 1);
  if (!text) {
    printf("Unable to malloc a string when reading the file!\n");
    fclose(fp);
    return NULL;
  }
  fread(text, 1, size, fp);
  text[size] = '\0';
  fclose(fp);
  return text;
}

char *read_file() {

  FILE *pipe = popen("osascript -e 'POSIX path of (choose file)'", "r");
  if (!pipe) {
    return NULL;
  }

  char path[1024];
  if (fgets(path, sizeof(path), pipe) != NULL) {
    printf("File selected: %s", path);
  }

  fclose(pipe);

  // fgets parsing stops if a newline character is found
  // and fgets keeps the newline when it reads a line
  // and '\n' will cause fopen cannot open the file
  // we need remove the newline:
  path[strcspn(path, "\n")] = '\0';

  return read_file_by_path(path);
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
  TTF_Font *small_font =
      TTF_OpenFont("/System/Library/Fonts/Supplemental/Arial.ttf", 16.0f);

  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;

  sdl_check(SDL_CreateWindowAndRenderer("Ctoy Text Editor", 800, 600, 0,
                                        &window, &renderer));

  SDL_Color white = {255, 255, 255, 255};

  // Prepare title
  SDL_Surface *surface =
      TTF_RenderText_Blended(font, "Hello and Welcome", 0, white);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

  float tw, th = 0;
  SDL_GetTextureSize(texture, &tw, &th);

  SDL_FRect dst = {800.0f / 2 - tw / 2, 50.0f, (float)surface->w,
                   (float)surface->h};
  SDL_DestroySurface(surface);

  Button open_file_btn = {.rect = {300.0f, 250.0f, 200.0f, 70.0f},
                          .text = "Open file",
                          .hovered = false,
                          .pressed = false};

  Button back_btn = {.rect = {.x = 0.0f, .y = 0.0f, .w = 90.0f, .h = 30.0f},
                     .text = "<-Back",
                     .hovered = false,
                     .pressed = false};

  char *text = "DEFAULT FALLBACK TEXT";
  SDL_Surface *t_surface;
  SDL_Texture *t_texture;
  float scroll_y = 0.0f;

  bool running = true;
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        running = false;
      }

      if (event.type == SDL_EVENT_MOUSE_MOTION) {
        open_file_btn.hovered =
            point_in_rect(event.motion.x, event.motion.y, &open_file_btn.rect);
        back_btn.hovered =
            point_in_rect(event.motion.x, event.motion.y, &back_btn.rect);
      }

      if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
          event.button.button == SDL_BUTTON_LEFT) {
        back_btn.pressed =
            point_in_rect(event.motion.x, event.motion.y, &back_btn.rect);
        open_file_btn.pressed =
            point_in_rect(event.button.x, event.button.y, &open_file_btn.rect);
        if (back_btn.pressed) {
          // Go back to main screen
          pageStatus = 0;
          scroll_y = 0;
        } else if (open_file_btn.pressed) {
          text = read_file();
          if (text) {
            t_surface = TTF_RenderText_Blended_Wrapped(small_font, text,
                                                       strlen(text), white, 0);
            ;
            t_texture = SDL_CreateTextureFromSurface(renderer, t_surface);
            pageStatus = 1;
          } else {
            printf("Unable to read file");
          }
        }
      }

      // Scrollable
      if (event.type == SDL_EVENT_MOUSE_WHEEL && pageStatus == 1) {
        // scroll down -> event.wheel.y is negative
        // we let viewport stay fixed
        // move the texture of content
        scroll_y += event.wheel.y * 25.0f;

        if (scroll_y > 0) {
          scroll_y = 0;
        }

        float min_scroll = 600 - t_surface->h - 20;

        // it is not scrollable
        if (min_scroll > 0) {
          min_scroll = 0;
        }

        if (scroll_y < min_scroll) {
          scroll_y = min_scroll;
        }
      }
    }
    SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
    SDL_RenderClear(renderer);

    if (pageStatus == 0) {
      // Draw main title
      SDL_RenderTexture(renderer, texture, NULL, &dst);

      draw_button(renderer, font, &open_file_btn);
    } else if (pageStatus == 1) {
      // Editor Mode
      // char* line = strtok(text, "\n");
      // while(line) {}
      SDL_FRect t_dst = {
          .x = 10, .y = 35 + scroll_y, .w = t_surface->w, .h = t_surface->h};
      SDL_RenderTexture(renderer, t_texture, NULL, &t_dst);

      draw_button(renderer, small_font, &back_btn);
    }

    SDL_RenderPresent(renderer);
  }

  SDL_Quit();
  printf("Goodbye\n");
  return 0;
}
