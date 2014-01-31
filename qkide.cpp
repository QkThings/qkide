#include "qkide.h"
#include "ui_qkide.h"

#include "qkide_global.h"
#include "core/qkproject.h"
#include "core/projectwizard.h"
#include "widgets/ptextdock.h"
#include "browser/browser.h"
#include "editor/editor.h"
#include "editor/codeparser.h"
#include "editor/highlighter.h"
#include "editor/completer.h"

#include "core/optionsdialog.h"
#include "ui_optionsdialog.h"

#include "qkconnect.h"
#include "qkconnectthread.h"
#include "qkexplorerwidget.h"

#include <QtGlobal>
#include <QDebug>
#include <QSettings>
#include <QDateTime>
#include <QFileDialog>
#include <QWizard>
#include <QWizardPage>
#include <QInputDialog>
#include <QDir>
#include <QProcess>
#include <QMessageBox>
#include <QTextEdit>
#include <QPalette>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QRegExp>
#include <QtSerialPort/QSerialPortInfo>

QkIDE::QkIDE(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QkIDE),
    m_curProject(0)
{
    ui->setupUi(this);

    m_browser = new Browser(this);
    connect(m_browser, SIGNAL(createProject()), this, SLOT(slotCreateProject()));
    connect(m_browser, SIGNAL(openProject()), this, SLOT(slotOpenProject()));
    connect(m_browser, SIGNAL(openRecentProject(int)), this, SLOT(slotOpenRecentProject(int)));
    //m_browser->hide();

    m_editor = new Editor(this);

    m_stackedWidget = new QStackedWidget();
    m_stackedWidget->addWidget(m_browser);
    m_stackedWidget->addWidget(m_editor);

    m_outputWindow = new pTextDock(tr("Output"), QColor("#444"), this);
    m_outputWindow->textEdit()->setReadOnly(true);
    m_outputWindow->textEdit()->setWordWrapMode(QTextOption::WordWrap);
    m_outputWindow->setDefaultTextColor(Qt::white);
    m_outputWindow->setFeatures(pTextDock::DockWidgetMovable | pTextDock::DockWidgetClosable);
    m_outputWindow->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::RightDockWidgetArea);
    m_outputWindow->hide();


//    m_testAct = new QAction(tr("TEST"), this);
//    connect(m_testAct, SIGNAL(triggered()), this, SLOT(slotTest()));

    m_cleanProcess = new QProcess(this);
    m_cleanProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_cleanProcess, SIGNAL(started()), this, SLOT(slotCleanProcessStarted()));
    connect(m_cleanProcess, SIGNAL(readyRead()), this, SLOT(slotCleanProcessOutput()));
    connect(m_cleanProcess, SIGNAL(finished(int)), this, SLOT(slotProcessFinished()));

    m_verifyProcess = new QProcess(this);
    m_verifyProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_verifyProcess, SIGNAL(started()), this, SLOT(slotVerifyProcessStarted()));
    connect(m_verifyProcess, SIGNAL(readyRead()), this, SLOT(slotVerifyProcessOutput()));
    connect(m_verifyProcess, SIGNAL(finished(int)), this, SLOT(slotProcessFinished()));

    m_uploadProcess = new QProcess(this);
    m_uploadProcess->setProcessChannelMode(QProcess::MergedChannels);
    m_uploadProcess->setWorkingDirectory(QApplication::applicationDirPath());
    connect(m_uploadProcess, SIGNAL(started()), this, SLOT(slotUploadProcessStarted()));
    connect(m_uploadProcess, SIGNAL(readyRead()), this, SLOT(slotUploadProcessOutput()));
    connect(m_uploadProcess, SIGNAL(finished(int)), this, SLOT(slotUploadProcessFinished()));


    QDir().mkdir(QApplication::applicationDirPath() + TEMP_DIR);

    m_codeParser = new CodeParser();

    QString qkprogramDir = QApplication::applicationDirPath() + QKPROGRAM_DIR;

    CodeParser *parser = m_codeParser;
    parser->parse(qkprogramDir);
    m_libElements.append(parser->allElements());

    Highlighter::addElements(m_libElements, true);

    m_parserTimer = new QTimer(this);
    m_parserTimer->setInterval(500);
    m_parserTimer->setSingleShot(true);
    connect(m_parserTimer, SIGNAL(timeout()), this, SLOT(slotParse()));

    connect(m_codeParser, SIGNAL(parsed()), this, SLOT(slotParsed()));

    connect(this, SIGNAL(currentProjectChanged()), this, SLOT(slotCurrentProjectChanged()));

    createActions();
    createMenus();
    createToolbars();
    createExamples();
    setupLayout();
    readSettings();

    m_serialConn = new QkSerialConnection(m_uploadPortName, 38400, this);
    connect(m_serialConn, SIGNAL(error(QString)), this, SLOT(slotError(QString)));

//    QkSerialConnection *serialConn = new QkSerialConnection(m_uploadPortName, 38400);
//    QThread* thread = new QThread;
//    serialConn->setParent(0);
//    serialConn->moveToThread(thread);
//    connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
//    connect(thread, SIGNAL(started()), worker, SLOT(process()));
//    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
//    thread->start();

    //m_serialConn = serialConn;

