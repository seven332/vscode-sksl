set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_path(SET SKIA_SOURCE_DIR NORMALIZE ${CMAKE_CURRENT_SOURCE_DIR}/../externals/skia)
cmake_path(SET SKIA_CMAKE_DIR NORMALIZE ${CMAKE_CURRENT_BINARY_DIR}/cmake)

add_library(skia_sksl STATIC
    ${SKIA_SOURCE_DIR}/src/sksl/SkSLAnalysis.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/SkSLBuiltinTypes.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/SkSLCompiler.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/SkSLConstantFolder.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/SkSLContext.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/SkSLErrorReporter.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/SkSLInliner.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/SkSLIntrinsicList.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/SkSLLexer.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/SkSLMangler.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/SkSLModuleLoader.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/SkSLOperator.cpp
    # ${SKIA_SOURCE_DIR}/src/sksl/SkSLOutputStream.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/SkSLParser.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/SkSLPool.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/SkSLPosition.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/SkSLSampleUsage.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/SkSLString.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/SkSLThreadContext.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/SkSLUtil.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/analysis/SkSLCanExitWithoutReturningValue.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/analysis/SkSLCheckProgramStructure.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/analysis/SkSLFinalizationChecks.cpp
    # ${SKIA_SOURCE_DIR}/src/sksl/analysis/SkSLGetLoopControlFlowInfo.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/analysis/SkSLGetLoopUnrollInfo.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/analysis/SkSLGetReturnComplexity.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/analysis/SkSLHasSideEffects.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/analysis/SkSLIsConstantExpression.cpp
    # ${SKIA_SOURCE_DIR}/src/sksl/analysis/SkSLIsDynamicallyUniformExpression.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/analysis/SkSLIsSameExpressionTree.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/analysis/SkSLIsTrivialExpression.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/analysis/SkSLProgramUsage.cpp
    # ${SKIA_SOURCE_DIR}/src/sksl/analysis/SkSLReturnsInputAlpha.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/analysis/SkSLSwitchCaseContainsExit.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/analysis/SkSLSymbolTableStackBuilder.cpp
    # ${SKIA_SOURCE_DIR}/src/sksl/codegen/SkSLRasterPipelineBuilder.cpp
    # ${SKIA_SOURCE_DIR}/src/sksl/codegen/SkSLRasterPipelineCodeGenerator.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLBinaryExpression.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLBlock.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLChildCall.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLConstructor.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLConstructorArray.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLConstructorArrayCast.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLConstructorCompound.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLConstructorCompoundCast.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLConstructorDiagonalMatrix.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLConstructorMatrixResize.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLConstructorScalarCast.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLConstructorSplat.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLConstructorStruct.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLDiscardStatement.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLDoStatement.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLExpression.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLExpressionStatement.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLExtension.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLFieldAccess.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLForStatement.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLFunctionCall.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLFunctionDeclaration.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLFunctionDefinition.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLIfStatement.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLIndexExpression.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLInterfaceBlock.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLLayout.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLLiteral.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLModifierFlags.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLModifiersDeclaration.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLPostfixExpression.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLPrefixExpression.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLProgram.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLSetting.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLStructDefinition.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLSwitchCase.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLSwitchStatement.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLSwizzle.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLSymbolTable.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLTernaryExpression.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLType.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLTypeReference.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLVarDeclarations.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLVariable.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/ir/SkSLVariableReference.cpp
    # ${SKIA_SOURCE_DIR}/src/sksl/tracing/SkSLDebugTracePriv.cpp
    # ${SKIA_SOURCE_DIR}/src/sksl/tracing/SkSLTraceHook.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/transform/SkSLAddConstToVarModifiers.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/transform/SkSLEliminateDeadFunctions.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/transform/SkSLEliminateDeadGlobalVariables.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/transform/SkSLEliminateDeadLocalVariables.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/transform/SkSLEliminateEmptyStatements.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/transform/SkSLEliminateUnreachableCode.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/transform/SkSLFindAndDeclareBuiltinFunctions.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/transform/SkSLFindAndDeclareBuiltinVariables.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/transform/SkSLHoistSwitchVarDeclarationsAtTopLevel.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/transform/SkSLRenamePrivateSymbols.cpp
    ${SKIA_SOURCE_DIR}/src/sksl/transform/SkSLReplaceConstVarsWithLiterals.cpp
    # ${SKIA_SOURCE_DIR}/src/sksl/transform/SkSLRewriteIndexedSwizzle.cpp
    ${SKIA_SOURCE_DIR}/src/base/SkArenaAlloc.cpp
    # ${SKIA_SOURCE_DIR}/src/base/SkBlockAllocator.cpp
    ${SKIA_SOURCE_DIR}/src/base/SkContainers.cpp
    ${SKIA_SOURCE_DIR}/src/base/SkHalf.cpp
    # ${SKIA_SOURCE_DIR}/src/base/SkMalloc.cpp
    # ${SKIA_SOURCE_DIR}/src/base/SkMathPriv.cpp
    ${SKIA_SOURCE_DIR}/src/base/SkSafeMath.cpp
    ${SKIA_SOURCE_DIR}/src/base/SkSemaphore.cpp
    ${SKIA_SOURCE_DIR}/src/base/SkThreadID.cpp
    ${SKIA_SOURCE_DIR}/src/base/SkUTF.cpp
    ${SKIA_SOURCE_DIR}/src/base/SkUtils.cpp
    ${SKIA_SOURCE_DIR}/src/core/SkChecksum.cpp
    # ${SKIA_SOURCE_DIR}/src/core/SkCpu.cpp
    ${SKIA_SOURCE_DIR}/src/core/SkData.cpp
    ${SKIA_SOURCE_DIR}/src/core/SkMatrixInvert.cpp
    ${SKIA_SOURCE_DIR}/src/core/SkStream.cpp
    ${SKIA_SOURCE_DIR}/src/core/SkString.cpp
    # ${SKIA_SOURCE_DIR}/src/core/SkStringUtils.cpp
    # ${SKIA_SOURCE_DIR}/src/gpu/ganesh/GrMemoryPool.cpp
    ${SKIA_SOURCE_DIR}/src/ports/SkMemory_malloc.cpp
    ${SKIA_SOURCE_DIR}/src/ports/SkDebug_stdio.cpp
    ${SKIA_SOURCE_DIR}/src/ports/SkOSFile_stdio.cpp
    ${SKIA_SOURCE_DIR}/src/ports/SkOSFile_posix.cpp
    # ${SKIA_SOURCE_DIR}/src/utils/SkJSON.cpp
    # ${SKIA_SOURCE_DIR}/src/utils/SkJSONWriter.cpp
    # ${SKIA_SOURCE_DIR}/src/utils/SkOSPath.cpp
    # ${SKIA_SOURCE_DIR}/src/utils/SkParse.cpp
    # ${SKIA_SOURCE_DIR}/src/utils/SkShaderUtils.cpp
    ${SKIA_SOURCE_DIR}/src/utils/SkEventTracer.cpp
)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_definitions(skia_sksl PUBLIC SK_BUILD_FOR_UNIX)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_compile_definitions(skia_sksl PUBLIC SK_BUILD_FOR_MAC)
endif()
target_include_directories(skia_sksl PUBLIC ${SKIA_SOURCE_DIR})
