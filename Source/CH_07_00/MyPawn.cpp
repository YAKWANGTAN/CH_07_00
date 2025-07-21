// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// ĸ�� ������Ʈ�� ����� -> RootComponent�� ���δ�.
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCompnent"));
	RootComponent = CapsuleComponent;

	// ���� �䱸����
	CapsuleComponent->SetSimulatePhysics(false);

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
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();

	// ���� ���� �� Enhanced Input �ý����� Ȱ��ȭ�մϴ�.
	// �츮�� ���� IMC�� ������ ����Ѵ�
	// Pawn�� �ִ� Controller�� -> �÷��̾�� PlayerController�� ��ȯ! AI��� AI������ ��ȯ!
	// �𸮾󿡼� �Է��� Subsystem���� ����
	// ���� ���� �÷��̾��� ����ý����� �����ͼ� �����Ϳ��� ���� IMC�� �����մϴ�.
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AMyPawn::Move(const FInputActionValue& Value)
{
	// WASD �Է°��� ������ ����
	CurrentMoveInput = Value.Get<FVector2D>(); // (1.0, 0)
}

void AMyPawn::StopMove(const FInputActionValue& Value)
{
	CurrentMoveInput = FVector2D::ZeroVector; // (0.0, 0.0)
}

void AMyPawn::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	// �¿�  (1.0, 0.0)

	// AddControllerInput ��� ���� Actor ȸ��
	// ��/�� ȸ��(Yaw)�� Pawn ��ü�� ���� ȸ��
	// GetWorld()->GetDeltaSeconds() -> tick�� DeltaTime�� ���� ��
	AddActorLocalRotation(FRotator(0.0f, LookAxisVector.X * LookSpeed * GetWorld()->GetDeltaSeconds(), 0.0f));

	// ��/�Ʒ� ȸ��(pitch)�� ī�޶� �޷��ִ� ������ ���� ȸ -> ĳ���Ͱ� ��/�ڷ� �����̸� �ȵ˴ϴ�.	
	FRotator CurrentSpringArmRotation = SpringArm->GetRelativeRotation(); // ���� SpringArm�� rotation ��������
	// ���� Pitch���� -  ���콺�� �� �Ʒ� * ȸ�� �ӵ� * ������ ����
	float NewPitch = CurrentSpringArmRotation.Pitch - LookAxisVector.Y * LookSpeed * GetWorld()->GetDeltaSeconds();

	// ī�޶� ���� �İ��ų� �ϴø� ���� �ʵ��� ������ ����
	NewPitch = FMath::Clamp(NewPitch, -75.f, 0.f);

	SpringArm->SetRelativeRotation(FRotator(NewPitch, 0.f, 0.f));
}

void AMyPawn::Jump(const FInputActionValue& Value)
{
	// ���� üũ �� ���� ���� �ϵ��� ����
	if (bIsOnGround)
	{
		Velocity.Z = JumpForce; // Z�� force ����
		bIsOnGround = false; // ���� ���·� ǥ��
	}
}
// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 1. ���� ���� (LineTrace)
	FHitResult HitResult;
	FVector StartTrace = GetActorLocation();
	// ĸ�� ���� + ����
	FVector EndTrace = StartTrace + FVector(0.f, 0.f, -(CapsuleComponent->GetScaledCapsuleHalfHeight() + 10.0f));
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	bIsOnGround = GetWorld()->
		LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Visibility, CollisionParams);

	// ����� ���� (�׽�Ʈ��, ���: ����, ����: ����)
	DrawDebugLine(
		GetWorld(),
		StartTrace,
		EndTrace,
		bIsOnGround ? FColor::Green : FColor::Red,
		false,
		0.0f,
		0,
		1.0f
	);

	// 2. �߷� �� �ӵ� ���
	if (!bIsOnGround)
	{
		Velocity.Z += Gravity * DeltaTime; // �߷� ���� (����)
	}

	// 3. �̵� �Է� ��� (����/���� ����)
	float CurrentMoveSpeed = bIsOnGround ? MoveSpeed : MoveSpeed * AirControl;
	FVector DesiredMovement = FVector(CurrentMoveInput.X, CurrentMoveInput.Y, 0.0f).GetSafeNormal() * CurrentMoveSpeed;

	// ����: ��� �ӵ� ���� / ����: �ε巴�� �߰� (������Ʈ��)
	if (bIsOnGround)
	{
		Velocity.X = DesiredMovement.X;
		Velocity.Y = DesiredMovement.Y;
		if (Velocity.Z < 0.f) Velocity.Z = 0.f; // ���󿡼� �Ʒ��� ���� ����
	}
	else
	{
		Velocity.X = FMath::Lerp(Velocity.X, DesiredMovement.X, AirControl); // �ε巴��
		Velocity.Y = FMath::Lerp(Velocity.Y, DesiredMovement.Y, AirControl);
	}

	// 4. ���� ó��
	if (bIsOnGround && Velocity.Z < 0.0f)
	{
		Velocity.Z = 0.0f; // �ӵ� �ʱ�ȭ
		// ��ġ ���� (�ո� ����)
		SetActorLocation(HitResult.ImpactPoint + FVector(0.0f, 0.0f, CapsuleComponent->GetScaledCapsuleHalfHeight()));
	}

	// 5. ���� �̵� ���� (���� -> ���� ��ȯ)
	FVector WorldVelocity = GetActorRotation().RotateVector(Velocity);
	AddActorWorldOffset(WorldVelocity * DeltaTime, true); // Sweep���� �浹 üũ
}


// �Է� -> Enhanced Input Subsystem -> PlayerInput -> PlayerController(SetupPlayerInputComponent) -> ���ε��� �Լ� -> �̵�
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// IA_Move �׼��� �ߵ��ϸ� �� Ŭ������ Move �Լ��� ȣ���ϵ��� ���ε�!
		EnhancedInputComponent->BindAction(
			MoveAction, // MoveAction�� 
			ETriggerEvent::Triggered, // �ߵ��Ǹ�
			this, // ���� ��ü
			&AMyPawn::Move // Move�Լ��� ����
		);
		EnhancedInputComponent->BindAction(
			MoveAction,
			ETriggerEvent::Completed, // Completed -> StopMove����
			this,
			&AMyPawn::StopMove
		);
		EnhancedInputComponent->BindAction(
			LookAction,
			ETriggerEvent::Triggered,
			this,
			&AMyPawn::Look
		);
		EnhancedInputComponent->BindAction(
			JumpAction,
			ETriggerEvent::Triggered,
			this,
			&AMyPawn::Jump
		);
	}
}