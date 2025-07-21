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
	// ĸ��, �޽�, SpringArm, ī�޶� ������Ʈ ����
	// TObjectPtr -> ����Ʈ ������ -> Ÿ�� �˻簡 ���� and �޸� ������ �����ϴ�.
	// TObjectPtr<UCapsuleComponent> CapsuleComp -> CapsuleComp�� UCapsuleComponentŸ��(�ڽ�)�� �ּҸ� �޴´�!
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCapsuleComponent> CapsuleComp; // �浹 ó��

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> Mesh; // ���� ǥ��

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArm; // 3��Ī���� �ϱ� ���� ��ī��

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> Camera; // ȭ����� ī�޶�

	// Enhanced Input ���� ������
	// UInputAction -> �÷��̾��� �ൿ -> MoveAction : �̵� , LookAction : ���� -> �ൿ�� �̸��� ���δ�.
	// UInputMappingContext -> WŰ�� ���Ǵٸ� ������(X��) X���� +1�� �Ѵ� -> Input�� Mapping�Ѵ�
	// SetupPlayerInputComponent -> MoveAction�� �ߵ� -> Move() ȣ��
	// SetupPlayerInputComponent -> LookAction �ߵ� -> Look() ȣ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	// �̵� ȸ�� �ӵ� ���� ������
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MoveSpeed = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float LookSpeed = 100.f;

	// ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> AscendAction; // ��/�� �̵� �׼�

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> RollAction; // ��/�� ����(Roll)�� �׼�

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|6DOF") // Six Degrees of Freedom
		float AscendSpeed = 300.0f; // ��/�� �ӵ�

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|6DOF")
	float RollSpeed = 100.0f; // ��/�� ���� �ӵ�

private:
	// �Է� ó�� �Լ�
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void Ascend(const FInputActionValue& Value);
	void Roll(const FInputActionValue& Value);

	// ������ �ʿ��� �Լ���
	void StopMove(const FInputActionValue& Value);
	void StopLook(const FInputActionValue& Value);
	void StopAscend(const FInputActionValue& Value);
	void StopRoll(const FInputActionValue& Value);

	// �Է� ���� ������ ������
	FVector2D CurrentMoveInput;
	FVector2D CurrentLookInput;
	float CurrentAscendInput;
	float CurrentRollInput;
};
