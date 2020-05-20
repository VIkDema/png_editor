#ifndef PNG_EDIT_H
#define PNG_EDIT_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "mainwindow.h"

#define PNG_DEBUG 3

#include <png.h>
#include <cstring>

#define GREEN 0
#define RED 1
#define YELLOW 2
#define PINK 3
#define BLUE 4
#define ORANGE 5
#define BLACK 6
#define WHITE 7
#define CIRCULAR 0
#define DIAGONAL 1


struct Png {
    int width, height;
    png_byte color_type;
    png_byte bit_depth;

    png_structp png_ptr;
    png_infop info_ptr;
    int number_of_passes;
    png_bytep *row_pointers;
};
void read_png_file(char *file_name, struct Png *image);
void write_png_file(char *file_name, struct Png *image);
void swap_png(struct Png *image, int x_s, int y_s, int x_e, int y_e, int mode);
void make_square(struct Png *image, int xc, int yc, int side_size, int line_thickness, QColor color_for_line, bool is_filled, QColor fill_color);
void find_color_and_replacing(struct Png *image, QColor color_for_swap);

#endif // PNG_EDIT_H
