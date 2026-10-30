#include "theme/Theme.h"
#include "integration/CoreWorkflowGateway.h"
#include "views/MainWindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QTimer>
#include <exception>

int main(int argc, char **argv)
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication app(argc, argv);
    QApplication::setApplicationName("TIVIDY");
    QApplication::setOrganizationName("COS214");
    tividy::Theme::apply(app);
    try {
        tividy::MainWindow window(tividy::createGateway());
        window.show();
        // Build-time launch check; never supplies a simulated workflow.
        if (app.arguments().contains("--smoke")) QTimer::singleShot(1500, &app, &QApplication::quit);
        return app.exec();
    } catch (const std::exception &error) {
        QMessageBox::critical(nullptr, "TIVIDY could not start", QString::fromUtf8(error.what()));
    } catch (...) {
        QMessageBox::critical(nullptr, "TIVIDY could not start", "An unexpected startup error occurred.");
    }
    return 1;
}
