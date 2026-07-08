#include "button.h"

void draw_button(SDL_Renderer *renderer, TTF_Font *font, Button *btn) {
  if (btn->pressed) {
    SDL_SetRenderDrawColor(renderer, 40, 90, 170, 255);
  } else if (btn->hovered) {
    SDL_SetRenderDrawColor(renderer, 70, 130, 220, 255);
  } else {
    SDL_SetRenderDrawColor(renderer, 50, 100, 120, 255);
  }
  SDL_RenderFillRect(renderer, &btn->rect);

  // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 25);
  // SDL_RenderFillRect(renderer, &btn->rect);

  draw_button_text(renderer, font, btn->text, btn->rect);
}

void draw_button_text(SDL_Renderer *renderer, TTF_Font *font, const char *text,
                      SDL_FRect box) {
  SDL_Color color = {255, 255, 255, 255};
  SDL_Surface *surface = TTF_RenderText_Blended(font, text, 0, color);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_DestroySurface(surface);

  // Font width and height
  float tw, th = 0.0f;
  SDL_GetTextureSize(texture, &tw, &th);

  // make font centered
  SDL_FRect dst = {box.x + (box.w - tw) / 2.0f, box.y + (box.h - th) / 2.0f, tw,
                   th};
  SDL_RenderTexture(renderer, texture, NULL, &dst);
  SDL_DestroyTexture(texture);
}