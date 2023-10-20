#include "action/format.h"

#include "formatter.h"

FormatResult Format(Modules* modules, const FormatParams& params) {
    FormatResult result;

    auto iter = modules->find(params.file);
    if (iter == modules->end()) {
        return result;
    }

    result.newContent = Formatter().Format(iter->second.content);
    return result;
}
