#ifndef TIVIDY_GUI_THEME_H
#define TIVIDY_GUI_THEME_H

#include <QColor>
#include <QIcon>
#include <QString>
class QApplication;

namespace tividy {

/** Single source of truth for the five supplied theme colours and styling. */
struct Theme {
    /** Install the five-colour application palette and matching widget styling. */
    static void apply(QApplication &application);
    /** Main surface and light text. */
    static QColor dustGrey();
    /** Primary action and selection accent. */
    static QColor cerulean();
    /** Primary text and structural background. */
    static QColor coffeeBean();
    /** Attention indicator. */
    static QColor cinnabar();
    /** Destructive action and failure indicator. */
    static QColor brownRed();
    /** Application-wide widget styles, derived from these five colours. */
    static QString styleSheet();
    /** Palette-only vector-painted icon, rendered for the current DPI. */
    static QIcon icon(const QString &kind, const QColor &colour = coffeeBean());
    /** State accent; icon and label always provide a second distinction. */
    static QColor stateColour(const QString &state);
    /** Readable label for a core state code. */
    static QString stateLabel(const QString &state);
};

} // namespace tividy
#endif
