// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FlyingPawn.generated.h"

class UCapsuleComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class CH_07_00_API AFlyingPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AFlyingPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	// 캡슐, 메시, SpringArm, 카메라 컴포넌트 선언
	// TObjectPtr -> 스마트 포인터 -> 타입 검사가 엄격 and 메모리 관리가 쉽습니다.
	// TObjectPtr<UCapsuleComponent> CapsuleComp -> CapsuleComp는 UCapsuleComponent타입(자식)의 주소만 받는다!
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCapsuleComponent> CapsuleComp; // 충돌 처리

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> Mesh; // 외형 표시

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArm; // 3인칭으로 하기 위한 셀카봉

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> Camera; // 화면찍는 카메라

	// Enhanced Input 관련 변수들
	// UInputAction -> 플레이어의 행동 -> MoveAction : 이동 , LookAction : 보기 -> 행동에 이름을 붙인다.
	// UInputMappingContext -> W키가 눌렷다면 앞으로(X축) X축을 +1로 한다 -> Input을 Mapping한다
	// SetupPlayerInputComponent -> MoveAction이 발동 -> Move() 호출
	// SetupPlayerInputComponent -> LookAction 발동 -> Look() 호출
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	// 이동 회전 속도 관련 변수들
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MoveSpeed = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float LookSpeed = 100.f;

	// 도전 과제
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> AscendAction; // 상/하 이동 액션

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> RollAction; // 좌/우 기울기(Roll)용 액션

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|6DOF") // Six Degrees of Freedom
		float AscendSpeed = 300.0f; // 상/하 속도

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|6DOF")
	float RollSpeed = 100.0f; // 좌/우 기울기 속도

private:
	// 입력 처리 함수
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void Ascend(const FInputActionValue& Value);
	void Roll(const FInputActionValue& Value);

	// 정지에 필요한 함수들
	void StopMove(const FInputActionValue& Value);
	void StopLook(const FInputActionValue& Value);
	void StopAscend(const FInputActionValue& Value);
	void StopRoll(const FInputActionValue& Value);

	// 입력 값을 저장할 변수들
	FVector2D CurrentMoveInput;
	FVector2D CurrentLookInput;
	float CurrentAscendInput;
	float CurrentRollInput;
};