//    QkConnection::Descriptor connDesc;
//    connDesc.type = QkConnection::ctSerial;
//    connDesc.params.append(m_uploadPortName);
//    connDesc.params.append(QString::number(38400));
//    m_connThread = new QkConnectionThread(connDesc);
////    connect(clientThread, SIGNAL(clientConnected(int)),
////            this, SLOT(_handleClientConnected(int)));
////    connect(clientThread, SIGNAL(clientDisconnected(int)),
////            this, SLOT(_handleClientDisconnected(int)), Qt::DirectConnection);
//    connect(m_connThread, SIGNAL(finished()), m_connThread, SLOT(deleteLater()));
//    m_connThread->start();


//    m_serialConn = m_connThread->connection();
//    qDebug() << "serial conn:" << (m_serialConn == 0);
//    qDebug() << m_serialConn->descriptor().type << m_serialConn->descriptor().params;
//    if(m_serialConn == 0)
//        qDebug() << "serialConn == 0";

    m_explorerWidget = new QkExplorerWidget(this);
    m_explorerWidget->setCurrentConnection(m_serialConn);
    m_explorerWidget->setModeFlags(QkExplorerWidget::mfSingleNode | QkExplorerWidget::mfSingleConnection);

    m_explorerDock = new QDockWidget(tr("Explorer"), this);
    m_explorerDock->setWidget(m_explorerWidget);
    m_explorerDock->hide();

    addDockWidget(Qt::RightDockWidgetArea, m_explorerDock, Qt::Horizontal);

//    m_connThread = new QThread();
//    m_serialConn->setParent(0);
//    m_serialConn->moveToThread(m_connThread);
//    connect(m_serialConn, SIGNAL(destroyed()), m_connThread, SLOT(quit()));
//    connect(m_connThread, SIGNAL(finished()), m_connThread, SLOT(deleteLater()));
//    m_connThread->start();

    slotReloadSerialPorts();
    updateInterface();
}

QkIDE::~QkIDE()
{
    delete m_editor;
    delete m_browser;
    delete ui;
}

void QkIDE::createActions()
{
    m_searchAct = new QAction(tr("Search..."), this);
    m_searchAct->setShortcuts(QKeySequence::Find);
    connect(m_searchAct, SIGNAL(triggered()), this, SLOT(slotSearch()));

    m_homeAct = new QAction(QIcon(":/img/home.png"), "", this);
    m_homeAct->setCheckable(true);
    m_homeAct->setChecked(true);
    m_homeAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_H));
    connect(m_homeAct, SIGNAL(toggled(bool)), this, SLOT(slotHome(bool)));

    m_createProjectAct = new QAction(QIcon(":/img/create_project.png"),tr("New Project"),this);
    connect(m_createProjectAct, SIGNAL(triggered()), this, SLOT(slotCreateProject()));

    m_openProjectAct = new QAction(QIcon(":/img/open_project.png"),tr("Open Project..."),this);
    m_openProjectAct->setShortcuts(QKeySequence::Open);
    connect(m_openProjectAct, SIGNAL(triggered()), this, SLOT(slotOpenProject()));

    m_newFileAct = new QAction(QIcon(":/img/page_add.png"),tr("New Empty File"),this);
    //connect(newFileAct, SIGNAL(triggered()), this, SLOT(addNewFile()));

    m_saveProjectAct = new QAction(QIcon(":/img/save.png"),tr("Save Project"),this);
    m_saveProjectAct->setShortcut(QKeySequence::Save);
    connect(m_saveProjectAct, SIGNAL(triggered()), this, SLOT(slotSaveProject()));

    m_saveAsProjectAct = new QAction(tr("Save As..."),this);
    connect(m_saveAsProjectAct, SIGNAL(triggered()), this, SLOT(slotSaveAsProject()));

    m_showProjectFolderAct = new QAction(tr("Show Folder"), this);
    connect(m_showProjectFolderAct, SIGNAL(triggered()), this, SLOT(slotShowFolder()));

    m_ProjectPreferencesAct = new QAction(tr("Preferences"), this);
    connect(m_ProjectPreferencesAct, SIGNAL(triggered()), this, SLOT(slotProjectPreferences()));


    m_zoomInAct = new QAction(QIcon(":/img/zoom_in.png"),tr("Zoom In"),this);
    m_zoomOutAct = new QAction(QIcon(":/img/zoom_out.png"),tr("Zoom Out"),this);
    connect(m_zoomInAct, SIGNAL(triggered()), this, SLOT(slotZoomIn()));
    connect(m_zoomOutAct, SIGNAL(triggered()), this, SLOT(slotZoomOut()));

    m_undoAct = new QAction(QIcon(":/img/arrow_undo.png"),tr("Undo"),this);
    m_undoAct->setShortcuts(QKeySequence::Undo);
    connect(m_undoAct, SIGNAL(triggered()), this, SLOT(slotUndo()));
    m_redoAct = new QAction(QIcon(":/img/arrow_redo.png"),tr("Redo"),this);
    m_redoAct->setShortcuts(QKeySequence::Redo);
    connect(m_redoAct, SIGNAL(triggered()), this, SLOT(slotRedo()));

    m_cleanAct = new QAction(QIcon(":/img/clean.png"),tr("Clean"),this);
    m_cleanAct->setStatusTip(tr("Clean"));
    m_verifyAct = new QAction(QIcon(":/img/verify.png"),tr("Verify"),this);
    m_verifyAct->setStatusTip(tr("Verify"));
    m_verifyAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    m_uploadAct = new QAction(QIcon(":/img/upload.png"),tr("Upload"),this);
    m_uploadAct->setStatusTip(tr("Upload"));
    m_uploadAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
    connect(m_cleanAct, SIGNAL(triggered()), this, SLOT(slotClean()));
    connect(m_verifyAct, SIGNAL(triggered()), this, SLOT(slotVerify()));
    connect(m_uploadAct, SIGNAL(triggered()), this, SLOT(slotUpload()));

    m_explorerAct = new QAction(QIcon(":/img/explorer.png"), tr("Show/Hide Explorer"), this);
    connect(m_explorerAct, SIGNAL(triggered()), this, SLOT(slotShowHideExplorer()));

