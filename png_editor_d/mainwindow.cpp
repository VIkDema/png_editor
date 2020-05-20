#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    this->setWindowFlags(Qt::FramelessWindowHint );
    ui->setupUi(this);
    ui->side_size->setValidator(new QIntValidator);
    ui->line_thicknes->setValidator(new QIntValidator);
    absolute_path=QDir::currentPath();
    absolute_path=absolute_path+name_static_file;
    ui->Hand->click();
    ui->Circular->click();



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QMessageBox::StandardButton reply=QMessageBox::question(this,"Выход","Вы уверены выйти?",QMessageBox::Yes|QMessageBox::No);
    if(reply==QMessageBox::Yes){
        close();
    }


}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this,"Справка","Png_editor - программа для обработки PNG. Вашему вниманию представлены три функции.\n Первая функция позволяет рисовать квадрат, задавая при этом в окне функции параметры.\n");

}

void MainWindow::on_actionOpen_triggered()
{
    path_true_file = QFileDialog::getOpenFileName(0, "Open Dialog", "", "*.png");
    if(path_true_file!=""){
    statusBar()->showMessage("Файл открыт");
    img_before_edit.load(path_true_file);
    w=ui->image->width();
    h=ui->image->height();
    x_max=img_before_edit.width();
    y_max=img_before_edit.height();
    ui->image->setPixmap(img_before_edit.scaled(w,h,Qt::KeepAspectRatio));
    //ui->image->setPixmap(img_before_edit.scaled(w,h,Qt::KeepAspectRatioByExpanding));


     read_png_file((char*)path_true_file.toLocal8Bit().data(),&image_png);
     write_png_file((char*)absolute_path.toLocal8Bit().data(),&image_png);
    /*
     absolute_path=QDir::currentPath();
    absolute_path=absolute_path+name_static_file;
     write_png_file((char*)absolute_path.toLocal8Bit().data(),&image_png);
*/
    }else{
        statusBar()->showMessage("Ничего не выбрано");
    }






}


void MainWindow::get_cord(){
    QPoint cur = ui->image->mapFromGlobal(QCursor::pos());

    if((float)y_max<(float)((x_max*967)/1538)){
        if(y_max>floor((float)((cur.y()*x_max)/(1538))) && x_max>floor((float)((cur.x()*x_max)/(1538)))){
        y_on_img=floor((float)((cur.y()*x_max)/(1538)));
        x_on_img=floor((float)((cur.x()*x_max)/(1538)));
        statusBar()->showMessage("Выбраны x="+QString::number(x_on_img)+" y="+QString::number(y_on_img)+"Изображение размером ("+QString::number(x_max)+","+QString::number(y_max)+")");
        }else{
            statusBar()->showMessage("Вышли за пределы");
        }
    }else{
        if(y_max>floor((float)((cur.y()*y_max)/(967))) && x_max>floor((float)((cur.x()*y_max)/(967)))){
        y_on_img=floor((float)((cur.y()*y_max)/(967)));
        x_on_img=floor((float)((cur.x()*y_max)/(967)));
        statusBar()->showMessage("Выбраны x="+QString::number(x_on_img)+" y="+QString::number(y_on_img)+"Изображение размером ("+QString::number(x_max)+","+QString::number(y_max)+")");
        }else{
            statusBar()->showMessage("Вышли за пределы");
        }
    }
}



void MainWindow::on_image_clicked()
{


    MainWindow::get_cord();

    if(ui->but_for_fun1->isChecked()){

        MainWindow::on_chouse_cord_1_clicked();
        MainWindow::on_push_create_a_square_clicked();
    }

    if(ui->but_for_fun2->isChecked()){
        if(k){
        MainWindow::on_chouse_cord_2_clicked();
        statusBar()->showMessage("Выбор второй координаты");
        k=false;

        }else{
        MainWindow::on_chouse_cord_3_clicked();
        MainWindow::on_Transposition_2_clicked();
        k=true;
        }

    }



}

void MainWindow::on_chouse_cord_1_clicked()
{
     x_for_fun1=x_on_img;
     y_for_fun1=y_on_img;
     ui->x_up_and_y_up1->setText("Выбраны x="+QString::number(x_for_fun1)+"y="+QString::number(y_for_fun1));
}

void MainWindow::on_chouse_cord_2_clicked()
{
    xs_for_fun2=x_on_img;
    ys_for_fun2=y_on_img;
    ui->x_up_and_y_up2->setText("Выбраны x="+QString::number(xs_for_fun2)+"y="+QString::number(ys_for_fun2));
}

