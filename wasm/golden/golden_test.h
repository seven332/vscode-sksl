#pragma once

#include <string>

std::string ReadSkSL(const char* source_path);

std::string GetSkSLProgramKind(const std::string& content);
