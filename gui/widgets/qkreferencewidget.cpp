#include "qkreferencewidget.h"
#include "ui_qkreferencewidget.h"
#include "browser.h"

#include "qkide_global.h"

QkReferenceWidget::QkReferenceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QkReferenceWidget)
{
    ui->setupUi(this);

    QStringList refNames;
    refNames << "QkProgram" << "QkPeripheral" << "QkDSP";

    ui->comboRefName->addItems(refNames);
    slotReload();

    connect(ui->comboRefName, SIGNAL(currentIndexChanged(int)), this, SLOT(slotReload()));

    setWindowFlags(Qt::Tool);
    setWindowTitle("QkReference");
    resize(750,600);


//        m_referenceWindow = new QMainWindow(this);

//        QWidget *referenceWidget = new QWidget(m_referenceWindow);

//    //    QComboBox *comboReference = new QComboBox(referenceWidget);
//        m_comboReference = new QComboBox(referenceWidget);
//        QComboBox *comboReference = m_comboReference;
//        QStringList referenceItems;
//        referenceItems << "QkProgram" << "QkPeripheral" << "QkDSP";
//        comboReference->addItems(referenceItems);

//        QString qkprogramRef = QKPROGRAM_DOC_DIR + "/html/index.html";

//        QString urlStr = "file://" + qApp->applicationDirPath() + qkprogramRef;
//        qDebug() << "reference:" << urlStr;

//        Browser *referenceBrowser = new Browser(referenceWidget);
//        referenceBrowser->load(QUrl(urlStr));

//        QVBoxLayout *vBox = new QVBoxLayout;
//        vBox->addWidget(comboReference);
//        vBox->addWidget(referenceBrowser);
//        referenceWidget->setLayout(vBox);

//        m_referenceWindow->setCentralWidget(referenceWidget);
//        m_referenceWindow->setWindowTitle("qkreference");
//        m_referenceWindow->resize(750,600);

//        connect(comboReference, SIGNAL(currentIndexChanged(int)), this, SLOT(slotReloadReference()));
}

QkReferenceWidget::~QkReferenceWidget()
{
    delete ui;
}

void QkReferenceWidget::slotReload()
{
    QString refName = ui->comboRefName->currentText().toLower();
    QString urlStr;

    urlStr += "file://" +  qApp->applicationDirPath();

    if(refName == "qkprogram")
        urlStr += QKPROGRAM_DOC_DIR;
    else if(refName == "qkperipheral")
        urlStr += QKPERIPHERAL_DOC_DIR;
    else if(refName == "qkdsp")
        urlStr += QKDSP_DOC_DIR;

    urlStr += "/html/index.html";

    qDebug() << "reference:" << urlStr;
    ui->browser->load(QUrl(urlStr));
}
