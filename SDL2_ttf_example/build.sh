emcc sdl2_ttf.c -O2 -sUSE_SDL=2 -sUSE_SDL_TTF=2 --embed-file LiberationSansBold.ttf -o sdl2_ttf.html 
# we compile a standalone app but no window...
# gcc sdl2_ttf.c -I /usr/include/SDL -lSDL -o sdl2_ttf