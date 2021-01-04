;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; POSITION                 0   xyz         0     NONE   float   xyz 
; NORMAL                   0   xyz         1     NONE   float   xyz 
; TANGENT                  0   xyz         2     NONE   float       
; TEXCOORD                 0   xy          3     NONE   float   xy  
; COLOR                    0   xyzw        4     NONE   float       
; SV_InstanceID            0   x           5   INSTID    uint   x   
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; NORMAL                   0   xyz         1     NONE   float   xyz 
; TEXCOORD                 1      w        1     NONE   float      w
; TEXCOORD                 0   xy          2     NONE   float   xy  
;
; shader hash: 7c7dc2a2c9185e3f965d10795b78d2cd
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; POSITION                 0                              
; NORMAL                   0                              
; TANGENT                  0                              
; TEXCOORD                 0                              
; COLOR                    0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; NORMAL                   0                 linear       
; TEXCOORD                 0                 linear       
; TEXCOORD                 1                 linear       
;
; Buffer Definitions:
;
; cbuffer GlobalConstants
; {
;
;   struct dx.alignment.legacy.GlobalConstants
;   {
;
;       column_major float4x4 matViewProj;            ; Offset:    0
;       float3 lightDir;                              ; Offset:   64
;       float padding0;                               ; Offset:   76
;       float4 ambient;                               ; Offset:   80
;   
;   } GlobalConstants;                                ; Offset:    0 Size:    96
;
; }
;
; Resource bind info for instanceDatas
; {
;
;   struct dx.alignment.legacy.struct.InstanceData
;   {
;
;       column_major float4x4 matWorld;               ; Offset:    0
;       int materialIndex;                            ; Offset:   64
;       float padding0;                               ; Offset:   68
;       float padding1;                               ; Offset:   72
;       float padding2;                               ; Offset:   76
;   
;   } $Element;                                       ; Offset:    0 Size:    80
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; GlobalConstants                   cbuffer      NA          NA     CB0            cb0     1
; instanceDatas                     texture  struct         r/o      T0      t0,space1     1
;
;
; ViewId state:
;
; Number of inputs: 21, outputs: 10
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0, 1, 2, 20 }
;   output 1 depends on inputs: { 0, 1, 2, 20 }
;   output 2 depends on inputs: { 0, 1, 2, 20 }
;   output 3 depends on inputs: { 0, 1, 2, 20 }
;   output 4 depends on inputs: { 4, 5, 6, 20 }
;   output 5 depends on inputs: { 4, 5, 6, 20 }
;   output 6 depends on inputs: { 4, 5, 6, 20 }
;   output 7 depends on inputs: { 20 }
;   output 8 depends on inputs: { 12 }
;   output 9 depends on inputs: { 13 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:32-i16:32-i32:32-i64:64-f16:32-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%dx.types.ResRet.i32 = type { i32, i32, i32, i32, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%"dx.alignment.legacy.class.StructuredBuffer<InstanceData>" = type { %dx.alignment.legacy.struct.InstanceData }
%dx.alignment.legacy.struct.InstanceData = type { [4 x <4 x float>], i32, float, float, float }
%dx.alignment.legacy.GlobalConstants = type { [4 x <4 x float>], <3 x float>, float, <4 x float> }

define void @VS() {
  %1 = call %dx.types.Handle @dx.op.createHandle(i32 57, i8 0, i32 0, i32 0, i1 false)  ; CreateHandle(resourceClass,rangeId,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.createHandle(i32 57, i8 2, i32 0, i32 0, i1 false)  ; CreateHandle(resourceClass,rangeId,index,nonUniformIndex)
  %3 = call i32 @dx.op.loadInput.i32(i32 4, i32 5, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %6 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %7 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %8 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %9 = call float @dx.op.loadInput.f32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %10 = call float @dx.op.loadInput.f32(i32 4, i32 0, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %11 = call float @dx.op.loadInput.f32(i32 4, i32 0, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %12 = call %dx.types.ResRet.f32 @dx.op.bufferLoad.f32(i32 68, %dx.types.Handle %1, i32 %3, i32 0)  ; BufferLoad(srv,index,wot)
  %13 = extractvalue %dx.types.ResRet.f32 %12, 0
  %14 = extractvalue %dx.types.ResRet.f32 %12, 1
  %15 = extractvalue %dx.types.ResRet.f32 %12, 2
  %16 = extractvalue %dx.types.ResRet.f32 %12, 3
  %17 = call %dx.types.ResRet.f32 @dx.op.bufferLoad.f32(i32 68, %dx.types.Handle %1, i32 %3, i32 16)  ; BufferLoad(srv,index,wot)
  %18 = extractvalue %dx.types.ResRet.f32 %17, 0
  %19 = extractvalue %dx.types.ResRet.f32 %17, 1
  %20 = extractvalue %dx.types.ResRet.f32 %17, 2
  %21 = extractvalue %dx.types.ResRet.f32 %17, 3
  %22 = call %dx.types.ResRet.f32 @dx.op.bufferLoad.f32(i32 68, %dx.types.Handle %1, i32 %3, i32 32)  ; BufferLoad(srv,index,wot)
  %23 = extractvalue %dx.types.ResRet.f32 %22, 0
  %24 = extractvalue %dx.types.ResRet.f32 %22, 1
  %25 = extractvalue %dx.types.ResRet.f32 %22, 2
  %26 = extractvalue %dx.types.ResRet.f32 %22, 3
  %27 = call %dx.types.ResRet.f32 @dx.op.bufferLoad.f32(i32 68, %dx.types.Handle %1, i32 %3, i32 48)  ; BufferLoad(srv,index,wot)
  %28 = extractvalue %dx.types.ResRet.f32 %27, 0
  %29 = extractvalue %dx.types.ResRet.f32 %27, 1
  %30 = extractvalue %dx.types.ResRet.f32 %27, 2
  %31 = extractvalue %dx.types.ResRet.f32 %27, 3
  %32 = call %dx.types.ResRet.i32 @dx.op.bufferLoad.i32(i32 68, %dx.types.Handle %1, i32 %3, i32 64)  ; BufferLoad(srv,index,wot)
  %33 = extractvalue %dx.types.ResRet.i32 %32, 0
  %34 = fmul fast float %13, %9
  %35 = call float @dx.op.tertiary.f32(i32 46, float %10, float %14, float %34)  ; FMad(a,b,c)
  %36 = call float @dx.op.tertiary.f32(i32 46, float %11, float %15, float %35)  ; FMad(a,b,c)
  %37 = fadd fast float %36, %16
  %38 = fmul fast float %18, %9
  %39 = call float @dx.op.tertiary.f32(i32 46, float %10, float %19, float %38)  ; FMad(a,b,c)
  %40 = call float @dx.op.tertiary.f32(i32 46, float %11, float %20, float %39)  ; FMad(a,b,c)
  %41 = fadd fast float %40, %21
  %42 = fmul fast float %23, %9
  %43 = call float @dx.op.tertiary.f32(i32 46, float %10, float %24, float %42)  ; FMad(a,b,c)
  %44 = call float @dx.op.tertiary.f32(i32 46, float %11, float %25, float %43)  ; FMad(a,b,c)
  %45 = fadd fast float %44, %26
  %46 = fmul fast float %28, %9
  %47 = call float @dx.op.tertiary.f32(i32 46, float %10, float %29, float %46)  ; FMad(a,b,c)
  %48 = call float @dx.op.tertiary.f32(i32 46, float %11, float %30, float %47)  ; FMad(a,b,c)
  %49 = fadd fast float %48, %31
  %50 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %51 = extractvalue %dx.types.CBufRet.f32 %50, 0
  %52 = extractvalue %dx.types.CBufRet.f32 %50, 1
  %53 = extractvalue %dx.types.CBufRet.f32 %50, 2
  %54 = extractvalue %dx.types.CBufRet.f32 %50, 3
  %55 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %56 = extractvalue %dx.types.CBufRet.f32 %55, 0
  %57 = extractvalue %dx.types.CBufRet.f32 %55, 1
  %58 = extractvalue %dx.types.CBufRet.f32 %55, 2
  %59 = extractvalue %dx.types.CBufRet.f32 %55, 3
  %60 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %2, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %61 = extractvalue %dx.types.CBufRet.f32 %60, 0
  %62 = extractvalue %dx.types.CBufRet.f32 %60, 1
  %63 = extractvalue %dx.types.CBufRet.f32 %60, 2
  %64 = extractvalue %dx.types.CBufRet.f32 %60, 3
  %65 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %2, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %66 = extractvalue %dx.types.CBufRet.f32 %65, 0
  %67 = extractvalue %dx.types.CBufRet.f32 %65, 1
  %68 = extractvalue %dx.types.CBufRet.f32 %65, 2
  %69 = extractvalue %dx.types.CBufRet.f32 %65, 3
  %70 = fmul fast float %51, %37
  %71 = call float @dx.op.tertiary.f32(i32 46, float %41, float %52, float %70)  ; FMad(a,b,c)
  %72 = call float @dx.op.tertiary.f32(i32 46, float %45, float %53, float %71)  ; FMad(a,b,c)
  %73 = call float @dx.op.tertiary.f32(i32 46, float %49, float %54, float %72)  ; FMad(a,b,c)
  %74 = fmul fast float %56, %37
  %75 = call float @dx.op.tertiary.f32(i32 46, float %41, float %57, float %74)  ; FMad(a,b,c)
  %76 = call float @dx.op.tertiary.f32(i32 46, float %45, float %58, float %75)  ; FMad(a,b,c)
  %77 = call float @dx.op.tertiary.f32(i32 46, float %49, float %59, float %76)  ; FMad(a,b,c)
  %78 = fmul fast float %61, %37
  %79 = call float @dx.op.tertiary.f32(i32 46, float %41, float %62, float %78)  ; FMad(a,b,c)
  %80 = call float @dx.op.tertiary.f32(i32 46, float %45, float %63, float %79)  ; FMad(a,b,c)
  %81 = call float @dx.op.tertiary.f32(i32 46, float %49, float %64, float %80)  ; FMad(a,b,c)
  %82 = fmul fast float %66, %37
  %83 = call float @dx.op.tertiary.f32(i32 46, float %41, float %67, float %82)  ; FMad(a,b,c)
  %84 = call float @dx.op.tertiary.f32(i32 46, float %45, float %68, float %83)  ; FMad(a,b,c)
  %85 = call float @dx.op.tertiary.f32(i32 46, float %49, float %69, float %84)  ; FMad(a,b,c)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %73)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %77)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %81)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %85)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  %86 = fmul fast float %13, %6
  %87 = call float @dx.op.tertiary.f32(i32 46, float %7, float %14, float %86)  ; FMad(a,b,c)
  %88 = call float @dx.op.tertiary.f32(i32 46, float %8, float %15, float %87)  ; FMad(a,b,c)
  %89 = fmul fast float %18, %6
  %90 = call float @dx.op.tertiary.f32(i32 46, float %7, float %19, float %89)  ; FMad(a,b,c)
  %91 = call float @dx.op.tertiary.f32(i32 46, float %8, float %20, float %90)  ; FMad(a,b,c)
  %92 = fmul fast float %23, %6
  %93 = call float @dx.op.tertiary.f32(i32 46, float %7, float %24, float %92)  ; FMad(a,b,c)
  %94 = call float @dx.op.tertiary.f32(i32 46, float %8, float %25, float %93)  ; FMad(a,b,c)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %88)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %91)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 2, float %94)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 0, float %4)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 1, float %5)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  %95 = sitofp i32 %33 to float
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 0, float %95)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.tertiary.f32(i32, float, float, float) #0

