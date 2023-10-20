#include "action/close.h"

CloseResult Close(Modules* modules, const CloseParams& params) {
    CloseResult result;

    auto iter = modules->find(params.file);
    if (iter == modules->end()) {
        result.succeed = false;
    } else {
        modules->erase(iter);
        result.succeed = true;
    }

    return result;
}
