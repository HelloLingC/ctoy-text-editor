#ifndef BUTTON_H
#define BUTTON_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>

typedef struct {
  SDL_FRect rect;
  const char *text;
  bool hovered;
  bool pressed;
} Button;

void draw_button(SDL_Renderer *renderer, TTF_Font *font, Button *btn);

void draw_button_text(SDL_Renderer *renderer, TTF_Font *font, const char *text,
                      SDL_FRect box);

#endif