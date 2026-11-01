#ifndef TIVIDY_GUI_JSONCODEC_H
#define TIVIDY_GUI_JSONCODEC_H
#include "integration/WorkflowTypes.h"
#include <stdexcept>

namespace tividy {
/** Readable syntax/transport errors, separate from core domain validation. */
class DocumentError : public std::runtime_error {
public:
    /** Store all collected errors without accepting partial input. */
    explicit DocumentError(std::vector<Diagnostic> diagnostics);
    /** Return syntax positions or semantic JSON paths. */
    const std::vector<Diagnostic> &diagnostics() const;
private:
    std::vector<Diagnostic> errors;
};

/** C++11 JSON transport codec. It never constructs/mutates engine objects. */
class JsonCodec {
public:
    /** Decode schema 1. Syntax and transport shape errors reject the whole file. */
    static WorkflowDocument decode(const std::string &utf8);
    /** Encode the core's export document; permissions are never serialised. */
    static std::string encode(const WorkflowDocument &document);
};
}
#endif
