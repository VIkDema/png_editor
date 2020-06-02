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


void paint(struct Png *image,int size,int y_start,int y_end,int x_start,int x_end, QColor color,bool tr=false,int x_for_tr=0){
    for (int y = y_start; y < y_end; ++y) {
        png_byte *row = image->row_pointers[y];
        for (int x = x_start; x < x_end; x++) {
            png_byte *ptr = &(row[x * size]);
            memcpy(ptr, get_color(color), sizeof(png_byte) * 3);
            if(tr){
                ptr = &(row[(x + x_for_tr) * 4]);
                memcpy(ptr, get_color(color), sizeof(png_byte) * 3);
            }
            if (size == 4) {
                ptr[3] = 255;
            }
        }
    }
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


    if (is_filled) {
        paint(image,size,yc,yc+side_size_y,xc,xc+side_size_x,fill_color);
    }

    paint(image,size,yc,yc+line_thickness,xc,xc+side_size_x,color_for_line);

    paint(image,size,yc+line_thickness,yc+side_size_y-line_thickness,xc,xc+line_thickness,color_for_line,true,side_size_x - line_thickness);

    paint(image,size,yc+side_size_y-line_thickness,yc+side_size_y,xc,xc+side_size_x,color_for_line);

}
void swap_png_byte(png_byte *ptr1, png_byte *ptr2, int size) {
    png_byte *swap_ptr = (png_byte *) malloc(sizeof(png_byte) * size);
    memcpy(swap_ptr, ptr1, sizeof(png_byte) * size);
    memcpy(ptr1, ptr2, sizeof(png_byte) * size);
    memcpy(ptr2, swap_ptr, sizeof(png_byte) * size);
}

void swap_png_one_block1(struct Png *image,int y_start,int y_end,int x_start,int x_end,int dif,int size){

    for (int y = y_start; y < y_end; ++y) {
        png_byte *row = image->row_pointers[y];
        for (int x = x_start; x < x_end; x++) {
            png_byte *ptr1 = &(row[x * size]);
            png_byte *ptr2 = &(row[(x + dif) * size]);
            swap_png_byte(ptr1, ptr2, size);
        }
    }
}
void swap_png_one_block2(struct Png *image,int y_start,int y_end,int x_start,int x_end,int dif,int size){
    for (int x = y_start; x < y_end; x++) {
        for (int y = x_start; y < x_end; ++y) {
            png_byte *row1 = image->row_pointers[y];
            png_byte *row2 = image->row_pointers[y + dif];
            png_byte *ptr1 = &(row1[x * size]);
            png_byte *ptr2 = &(row2[x * size]);
            swap_png_byte(ptr1, ptr2, size);
        }
    }
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
        swap_png_one_block1(image,y_s,y_s+hight,x_s,x_s+wight,wight+dif_x,size);
        swap_png_one_block2(image,x_s,x_s+wight,y_s,y_s+hight,hight+dif_y,size);
        swap_png_one_block1(image,y_s + hight + dif_y,y_e + 1,x_s,x_s+wight,wight+dif_x,size);
    }
    if (mode == DIAGONAL) {
        swap_png_one_block1(image,y_s,y_s+hight,x_s,x_s+wight,wight+dif_x,size);
        swap_png_one_block2(image,x_s + wight + dif_x,x_e,y_s,y_s+hight,hight+dif_y,size);
        swap_png_one_block2(image,x_s,x_s + wight,y_s,y_s+hight,hight+dif_y,size);
        swap_png_one_block1(image,y_s,y_s+hight,x_s,x_s+wight,wight+dif_x,size);
    }
}
bool is_color(png_byte *ptr_for_search,png_byte* ptr,int accuracy){
    if ((ptr[0] > ptr_for_search[0] - accuracy && ptr[0] < ptr_for_search[0] + accuracy) &&
            (ptr[1] > ptr_for_search[1] - accuracy && ptr[1] < ptr_for_search[1] + accuracy) &&
            (ptr[2] > ptr_for_search[2] - accuracy && ptr[2] < ptr_for_search[2] + accuracy)) {
        return true;
    }
    return false;
}

int
search_count_of_color_in_image(struct Png *image, png_byte *ptr_for_search, int size, int yt, int accuracy, int alpha) {
    int count = 0;

    for (int y = yt; y < image->height; ++y) {
        png_byte *row = image->row_pointers[y];
        for (int x = 0; x < image->width; x=x+5) {
            png_byte *ptr = &(row[x * size]);
            if (size == 3) {
                if (is_color(ptr_for_search,ptr,accuracy)) {
                    count++;
                }
            }
            if (size == 4 && ptr[3] > alpha) {
                if (is_color(ptr_for_search,ptr,accuracy)) {
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
    int alpha = 200;
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
        for (int x = 0; x < image->width; x=x+5) {
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
                if (!is_color(ptr_color_for_replace,ptr,accuracy)) {
                    count = search_count_of_color_in_image(image, ptr, size, y, accuracy, alpha);
                    if (count > max) {
                        max = count;
                        memcpy(ptr_color_for_replace, ptr, sizeof(png_byte) * 3);
                    }
                }
            } else if (size == 4 && ptr[3] > alpha) {

                if (!is_color(ptr_color_for_replace,ptr,accuracy)) {
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
            if (size == 3) {
                if (is_color(ptr_color_for_replace,ptr,accuracy)) {
                    for (int i = 0; i < 3; ++i) {
                        ptr[i] = ptr_color[i];
                    }
                }
            } else if (size == 4 && ptr[3] > alpha) {
                if (is_color(ptr_color_for_replace,ptr,accuracy)) {
                    for (int i = 0; i < 3; ++i) {
                        ptr[i] = ptr_color[i];
                    }
                }
            }
        }
    }
}
