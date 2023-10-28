#include "action/hover.h"

#include <algorithm>
#include <optional>
#include <string>
#include <unordered_map>

#include "overloaded.h"
#include "token.h"

struct BuiltinFunctionDoc {
    std::string url;
    std::string summary;
};

static std::optional<BuiltinFunctionDoc> GetBuiltinFunctionDoc(const std::string& name) {
    static const std::unordered_map<std::string, BuiltinFunctionDoc> kMap = {
        {"radians",          {"radians", "convert a quantity in degrees to radians"}                                   },
        {"degrees",          {"degrees", "convert a quantity in radians to degrees"}                                   },
        {"sin",              {"sin", "return the sine of the parameter"}                                               },
        {"cos",              {"cos", "return the cosine of the parameter"}                                             },
        {"tan",              {"tan", "return the tangent of the parameter"}                                            },
        {"asin",             {"asin", "return the arcsine of the parameter"}                                           },
        {"acos",             {"acos", "return the arccosine of the parameter"}                                         },
        {"atan",             {"atan", "return the arc-tangent of the parameters"}                                      },
        {"sinh",             {"sinh", "return the hyperbolic sine of the parameter"}                                   },
        {"cosh",             {"cosh", "return the hyperbolic cosine of the parameter"}                                 },
        {"tanh",             {"tanh", "return the hyperbolic tangent of the parameter"}                                },
        {"asinh",            {"asinh", "return the arc hyperbolic sine of the parameter"}                              },
        {"acosh",            {"acosh", "return the arc hyperbolic cosine of the parameter"}                            },
        {"atanh",            {"atanh", "return the arc hyperbolic tangent of the parameter"}                           },
        {"pow",              {"pow", "return the value of the first parameter raised to the power of the second"}      },
        {"exp",              {"exp", "return the natural exponentiation of the parameter"}                             },
        {"log",              {"log", "return the natural logarithm of the parameter"}                                  },
        {"exp2",             {"exp2", "return 2 raised to the power of the parameter"}                                 },
        {"log2",             {"log2", "return the base 2 logarithm of the parameter"}                                  },
        {"sqrt",             {"sqrt", "return the square root of the parameter"}                                       },
        {"inversesqrt",      {"inversesqrt", "return the inverse of the square root of the parameter"}                 },
        {"abs",              {"abs", "return the absolute value of the parameter"}                                     },
        {"sign",             {"sign", "extract the sign of the parameter"}                                             },
        {"floor",            {"floor", "find the nearest integer less than or equal to the parameter"}                 },
        {"ceil",             {"ceil", "find the nearest integer that is greater than or equal to the parameter"}       },
        {"fract",            {"fract", "compute the fractional part of the argument"}                                  },
        {"mod",              {"mod", "compute value of one parameter modulo another"}                                  },
        {"min",              {"min", "return the lesser of two values"}                                                },
        {"max",              {"max", "return the greater of two values"}                                               },
        {"clamp",            {"clamp", "constrain a value to lie between two further values"}                          },
        {"saturate",         {"", "`clamp(arg, 0, 1)`"}                                                                },
        {"mix",              {"mix", "linearly interpolate between two values"}                                        },
        {"step",             {"step", "generate a step function by comparing two values"}                              },
        {"smoothstep",       {"smoothstep", "perform Hermite interpolation between two values"}                        },
        {"floatBitsToInt",   {"floatBitsToInt", "produce the encoding of a floating point value as an integer"}        },
        {"floatBitsToUint",  {"floatBitsToInt", "produce the encoding of a floating point value as an integer"}        },
        {"intBitsToFloat",   {"intBitsToFloat", "produce a floating point using an encoding supplied as an integer"}   },
        {"uintBitsToFloat",  {"intBitsToFloat", "produce a floating point using an encoding supplied as an integer"}   },
        {"trunc",            {"trunc", "find the truncated value of the parameter"}                                    },
        {"round",            {"round", "find the nearest integer to the parameter"}                                    },
        {"roundEven",        {"roundEven", "find the nearest even integer to the parameter"}                           },
        {"isnan",            {"isnan", "determine whether the parameter is a number"}                                  },
        {"isinf",            {"isinf", "determine whether the parameter is positive or negative infinity"}             },
        {"modf",             {"modf", "separate a value into its integer and fractional components"}                   },
        {"packUnorm2x16",    {"packUnorm", "pack floating-point values into an unsigned integer"}                      },
        {"unpackUnorm2x16",  {"unpackUnorm", "unpack floating-point values from an unsigned integer"}                  },
        {"length",           {"length", "calculate the length of a vector"}                                            },
        {"distance",         {"distance", "calculate the distance between two points"}                                 },
        {"dot",              {"dot", "calculate the dot product of two vectors"}                                       },
        {"cross",            {"cross", "calculate the cross product of two vectors"}                                   },
        {"normalize",        {"normalize", "calculates the unit vector in the same direction as the original vector"}  },
        {"faceforward",      {"faceforward", "return a vector pointing in the same direction as another"}              },
        {"reflect",          {"reflect", "calculate the reflection direction for an incident vector"}                  },
        {"matrixCompMult",   {"matrixCompMult", "perform a component-wise multiplication of two matrices"}             },
        {"inverse",          {"inverse", "calculate the inverse of a matrix"}                                          },
        {"determinant",      {"determinant", "calculate the determinant of a matrix"}                                  },
        {"transpose",        {"transpose", "calculate the transpose of a matrix"}                                      },
        {"outerProduct",     {"outerProduct", "calculate the outer product of a pair of vectors"}                      },
        {"lessThan",         {"lessThan", "perform a component-wise less-than comparison of two vectors"}              },
        {"lessThanEqual",    {"lessThanEqual", "perform a component-wise less-than-or-equal comparison of two vectors"}},
        {"greaterThan",      {"greaterThan", "perform a component-wise greater-than comparison of two vectors"}        },
        {"greaterThanEqual",
         {"greaterThanEqual", "perform a component-wise greater-than-or-equal comparison of two vectors"}              },
        {"equal",            {"equal", "perform a component-wise equal-to comparison of two vectors"}                  },
        {"notEqual",         {"notEqual", "perform a component-wise not-equal-to comparison of two vectors"}           },
        {"any",              {"any", "check whether any element of a boolean vector is true"}                          },
        {"all",              {"all", "check whether all elements of a boolean vector are true"}                        },
        {"not",              {"not", "logically invert a boolean vector"}                                              },
        {"dFdx",             {"dFdx", "return the partial derivative of an argument with respect to x or y"}           },
        {"dFdy",             {"dFdx", "return the partial derivative of an argument with respect to x or y"}           },
        {"fwidth",           {"fwidth", "return the sum of the absolute derivatives in x and y"}                       },
        {"unpremul",         {"", "`float4(color.rgb / max(color.a, 0.0001), color.a)`"}                               },
    };
    auto iter = kMap.find(name);
    if (iter == kMap.end()) {
        return std::nullopt;
    } else {
        return iter->second;
    }
}