//    m_targetAct = new QAction(QIcon(":/img/target_16.png"), tr("Show/Hide Target"), this);
//    connect(m_targetAct, SIGNAL(triggered()), this, SLOT(slotShowHideTarget()));

//    m_connectAct = new QAction(QIcon(":/img/connect_16.png"), tr("Show/Hide Target"), this);
//    connect(m_connectAct, SIGNAL(triggered()), this, SLOT(slotShowHideConnection()));

    m_toggleFoldAct = new QAction(tr("Toggle Fold"),this);
    m_toggleFoldAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F10));
    connect(m_toggleFoldAct, SIGNAL(triggered()), this, SLOT(slotToggleFold()));

    m_fullScreenAct = new QAction(tr("Full Screen"), this);
    m_fullScreenAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F11));
    m_fullScreenAct->setCheckable(true);
    connect(m_fullScreenAct, SIGNAL(triggered(bool)), this, SLOT(slotFullScreen(bool)));

    m_splitHorizontalAct = new QAction(tr("Split Horizontal"), this);
    connect(m_splitHorizontalAct, SIGNAL(triggered()), this, SLOT(slotSplitHorizontal()));
    m_splitVerticalAct = new QAction(tr("Split Vertical"), this);
    connect(m_splitVerticalAct, SIGNAL(triggered()), this, SLOT(slotSplitVertical()));
    m_removeSplitAct = new QAction(tr("Remove Split"), this);
    connect(m_removeSplitAct, SIGNAL(triggered()), this, SLOT(slotRemoveSplit()));
    m_removeSplitAct->setEnabled(false);

    m_optionsAct = new QAction(tr("Options..."), this);
    connect(m_optionsAct, SIGNAL(triggered()), this, SLOT(slotOptions()));

    m_aboutAct = new QAction(tr("About"),this);
    //connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    m_exitAct = new QAction(tr("Exit"), this);
    //connect(exitAct, SIGNAL(triggered()), this, SLOT(close()))


    for (int i = 0; i < MaxRecentProjects; ++i) {
        m_recentProjectsActs[i] = new QAction(QString::number(i),this);
        m_recentProjectsActs[i]->setVisible(false);
        connect(m_recentProjectsActs[i], SIGNAL(triggered()),
                this, SLOT(slotOpenRecentProject()));
    }
}

void QkIDE::createMenus()
{
    m_fileMenu = new QMenu(tr("&File"));
    m_examplesMenu = new QMenu(tr("Examples"));
    m_recentProjectsMenu = new QMenu(tr("Recent projects..."));
    m_editMenu = new QMenu(tr("&Edit"));
    m_viewMenu = new QMenu(tr("&View"));
    m_projectMenu = new QMenu(tr("&Project"));
    m_toolsMenu = new QMenu(tr("&Tools"));
    m_windowMenu = new QMenu(tr("&Window"));
    m_helpMenu = new QMenu(tr("&Help"));

    for(int i = 0; i < MaxRecentProjects; i++)
        m_recentProjectsMenu->addAction(m_recentProjectsActs[i]);

    m_fileMenu->addMenu(m_recentProjectsMenu);
    m_fileMenu->addMenu(m_examplesMenu);
    m_fileMenu->addSeparator();
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAct);

    m_viewMenu->addAction(m_toggleFoldAct);

    m_editMenu->addAction(m_undoAct);
    m_editMenu->addAction(m_redoAct);
    m_editMenu->addSeparator();
    m_editMenu->addAction(m_searchAct);

    m_projectMenu->addAction(m_createProjectAct);
    m_projectMenu->addAction(m_openProjectAct);
    m_projectMenu->addAction(m_saveProjectAct);
    m_projectMenu->addAction(m_saveAsProjectAct);
    m_projectMenu->addSeparator();
    m_projectMenu->addAction(m_showProjectFolderAct);
    m_projectMenu->addSeparator();
    m_projectMenu->addAction(m_ProjectPreferencesAct);
    m_projectMenu->addSeparator();
    m_projectMenu->addAction(m_verifyAct);
    m_projectMenu->addAction(m_uploadAct);

    m_toolsMenu->addAction(m_optionsAct);

    m_windowMenu->addAction(m_fullScreenAct);
    m_windowMenu->addSeparator();
    m_windowMenu->addAction(m_splitHorizontalAct);
    m_windowMenu->addAction(m_splitVerticalAct);
    m_windowMenu->addAction(m_removeSplitAct);

    m_helpMenu->addAction(m_aboutAct);

    ui->menuBar->clear();
    QList<QAction *> list;
    list.append(m_homeAct);
    ui->menuBar->addActions(list);
    ui->menuBar->addSeparator();
    ui->menuBar->addMenu(m_fileMenu);
    ui->menuBar->addMenu(m_editMenu);
    //ui->menuBar->addMenu(m_viewMenu);
    ui->menuBar->addMenu(m_projectMenu);
    ui->menuBar->addMenu(m_toolsMenu);
    ui->menuBar->addMenu(m_windowMenu);
    ui->menuBar->addMenu(m_helpMenu);
}

