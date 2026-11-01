#ifndef TIVIDY_GUI_GUIOUTPUTADAPTER_H
#define TIVIDY_GUI_GUIOUTPUTADAPTER_H
#include "adapter/EmailService.h"
#include <functional>
#include <memory>

namespace tividy {
/** Copied adapter output; the receiving engine sink adds its canonical timestamp. */
struct AdapterOutput {
    std::string recipient, subject, body;
};

/**
 * Routes through the project's actual EmailService target/adapter, then forwards
 * the successful message to a structured GUI sink. No global cout interception.
 */
class GuiOutputAdapter : public EmailService {
public:
    /** Retain a real adapter and a value-only sink. */
    GuiOutputAdapter(std::unique_ptr<EmailService> adapter, std::function<void(AdapterOutput)> sink);
    /** Deliver through the adapter and publish copied display data. */
    void sendEmail(std::string email, std::string subject, std::string body) override;
private:
    std::unique_ptr<EmailService> adapter;
    std::function<void(AdapterOutput)> sink;
};
}
#endif
