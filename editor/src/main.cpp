#include "include/leveleditor.h"
#include <QApplication>
#include <iostream>
#include <Windows.h>
#include "DarkStyle.h"
#include "framelesswindow/framelesswindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LevelEditor w;
    //w.show();
    // style our application with custom dark style
    a.setStyle(new DarkStyle);

    // create frameless window (and set windowState or title)
    FramelessWindow framelessWindow;
    framelessWindow.setWindowState(Qt::WindowMaximized);
    framelessWindow.setWindowTitle("Epsilon Level Editor");
    //framelessWindow.setWindowIcon(a.style()->standardIcon(QStyle::SP_DesktopIcon));
    framelessWindow.setUpdatesEnabled(true);

    // create our mainwindow instance

    //LevelEditor *mainWindow = new LevelEditor;

    //mainWindow->setWindowState(Qt::WindowMaximized);
    //mainWindow->showMaximized();

    // add the mainwindow to our custom frameless window
    framelessWindow.setContent(w);
    framelessWindow.show();
    framelessWindow.setWindowState(Qt::WindowMaximized);
    //framelessWindow.showMaximized();

    return a.exec();
}