void QkIDE::createToolbars()
{
    ui->mainToolBar->setFloatable(false);

    ui->mainToolBar->setWindowTitle(tr("edit toolbar"));
    //ui->mainToolBar->addAction(newFileAct);
    //ui->mainToolBar->addAction(m_homeAct);
    ui->mainToolBar->addAction(m_createProjectAct);
    ui->mainToolBar->addAction(m_openProjectAct);
    ui->mainToolBar->addAction(m_saveProjectAct);
    //ui->mainToolBar->addAction(m_explorerAct);
    //ui->mainToolBar->addSeparator();
    //ui->mainToolBar->addAction(m_undoAct);
    //ui->mainToolBar->addAction(m_redoAct);
    //ui->mainToolBar->addAction(m_zoomInAct);
    //ui->mainToolBar->addAction(m_zoomOutAct);

    ui->mainToolBar->setIconSize(QSize(16,16));
    ui->mainToolBar->setMovable(true);
    ui->mainToolBar->setFloatable(false);

    m_programToolBar = new QToolBar(tr("builder toolbar"));
    m_programToolBar->setMovable(true);
    m_programToolBar->setFloatable(false);
    m_programToolBar->setIconSize(QSize(16,16));

    //m_programToolBar->addAction(m_explorerAct);
    m_programToolBar->addAction(m_cleanAct);
    m_programToolBar->addAction(m_verifyAct);
    m_programToolBar->addAction(m_uploadAct);

    m_qkToolbar = new QToolBar(tr("qk toolbar"));
    m_qkToolbar->setMovable(true);
    m_qkToolbar->setFloatable(false);
    m_qkToolbar->setIconSize(QSize(16,16));

    m_qkToolbar->addAction(m_explorerAct);
//    m_qkToolbar->addAction(m_targetAct);
//    m_qkToolbar->addAction(m_connectAct);
//    m_qkToolbar->addAction(m_testAct);

    m_comboTarget = new QComboBox(m_qkToolbar);
    m_comboTarget->setMinimumWidth(120);

    m_buttonRefreshPorts = new QAction(QIcon(":/img/reload.png"),tr("Reload"),this);
    connect(m_buttonRefreshPorts, SIGNAL(triggered()), this, SLOT(slotReloadSerialPorts()));

    m_comboPort = new QComboBox(m_qkToolbar);
    m_comboPort->addItem("ttyACM0");

//    m_comboBaud = new QComboBox(m_qkToolbar);
//    m_comboBaud->addItem("38400");

    m_buttonConnect = new QPushButton(tr("Connect"),m_qkToolbar);

    QWidget *spacer = new QWidget(m_qkToolbar);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_qkToolbar->addWidget(spacer);
    m_qkToolbar->addWidget(m_comboTarget);
    m_qkToolbar->addSeparator();
    m_qkToolbar->addAction(m_buttonRefreshPorts);
    m_qkToolbar->addWidget(m_comboPort);
    //m_qkToolbar->addWidget(m_comboBaud);
    m_qkToolbar->addWidget(m_buttonConnect);

    addToolBar(m_programToolBar);
    addToolBar(m_qkToolbar);
}

void QkIDE::createExamples()
{
    qDebug() << "create examples";

    QMenu *menu;
    QDir projectsDir, topicsDir("examples/");
    QStringList topicNames;
    QStringList projectNames;

    topicNames = topicsDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

    foreach(QString topic, topicNames) {
        menu = m_examplesMenu->addMenu(topic);

        QString path = topicsDir.path() + SLASH + topic + SLASH;
        projectsDir.setPath(path);

        projectNames = projectsDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
        qDebug() << projectsDir.path() << projectNames;
        foreach(QString project, projectNames) {
            QAction *act = menu->addAction(project);
            QString projectPath = projectsDir.path() + SLASH +
                                  project + SLASH + project + ".qkpro";
            act->setData(projectPath);
            connect(act, SIGNAL(triggered()), this, SLOT(slotOpenExample()));
         }
    }
}

void QkIDE::setupLayout()
{
    setDockOptions(QMainWindow::AllowNestedDocks);

    addDockWidget(Qt::BottomDockWidgetArea, m_outputWindow);

    setCentralWidget(m_stackedWidget);

    resize(680,600);

    setWindowIcon(QIcon(":/img/application.png"));
}

void QkIDE::readSettings()
{
    int i, size;
    QSettings settings;

    settings.beginGroup("mainWindow");
    /*resize(settings.value("width", QVariant(500)).toInt(),
           settings.value("heigth", QVariant(500)).toInt());
    if(settings.value("maximized", QVariant(false)).toBool())
        showMaximized();*/
    settings.endGroup();

    settings.beginGroup("preferences");
    m_uploadPortName = settings.value("serialPort").toString();
    m_projectDefaultLocation = settings.value("projectDefaultLocation").toString();
    settings.endGroup();

    size = settings.beginReadArray("recentProjectsList");
    size = qMin(size, (int)MaxRecentProjects);
    for(i = 0; i < size; i++) {
        settings.setArrayIndex(i);
        RecentProject recent;
        recent.name = settings.value("name").toString();
        recent.path = settings.value("path").toString();
        m_recentProjects.append(recent);
    }
    settings.endArray();

    updateRecentProjects();
}