void MainWindow::on_chouse_cord_3_clicked()
{
    xe_for_fun2=x_on_img;
    ye_for_fun2=y_on_img;
    ui->x_up_and_y_up3->setText("Выбраны x="+QString::number(xe_for_fun2)+"y="+QString::number(ye_for_fun2));
}

void MainWindow::on_but_for_line_thicknes_clicked()
{

    line_thicknes=ui->line_thicknes->text().toInt();
    statusBar()->showMessage("Выбраны x="+QString::number(x_on_img)+" y="+QString::number(y_on_img)+"Изображение размером ("+QString::number(x_max)+","+QString::number(y_max)+")      Выбрана толщина линии ="+QString::number(line_thicknes));
}

void MainWindow::on_but_for_side_size_clicked()
{
    side_size=ui->side_size->text().toInt();
    statusBar()->showMessage("Выбраны x="+QString::number(x_on_img)+" y="+QString::number(y_on_img)+"Изображение размером ("+QString::number(x_max)+","+QString::number(y_max)+")      Выбрана длина стороны ="+QString::number(side_size));
}



void MainWindow::on_checkBox_clicked()
{
    if(ui->checkBox->isChecked()){
        is_filled=1;
        statusBar()->showMessage(QString::number(is_filled));
    }else{
        is_filled=0;
        statusBar()->showMessage(QString::number(is_filled));
    }
}

void MainWindow::on_color_for_line_clicked()
{
   color_for_line= QColorDialog::getColor();
}

void MainWindow::on_color_for_fill_clicked()
{
    fill_color=QColorDialog::getColor();
}

void MainWindow::on_push_create_a_square_clicked()
{
    read_png_file((char*)absolute_path.toLocal8Bit().data(),&image_png);
    make_square(&image_png,x_for_fun1,y_for_fun1,side_size,line_thicknes,color_for_line,is_filled,fill_color);
    write_png_file((char*)absolute_path.toLocal8Bit().data(),&image_png);
    img_after_edit.load(absolute_path);
    img_before_edit.swap(img_after_edit);
    ui->image->setPixmap(img_before_edit.scaled(w,h,Qt::KeepAspectRatio));
}



void MainWindow::on_Transposition_2_clicked()
{
    if(ui->Diagonal->isChecked()){
        mode=DIAGONAL;
    }else if(ui->Circular->isChecked()){
        mode=CIRCULAR;
    }

    read_png_file((char*)absolute_path.toLocal8Bit().data(),&image_png);
    swap_png(&image_png,xs_for_fun2,ys_for_fun2,xe_for_fun2,ye_for_fun2,mode);
    write_png_file((char*)absolute_path.toLocal8Bit().data(),&image_png);
    img_after_edit.load(absolute_path);
    img_before_edit.swap(img_after_edit);
    ui->image->setPixmap(img_before_edit.scaled(w,h,Qt::KeepAspectRatio));

}


void MainWindow::on_color_for_swap_clicked()
{
    color_for_swap=QColorDialog::getColor();
}

void MainWindow::on_color_swap_clicked()
{
    statusBar()->showMessage("Ожидайте");
    read_png_file((char*)absolute_path.toLocal8Bit().data(),&image_png);
    find_color_and_replacing(&image_png,color_for_swap);
    write_png_file((char*)absolute_path.toLocal8Bit().data(),&image_png);
    statusBar()->showMessage("Выполнено");
    img_after_edit.load(absolute_path);
    img_before_edit.swap(img_after_edit);
    ui->image->setPixmap(img_before_edit.scaled(w,h,Qt::KeepAspectRatio));
}

void MainWindow::on_actionSave_as_triggered()
{
    if(path_true_file==""){
        QMessageBox::warning(this,"Ошибка","Не открыт файл для сохранения");
        return;
    }
    if(path_to_save==""){
        //path_to_save= QFileDialog::getOpenFileName(0, "Open Dialog", "", "*.png");
        QUrl path = QFileDialog::getSaveFileUrl(this,
                 tr("png"),
                 tr("png(*.png);;All Files (*.png)"),"(*.png)");
        path_to_save=path.path();

        read_png_file((char*)absolute_path.toLocal8Bit().data(),&image_png);
        write_png_file((char*)path_to_save.toLocal8Bit().data(),&image_png);

        statusBar()->showMessage("Сохранено:"+path_to_save);
        path_to_save="";
    }else{
        statusBar()->showMessage("Ничего не выбрано");
    }
}

