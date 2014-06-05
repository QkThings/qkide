#ifndef QKIDE_H
#define QKIDE_H

#include <QMainWindow>
#include "qkide_global.h"
#include "editor/codeparser.h"
#include "qkutils.h"

//using namespace QkUtils;

class QkProject;
class Browser;
class Editor;
class Page;
class Completer;

class OptionsDialog;

class QSplitter;
class QProcess;
class QStackedWidget;
class pTextDock;
class HomeHeader;
class QkExplorerWidget;
class QkConnSerial;
class CodeParser;
class CodeParserThread;
class QComboBox;
class QPushButton;
class QToolButton;

namespace Ui {
class QkIDE;
}

class QkIDE : public QMainWindow
{
    Q_OBJECT
    friend class ProjectWizard;
    friend class PreferencesDialog;
    friend class Builder;
public:
    explicit QkIDE(QWidget *parent = 0);
    ~QkIDE();

public slots:
    void showInfoMessage(const QString &msg);
    void showErrorMessage(const QString &msg);

protected:
    void closeEvent(QCloseEvent *e);

private slots:
    void slotCleanProcessStarted();
    void slotCleanProcessOutput();
    void slotVerifyProcessStarted();
    void slotVerifyProcessOutput();
    void slotUploadProcessStarted();
    void slotUploadProcessOutput();
    void slotUploadProcessFinished();
    void slotProcessFinished();
    void slotHome(bool go);
    void slotOptions();
    void slotOpenExample();
    void slotOpenRecentProject();
    void slotOpenRecentProject(int i);
    void slotCreateProject();
    void slotOpenProject();
    void slotCloseProject();
    void slotSaveProject();
    void slotSaveAsProject();
    void slotSaveAllFiles();
    void slotShowFolder();
    void slotProjectPreferences();
    void slotSearch();
    void slotUndo();
    void slotRedo();
    void slotZoomIn();
    void slotZoomOut();
    void slotClean();
    void slotVerify();
    void slotUpload();
    void slotConnect();

    void slotShowHideReference();
    void slotShowHideExplorer();
    void slotShowHideTarget();
    void slotShowHideConnect();

    void slotToggleFold();
    void slotFullScreen(bool on);
    void slotSplitHorizontal();
    void slotSplitVertical();
    void slotRemoveSplit();
    void slotCurrentProjectChanged();
    void slotParse();
    void slotParsed();
    bool doYouReallyWantToQuit();
    void updateInterface();
    void slotError(const QString &message);
    void slotReloadSerialPorts();
    void slotTest();

signals:
    void currentProjectChanged();

private:
    void createActions();
    void createMenus();
    void createToolbars();
    void createExamples();
    void createReference();
    void setupLayout();
    void readSettings();
    void writeSettings();
    QkProject* createProject(const QString &name = QString());
    void openProject(const QString &path);
    void createMakefile(QkProject *project);
    void deleteMakefile(QkProject *project);
    void updateWindowTitle();
    void updateCurrentProject();
    void updateRecentProjects();

    void setupPage(Page *page);

    enum Constants {
        MaxRecentProjects = 6
    };

    class RecentProject {
    public:
         QString name;
         QString path;
         bool operator==(RecentProject recentProject)
         {
             return (path == recentProject.path && name == recentProject.name);
         }
    };

    Ui::QkIDE *ui;

    OptionsDialog *m_optionsDialog;

    QkProject *m_curProject;
    QList<QkProject*> m_projects;

    CodeParser *m_codeParser;
    CodeParserThread *m_codeParserThread;
    QTimer *m_parserTimer;

    QList<CodeParser::Element> m_libElements;

    QMap<QString, QkUtils::Target> m_targets;

    QLayout *m_mainLayout;

    QAction *m_recentProjectsActs[MaxRecentProjects];

    QAction *m_ProjectPreferencesAct;

    QAction *m_searchAct;

    QAction *m_homeAct;

    QAction *m_createProjectAct;
    QAction *m_openProjectAct;
    QAction *m_newFileAct;
    QAction *m_saveProjectAct;
    QAction *m_saveAsProjectAct;
    QAction *m_showProjectFolderAct;
    QAction *m_undoAct;
    QAction *m_redoAct;
    QAction *m_zoomInAct;
    QAction *m_zoomOutAct;
    QAction *m_cleanAct;
    QAction *m_verifyAct;
    QAction *m_uploadAct;

    QAction *m_referenceAct;
    QAction *m_explorerAct;
    QAction *m_connectAct;
    QAction *m_targetAct;
    QAction *m_testAct;

    QAction *m_toggleFoldAct;

    QAction *m_fullScreenAct;

    QAction *m_splitHorizontalAct;
    QAction *m_splitVerticalAct;
    QAction *m_removeSplitAct;

    QAction *m_optionsAct;

    QAction *m_aboutAct;
    QAction *m_exitAct;

    QMenu *m_fileMenu;
    QMenu *m_examplesMenu;
    QMenu *m_recentProjectsMenu;
    QMenu *m_editMenu;
    QMenu *m_viewMenu;
    QMenu *m_projectMenu;
    QMenu *m_toolsMenu;
    QMenu *m_windowMenu;
    QMenu *m_helpMenu;

    QSplitter *m_mainVSplitter;
    QSplitter *m_mainHSplitter;

    QList<RecentProject> m_recentProjects;

    QToolBar *m_programToolBar;
    QToolBar *m_qkToolbar;

    Browser *m_browser;
    QWidget *m_homeWidget;
    Editor *m_editor;
    QStackedWidget *m_stackedWidget;
    pTextDock *m_outputWindow;

    QProcess *m_cleanProcess;
    QProcess *m_verifyProcess;
    QProcess *m_uploadProcess;

    QString m_uploadPortName;
    QString m_projectDefaultLocation;

    QkConnSerial *m_serialConn;

    QAction *m_buttonRefreshPorts;
    QComboBox *m_comboPort;
    QComboBox *m_comboBaud;
    QPushButton *m_buttonConnect;

    QComboBox *m_comboTargetName;
    QComboBox *m_comboTargetVariant;

    QkExplorerWidget *m_explorerWidget;
    QDockWidget *m_explorerDock;

    QMainWindow *m_explorerWindow;
    QMainWindow *m_referenceWindow;

};

#endif // QKIDE_H
