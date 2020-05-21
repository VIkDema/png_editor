#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QMessageBox>
#include <QFileDialog>
#include <QLabel>
#include <math.h>
#include "png_edit.h"
#include <QFile>
#include <QMouseEvent>
#include <QDir>
#include <QColorDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);


    ~MainWindow();

private slots:
    void on_actionQuit_triggered();

    void on_actionAbout_triggered();

    void on_actionOpen_triggered();


    void on_image_clicked();

    void on_chouse_cord_1_clicked();

    void on_chouse_cord_2_clicked();

    void on_chouse_cord_3_clicked();

    void on_but_for_line_thicknes_clicked();

    void on_but_for_side_size_clicked();

    void on_checkBox_clicked();

    void on_color_for_line_clicked();

    void on_color_for_fill_clicked();

    void on_push_create_a_square_clicked();

    void get_cord();

    void on_Transposition_2_clicked();

    void on_color_for_swap_clicked();

    void on_color_swap_clicked();

    void on_actionSave_as_triggered();


private:
    Ui::MainWindow *ui;
    QString path_true_file="";
    QString name_static_file="static.png";
    QString path_to_save="";
    QString absolute_path;
    QColor color_for_line;
    QColor color_for_swap;
    QColor fill_color;
    QPixmap img_before_edit;
    QPixmap img_after_edit;
    int x_on_img=0;
    int y_on_img=0;
    int x_max=0;
    int y_max=0;
    int x_for_fun1=0;
    int y_for_fun1=0;
    int xs_for_fun2=0;
    int ys_for_fun2=0;
    int xe_for_fun2=0;
    int ye_for_fun2=0;
    int side_size=0;
    int line_thicknes=0;
    struct Png image_png;
    int is_filled=0;
     int h;
     int w;
     int mode=CIRCULAR;
     bool k=true;
     bool file_is_open=false;


};

#endif // MAINWINDOW_H
