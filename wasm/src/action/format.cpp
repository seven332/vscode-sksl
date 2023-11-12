#include "action/format.h"

#include "formatter.h"

FormatResult Format(Modules* modules, const FormatParams& params) {
    FormatResult result;

    auto iter = modules->find(params.file);
    if (iter == modules->end() || !iter->second.document) {
        return result;
    }

    result.new_content = Formatter().Format(iter->second.content);
    return result;
}