void QkIDE::writeSettings()
{
    QSettings settings;
    int i;

    if(m_recentProjects.count() > MaxRecentProjects) {
        qDebug() << "recentProjects.count() greater than" << MaxRecentProjects;
        int excess = m_recentProjects.count() - MaxRecentProjects;
        while(excess-- > 0)
            m_recentProjects.removeLast();
    }

    settings.remove("recentProjectsList");

    settings.beginWriteArray("recentProjectsList");
    for(i = 0; i < m_recentProjects.count(); i++) {
        settings.setArrayIndex(i);
        settings.setValue("name",m_recentProjects[i].name);
        settings.setValue("path",m_recentProjects[i].path);
    }
    settings.endArray();

    qDebug() << "settings written";
}

void QkIDE::slotHome(bool go)
{
    if(go) {
        m_homeAct->setChecked(true);
        m_stackedWidget->setCurrentIndex(0);
        ui->statusBar->showMessage(tr("Home"),1000);
    }
    else {
        m_homeAct->setChecked(false);
        m_stackedWidget->setCurrentIndex(1);
        ui->statusBar->showMessage(tr("Editor"),1000);
    }
}

void QkIDE::slotOptions()
{
    int i;
    QSettings settings;
    OptionsDialog dialog(this);

    if(dialog.exec() == QDialog::Accepted)
    {
        m_uploadPortName = dialog.ui->comboPortName->currentText();
        m_serialConn->setPortName(m_uploadPortName);

        ui->statusBar->showMessage(tr("Saving preferences..."));
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        settings.beginGroup("preferences");
        settings.setValue("serialPort", m_uploadPortName);
        settings.setValue("projectDefaultLocation", m_projectDefaultLocation);
        settings.endGroup();
        QApplication::restoreOverrideCursor();
        ui->statusBar->clearMessage();
    }
}

void QkIDE::slotOpenExample()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if(action == 0) return;

    qDebug() << "open example" << action->data().toString();

    openProject(action->data().toString());
}

void QkIDE::slotOpenRecentProject()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action){
        qDebug() << "open recent project:" << action->data().toString();
        RecentProject recent;
        foreach(RecentProject rp, m_recentProjects) {
            if(rp.name.compare(action->data().toString()) == 0){
                recent = rp;
                break;
            }
        }
        openProject(recent.path + recent.name + ".qkpro");
    }
}

void QkIDE::slotOpenRecentProject(int i)
{
    RecentProject recent = m_recentProjects.at(i);
    qDebug() << "open recent" << recent.path + recent.name + ".qkpro";
    openProject(recent.path + recent.name + ".qkpro");
}

void QkIDE::slotCreateProject()
{
    ProjectWizard projectWizard(this, this);
    if(projectWizard.exec() == QDialog::Accepted) {
        slotCloseProject();
        QString name = projectWizard.projectName;
        QString path = projectWizard.createIn;
        m_curProject = createProject(name);
        path = path.replace('\\', SLASH);
        path.append(SLASH + name + SLASH);
        m_curProject->setPath(path);
        qDebug() << "new project path" << m_curProject->path();
        m_curProject->update();
        slotSaveAllFiles();
        updateCurrentProject();
        updateInterface();

        emit currentProjectChanged();
    }
}

void QkIDE::slotOpenProject()
{
    QString filter("(*.qkpro)");
    QString path = QFileDialog::getOpenFileName(this, tr("Open project"), "",
                                                filter);

    if(!path.isEmpty())
        openProject(path);
}

void QkIDE::slotCloseProject()
{
    if(m_curProject == 0)
        return;

    if(m_editor->hasModifiedPages()) {
        int r = QMessageBox::question(this, tr("Close"),
                                      tr("Save changes for \"") + m_curProject->name() + "\"?",
                                      QMessageBox::Save | QMessageBox::Discard, QMessageBox::Save);
        if(r == QMessageBox::Save) {
            slotSaveProject();
        }
    }

    m_editor->closeAllPages();
    delete m_curProject;
}

void QkIDE::slotSaveProject()
{
    slotSaveAllFiles();
    updateCurrentProject();
    ui->statusBar->showMessage("Saved", 2000);
}

void QkIDE::slotSaveAsProject()
{
    QString path = QFileDialog::getExistingDirectory(this,tr("Save As..."));
    path.replace('\\', SLASH);
    path.append(SLASH);
    qDebug() << "save as new path" << path;
    m_curProject->setPath(path);
    m_curProject->update();
    slotSaveAllFiles();
    updateCurrentProject();

    ui->statusBar->showMessage(tr("Project saved on \"") + path + "\"", 3000);
}

void QkIDE::slotSaveAllFiles()
{
    if(m_curProject->path().isEmpty()){
        qDebug() << "can't save files: project path is empty";
        return;
    }

    if(m_curProject->files().isEmpty()) {
        qDebug() << "project has no files to be saved";
        return;
    }

    for(int i=0; i < m_editor->countPages(); i++)
    {
        QString filePath = m_curProject->path() + SLASH + m_editor->page(i)->name();
        m_editor->savePage(i,filePath);
    }

    //m_curProject->save();
}

void QkIDE::slotShowFolder()
{
    QDesktopServices::openUrl( QUrl::fromLocalFile(m_curProject->path()) );
}

void QkIDE::slotProjectPreferences()
{

}

void QkIDE::slotSearch()
{
    m_editor->showSearch();
}

void QkIDE::slotUndo()
{
    m_editor->currentPage()->undo();
}

void QkIDE::slotRedo()
{
    m_editor->currentPage()->redo();
}

void QkIDE::slotZoomIn()
{
    //m_editor->currentPage()->zoomIn();
}

