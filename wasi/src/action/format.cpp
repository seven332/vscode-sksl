#include "action/format.h"

FormatResult Format(Modules* modules, const FormatParams& params) {
    FormatResult result;

    auto iter = modules->find(params.file);
    if (iter == modules->end()) {
        return result;
    }

    // TODO:
    result.newContent = iter->second.content;
    return result;
}
