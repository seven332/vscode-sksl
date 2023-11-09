# SkSL Language Server

![workflow](https://github.com/seven332/vscode-sksl/actions/workflows/build.yml/badge.svg)

Code IntelliSense for [Skia's shading language](https://skia.org/docs/user/sksl/).

## Usage

Install the [extension](https://marketplace.visualstudio.com/items?itemName=seven332.vscode-sksl).

The kind of SkSL must be presented in source code.

```
// kind=shader
half4 main(float2 coord) {
    return float4(1, 1, 1, 1);
}
```

It could be `shader`, `colorfilter`, `blender`, `meshfrag` or `meshvert`.

## Features

-   [x] Diagnostic
-   [x] Document Symbol
-   [x] Document Formatting (may **NOT** work as expected)
-   [x] Semantic Token
-   [x] Hover
-   [x] Definition
