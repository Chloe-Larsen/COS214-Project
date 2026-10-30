#include "Theme.h"
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QApplication>
#include <QScreen>
#include <QPalette>
#include <QStyleFactory>

namespace tividy {

QColor Theme::dustGrey() { return QColor("#CCCDCA"); }
QColor Theme::cerulean() { return QColor("#477998"); }
QColor Theme::coffeeBean() { return QColor("#291F1E"); }
QColor Theme::cinnabar() { return QColor("#F64740"); }
QColor Theme::brownRed() { return QColor("#A3333D"); }

void Theme::apply(QApplication &application)
{
    application.setStyle(QStyleFactory::create("Fusion"));
    QPalette palette;
    for (QPalette::ColorGroup group : {QPalette::Active, QPalette::Inactive, QPalette::Disabled}) {
        for (QPalette::ColorRole role : {QPalette::Window, QPalette::Base, QPalette::AlternateBase, QPalette::Button, QPalette::Light, QPalette::Midlight}) palette.setColor(group, role, dustGrey());
        for (QPalette::ColorRole role : {QPalette::WindowText, QPalette::Text, QPalette::ButtonText, QPalette::Dark, QPalette::Mid, QPalette::Shadow}) palette.setColor(group, role, coffeeBean());
        palette.setColor(group, QPalette::Highlight, coffeeBean()); palette.setColor(group, QPalette::HighlightedText, dustGrey());
        palette.setColor(group, QPalette::ToolTipBase, coffeeBean()); palette.setColor(group, QPalette::ToolTipText, dustGrey());
        palette.setColor(group, QPalette::Link, cerulean()); palette.setColor(group, QPalette::LinkVisited, brownRed());
    }
    palette.setColor(QPalette::Disabled, QPalette::Text, cerulean()); palette.setColor(QPalette::Disabled, QPalette::ButtonText, cerulean());
    application.setPalette(palette); application.setStyleSheet(styleSheet());
}

QString Theme::styleSheet()
{
    return QString::fromLatin1(R"STYLE(
        QWidget { background: %1; color: %2; font-family: 'Segoe UI', 'DejaVu Sans'; font-size: 10pt; }
        QMainWindow, QDialog { background: %1; }
        QToolBar { background: %2; border: none; padding: 10px; spacing: 8px; }
        QToolBar QLabel { background: transparent; color: %1; }
        QToolBar QWidget#toolbarSpacer { background: %2; }
        QLabel#brand { font-size: 19pt; font-weight: 700; letter-spacing: 3px; padding: 4px 12px; }
        QLabel#eyebrow { color: %2; font-size: 9pt; font-weight: 700; }
        QLabel#pageTitle { font-size: 23pt; font-weight: 650; }
        QLabel#pageTitle[compact="true"] { font-size: 17pt; }
        QLabel#sectionTitle { font-size: 12pt; font-weight: 650; }
        QLabel#availability { color: %5; padding: 8px; border-left: 3px solid %5; }
        QLabel#attention { color: %4; }
        QLabel#graphStatus { background: %2; color: %1; padding: 6px 10px; border-radius: 4px; }
        QLineEdit, QComboBox, QSpinBox, QDateEdit, QPlainTextEdit { border: 1px solid %2; border-radius: 5px; padding: 7px; selection-background-color: %2; selection-color: %1; }
        QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QPlainTextEdit:focus { border: 2px solid %2; }
        QPushButton { min-height: 18px; padding: 8px 14px; border: 1px solid %2; border-radius: 5px; }
        QPushButton:hover { border: 2px solid %3; padding: 7px 13px; }
        QPushButton:focus { border: 2px solid %2; padding: 7px 13px; }
        QPushButton#primary { background: %2; color: %1; font-weight: 600; }
        QPushButton#danger { color: %5; border-color: %5; }
        QPushButton:disabled, QToolButton:disabled { color: %3; border-style: dotted; }
        QToolButton { padding: 7px; border-radius: 4px; }
        QToolBar QToolButton { color: %1; background: %2; border: 1px solid %1; padding: 7px 10px; }
        QToolBar QToolButton:hover { border: 2px solid %3; padding: 6px 9px; }
        QTreeView, QTableView { border: 1px solid %2; border-radius: 5px; outline: 0; show-decoration-selected: 1; }
        QTreeView::item, QTableView::item { padding: 8px 4px; min-height: 24px; }
        QTreeView::item:selected, QTableView::item:selected { background: %2; color: %1; }
        QTreeView::branch:selected { background: %2; }
        QTreeView::item:hover:!selected, QTableView::item:hover:!selected { border-bottom: 1px solid %3; }
        QHeaderView::section { background: %1; color: %2; padding: 10px 6px; border: none; border-bottom: 2px solid %2; font-weight: 600; }
        QTabWidget::pane { border: none; padding-top: 8px; }
        QTabBar::tab { min-width: 125px; padding: 8px 20px; border-bottom: 2px solid %1; font-weight: 600; }
        QTabBar::tab:selected { border-bottom: 3px solid %3; }
        QSplitter::handle { background: %1; }
        QGroupBox { border: 1px solid %2; border-radius: 5px; margin-top: 18px; padding: 12px; }
        QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 6px; font-weight: 600; }
        QMenu { border: 1px solid %2; padding: 5px; }
        QMenu::item { padding: 7px 22px; }
        QMenu::item:selected { background: %2; color: %1; }
        QStatusBar { border-top: 1px solid %2; padding: 3px; }
        QDockWidget::title { padding: 8px; font-weight: 600; }
        QScrollBar:vertical { width: 12px; }
        QScrollBar::handle:vertical { background: %3; border-radius: 5px; min-height: 24px; }
        QToolTip { background: %2; color: %1; padding: 7px; border: 1px solid %1; }
    )STYLE").arg(dustGrey().name(), coffeeBean().name(), cerulean().name(), cinnabar().name(), brownRed().name());
}

