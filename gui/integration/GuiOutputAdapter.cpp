#include "GuiOutputAdapter.h"
#include <stdexcept>
#include <utility>

namespace tividy {
GuiOutputAdapter::GuiOutputAdapter(std::unique_ptr<EmailService> service, std::function<void(AdapterOutput)> output)
    : adapter(std::move(service)), sink(std::move(output))
{
    if (!adapter || !sink) throw std::invalid_argument("An email adapter and structured output sink are required.");
}
void GuiOutputAdapter::sendEmail(std::string email, std::string subject, std::string body)
{
    adapter->sendEmail(email, subject, body);
    AdapterOutput output; output.recipient = std::move(email); output.subject = std::move(subject); output.body = std::move(body);
    // Display failures must not turn an already-delivered external action into
    // an apparent failed transaction. The engine sink should be non-throwing.
    try { sink(std::move(output)); } catch (...) {}
}
}
