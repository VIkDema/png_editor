
#include "mainwindow.h"
#include <png.h>
#include <stdlib.h>

void read_png_file(char *file_name, struct Png *image) {
    char header[8];    // 8 is the maximum size that can be checked

    /* open file and test for it being a png */
    FILE *fp = fopen(file_name, "rb");
    if (!fp) {
        QMessageBox::warning(0,"Ошибка","File could not be opened");

        // Some error handling: file could not be opened
        return;

    }

    fread(header, 1, 8, fp);
    if (png_sig_cmp(reinterpret_cast<png_const_bytep>(header), 0, 8)) {
        QMessageBox::warning(0,"Ошибка","File is not recognized as a PNG");
        // Some error handling: file is not recognized as a PNG
        return;

    }

    /* initialize stuff */
    image->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!image->png_ptr) {
        QMessageBox::warning(0,"Ошибка","png_create_read_struct failed");
        return;
        // Some error handling: png_create_read_struct failed
    }

    image->info_ptr = png_create_info_struct(image->png_ptr);
    if (!image->info_ptr) {
        QMessageBox::warning(0,"Ошибка","png_create_info_struct failed");
        return;
        // Some error handling: png_create_info_struct failed

    }

    if (setjmp(png_jmpbuf(image->png_ptr))) {
        QMessageBox::warning(0,"Ошибка","error during init_io");
        return;
        // Some error handling: error during init_io

    }

    png_init_io(image->png_ptr, fp);
    png_set_sig_bytes(image->png_ptr, 8);

    png_read_info(image->png_ptr, image->info_ptr);

    image->width = png_get_image_width(image->png_ptr, image->info_ptr);
    image->height = png_get_image_height(image->png_ptr, image->info_ptr);
    image->color_type = png_get_color_type(image->png_ptr, image->info_ptr);
    image->bit_depth = png_get_bit_depth(image->png_ptr, image->info_ptr);

    image->number_of_passes = png_set_interlace_handling(image->png_ptr);
    png_read_update_info(image->png_ptr, image->info_ptr);

    /* read file */
    if (setjmp(png_jmpbuf(image->png_ptr))) {
        QMessageBox::warning(0,"Ошибка","error during read_image");
        return;
        // Some error handling: error during read_image
    }

    image->row_pointers = (png_bytep *) malloc(sizeof(png_bytep) * image->height);
    for (int y = 0; y < image->height; y++)
        image->row_pointers[y] = (png_byte *) malloc(png_get_rowbytes(image->png_ptr, image->info_ptr));

    png_read_image(image->png_ptr, image->row_pointers);

    fclose(fp);
}


void write_png_file(char *file_name, struct Png *image) {
    int  y;
    /* create file */
    FILE *fp = fopen(file_name, "wb");
    if (!fp) {
        QMessageBox::warning(0,"Ошибка","File could not be opened");
        // Some error handling: file could not be opened
        return;
        // Some error handling: file could not be opened

    }

    /* initialize stuff */
    image->png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!image->png_ptr) {
        QMessageBox::warning(0,"Ошибка","png_create_write_struct failed");
        // Some error handling: png_create_write_struct failed
        return;
    }

    image->info_ptr = png_create_info_struct(image->png_ptr);
    if (!image->info_ptr) {
       QMessageBox::warning(0,"Ошибка","png_create_info_struct failed");
        return;
        // Some error handling: png_create_info_struct failed
    }

    if (setjmp(png_jmpbuf(image->png_ptr))) {
        QMessageBox::warning(0,"Ошибка","error during init_io");
        // Some error handling: error during init_io
        return;
    }

    png_init_io(image->png_ptr, fp);


    /* write header */
    if (setjmp(png_jmpbuf(image->png_ptr))) {
        QMessageBox::warning(0,"Ошибка","error during writing header");
        // Some error handling: error during writing header
        return;
    }

    png_set_IHDR(image->png_ptr, image->info_ptr, image->width, image->height,
                 image->bit_depth, image->color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(image->png_ptr, image->info_ptr);


    /* write bytes */
    if (setjmp(png_jmpbuf(image->png_ptr))) {
        QMessageBox::warning(0,"Ошибка","error during writing bytes");
        // Some error handling: error during writing bytes
        return;
    }

    png_write_image(image->png_ptr, image->row_pointers);


    /* end write */
    if (setjmp(png_jmpbuf(image->png_ptr))) {
       QMessageBox::warning(0,"Ошибка","error during end of write");
        // Some error handling: error during end of write
        return;
    }

    png_write_end(image->png_ptr, NULL);

    /* cleanup heap allocation */
    for (y = 0; y < image->height; y++)
        free(image->row_pointers[y]);
    free(image->row_pointers);

    fclose(fp);
}