QColor Theme::stateColour(const QString &value)
{
    const QString state = value.toUpper();
    if (state == "CANCELLED" || state == "REJECTED") return brownRed();
    if (state == "BLOCKED" || state == "ESCALATED") return cinnabar();
    if (state == "APPROVED" || state == "COMPLETED" || state == "IN_PROGRESS") return cerulean();
    return coffeeBean();
}

QString Theme::stateLabel(const QString &state)
{
    QString result = state.toLower();
    result.replace('_', ' ');
    if (!result.isEmpty()) result[0] = result[0].toUpper();
    return result.isEmpty() ? QStringLiteral("Unavailable") : result;
}

QIcon Theme::icon(const QString &kind, const QColor &colour)
{
    const qreal dpi = qApp && qApp->primaryScreen() ? qApp->primaryScreen()->devicePixelRatio() : 1;
    QPixmap image(QSize(24, 24) * dpi);
    image.setDevicePixelRatio(dpi);
    image.fill(Qt::transparent);
    QPainter p(&image);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(colour, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    if (kind == "group") {
        QPainterPath path;
        path.moveTo(3, 7); path.lineTo(9, 7); path.lineTo(11, 10); path.lineTo(21, 10);
        path.lineTo(21, 20); path.lineTo(3, 20); path.closeSubpath(); p.drawPath(path);
    } else if (kind == "add") {
        p.drawLine(5, 12, 19, 12); p.drawLine(12, 5, 12, 19);
    } else if (kind == "worker") {
        p.drawEllipse(QRectF(8, 3, 8, 8)); p.drawArc(QRectF(4, 12, 16, 14), 0, 180 * 16);
    } else if (kind == "BLOCKED") {
        p.drawRoundedRect(QRectF(5, 10, 14, 11), 2, 2); p.drawArc(QRectF(8, 3, 8, 12), 0, 180 * 16);
    } else if (kind == "COMPLETED" || kind == "APPROVED") {
        if (kind == "COMPLETED") p.drawEllipse(QRectF(3, 3, 18, 18));
        p.drawLine(6, 12, 10, 16); p.drawLine(10, 16, 18, 7);
    } else if (kind == "CANCELLED" || kind == "REJECTED") {
        p.drawEllipse(QRectF(3, 3, 18, 18)); p.drawLine(7, 7, 17, 17);
        if (kind == "REJECTED") p.drawLine(17, 7, 7, 17);
    } else if (kind == "IN_PROGRESS") {
        p.drawEllipse(QRectF(3, 3, 18, 18)); p.drawLine(12, 6, 12, 12); p.drawLine(12, 12, 17, 15);
    } else {
        p.drawEllipse(QRectF(4, 4, 16, 16)); p.drawEllipse(QRectF(10, 10, 4, 4));
    }
    return QIcon(image);
}

} // namespace tividy
