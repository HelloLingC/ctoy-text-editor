#include "button.h"
#include "document.h"
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

// this function won't free orig arg
char *replace_string(const char *orig, const char *old, const char *new,
                     bool notice) {
  char *result;
  size_t len = strlen(orig);
  size_t old_len = strlen(old);
  size_t new_len = strlen(new);

  int cnt = 0;
  // calculate how many times old string appear in string.
  const char *p = orig;
  // while ((p = strstr(p + 1, old))) {
  //   cnt++;
  //   printf("%d\n", cnt);
  // }

  while ((p = strstr(p, old)) != NULL) {
    cnt++;
    p += old_len;
  }

  result = (char *)malloc(len - (new_len - old_len) * cnt);
  if (!result) {
    return NULL;
  }

  char *mod_str = result;
  while (cnt--) {
    const char *next_match = strstr(orig, old);

    memcpy(mod_str, orig, next_match - orig);
    mod_str += next_match - orig;

    memcpy(mod_str, new, new_len);
    mod_str += new_len;

    orig = next_match + old_len;

    if (notice) {
      printf("Replaced %s with %s\n", old, new);
    }
  }
  strcpy(mod_str, orig); // strcpy also copy terminator \0
  return result;
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
    printf("Cannot open choose file pipe!");
    return NULL;
  }

  char path[1024];
  if (fgets(path, sizeof(path), pipe) == NULL) {
    printf("No path received: eof=%d, error=%d", feof(pipe), ferror(pipe));
    pclose(pipe);
    return NULL;
  }

  pclose(pipe);

  // fgets parsing stops if a newline character is found
  // and fgets keeps the newline when it reads a line
  // and '\n' will cause fopen cannot open the file
  // we need remove the newline:
  path[strcspn(path, "\n")] = '\0';

  char *text = read_file_by_path(path);
  if (!text) {
    return NULL;
  }

  // normalize CRLF to LF
  char *temp = NULL;
  temp = replace_string(text, "\r\n", "\n", true);

  free(text);
  if (!temp) {
    return NULL;
  }

  return temp;
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

  sdl_check(SDL_StartTextInput(window));

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

  Document doc;
  if (!document_init(&doc, 1024)) {
    printf("Cannot init document struct");
    exit(0);
  }

  SDL_Surface *t_surface = NULL;
  SDL_Texture *t_texture = NULL;
  SDL_Surface *new_t_surface;
  SDL_Texture *new_t_texture;
  float scroll_y = 0.0f;
  bool document_changed = false;
  size_t cursor_pos = 0;

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

      if (event.type == SDL_EVENT_TEXT_INPUT && pageStatus == 1) {
        if (!document_insert_string(&doc, event.text.text, cursor_pos)) {
          printf("Cannot append string to document struct");
          continue;
        }
        cursor_pos += strlen(event.text.text);
        document_changed = true;
      }

      if (event.type == SDL_EVENT_KEY_DOWN && pageStatus == 1) {
        if (event.key.key == SDLK_BACKSPACE) {
          if (document_backspace_at(&doc, cursor_pos)) {
            document_changed = true;
            cursor_pos -= 1;
          } else {
            printf("Cannot perform backspace\n");
          }
        }

        if (event.key.key == SDLK_RETURN) {
          if (document_insert_char(&doc, '\n', cursor_pos)) {
            document_changed = true;
            cursor_pos += 1;
          } else {
            printf("Cannot perform return key\n");
          }
        }
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
          char *text = read_file();

          if (!text) {
            printf("Cannot read the file\n");
            continue;
          }

          document_clear(&doc);

          bool appended = document_append_string(&doc, text);
          free(text);
          text = NULL;
          if (!appended) {
            printf("Cannot appeend string when reading the file");
            continue;
          }

          document_changed = true;
          pageStatus = 1;
        }
      }

      // Scrollable
      if (event.type == SDL_EVENT_MOUSE_WHEEL && pageStatus == 1) {
        if (!t_surface) {
          // User may currentlt type something
          // t_surface could be NULL;
          continue;
        }

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

      if (document_changed) {

        new_t_surface = TTF_RenderText_Blended_Wrapped(small_font, doc.buffer,
                                                       doc.length, white, 0);
        ;
        if (!new_t_surface) {
          printf("%s\n", SDL_GetError());
          continue;
        }
        new_t_texture = SDL_CreateTextureFromSurface(renderer, new_t_surface);
        if (!new_t_texture) {
          printf("%s\n", SDL_GetError());
          SDL_DestroySurface(new_t_surface);
          continue;
        }

        SDL_DestroySurface(t_surface);
        SDL_DestroyTexture(t_texture);
        t_surface = new_t_surface;
        t_texture = new_t_texture;

        document_changed = false;
      }
      // Draw article content
      SDL_FRect t_dst = {
          .x = 10, .y = 35 + scroll_y, .w = t_surface->w, .h = t_surface->h};
      SDL_RenderTexture(renderer, t_texture, NULL, &t_dst);

      draw_button(renderer, small_font, &back_btn);
    }

    SDL_RenderPresent(renderer);
  }

  // Resource relif
  TTF_Quit();
  SDL_DestroyTexture(t_texture);
  SDL_StopTextInput(window);
  SDL_Quit();
  document_destroy(&doc);
  printf("Goodbye\n");
  return 0;
}