png_byte *get_color(QColor color) {
    png_byte *ptr = (png_byte *) malloc(sizeof(png_byte) * 3);
    int r=10,g=10,b=10;
    color.getRgb(&r,&g,&b);
    ptr[0]=r;
    ptr[1]=g;
    ptr[2]=b;

    return ptr;
}

void make_square(struct Png *image, int xc, int yc, int side_size, int line_thickness, QColor color_for_line, bool is_filled,
                 QColor fill_color) {

    if (xc > image->width || xc < 0||yc > image->height || xc < 0||line_thickness > xc / 2 || line_thickness < 0||side_size<0) { return; }

    int side_size_x = side_size;
    int side_size_y = side_size;

    if (xc + side_size > image->width) {
        side_size_x = image->width - xc - 1;
    }

    if (yc + side_size > image->height) {
        side_size_y = image->height - yc - 1;
    }

    int size = 0;

    if (png_get_color_type(image->png_ptr, image->info_ptr) == PNG_COLOR_TYPE_RGB) {
        size = 3;
    }

    if (png_get_color_type(image->png_ptr, image->info_ptr) == PNG_COLOR_TYPE_RGBA) {
        size = 4;
    }


    //заливка
    if (is_filled) {
        for (int y = yc; y < yc + side_size_y; ++y) {
            png_byte *row = image->row_pointers[y];
            for (int x = xc; x < xc + side_size_x; x++) {
                png_byte *ptr = &(row[x * size]);
                memcpy(ptr, get_color(fill_color), sizeof(png_byte) * 3);
                if (size == 4) {
                    ptr[3] = 255;
                }
            }
        }

    }

    //линия
    //верхняя
    for (int y = yc; y < yc + line_thickness; ++y) {
        png_byte *row = image->row_pointers[y];
        for (int x = xc; x < xc + side_size_x; x++) {
            png_byte *ptr = &(row[x * size]);
            memcpy(ptr, get_color(color_for_line), sizeof(png_byte) * 3);
            if (size == 4) {
                ptr[3] = 255;
            }
        }
    }

    for (int y = yc + line_thickness; y < yc + side_size_y - line_thickness; ++y) {
        png_byte *row = image->row_pointers[y];
        for (int x = xc; x < xc + line_thickness; x++) {
            png_byte *ptr = &(row[x * size]);
            memcpy(ptr, get_color(color_for_line), sizeof(png_byte) * 3);
            if (size == 4) {
                ptr[3] = 255;
            }
            ptr = &(row[(x + side_size_x - line_thickness) * 4]);
            memcpy(ptr, get_color(color_for_line), sizeof(png_byte) * 3);
            if (size == 4) {
                ptr[3] = 255;
            }
        }
    }
    //нижняя
    for (int y = yc + side_size_y - line_thickness; y < yc + side_size_y; ++y) {
        png_byte *row = image->row_pointers[y];
        for (int x = xc; x < xc + side_size_x; x++) {
            png_byte *ptr = &(row[x * size]);
            memcpy(ptr, get_color(color_for_line), sizeof(png_byte) * 3);
            if (size == 4) {
                ptr[3] = 255;
            }
        }
    }


}

void swap_png_byte(png_byte *ptr1, png_byte *ptr2, int size) {
    png_byte *swap_ptr = (png_byte *) malloc(sizeof(png_byte) * size);
    memcpy(swap_ptr, ptr1, sizeof(png_byte) * size);
    memcpy(ptr1, ptr2, sizeof(png_byte) * size);
    memcpy(ptr2, swap_ptr, sizeof(png_byte) * size);
}

