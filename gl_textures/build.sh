emcc gl_textures.cpp -o gl_textures.html
# we compile a standalone app but no window...
gcc gl_textures.cpp -I /usr/include/GLES -lGL -o gl_textures