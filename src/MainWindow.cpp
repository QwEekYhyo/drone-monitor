#include <MainWindow.hpp>
#include <QDockWidget>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  m_mainLayout = new QGridLayout;
  m_mainWidget = new QWidget;
  m_logViewer = new LogViewer;
  m_commands = new Commands;
  m_view3d = new View3D;

  setWindowTitle("Drone Monitoring");
  resize(1920, 1080);

  setupActions();
  setupMenus();
  setupToolbar();
  // Top left
  m_mainLayout->addWidget(QWidget::createWindowContainer(m_view3d), 0, 0);

  // Top right

  // Bottom left
  m_logViewer->setSizePolicy(
      QSizePolicy::Minimum,
      QSizePolicy::Minimum);  // To make logViewer fill the whole cell without
                              // growing too much

  // Bottom right

  m_mainWidget->setLayout(m_mainLayout);
  setCentralWidget(m_mainWidget);

  // Bottom dock
  auto* logDockWidget = new QDockWidget("Logs");
  logDockWidget->setWidget(m_logViewer);
  addDockWidget(Qt::BottomDockWidgetArea, logDockWidget);
  m_viewMenu->addAction(logDockWidget->toggleViewAction());

  m_logViewer->printLog("Starting Drone Monitor...");

  // Commands

  connect(m_commands, &Commands::autoDestruction, this, [=]() {
    m_logViewer->printLog("KABOOM");

    for (auto object : m_biteObject) delete object;

    m_biteObject.clear();
  });
}

void MainWindow::setupMenus() {
  m_viewMenu = menuBar()->addMenu(tr("&View"));
  m_settingsMenu = menuBar()->addMenu(tr("&Settings"));
  m_helpMenu = menuBar()->addMenu(tr("&Help"));

  m_timerMenu = new QMenu("Refresh Rate");
  m_helpMenu->insertActions(nullptr, {m_aboutQtAction, m_aboutAction});
  m_settingsMenu->insertMenu(nullptr, m_timerMenu);
  m_timerMenu->insertActions(nullptr, {m_refreshTen, m_refreshFifty,
                                       m_refreshHundred, m_refreshThousand});
}

void MainWindow::setupActions() {
  m_aboutQtAction = new QAction(tr("About &Qt"), this);
  connect(m_aboutQtAction, &QAction::triggered, this, &MainWindow::showAboutQt);

  m_aboutAction = new QAction(tr("&About"), this);
  connect(m_aboutAction, &QAction::triggered, this, &MainWindow::showAbout);

  m_refreshTen = new QAction(tr("10 ms"));
  m_refreshFifty = new QAction(tr("50 ms"));
  m_refreshHundred = new QAction(tr("100 ms"));
  m_refreshThousand = new QAction(tr("1000 ms"));
}

void MainWindow::showAbout() {
  QMessageBox::about(this, "About",
                     "Drone Monitor, made by Robin 'Peasant' Perdreau and "
                     "debbuged by Romain 'Chad' Chardiny");
}

void MainWindow::showAboutQt() {
  QMessageBox::aboutQt(this);
}

void MainWindow::setupToolbar() {
  QPixmap draw(":/images/draw.png");
  QPixmap center(":/images/center.png");

  m_toolbar = addToolBar("Gorgeous toolbar");

  m_drawFileAction = m_toolbar->addAction(QIcon(draw), "Draw File");

  connect(m_drawFileAction, &QAction::triggered, this, [=] {
    auto targetFilename = QFileDialog::getOpenFileName(
        this, "Open 3D Object", QDir::currentPath(), "Text files (*.txt)");
    if (targetFilename.isEmpty()) return;

    m_biteObject = m_view3d->drawFile(targetFilename);
  });

  m_toolbar->addSeparator();
  m_centerCamAction = m_toolbar->addAction(QIcon(center), "Center Camera");

  connect(m_centerCamAction, &QAction::triggered, this, [=]() {
    m_logViewer->printLog("Centering the camera");
    m_view3d->centerCamera();
  });
}
void MainWindow::setupTimer() {
  m_refreshRate = 1000;
  m_mainTimer = new QTimer;
  m_mainTimer->setInterval(m_refreshRate);
  connect(m_refreshTen, &QAction::triggered, this, [=] {
    m_refreshRate = 10;
  });
  connect(m_refreshFifty, &QAction::triggered, this, [=] {
    m_refreshRate = 50;
  });
  connect(m_refreshHundred, &QAction::triggered, this, [=] {
    m_refreshRate = 100;
  });
  connect(m_refreshThousand, &QAction::triggered, this, [=] {
    m_refreshRate = 1000;
  });
  m_mainTimer->start();
}
