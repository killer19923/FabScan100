#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "fscontroller.h"

#include <QBasicTimer>

#include "qextserialport.h"
#include "qextserialenumerator.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    hwTimer(new QBasicTimer),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    hwTimer->start(5000, this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_myButton_clicked()
{
    //image= cv::imread("cube.png");
    //cv::namedWindow("Original Image");
    //cv::imshow("Original Image", image);
    ui->label->setText("Press a button to close...");
    qDebug("Hello World!");

    // Open the video file
          cv::VideoCapture capture(-1);
          // check if video successfully opened
          if (!capture.isOpened()) return;
          // Get the frame rate
          double rate= capture.get(CV_CAP_PROP_FPS);
          bool stop(false);
          cv::Mat frame; // current video frame
          cv::namedWindow("Extracted Frame");
          // Delay between each frame in ms
          // corresponds to video frame rate
          int delay= 1000/rate;
          // for all frames in video
          while (!stop) {
             // read next frame if any
             if (!capture.read(frame))
                break;
             cv::imshow("Extracted Frame",frame);
             // introduce a delay
             // or press key to stop
             if (cv::waitKey(1)>=0)
            stop = true;
    }
          // Close the video file.
          // Not required since called by destructor
          capture.release();
          cvDestroyWindow("Extracted Frame");
          ui->label->setText("Idling...");
}

void MainWindow::on_convertButton_clicked()
{
    qDebug("converting...");
    FSController::getInstance()->model->convertPointCloudToSurfaceMesh();
    FSController::getInstance()->geometries->setSurfaceMeshTo(
                FSController::getInstance()->model->triangles,
                FSController::getInstance()->model->pointCloud);
    ui->widget->drawState = 1; //display surface mesh
    ui->widget->updateGL();
}


void MainWindow::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open File","","Files (*.pcd)");
    FSController::getInstance()->model->loadPointCloud(fileName.toStdString());
    ui->widget->updateGL();
}


void MainWindow::on_toggleViewButton_clicked()
{
    char currentDrawState = ui->widget->drawState;
    ui->widget->drawState = 1-currentDrawState;
    ui->widget->updateGL();
}

void MainWindow::timerEvent(QTimerEvent *e)
{
    Q_UNUSED(e);
    this->updateConnectedSerialPorts();
}

void MainWindow::selectSerialPort()
{
    QAction* action=qobject_cast<QAction*>(sender());
    if(!action) return;
    action->setChecked(true);
    qDebug() << action->iconText();
}

void MainWindow::updateConnectedSerialPorts()
{
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
    ui->menuSerialPort->clear();

    foreach (QextPortInfo info, ports) {
        if(!info.portName.isEmpty())
        //ui->menuSerialPort->addAction(info.portName,)
        ui->menuSerialPort->addAction(info.portName, this, SLOT(selectSerialPort()));
        //qDebug() << "port name:"       << info.portName;
        //qDebug() << "friendly name:"   << info.friendName;
        //qDebug() << "physical name:"   << info.physName;
        //qDebug() << "enumerator name:" << info.enumName;
        //qDebug() << "vendor ID:"       << info.vendorID;
        //qDebug() << "product ID:"      << info.productID;
        //qDebug() << "===================================";
    }
}
