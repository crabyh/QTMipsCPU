#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "assembly.h"
#include "stimulate.h"
#include <QTextEdit>
#include <QFile>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    path = QFileDialog::getOpenFileName(this,
    tr("Open File"),
    "‪C:\Users\myh\Desktop",
    tr("Text Files(*.txt)"));
    p=path;
    if(!path.isEmpty()) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(this, tr("Read File"),
    tr("Cannot open file:\n%1").arg(path));
    return;
    }
    QTextStream in(&file);
    ui->textEdit->setText(in.readAll());
    file.close();
    } else {
    QMessageBox::warning(this, tr("Path"),
    tr("You did not select any file."));
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    assembly a;
    QString bin = a.start_assembly(path);
    ui->textEdit_2->append(bin);

    /*QString binpath = p+"../binary.o";
    QFile file(binpath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(this, tr("Read File"),
    tr("Cannot open file:\n%1").arg(binpath));
    return;
    }
    QTextStream in(&file);
    ui->textEdit_2->setText(in.readAll());
    ui->textEdit_2->append(binary);
*/
}

void MainWindow::on_pushButton_3_clicked()
{
    stimulate s;
    QString qbin= ui->textEdit_2->toPlainText();
    QString state=s.start_stimulate(qbin);
    ui->textEdit_3->append(state);
    //int i = qbin.contains( '\t');
    //QString qline=qbin.getline();

}
