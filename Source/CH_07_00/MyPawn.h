// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyPawn.generated.h"

class UCapsuleComponent;
class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class CH_07_00_API AMyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	// ������Ʈ(���)���� ������ ����

	// �浹�� ó���� ������Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	// ���� ǥ��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> Mesh;

	// 3��Ī���� �ϱ� ���� ��ī��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	// ȭ����� ī�޶�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;

	// IA, IMC
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	// �̵� �ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MoveSpeed = 500.f;

	// ȸ�� �ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float LookSpeed = 100.f;

	// ���� �׼� Space
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	// �߷� �� ���� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Gravity")
	float Gravity = -980.0f; // �߷� ���ӵ� - cm/s��

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Gravity")
	float JumpForce = 600.0f; // ���� ��

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Gravity")
	float AirControl = 0.5f; // ���� �����


private:
	// �Է� ó�� �Լ�
	void Move(const FInputActionValue& Value);
	void StopMove(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);

	// �Է� ���� �����ϴ� ����
	FVector2D CurrentMoveInput;

	// ���� ���� ����
	FVector Velocity = FVector::ZeroVector; // ���� �ӵ� (X, Y, Z)
	bool bIsOnGround = true; // ���� ���� �ƴ��� �Ǻ�
};
