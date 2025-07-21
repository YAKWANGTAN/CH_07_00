// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingPawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AFlyingPawn::AFlyingPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// ĸ�� ������Ʈ�� �����ϰ� ��Ʈ�� ����
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	RootComponent = CapsuleComp;
	// �䱸����: ���� �ùķ��̼� OFF
	CapsuleComp->SetSimulatePhysics(false);

	// ���̷�Ż �޽� ������Ʈ�� �����ϰ� ĸ���� ����
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	// �䱸���� -> ���� �ùķ��̼� OFF
	Mesh->SetSimulatePhysics(false);

	// ������ ���� �����ϰ� ĸ���� ���� (��ī��)
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.0f; // ī�޶���� �Ÿ�

	// ī�޶� �����ϰ� ������ �� ���� ����
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void AFlyingPawn::BeginPlay()
{
	Super::BeginPlay();
	
	// ���� ���� �� Enhanced Input �ý����� Ȱ��ȭ�մϴ�.
	// ���� pawn controller -> APlayerController�� ��ȯ
	// PlayerController -> Enhanced Input Local Player Subsystem�� �����ɴϴ�.
	// ���������� ������ Mapping Context �߰�.
	// �������Ʈ���� ���� Ű ����(IMC)�� ���� ���ӿ��� �ν��ϵ��� �ý��ۿ� ���
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AFlyingPawn::Move(const FInputActionValue& Value)
{
	CurrentMoveInput = Value.Get<FVector2D>();
	// W -> (1, 0)
	// S -> (-1, 0)
	// A -> (0, -1)
	// D -> (0, 1)
	// ? -> (x, y)
}

void AFlyingPawn::Look(const FInputActionValue& Value)
{
	CurrentLookInput = Value.Get<FVector2D>();
}

void AFlyingPawn::Ascend(const FInputActionValue& Value)
{
	CurrentAscendInput = Value.Get<float>();
	// �� -> CurrentAscendInput = 1;
	// �Ʒ� -> CurrentAscendInput = -1;
}

void AFlyingPawn::Roll(const FInputActionValue& Value)
{
	CurrentRollInput = Value.Get<float>();
	// 1, -1
}

void AFlyingPawn::StopMove(const FInputActionValue& Value)
{
	CurrentMoveInput = FVector2D::ZeroVector;
}

void AFlyingPawn::StopLook(const FInputActionValue& Value)
{
	CurrentLookInput = FVector2D::ZeroVector;
}

void AFlyingPawn::StopAscend(const FInputActionValue& Value)
{
	CurrentAscendInput = 0.0f;
}

void AFlyingPawn::StopRoll(const FInputActionValue& Value)
{
	CurrentRollInput = 0.0f;
}

// Called every frame
void AFlyingPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//  ȸ��(Rotation) ó��
	//  Pitch, Yaw, Roll ���� ���
	const float TargetPitch = CurrentLookInput.Y * LookSpeed * DeltaTime;
	const float TargetYaw = CurrentLookInput.X * LookSpeed * DeltaTime;
	const float TargetRoll = CurrentRollInput * RollSpeed * DeltaTime;

	// FRotator�� ����� �ѹ��� ȸ��
	const FRotator DeltaRotation = FRotator(TargetPitch, TargetYaw, TargetRoll);
	AddActorLocalRotation(DeltaRotation, true);

	//  �̵�(Location) ó��
	//  ��/��, ��/��, ��/�� �̵� ���� ���
	const FVector DeltaLocation = FVector(
		CurrentMoveInput.X * MoveSpeed, // ��/�� 
		CurrentMoveInput.Y * MoveSpeed, // ��/��
		CurrentAscendInput * AscendSpeed // ��/��
	) * DeltaTime;

	// ���� �̵��� ����
	AddActorLocalOffset(DeltaLocation, true);
}

// Called to bind functionality to input
void AFlyingPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFlyingPawn::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AFlyingPawn::StopMove);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFlyingPawn::Look);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Completed, this, &AFlyingPawn::StopLook);
		// ���ο� �׼� ���ε� �߰�
		EnhancedInputComponent->BindAction(AscendAction, ETriggerEvent::Triggered, this, &AFlyingPawn::Ascend);
		EnhancedInputComponent->BindAction(AscendAction, ETriggerEvent::Completed, this, &AFlyingPawn::StopAscend);
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &AFlyingPawn::Roll);
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Completed, this, &AFlyingPawn::StopRoll);
	}
}