void swap_png(struct Png *image, int x_s, int y_s, int x_e, int y_e, int mode) {
    if (x_s > image->width || y_s > image->height) {
        QMessageBox::warning(0,"Ошибка","Вышли за пределы");
        return; }
    int int_for_swap=0;
    if(x_s>x_e){
        int_for_swap=x_s;
        x_s=x_e;
        x_e=int_for_swap;
    }
    if(y_s>y_e){
        int_for_swap=y_s;
        y_s=y_e;
        y_e=int_for_swap;
    }
    int size = 0;
    if (png_get_color_type(image->png_ptr, image->info_ptr) == PNG_COLOR_TYPE_RGB) {
        size = 3;
    }

    if (png_get_color_type(image->png_ptr, image->info_ptr) == PNG_COLOR_TYPE_RGBA) {
        size = 4;
    }
    if (x_e < x_s || y_e < y_s || x_e < 0 || y_e < 0 || y_s < 0 || y_e < 0) { return; }
    int wight = (x_e - x_s) / 2;
    int hight = (y_e - y_s) / 2;
    int dif_x = (x_e - x_s) % 2;
    int dif_y = (y_e - y_s) % 2;
    if (mode == CIRCULAR) {
        //swap 1 и 2 +

        for (int y = y_s; y < y_s + hight; ++y) {
            png_byte *row = image->row_pointers[y];
            for (int x = x_s; x < x_s + wight; x++) {
                png_byte *ptr1 = &(row[x * size]);
                png_byte *ptr2 = &(row[(x + wight + dif_x) * size]);
                swap_png_byte(ptr1, ptr2, size);
            }
        }
        //swap 4 и 1
        for (int x = x_s; x < x_s + hight; x++) {
            for (int y = y_s; y < y_s + hight; ++y) {
                png_byte *row1 = image->row_pointers[y];
                png_byte *row2 = image->row_pointers[y + hight + dif_y];
                png_byte *ptr1 = &(row1[x * size]);
                png_byte *ptr2 = &(row2[x * size]);
                swap_png_byte(ptr1, ptr2, size);
            }
        }
        /*
        //swap 2 и 3
        for (int x = x_s+wight+dif_x; x < x_e+1; x++) {
            for (int y = y_s; y <y_s+hight; ++y) {
            png_byte *row1 = image->row_pointers[y];
            png_byte *row2 = image->row_pointers[y+hight+dif_y];
            png_byte *ptr1 = &(row1[x * 4]);
            png_byte *ptr2 = &(row2[x * 4]);
            swap_png_byte(ptr1,ptr2);
            }
        }
         */
        //swap 3 и 4
        for (int y = y_s + hight + dif_y; y < y_e + 1; ++y) {
            png_byte *row = image->row_pointers[y];
            for (int x = x_s; x < x_s + wight; x++) {
                png_byte *ptr1 = &(row[x * size]);
                png_byte *ptr2 = &(row[(x + wight + dif_x) * size]);
                swap_png_byte(ptr1, ptr2, size);
            }
        }
    }
    if (mode == DIAGONAL) {
        for (int y = y_s; y < y_s + hight; ++y) {
            png_byte *row = image->row_pointers[y];
            for (int x = x_s; x < x_s + wight; x++) {
                png_byte *ptr1 = &(row[x * size]);
                png_byte *ptr2 = &(row[(x + wight + dif_x) * size]);
                swap_png_byte(ptr1, ptr2, size);
            }
        }
        for (int x = x_s + wight + dif_x; x < x_e + 1; x++) {
            for (int y = y_s; y < y_s + hight; ++y) {
                png_byte *row1 = image->row_pointers[y];
                png_byte *row2 = image->row_pointers[y + hight + dif_y];
                png_byte *ptr1 = &(row1[x * size]);
                png_byte *ptr2 = &(row2[x * size]);
                swap_png_byte(ptr1, ptr2, size);
            }
        }
        for (int x = x_s; x < x_s + hight; x++) {
            for (int y = y_s; y < y_s + hight; ++y) {
                png_byte *row1 = image->row_pointers[y];
                png_byte *row2 = image->row_pointers[y + hight + dif_y];
                png_byte *ptr1 = &(row1[x * size]);
                png_byte *ptr2 = &(row2[x * size]);
                swap_png_byte(ptr1, ptr2, size);
            }
        }
        for (int y = y_s; y < y_s + hight; ++y) {
            png_byte *row = image->row_pointers[y];
            for (int x = x_s; x < x_s + wight; x++) {
                png_byte *ptr1 = &(row[x * size]);
                png_byte *ptr2 = &(row[(x + wight + dif_x) * size]);
                swap_png_byte(ptr1, ptr2, size);
            }
        }

    }


}