; Function Attrs: nounwind readonly
declare %dx.types.Handle @dx.op.createHandle(i32, i8, i32, i32, i1) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.i32 @dx.op.bufferLoad.i32(i32, %dx.types.Handle, i32, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.bufferLoad.f32(i32, %dx.types.Handle, i32, i32) #2

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!2}
!dx.shaderModel = !{!3}
!dx.resources = !{!4}
!dx.viewIdState = !{!10}
!dx.entryPoints = !{!11}

!0 = !{!"clang version 3.7 (tags/RELEASE_370/final)"}
!1 = !{i32 1, i32 0}
!2 = !{i32 1, i32 5}
!3 = !{!"vs", i32 6, i32 0}
!4 = !{!5, null, !8, null}
!5 = !{!6}
!6 = !{i32 0, %"dx.alignment.legacy.class.StructuredBuffer<InstanceData>"* undef, !"", i32 1, i32 0, i32 1, i32 12, i32 0, !7}
!7 = !{i32 1, i32 80}
!8 = !{!9}
!9 = !{i32 0, %dx.alignment.legacy.GlobalConstants* undef, !"", i32 0, i32 0, i32 1, i32 96, null}
!10 = !{[23 x i32] [i32 21, i32 10, i32 15, i32 15, i32 15, i32 0, i32 112, i32 112, i32 112, i32 0, i32 0, i32 0, i32 0, i32 0, i32 256, i32 512, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 255]}
!11 = !{void ()* @VS, !"VS", !12, !4, !31}
!12 = !{!13, !24, null}
!13 = !{!14, !17, !18, !19, !21, !22}
!14 = !{i32 0, !"POSITION", i8 9, i8 0, !15, i8 0, i32 1, i8 3, i32 0, i8 0, !16}
!15 = !{i32 0}
!16 = !{i32 3, i32 7}
!17 = !{i32 1, !"NORMAL", i8 9, i8 0, !15, i8 0, i32 1, i8 3, i32 1, i8 0, !16}
!18 = !{i32 2, !"TANGENT", i8 9, i8 0, !15, i8 0, i32 1, i8 3, i32 2, i8 0, null}
!19 = !{i32 3, !"TEXCOORD", i8 9, i8 0, !15, i8 0, i32 1, i8 2, i32 3, i8 0, !20}
!20 = !{i32 3, i32 3}
!21 = !{i32 4, !"COLOR", i8 9, i8 0, !15, i8 0, i32 1, i8 4, i32 4, i8 0, null}
!22 = !{i32 5, !"SV_InstanceID", i8 5, i8 2, !15, i8 0, i32 1, i8 1, i32 5, i8 0, !23}
!23 = !{i32 3, i32 1}
!24 = !{!25, !27, !28, !29}
!25 = !{i32 0, !"SV_Position", i8 9, i8 3, !15, i8 4, i32 1, i8 4, i32 0, i8 0, !26}
!26 = !{i32 3, i32 15}
!27 = !{i32 1, !"NORMAL", i8 9, i8 0, !15, i8 2, i32 1, i8 3, i32 1, i8 0, !16}
!28 = !{i32 2, !"TEXCOORD", i8 9, i8 0, !15, i8 2, i32 1, i8 2, i32 2, i8 0, !20}
!29 = !{i32 3, !"TEXCOORD", i8 9, i8 0, !30, i8 2, i32 1, i8 1, i32 1, i8 3, !23}
!30 = !{i32 1}
!31 = !{i32 0, i64 16}
