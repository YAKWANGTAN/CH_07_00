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

	// 캡슐 컴포넌트를 생성하고 루트로 설정
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	RootComponent = CapsuleComp;
	// 요구사항: 물리 시뮬레이션 OFF
	CapsuleComp->SetSimulatePhysics(false);

	// 스켈레탈 메시 컴포넌트를 생성하고 캡슐에 부착
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	// 요구사항 -> 물리 시뮬레이션 OFF
	Mesh->SetSimulatePhysics(false);

	// 스프링 암을 생성하고 캡슐에 부착 (셀카봉)
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.0f; // 카메라와의 거리

	// 카메라를 생성하고 스프링 암 끝에 부착
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void AFlyingPawn::BeginPlay()
{
	Super::BeginPlay();
	
	// 게임 시작 시 Enhanced Input 시스템을 활성화합니다.
	// 현재 pawn controller -> APlayerController로 변환
	// PlayerController -> Enhanced Input Local Player Subsystem을 가져옵니다.
	// 마지막으로 가져온 Mapping Context 추가.
	// 블루프린트에서 만든 키 설정(IMC)을 실제 게임에서 인식하도록 시스템에 등록
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
	// 위 -> CurrentAscendInput = 1;
	// 아래 -> CurrentAscendInput = -1;
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

	//  회전(Rotation) 처리
	//  Pitch, Yaw, Roll 값을 계산
	const float TargetPitch = CurrentLookInput.Y * LookSpeed * DeltaTime;
	const float TargetYaw = CurrentLookInput.X * LookSpeed * DeltaTime;
	const float TargetRoll = CurrentRollInput * RollSpeed * DeltaTime;

	// FRotator를 만들어 한번에 회전
	const FRotator DeltaRotation = FRotator(TargetPitch, TargetYaw, TargetRoll);
	AddActorLocalRotation(DeltaRotation, true);

	//  이동(Location) 처리
	//  전/후, 좌/우, 상/하 이동 값을 계산
	const FVector DeltaLocation = FVector(
		CurrentMoveInput.X * MoveSpeed, // 전/후 
		CurrentMoveInput.Y * MoveSpeed, // 좌/우
		CurrentAscendInput * AscendSpeed // 상/하
	) * DeltaTime;

	// 최종 이동량 적용
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
		// 새로운 액션 바인딩 추가
		EnhancedInputComponent->BindAction(AscendAction, ETriggerEvent::Triggered, this, &AFlyingPawn::Ascend);
		EnhancedInputComponent->BindAction(AscendAction, ETriggerEvent::Completed, this, &AFlyingPawn::StopAscend);
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &AFlyingPawn::Roll);
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Completed, this, &AFlyingPawn::StopRoll);
	}
}