int
search_count_of_color_in_image(struct Png *image, png_byte *ptr_for_search, int size, int yt, int accuracy, int alpha) {
    int count = 0;

    for (int y = yt; y < image->height; ++y) {
        png_byte *row = image->row_pointers[y];
        for (int x = 0; x < image->width; x=x+3) {
            png_byte *ptr = &(row[x * size]);
            //if ((ptr_for_search[0] == ptr[0]) && ptr_for_search[1] == ptr[1] && ptr_for_search[2] == ptr[2]) {
            if (size == 3) {
                if ((ptr[0] > ptr_for_search[0] - accuracy && ptr[0] < ptr_for_search[0] + accuracy) &&
                    (ptr[1] > ptr_for_search[1] - accuracy && ptr[1] < ptr_for_search[1] + accuracy) &&
                    (ptr[2] > ptr_for_search[2] - accuracy && ptr[2] < ptr_for_search[2] + accuracy)) {
                    count++;
                }
            }
            if (size == 4 && ptr[3] > alpha) {
                if ((ptr[0] > ptr_for_search[0] - accuracy && ptr[0] < ptr_for_search[0] + accuracy) &&
                    (ptr[1] > ptr_for_search[1] - accuracy && ptr[1] < ptr_for_search[1] + accuracy) &&
                    (ptr[2] > ptr_for_search[2] - accuracy && ptr[2] < ptr_for_search[2] + accuracy)) {
                    count++;
                }
            }
        }
    }
    return count;
}

void find_color_and_replacing(struct Png *image, QColor color_for_swap) {
    int size = 0;
    int accuracy = 20;
    int alpha = 250;
    if (png_get_color_type(image->png_ptr, image->info_ptr) == PNG_COLOR_TYPE_RGB) {
        size = 3;
    }

    if (png_get_color_type(image->png_ptr, image->info_ptr) == PNG_COLOR_TYPE_RGBA) {
        size = 4;
    }
    png_byte *ptr_color_for_replace = (png_byte *) malloc(sizeof(png_byte) * size);
    png_byte *ptr_color = get_color(color_for_swap);
    int max = 0;
    int count = 0;

    for (int y = 0; y < image->height; ++y) {
        png_byte *row = image->row_pointers[y];
        for (int x = 0; x < image->width; x=x+3) {
            png_byte *ptr = &(row[x * size]);

            if (x == 0 && y == 0) {
                memcpy(ptr_color_for_replace, ptr, sizeof(png_byte) * 3);
                count = search_count_of_color_in_image(image, ptr, size, y, accuracy, alpha);
                if (count > max) {
                    max = count;
                    memcpy(ptr_color_for_replace, ptr, sizeof(png_byte) * 3);
                }
            }
            if (size == 3) {
                if (!((ptr[0] > ptr_color_for_replace[0] - accuracy && ptr[0] < ptr_color_for_replace[0] + accuracy) &&
                      (ptr[1] > ptr_color_for_replace[1] - accuracy && ptr[1] < ptr_color_for_replace[1] + accuracy) &&
                      (ptr[2] > ptr_color_for_replace[2] - accuracy && ptr[2] < ptr_color_for_replace[2] + accuracy))) {
                    count = search_count_of_color_in_image(image, ptr, size, y, accuracy, alpha);
                    if (count > max) {
                        max = count;
                        memcpy(ptr_color_for_replace, ptr, sizeof(png_byte) * 3);
                    }
                }
            } else if (size == 4 && ptr[3] > alpha) {

                if (!(ptr[0] > ptr_color_for_replace[0] - accuracy && ptr[0] < ptr_color_for_replace[0] + accuracy &&
                      (ptr[1] > ptr_color_for_replace[1] - accuracy && ptr[1] < ptr_color_for_replace[1] + accuracy) &&
                      (ptr[2] > ptr_color_for_replace[2] - accuracy && ptr[2] < ptr_color_for_replace[2] + accuracy))) {
                    count = search_count_of_color_in_image(image, ptr, size, y, accuracy, alpha);
                    if (count > max) {
                        max = count;
                        memcpy(ptr_color_for_replace, ptr, sizeof(png_byte) * 3);
                    }
                }
            }
        }
    }
    for (int y = 0; y < image->height; ++y) {
        png_byte *row = image->row_pointers[y];
        for (int x = 0; x < image->width; x++) {
            png_byte *ptr = &(row[x * size]);
            /*
            if (ptr[0] == ptr_color_for_replace[0] && ptr[1] == ptr_color_for_replace[1] &&
                ptr[2] == ptr_color_for_replace[2]) {
                memcpy(ptr, ptr_color, sizeof(png_byte) * 3);
            }*/
            if (size == 3) {
                if ((ptr[0] > ptr_color_for_replace[0] - accuracy && ptr[0] < ptr_color_for_replace[0] + accuracy &&
                     (ptr[1] > ptr_color_for_replace[1] - accuracy && ptr[1] < ptr_color_for_replace[1] + accuracy) &&
                     (ptr[2] > ptr_color_for_replace[2] - accuracy && ptr[2] < ptr_color_for_replace[2] + accuracy))) {
                    for (int i = 0; i < 3; ++i) {
                        ptr[i] = ptr_color[i];
                    }
                }
            } else if (size == 4 && ptr[3] > alpha) {
                if ((ptr[0] > ptr_color_for_replace[0] - accuracy && ptr[0] < ptr_color_for_replace[0] + accuracy &&
                     (ptr[1] > ptr_color_for_replace[1] - accuracy && ptr[1] < ptr_color_for_replace[1] + accuracy) &&
                     (ptr[2] > ptr_color_for_replace[2] - accuracy && ptr[2] < ptr_color_for_replace[2] + accuracy))) {
                    for (int i = 0; i < 3; ++i) {
                        ptr[i] = ptr_color[i];
                    }
                }
            }
        }
    }


}