HoverResult Hover(Modules* modules, const HoverParams& params) {
    HoverResult result;
    result.found = false;

    auto iter = modules->find(params.file);
    if (iter == modules->end()) {
        return result;
    }

    // Convert position to utf-8
    auto position = iter->second.utf16_index.ToUTF8(params.position);

    // Find the first token that (position < token.range.end) is true
    auto i = std::upper_bound(
        iter->second.tokens.begin(),
        iter->second.tokens.end(),
        position,
        [](std::uint16_t position, const Token& token) { return position < token.range.end; }
    );
    if (i == iter->second.tokens.end()) {
        return result;
    }
    const auto& token = *i;
    if (token.range.start > position) {
        return result;
    }

    std::visit(
        Overloaded {
            [](const SkSL::Extension* /*value*/) {},
            [&result](const SkSL::FunctionDeclaration* value) {
                if (value->isBuiltin()) {
                    auto doc = GetBuiltinFunctionDoc(std::string(value->name()));
                    if (!doc) {
                        return;
                    }
                    result.found = true;
                    result.markdown = true;
                    result.content += "`" + value->description() + "`\n";
                    result.content += "\n---\n\n";
                    result.content += std::string(doc->summary);
                    if (!doc->url.empty()) {
                        result.content += " - [Reference](https://registry.khronos.org/OpenGL-Refpages/es3.0/html/" +
                                          doc->url + ".xhtml)";
                    }
                } else {
                    result.found = true;
                    result.markdown = true;
                    result.content += "`" + value->description() + "`";
                }
            },
            [&result](const SkSL::Variable* value) {
                result.found = true;
                result.markdown = true;
                result.content += "`" + value->description() + "`\n";
            },
            [](const SkSL::Type* /*value*/) {},
            [&result](const SkSL::Field* value) {
                result.found = true;
                result.markdown = true;
                result.content += "`" + value->description() + "`\n";
            },
            [&result](const SkSL::FieldSymbol* value) {
                result.found = true;
                result.markdown = true;
                result.content += "`" + value->description() + "`\n";
            },
            [](const SkSL::Literal* /*value*/) {},
            [](const SkSL::Setting* /*value*/) {},
            [](const SkSL::Swizzle* /*value*/) {},
            [&result](const SkSL::ChildCall* /*value*/) {
                result.found = true;
                result.markdown = true;
                result.content += "[Reference](https://skia.org/docs/user/sksl/#evaluating-sampling-other-skshaders)";
            },
        },
        token.value
    );

    // Convert result range to utf-16
    if (result.found) {
        result.range.start = iter->second.utf16_index.ToUTF16(token.range.start);
        result.range.end = iter->second.utf16_index.ToUTF16(token.range.end);
    }

    return result;
}
