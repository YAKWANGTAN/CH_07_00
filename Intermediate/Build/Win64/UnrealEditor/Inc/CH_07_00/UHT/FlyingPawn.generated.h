// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "FlyingPawn.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef CH_07_00_FlyingPawn_generated_h
#error "FlyingPawn.generated.h already included, missing '#pragma once' in FlyingPawn.h"
#endif
#define CH_07_00_FlyingPawn_generated_h

#define FID_CH_07_00_Source_CH_07_00_FlyingPawn_h_19_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesAFlyingPawn(); \
	friend struct Z_Construct_UClass_AFlyingPawn_Statics; \
public: \
	DECLARE_CLASS(AFlyingPawn, APawn, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/CH_07_00"), NO_API) \
	DECLARE_SERIALIZER(AFlyingPawn)


#define FID_CH_07_00_Source_CH_07_00_FlyingPawn_h_19_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	AFlyingPawn(AFlyingPawn&&); \
	AFlyingPawn(const AFlyingPawn&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AFlyingPawn); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AFlyingPawn); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(AFlyingPawn) \
	NO_API virtual ~AFlyingPawn();


#define FID_CH_07_00_Source_CH_07_00_FlyingPawn_h_16_PROLOG
#define FID_CH_07_00_Source_CH_07_00_FlyingPawn_h_19_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_CH_07_00_Source_CH_07_00_FlyingPawn_h_19_INCLASS_NO_PURE_DECLS \
	FID_CH_07_00_Source_CH_07_00_FlyingPawn_h_19_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> CH_07_00_API UClass* StaticClass<class AFlyingPawn>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_CH_07_00_Source_CH_07_00_FlyingPawn_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