void negativ(struct Png *image){
     int size=0;
    if (png_get_color_type(image->png_ptr, image->info_ptr) == PNG_COLOR_TYPE_RGB) {
        size = 3;
    }

    if (png_get_color_type(image->png_ptr, image->info_ptr) == PNG_COLOR_TYPE_RGBA) {
        size = 4;
    }
    for (int y = 0; y < image->height; ++y) {
        png_byte *row = image->row_pointers[y];
        for (int x = 0; x < image->width; x++) {
            png_byte *ptr = &(row[x * size]);
            ptr[0]=255-ptr[0];
            ptr[1]=255-ptr[1];
            ptr[2]=255-ptr[2];


        }


}
}


void chbe(struct Png *image, double brightness){
    int size=0;
    if (png_get_color_type(image->png_ptr, image->info_ptr) == PNG_COLOR_TYPE_RGB) {
        size = 3;
    }

    if (png_get_color_type(image->png_ptr, image->info_ptr) == PNG_COLOR_TYPE_RGBA) {
        size = 4;
    }

    int  separator = 255 / brightness / 2 * 3;
   int total =0;
   for (int y = 0; y < image->height; ++y) {
       png_byte *row = image->row_pointers[y];
       for (int x = 0; x < image->width; x++) {
           png_byte *ptr = &(row[x * size]);
           total = (int)ptr[0]+(int)ptr[1]+(int)ptr[2];
           if(total > separator){
                ptr[0]=255;
                ptr[1]=255;
                ptr[2]=255;
           }else{
               ptr[0]=0;
               ptr[1]=0;
               ptr[2]=0;
           }
       }
   }
}

/*
int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: program_name <file_in> <file_out>\n");
        return 0;
    }

    struct Png image;
    read_png_file(argv[1], &image);
    make_square(&image, 200, 200, 200, 50, GREEN, false, YELLOW);

    swap(&image, 200, 200, 500, 500, DIAGONAL);
    find_color_and_replacing(&image, BLUE);
    write_png_file(argv[2], &image);

    return 0;
}
*/