void QkIDE::slotZoomOut()
{
    //m_editor->currentPage()->zoomOut();
}

void QkIDE::slotClean()
{
    //slotSaveProject();
    deleteMakefile(m_curProject);
    createMakefile(m_curProject);

    qDebug() << "clean";

    QString appDir = QApplication::applicationDirPath();
    QString makeCmd;
#ifdef Q_OS_WIN
    makeCmd = appDir + GNUWIN_DIR + "/bin/make.exe";
#else
    makeCmd = "make";
#endif

    qDebug() << makeCmd;

    QString program = makeCmd;
    QStringList arguments("clean");

    m_cleanProcess->setWorkingDirectory(m_curProject->path());
    m_cleanProcess->waitForFinished();
    m_cleanProcess->start(program, arguments);
}

void QkIDE::slotCleanProcessStarted()
{
    m_outputWindow->clear();
    m_outputWindow->show();
    ui->statusBar->showMessage(tr("Cleaning..."));
}

void QkIDE::slotCleanProcessOutput()
{
    m_outputWindow->append(m_cleanProcess->readAll());
}

void QkIDE::slotVerify()
{
    //slotSaveProject();
    deleteMakefile(m_curProject);
    createMakefile(m_curProject);

    qDebug() << "verify";

    QString appDir = QApplication::applicationDirPath();
    QString makeCmd;
#ifdef Q_OS_WIN
    makeCmd = appDir + GNUWIN_DIR + "/bin/make.exe";
#else
    makeCmd = "make";
#endif

    QString program = makeCmd;
    QStringList arguments;

    m_verifyProcess->setWorkingDirectory(m_curProject->path());
    m_verifyProcess->waitForFinished();
    m_verifyProcess->start(program, arguments);
}

void QkIDE::slotVerifyProcessStarted()
{
    m_outputWindow->clear();
    m_outputWindow->show();
    ui->statusBar->showMessage(tr("Compiling..."));
}

void QkIDE::slotVerifyProcessOutput()
{
    while(m_verifyProcess->canReadLine())
    {
        QString line = m_verifyProcess->readLine();
        line.chop(1);
        if(line.toLower().contains("error"))
            m_outputWindow->append(line, QColor("#EB8679"));
        else if(line.toLower().contains("warning"))
            m_outputWindow->append(line, QColor("#F5EFB3"));
        else
            m_outputWindow->append(line);
    }
}

void QkIDE::slotUpload()
{
    qDebug() << "upload";

    if(m_serialConn->isConnected())
        m_serialConn->close();

    QString appDir = QApplication::applicationDirPath();
    QString makeCmd;
#ifdef Q_OS_WIN
    makeCmd = appDir + GNUWIN_DIR + "/bin/make.exe";
#else
    makeCmd = "make";
#endif

    QString program = makeCmd;
    QStringList arguments;
    arguments << "upload";
#ifdef Q_OS_WIN
    arguments << "PORT=" + m_uploadPortName;
#else
    arguments << "PORT=/dev/" + m_uploadPortName;
#endif

    m_verifyProcess->setWorkingDirectory(m_curProject->path());
    m_verifyProcess->waitForFinished();
    m_verifyProcess->start(program, arguments);
}

void QkIDE::slotUploadProcessStarted()
{
    ui->statusBar->showMessage(tr("Uploading"));
    m_outputWindow->show();
    m_outputWindow->clear();
    m_outputWindow->append("Uploading...");
}

void QkIDE::slotUploadProcessOutput()
{
    QString text = QString(m_uploadProcess->readAll());
    m_outputWindow->append(text);
}

void QkIDE::slotUploadProcessFinished()
{
    m_serialConn->open();
    ui->statusBar->showMessage(tr("Uploaded"), 1500);
}

void QkIDE::slotProcessFinished()
{
    //m_outputWindow->append(tr("Done"));
    ui->statusBar->showMessage(tr("Done"), 1500);
}

void QkIDE::slotShowHideExplorer()
{
    if(m_explorerDock->isVisible())
        m_explorerDock->hide();
    else
        m_explorerDock->show();
}

void QkIDE::slotShowHideTarget()
{
    if(m_comboTarget->isVisible())
        m_comboTarget->hide();
    else
        m_comboTarget->show();
}

void QkIDE::slotShowHideConnect()
{
    bool hide = m_buttonConnect->isVisible();
    m_buttonConnect->setHidden(hide);
    m_comboPort->setHidden(hide);
}

void QkIDE::slotToggleFold()
{
    //m_editor->currentPage()->toggleFold();
}

void QkIDE::slotFullScreen(bool on)
{
    if(on)
        showFullScreen();
    else
        showNormal();
}

QkProject* QkIDE::createProject(const QString &name)
{
    QString cName, hName;
    if(!name.isEmpty()) {
        cName = name + ".c";
        hName = name + ".h";
    }
    else {
        cName = QK_IDE_C_DEF_STR;
        hName = QK_IDE_H_DEF_STR;
    }

    QFile cTemplateFile(":/templates/my_board.c");
    cTemplateFile.open(QFile::ReadOnly);
    QFile hTemplateFile(":/templates/my_board.h");
    hTemplateFile.open(QFile::ReadOnly);

    QString cTemplate(cTemplateFile.readAll());
    QString hTemplate(hTemplateFile.readAll());

    cTemplate.replace("[project_name]", name);
    hTemplate.replace("[header_name]", name.toUpper());

    m_editor->addPage(cName)->setPlainText(cTemplate);
    m_editor->addPage(hName)->setPlainText(hTemplate);
    m_editor->setCurrentPage(0);

    foreach(Page *page, m_editor->pages())
        setupPage(page);

    QkProject *project = new QkProject(name);
    project->addFile(cName);
    project->addFile(hName);

    return project;
}

