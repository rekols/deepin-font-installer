#include "mainwindow.h"
#include "utils.h"
#include <QDebug>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QDir>

#include "../lib/dfontinfo.h"

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
{
    m_mainWidget = new QWidget;
    m_mainLayout = new QStackedLayout(m_mainWidget);
    m_homePage = new HomePage;
    m_singleFilePage = new SingleFilePage;
    m_multiFilePage = new MultiFilePage;

    // add widget to main layout.
    m_mainLayout->addWidget(m_homePage);
    m_mainLayout->addWidget(m_singleFilePage);
    m_mainLayout->addWidget(m_multiFilePage);

    // init window flags.
    setWindowTitle(tr("Deepin Font Installer"));
    setCentralWidget(m_mainWidget);
    setAcceptDrops(true);

    // connect the signals to the slot function.
    connect(m_homePage, &HomePage::fileSelected, this, &MainWindow::onSelected);
    connect(m_multiFilePage, &MultiFilePage::countChanged, this, &MainWindow::handleDelete);
    connect(m_singleFilePage, &SingleFilePage::installBtnClicked, this, [=] {
                                                                            Utils::fontInstall(listItems);
                                                                        });
}

MainWindow::~MainWindow()
{
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    auto *const mime = e->mimeData();

    // not has urls.
    if (!mime->hasUrls())
        return e->ignore();

    for (const auto &item : mime->urls()) {
        const QFileInfo info = item.path();
        if (info.isDir())
            return e->accept();
        if (info.isFile() && Utils::isFontSuffix(info.suffix()))
            return e->accept();
    }

    e->ignore();
}

void MainWindow::dropEvent(QDropEvent *e)
{
    auto *const mime = e->mimeData();

    if (!mime->hasUrls())
        return e->ignore();

    e->accept();

    // find font files.
    QStringList fileList;
    for (const auto &url : mime->urls()) {
        if (!url.isLocalFile())
            continue;

        const QString localPath = url.toLocalFile();
        const QFileInfo info(localPath);

        if (info.isFile() && Utils::isFontSuffix(info.suffix())) {
            fileList << localPath;
        }
    }

    onSelected(fileList);
}

void MainWindow::refreshPage()
{
    const int count = listItems.count();

    if (count == 0)
        return;

    if (count == 1) {
        // switch to single file page.
        m_mainLayout->setCurrentIndex(1);
        m_singleFilePage->updateInfo(listItems.first());
    } else {
        // switch to multi file page.
        m_mainLayout->setCurrentIndex(2);
        m_multiFilePage->loadItems(listItems);
    }
}

void MainWindow::onSelected(const QStringList &files)
{
    for (const auto &file : files) {
        if (!listItems.contains(file))
            listItems << file;
    }

    refreshPage();
}

void MainWindow::handleDelete(const int &count, const QString &filePath)
{
    listItems.removeAt(listItems.indexOf(filePath));

    refreshPage();
}