void QkIDE::openProject(const QString &path)
{
    Page *page;

    slotCloseProject();
    m_curProject = new QkProject;

    qDebug() << "load project from file" << path;

    if(m_curProject->loadFromFile(path))
    {
        qDebug() << "project path" << m_curProject->path();

        foreach(QString fileName, m_curProject->files())
        {
            QFile file(m_curProject->path() + fileName);
            if(!file.open(QIODevice::ReadOnly))
            {
                qDebug() << "can't open file:" << fileName;
                break;
            }
            QTextStream in(&file);
            QString text;
            while (!in.atEnd())
                text.append(in.readLine() + "\n");

            page = m_editor->addPage(fileName);
            page->setPlainText(text);
            setupPage(page);
        }

        m_editor->setCurrentPage(0);

        slotParse();
        updateCurrentProject();
        updateInterface();
    }

    emit currentProjectChanged();
}

void QkIDE::setupPage(Page *page)
{
    page->setReadOnly(m_curProject->readOnly());

    Completer *completer = page->completer();
    completer->addElements(m_libElements, true);
    connect(page, SIGNAL(info(QString)), this, SLOT(showInfoMessage(QString)));
    connect(page, SIGNAL(keyPressed()), m_parserTimer, SLOT(start()));
}

void QkIDE::createMakefile(QkProject *project)
{
    qDebug() << "create makefile";
    QFile makefileTemplateFile(":/templates/makefile_template");

    if(!makefileTemplateFile.open(QIODevice::ReadOnly)) {
        qDebug() << "unable to open makefile template";
        return;
    }

    QFile makefileFile(project->path() + SLASH + "Makefile");

    if(!makefileFile.open(QIODevice::WriteOnly)) {
        qDebug() << "unable to create makefile";
        return;
    }

    QTextStream in(&makefileTemplateFile);
    QTextStream out(&makefileFile);
    QString makefileTemplate = in.readAll();

    QString appDir = QApplication::applicationDirPath();

    makefileTemplate.replace("{{qkProgramRoot}}", appDir + QKPROGRAM_DIR);
    makefileTemplate.replace("{{toolchainDir}}", appDir + TOOLCHAIN_DIR);
    makefileTemplate.replace("{{appName}}", project->name());
    makefileTemplate.replace("{{appIncludeDir}}", project->path());
    makefileTemplate.replace("{{appSourceDir}}", project->path());
    makefileTemplate.replace("{{targetName}}", "arduino.uno");
    makefileTemplate.replace("{{buildTarget}}", "BUILD_DEVICE");

    out << makefileTemplate;

    makefileTemplateFile.close();
    makefileFile.close();
}

void QkIDE::deleteMakefile(QkProject *project)
{
    qDebug() << "delete makefile";
    QFile::remove(project->path() + SLASH + "Makefile");
}

void QkIDE::slotSplitHorizontal()
{
    m_editor->splitHorizontal();

    m_removeSplitAct->setEnabled(true);
    m_splitHorizontalAct->setEnabled(false);
    m_splitVerticalAct->setEnabled(true);
}

void QkIDE::slotSplitVertical()
{
    m_editor->splitVertical();

    m_removeSplitAct->setEnabled(true);
    m_splitHorizontalAct->setEnabled(true);
    m_splitVerticalAct->setEnabled(false);
}

void QkIDE::slotRemoveSplit()
{
    m_editor->removeSplit();

    m_removeSplitAct->setEnabled(false);
    m_splitHorizontalAct->setEnabled(true);
    m_splitVerticalAct->setEnabled(true);
}

void QkIDE::updateWindowTitle()
{
    setWindowTitle(QK_IDE_NAME_STR + " | " + m_curProject->name());
}

void QkIDE::updateCurrentProject()
{
    updateWindowTitle();

    qDebug() << "current project:" << m_curProject->name() << m_curProject->path();

    RecentProject recent;
    recent.name = m_curProject->name();
    recent.path = m_curProject->path();

    m_recentProjects.removeOne(recent);
    m_recentProjects.prepend(recent);

    if(m_recentProjects.count() > MaxRecentProjects)
        m_recentProjects.removeLast();

    slotHome(false);
    updateRecentProjects();
    writeSettings();
}

void QkIDE::updateRecentProjects()
{
    int i;
    QString text;
    int numRecentProjects = qMin(m_recentProjects.count(),(int)MaxRecentProjects);

    for (i = 0; i < numRecentProjects; ++i) {
        text = tr("&%1 %2 (%3)").arg(i + 1)
                                .arg(m_recentProjects[i].name)
                                .arg(m_recentProjects[i].path);
        m_recentProjectsActs[i]->setText(text);
        m_recentProjectsActs[i]->setData(m_recentProjects[i].name);
        m_recentProjectsActs[i]->setVisible(true);
    }
    for (int j = numRecentProjects; j < MaxRecentProjects; ++j)
        m_recentProjectsActs[j]->setVisible(false);

    QString htmlText;
    QString str = "<li id=\"recent\"><a href=\"prj:recent:%1\"><b>%2</b></a><br>%3</li>\n";

    for(i = 0; i < numRecentProjects; i++) {
        htmlText.append(tr(str.toLatin1().data()).arg(i)
                                                 .arg(m_recentProjects[i].name)
                                                 .arg(m_recentProjects[i].path +
                                                      m_recentProjects[i].name + ".qkpro"));
    }
    if(htmlText.isEmpty()) {
        htmlText.append(tr("(empty)"));
    }

    qDebug() << "create html";

    QFile homeTemplateFile(":/html/home.html");
    QFile homeFile(QApplication::applicationDirPath() + "/resources/html/home.html");

    if(!homeTemplateFile.open(QIODevice::ReadOnly)) {
        qDebug() << "unable to read html";
        return;
    }

    if(!homeFile.open(QIODevice::WriteOnly)) {
        qDebug() << "unable to create html";
        return;
    }

    QTextStream in(&homeTemplateFile);
    QTextStream out(&homeFile);
    QString html = in.readAll();

    html.replace("{{recentProjects}}", htmlText);

    out << html;

    homeTemplateFile.close();
    homeFile.close();

    m_browser->load(QUrl::fromLocalFile(QApplication::applicationDirPath() + "/resources/html/home.html"));

    qDebug() << "recentProjects updated";
}

void QkIDE::updateInterface()
{
    if(m_curProject == 0) {
        m_saveProjectAct->setEnabled(false);;
        m_saveAsProjectAct->setEnabled(false);;
        m_showProjectFolderAct->setEnabled(false);
        m_cleanAct->setEnabled(false);
        m_verifyAct->setEnabled(false);
        m_uploadAct->setEnabled(false);
    }
    else {
        m_saveProjectAct->setEnabled(true);;
        m_saveAsProjectAct->setEnabled(true);;
        m_showProjectFolderAct->setEnabled(true);
        m_cleanAct->setEnabled(true);
        m_verifyAct->setEnabled(true);
        m_uploadAct->setEnabled(true);
    }

    bool connected = (m_serialConn != 0 && m_serialConn->isConnected() ? true : false);
    if(connected)
    {
        m_buttonConnect->setText(tr("Connected"));
        m_buttonConnect->setPalette(QPalette(QColor("#b5eaa5")));
    }
    else
    {
        m_buttonConnect->setText(tr("Disconnected"));
        m_buttonConnect->setPalette(QPalette(QColor("#f8c3c2")));
    }
}

void QkIDE::showInfoMessage(const QString &msg)
{
    QMessageBox::information(this, tr("Information"), msg);
}

void QkIDE::showErrorMessage(const QString &msg)
{
    QMessageBox::critical(this, tr("Error"), msg);
}

bool QkIDE::doYouReallyWantToQuit()
{
    bool quit = true;
    if(m_editor->hasModifiedPages()) {
        QString msg = tr("There are unsaved changes.\n"
                         "Do you really want to quit?");

        int r = QMessageBox::question(this, tr("Quit"), msg,
                                      QMessageBox::Save |
                                      QMessageBox::Discard |
                                      QMessageBox::Cancel,
                                      QMessageBox::Save);
        switch(r) {
        case QMessageBox::Save:
            slotSaveAllFiles();
        case QMessageBox::Discard:
            quit = true;
            break;
        case QMessageBox::Cancel:
        default: quit = false;
        }

    }
    return quit;
}

void QkIDE::closeEvent(QCloseEvent *e)
{
    if(!doYouReallyWantToQuit()) {
        e->ignore();
    }
    else {
        /*QSettings settings;

        settings.beginGroup("mainWindow");
        settings.setValue("width", width());
        settings.setValue("heigth", height());
        settings.setValue("maximized", isMaximized());
        settings.endGroup();*/

        m_verifyProcess->kill();
        m_uploadProcess->kill();
    }
}

void QkIDE::slotCurrentProjectChanged()
{
    if(m_curProject != 0)
    {
        //m_codeParserThread->setParserPath(m_curProject->path());
    }
}

void QkIDE::slotParse()
{
    QString tagsPath = QApplication::applicationDirPath() + TAGS_DIR;

    QDir(tagsPath).removeRecursively();
    QDir().mkdir(tagsPath);

    QFile file;
    foreach(Page *page, m_editor->pages())
    {
        QString destPath = tagsPath + "/" + page->name();
        file.setFileName(destPath);
        if(file.open(QIODevice::WriteOnly))
        {
            file.write(page->text().toUtf8());
            file.close();
        }
        else
            qDebug() << "cant create file" << destPath << file.errorString();
    }

    CodeParser *parser = m_codeParser;
    parser->parse(tagsPath);
}

void QkIDE::slotParsed()
{
    qDebug() << "QkIDE::slotParsed()";


    Highlighter::clearElements();
    Highlighter::addElements(m_codeParser->allElements());

    foreach(Page *page, m_editor->pages())
    {
        Completer *completer = page->completer();
        if(!completer->popup()->isVisible())
        {
            page->completer()->clearElements();
            page->completer()->addElements(m_codeParser->allElements());
        }
        page->highlighter()->rehighlight();
    }
}

void QkIDE::slotError(const QString &message)
{
    QMessageBox::critical(this, tr("Error"), message);
}

void QkIDE::slotReloadSerialPorts()
{
    QStringList list;
    foreach(QSerialPortInfo info, QSerialPortInfo::availablePorts())
    {
        QString portName = info.portName();
        if(portName.contains("ACM") || portName.contains("USB"))
            list.append(portName);
    }
    m_comboPort->clear();
    m_comboPort->addItems(list);
}

void QkIDE::slotTest()
{
    qDebug() << "TEST";
}
